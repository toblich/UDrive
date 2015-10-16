#include "manejadorArchivosYMetadatos.h"

// TODO: LOGUEAR!!!!!
// TODO: Ver lo de la actualizacion de metadatos (ultima modificacion, etc)
// TODO: El username que se recibe en la mayoria de los metodos es el del que pide. Luego, el path tiene el path posta. Por ejemplo:
//		 username: pablo		path: tobi/hola/asd.txt
//		 Me deberia fijar si username es igual al primero del path. De ser asi, tiene los permisos seguro.
//		 Sino, como en este caso, deberia fijarme si pablo tiene permisos para acceder a ese path mediante lo que habra en la DB
//		 bajo la key '#permisos/pablo' y cuyo value sera algo del estilo 'pancho/datos/pepe.jpg#tobi/hola/asd.txt#santi/chau.txt'
//		 lo cual habra que parsear y ver si esta o no.
//
//		 REVISAR Y TESTEAR!!!

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos (BD* dbMetadatos) :
		ManejadorArchivosYMetadatos(dbMetadatos, defaultFileSystem) {
}

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos (BD* dbMetadatos, string path) : validador(dbMetadatos) {
	this->dbMetadatos = dbMetadatos;
	this->pathFileSystem = path;
	getcwd(homeDirectory, sizeof(homeDirectory));
}

ManejadorArchivosYMetadatos::~ManejadorArchivosYMetadatos () {
}

bool ManejadorArchivosYMetadatos::eliminar (string username, string path) {
	string pathConFS = this->pathFileSystem + "/" + path;
	if (validador.verificarPermisos(username, path)) {
		if (validador.existeCarpeta(pathConFS))
			return this->eliminarCarpeta(username, path);
		return this->eliminarArchivo(username, path);
	}
	return false;
}

// El path recibido no debe contener el nombre de un archivo.
// En caso de que sea asi, se debera modificar este metodo.
bool ManejadorArchivosYMetadatos::crearCarpeta (string username, string path) {
	bool tienePermisos = validador.verificarPermisos(username, path);
	// Agrego el FileSystem para que sea la "raiz"
	string pathCompletoConFS = this->pathFileSystem + "/" + path;
	vector<string> directorios = ParserURI::parsear(pathCompletoConFS, '/');
	string directorioAcumulado = "";
	const int SIZE = directorios.size();
	for (int i = 0; i < SIZE; i++) {
		string directorio = directorios[i];
		string directorioPadre = directorioAcumulado;
		directorioAcumulado += (directorio + "/");

		if ( not validador.existeCarpeta(directorioAcumulado) ) { // Me fijo si existe la carpeta, sino la creo
			if (tienePermisos) {
				mkdir(directorioAcumulado.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				Logger::logInfo("La carpeta " + directorio + " no existe dentro de " + directorioPadre
								+ " por lo que ha sido creada.");
			} else
				return false; // Si no existia la carpeta y no tiene permisos para crear
		}
	}
	return true;
}

bool ManejadorArchivosYMetadatos::crearCarpetaSegura (string username, string path) {
	if (validador.esPathValido(path))
		return this->crearCarpeta(username, path);
	return false;
}

bool ManejadorArchivosYMetadatos::agregarPermisosABD (string username) {
	string key = permisos + "/" + username;
	return this->dbMetadatos->put(key, ""); // PUT devuelve false si ya existia la clave, true si la agrega
}

bool ManejadorArchivosYMetadatos::crearUsuario (string username) {
	//Creo tanto la carpeta del username como su papelera
	string pathTrash = username + "/" + trash;
	bool agregoPermisos = this->agregarPermisosABD(username);
	return (agregoPermisos and this->crearCarpeta(username, pathTrash));
}

//Borrara todos los archivos de la carpeta y, en caso de que quede vacia, la carpeta fisica del fileSystem tambien
bool ManejadorArchivosYMetadatos::eliminarCarpeta (string username, string path) {
	string pathConFS = this->pathFileSystem + "/" + path;
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(pathConFS.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			string dirName(ent->d_name);
			if (dirName == "." or dirName == "..")
				continue;

			string pathInterno = path + "/" + dirName;
			string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;
			if (validador.existeCarpeta(pathInternoConFS))
				this->eliminarCarpeta(username, pathInterno);
			else
				this->eliminarArchivo(username, pathInterno);
		}
		closedir(dir);
		if (validador.carpetaVacia(pathConFS))
			return this->deleteCarpeta(pathConFS);
	} else
		Logger::logWarn("No existe el directorio " + path);
	return false;
}

// En la base de datos se guarda el path sin la carpeta del FS
bool ManejadorArchivosYMetadatos::subirArchivo (string username, string filepath, const char* data, int dataLen,
		string jsonMetadatos) {
	if (validador.verificarPermisos(username, filepath)) {
		string filepathCompleto = this->pathFileSystem + "/" + filepath;
		if (not validador.existeArchivo(filepathCompleto)) {
			if (dbMetadatos->contains(filepath)) {
				Logger::logWarn("Se quiso subir el archivo " + filepath
								+ " pero este ya existe. Debe utilizar el metodo actualizarArchivo.");
				return false;
			}
			if (not this->actualizarArchivo(username, filepath, data, dataLen))
				return false;
			return dbMetadatos->put(filepath, jsonMetadatos);
		} else
			// TODO: Versionado
			return false;
	} else
		return false;
}

bool ManejadorArchivosYMetadatos::guardarArchivo (const string& filepath, const string& username, const char* data, int dataLen) {
	vector<string> directorios = ParserURI::parsear(filepath, '/');
	string pathSinArchivo = ParserURI::join(directorios, '/', 0, directorios.size() - 1);

	//Verifico que existan todas las carpetas y sino las creo
	if (pathSinArchivo != "") {
		if (not crearCarpetaSegura(username, pathSinArchivo)) {
			Logger::logWarn("Al querer actualizar el archivo " + filepath + " no se pudieron crear las carpetas.");
			return false;
		}
	}
	string pathConFileSystem = this->pathFileSystem + "/" + filepath;
	if (dbMetadatos->contains(filepath)) {
		//Significa que no fui llamado desde el subirArchivo, por lo que la actualizacion se hará ahí
		string metadatos = dbMetadatos->get(filepath);
		string nuevosMetadatos = this->actualizarUsuarioFechaModificacion(metadatos, username);
		dbMetadatos->modify(filepath, nuevosMetadatos);
		//					Logger::logWarn("Se quiso consultar los metadatos del archivo " + filepath + " pero este no existe.");
		//					return false;
	}
	ofstream outFile(pathConFileSystem, ofstream::binary);
	outFile.write(data, dataLen);
	outFile.close();
	return true;
}

// El filename deberia venir con los path de carpetas tambien y dentro tambien el nombre de usuario
bool ManejadorArchivosYMetadatos::actualizarArchivo (string username, string filepath, const char* data, int dataLen) {
	//No le agrego el FileSystem porque se agrega despues en el metodo crearCarpeta
	if (validador.verificarPermisos(username, filepath)) {
		unsigned long int folderSize = 0;
		if (tamanioCarpeta(username, folderSize)) {
			if (folderSize + dataLen <= CUOTA) {
				return guardarArchivo(filepath, username, data, dataLen);
			}
			unsigned long int cuotaMB = CUOTA / 1048576;
			string texto = "No se ha podido subir el archivo " + filepath + " debido a que se ha superado la cuota de "; // + cuotaMB + " MB.";
			texto += cuotaMB;
			texto += " MB.";
			Logger::logWarn(texto);
		}
	}
	return false;
}

string ManejadorArchivosYMetadatos::consultarMetadatosArchivo (string username, string filepath) {
	if (validador.verificarPermisos(username, filepath)) {
		if (not dbMetadatos->contains(filepath)) {
			Logger::logWarn("Se quiso consultar los metadatos del archivo " + filepath + " pero este no existe.");
			return "";
		}
		return dbMetadatos->get(filepath);
	}
	return "";
}

void ManejadorArchivosYMetadatos::actualizarMetadatosChequeados (const string& filepath,
		const string& jsonNuevosMetadatos, const string& username) {
	ParserJson parser;
	string jsonMetadatosViejos = dbMetadatos->get(filepath);
	MetadatoArchivo metadatosViejos = parser.deserializarMetadatoArchivo(jsonMetadatosViejos);
	MetadatoArchivo metadatosNuevos = parser.deserializarMetadatoArchivo(jsonNuevosMetadatos);
	list<string> usuariosViejos = metadatosViejos.usuariosHabilitados;
	list<string> usuariosNuevos = metadatosNuevos.usuariosHabilitados;
	string nuevoJson;
	if (usuariosNuevos.empty()) {
		metadatosNuevos.usuariosHabilitados = usuariosViejos;
		nuevoJson = parser.serializarMetadatoArchivo(metadatosNuevos);
	} else {
		list<string>::iterator itUsuNuevos = usuariosNuevos.begin();
		for (; itUsuNuevos != usuariosNuevos.end(); itUsuNuevos++) {
			string nuevoUsuario = (*itUsuNuevos);
			// Si todavia no tenia permisos
			if (find(usuariosViejos.begin(), usuariosViejos.end(), nuevoUsuario) == usuariosViejos.end()) {
				this->agregarPermiso(username, filepath, nuevoUsuario);
			}
		}
		nuevoJson = jsonNuevosMetadatos;
	}
	dbMetadatos->modify(filepath, nuevoJson);
}

// Lo que se va hacer es lo siguiente: Los nuevos metadatos vendran integramente por lo que se va a cambiar salvo por los usuarios habilitados.
// En ese caso, puedo recibir ese campo vacio o lleno:
// - Vacio significa que no se modificaron los usuarios
// - Lleno significa que esos son los nuevos usuarios habilitados -> En este caso me tengo que fijar los que ya habia comparados con los nuevos
//																	 a ver cual es el/los nuevo/s usuarios a dar permisos.
// Por ahora no se pueden borrar permisos.
bool ManejadorArchivosYMetadatos::actualizarMetadatos (string username, string filepath, string jsonNuevosMetadatos) {
	if (validador.verificarPermisos(username, filepath)) {
		if (not dbMetadatos->contains(filepath)) {
			Logger::logWarn("Se quiso actualizar los metadatos del archivo " + filepath + " pero este no existe.");
			return false;
		}
		actualizarMetadatosChequeados(filepath, jsonNuevosMetadatos, username);
		return true;
	} else
		return false;
}

bool ManejadorArchivosYMetadatos::agregarPermiso (string usernameOrigen, string filepath, string usernameDestino) {
	if (validador.verificarPermisos(usernameOrigen, filepath)) {
		if (not dbMetadatos->contains(filepath)) {
			Logger::logWarn("Se quiso agregar un permiso al archivo " + filepath + " pero este no existe.");
			return false;
		}
		string pathPermisos = permisos + "/" + usernameDestino;
		if (not dbMetadatos->contains(pathPermisos)) {
			Logger::logWarn("Se quiso agregar un permiso al usuario " + usernameDestino + " pero este no existe.");
			return false;
		}
		string jsonArchivo = dbMetadatos->get(filepath);
		ParserJson parser;
		MetadatoArchivo metadato = parser.deserializarMetadatoArchivo(jsonArchivo);
		metadato.usuariosHabilitados.push_back(usernameDestino);
		string jsonModificado = parser.serializarMetadatoArchivo(metadato);
		dbMetadatos->modify(filepath, jsonModificado);
//		Como este metodo se llama directamente desde actualizarMetadatos, ya la fecha viene modificada
//		string nuevosMetadatos = this->actualizarUsuarioFechaModificacion(jsonModificado, usernameOrigen);
//		dbMetadatos->modify(filepath, nuevosMetadatos);

		string archivosPermitidos = dbMetadatos->get(pathPermisos);
		if (archivosPermitidos != "")
			archivosPermitidos += "#";
		archivosPermitidos += filepath;
		dbMetadatos->modify(pathPermisos, archivosPermitidos);

		return true;
	} else
		return false;
}

Batch ManejadorArchivosYMetadatos::armarBatchEliminarArchivo (const string& jsonMetadatos, const string& username,
		const string& filepath, const string& pathCompletoPapelera) {
	Batch batch;
	string jsonMetadatosConFechaModif = this->actualizarUsuarioFechaModificacion(jsonMetadatos, username);
	MetadatoArchivo metadatoConFechaModif = ParserJson().deserializarMetadatoArchivo(jsonMetadatosConFechaModif);
	list<string> usuariosHabilitados = metadatoConFechaModif.usuariosHabilitados;
	list<string>::iterator itUsu = usuariosHabilitados.begin();

	for (; itUsu != usuariosHabilitados.end(); itUsu++) {
		string usuario = (*itUsu);
		if (usuario == metadatoConFechaModif.propietario)
			continue;

		string permisosUsuario = permisos + "/" + usuario;
		if (dbMetadatos->contains(permisosUsuario)) {
			string archivosStr = dbMetadatos->get(permisosUsuario);
			vector<string> archivos = ParserURI::parsear(archivosStr, '#');
			archivos.erase(remove(archivos.begin(), archivos.end(), filepath), archivos.end());
			string joined = ParserURI::join(archivos, '#');
			batch.modify(permisosUsuario, joined);
		}
	}
	batch.modify(filepath, jsonMetadatosConFechaModif);
	batch.erase(filepath);
	batch.put(pathCompletoPapelera, jsonMetadatosConFechaModif);
	return batch;
}

// Lo que se hace es moverlo a la papelera y cambiar el key de los metadatos por ese
// Manda el archivo a la papelera del propietario y borra todos los permisos de todos, salvo el del propietario
bool ManejadorArchivosYMetadatos::eliminarArchivo (string username, string filepath) {
	if (validador.verificarPermisos(username, filepath)) {
		string filepathCompleto = this->pathFileSystem + "/" + filepath;
		if (validador.existeArchivo(filepathCompleto)) {
			if (not dbMetadatos->contains(filepath)) {
				Logger::logWarn("Se quiso eliminar el archivo " + filepath + " pero este no existe en la base de datos.");
				return false;
			}

			vector<string> directorios = ParserURI::parsear(filepath, '/');
			string jsonMetadatos = dbMetadatos->get(filepath);
			MetadatoArchivo metadato = ParserJson().deserializarMetadatoArchivo(jsonMetadatos);

			if (metadato.propietario != directorios.front()) {	// DEBUG: no deberia suceder nunca
				cout << "El propietario del metadato no coincide con la carpeta del FileSystem. EXIT." << endl;
				Logger::logError("El propietario del metadato no coincide con la carpeta del FileSystem. EXIT.");
				exit(1);
			}

			string pathSinUsernameConHash = ParserURI::join(directorios, '#', 1, directorios.size());

			string pathCompletoPapelera = metadato.propietario + "/" + trash + "/" + pathSinUsernameConHash;
			string nroSecuencia = this->validador.obtenerNumeroSecuencia(this->pathFileSystem, metadato.propietario, pathSinUsernameConHash);
			pathCompletoPapelera += "#" + nroSecuencia;
			string pathCompletoPapeleraConFS = this->pathFileSystem + "/" + pathCompletoPapelera;

			if (not rename(filepathCompleto.c_str(), pathCompletoPapeleraConFS.c_str())) {
				Logger::logInfo("La eliminacion del archivo " + filepath + " fue correcta.");

				Batch batch = armarBatchEliminarArchivo(jsonMetadatos, username, filepath, pathCompletoPapelera);

				if (this->dbMetadatos->writeBatch(batch))
					return true;
				Logger::logWarn("No se ha podido escribir el batch de eliminacion del archivo " + filepath + ".");
				return false;
			} else {
				Logger::logWarn("La eliminacion del archivo " + filepath + " no fue correcta.");
				return false;
			}
		} else
			return false;
	} else
		return false;
}

//TODO: Fijarse del caso especial de "compartidos conmigo"
//TODO: Fijarse de mandar pathInterno en vez de metadato.nombre
string ManejadorArchivosYMetadatos::obtenerEstructuraCarpeta (string path) {
	string pathCompleto = this->pathFileSystem + "/" + path;
	DIR* dir;
	if ((dir = opendir(pathCompleto.c_str())) != NULL) {
		map<string, string> mapa;
		struct dirent* ent;
		while ((ent = readdir(dir)) != NULL) {
			string dirName(ent->d_name);
			if (dirName == "." or dirName == "..")
				continue;

			string pathInterno = path + "/" + ent->d_name;
			string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;
			if (validador.existeCarpeta(pathInternoConFS)) {
				vector<string> directorios = ParserURI::parsear(pathInterno, '/');
				int size = directorios.size();
				string foldername = directorios[size - 1];
				mapa.insert(pair<string, string>(foldername, "#folder"));
			} else { //Es un archivo
				if (not dbMetadatos->contains(pathInterno)) {
					Logger::logWarn("Se quiso obtener los metadatos del archivo " + path + " pero este no existe.");
					return "";
				}
				string jsonMetadatos = this->dbMetadatos->get(pathInterno);
				MetadatoArchivo metadato = ParserJson().deserializarMetadatoArchivo(jsonMetadatos);
				string nombre = metadato.nombre;
				if (metadato.extension != "none")
					nombre += "." + metadato.extension;
				mapa.insert(pair<string, string>(nombre, metadato.extension));
			}
		}
		closedir(dir);
		string json = ParserJson().serializarMapa(mapa);
		return json;
	} else
		Logger::logWarn("No existe el directorio " + path);
	return "";
//
}

string ManejadorArchivosYMetadatos::descargarArchivo (string username, string filepath) {
	//OJO porque si no se corre desde la carpeta build como ./udrive esto va a pinchar seguramente (Ya que la carpeta del FileSystem no va a existir)
	if (validador.verificarPermisos(username, filepath)) {
		string filepathCompleto = this->pathFileSystem + "/" + filepath;
		if (validador.existeArchivo(filepathCompleto)) {
			string pathADevolver(this->homeDirectory);
			pathADevolver += "/" + filepathCompleto;
			return pathADevolver;
		} else
			Logger::logWarn("Se ha querido descargar el archivo de path " + filepath + ", el cual no existe.");
	}
	return "";
}

bool ManejadorArchivosYMetadatos::deleteCarpeta (string path) {
	if (validador.existeCarpeta(path)) {
		string command = "exec rm -r '" + path + "'";
		system(command.c_str());
		Logger logger;
		logger.loggear("Se elimino permanentemente la carpeta " + path + ".", TRACE);
		return true;
	} else {
		Logger::logWarn("No se ha podido eliminar la carpeta " + path + " porque no existe.");
		return false;
	}
}

bool ManejadorArchivosYMetadatos::tamanioCarpeta (string path, unsigned long int & size) {
	string pathConFS = this->pathFileSystem + "/" + path;
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(pathConFS.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			string dirName(ent->d_name);
			if (dirName == "." or dirName == "..")
				continue;

			string pathInterno = path + "/" + ent->d_name;
			string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;
			if (validador.existeCarpeta(pathInternoConFS))
				this->tamanioCarpeta(pathInterno, size);
			else {
				struct stat buffer;
				if (stat(pathInternoConFS.c_str(), &buffer) == 0)
					size += buffer.st_size;
				else
					Logger::logWarn("No se pudo encontrar el archivo " + pathInterno);
			}
		}
		closedir(dir);
		return true;
	}
	Logger::logWarn("No existe el directorio " + path);
	return false;
}

string ManejadorArchivosYMetadatos::actualizarUsuarioFechaModificacion (string jsonMetadatos,
		string usernameModificacion) {
	ParserJson parser;
	MetadatoArchivo metadato = parser.deserializarMetadatoArchivo(jsonMetadatos);
	metadato.usuarioUltimaModificacion = usernameModificacion;

	struct tm *tiempo;
	time_t fecha_sistema;
	time(&fecha_sistema);
	tiempo = localtime(&fecha_sistema);
	int anio = tiempo->tm_year + 1900;
	int mes = tiempo->tm_mon + 1;
	int dia = tiempo->tm_mday;
	string fecha = to_string(dia) + "/" + to_string(mes) + "/" + to_string(anio);
	metadato.fechaUltimaModificacion = fecha;
	string nuevosMetadatos = parser.serializarMetadatoArchivo(metadato);
	return nuevosMetadatos;
}

bool ManejadorArchivosYMetadatos::deleteFileSystem () {
	return this->deleteCarpeta(this->pathFileSystem);
}
