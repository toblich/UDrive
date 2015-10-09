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

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos(BD* dbMetadatos): ManejadorArchivosYMetadatos(dbMetadatos, defaultFileSystem){}

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos(BD* dbMetadatos, std::string path) {
	this->dbMetadatos = dbMetadatos;
	this->pathFileSystem = path;
	getcwd(homeDirectory, sizeof(homeDirectory));
}

ManejadorArchivosYMetadatos::~ManejadorArchivosYMetadatos() {
}

void ManejadorArchivosYMetadatos::logInfo(std::string mensaje) {
	Logger logger;
	logger.loggear(mensaje,INFO);
}

void ManejadorArchivosYMetadatos::logWarn(std::string mensaje) {
	Logger logger;
	logger.loggear(mensaje,WARN);
}

void ManejadorArchivosYMetadatos::logError(std::string mensaje) {
	Logger logger;
	logger.loggear(mensaje,ERROR);
}

bool ManejadorArchivosYMetadatos::existeArchivo(std::string filepath) {
	struct stat buffer;
	return ( stat (filepath.c_str(), &buffer) == 0 );
}

bool ManejadorArchivosYMetadatos::existeCarpeta(std::string path) {
	struct stat sb;
	return ( stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode) );
}

bool ManejadorArchivosYMetadatos::carpetaVacia(std::string pathConFS) {
	DIR* dir;
	struct dirent* ent;
	bool vacia = true;
	if ( ( dir = opendir(pathConFS.c_str()) ) != NULL ) {
		while ( (ent = readdir (dir)) != NULL && vacia) {
			if ( strcmp(ent->d_name, ".") == 0 ) continue;
			if ( strcmp(ent->d_name, "..") == 0 ) continue;
			vacia = false;
		}
		closedir (dir);
	} else
		return false;
	return vacia;
}

// Una carpeta no puede contener un #
bool ManejadorArchivosYMetadatos::verificarPathValido(std::string path) {
	if ( path.find('#') == std::string::npos )
		return true;
	this->logWarn("El path " + path + " no es valido.");
	return false;
}

bool ManejadorArchivosYMetadatos::tienePermisos(std::string username, std::string path){
	std::string key = permisos + "/" + username;
	if ( this->dbMetadatos->contains(key) ) {
		std::string archivosPermitidos = this->dbMetadatos->get(key);
		std::vector<std::string> archivos = ParserURI::parsear(archivosPermitidos, '#');
		if ( std::find( archivos.begin(), archivos.end(), path ) != archivos.end() )
			return true;
		else
			return false;
	}
	return false; //Significa que no se creo el usuario porque no existe esa key basicamente. No deberia pasar.
}

bool ManejadorArchivosYMetadatos::verificarPermisos(std::string username, std::string path) {
	vector<string> directorios = ParserURI::parsear(path, '/');
	if ( directorios.size() > 0){
		std::string fileOwner = directorios[0];
		if ( username == fileOwner )
			return true;
		else {
s			if ( this->tienePermisos(username, path) ) {
				return true;
			}
			this->logWarn("El usuario " + username + " no posee los permisos para el archivo " + path + ".");
			return false;
		}
	}
	return false; //Esto no deberia pasar jamás, pero bueno
}


bool ManejadorArchivosYMetadatos::eliminar(std::string username, std::string path) {
	std::string pathConFS = this->pathFileSystem + "/" + path;
	if ( this->verificarPermisos(username, path) ) {
		if ( this->existeCarpeta(pathConFS) )
			return this->eliminarCarpeta(username, path);
		else
			return this->eliminarArchivo(username, path);
	} else
		return false;
}

std::string ManejadorArchivosYMetadatos::actualizarUsuarioFechaModificacion(std::string jsonMetadatos,
		std::string usernameModificacion) {
	ParserJson parser;
	MetadatoArchivo metadato = parser.deserializarMetadatoArchivo(jsonMetadatos);
	metadato.usuarioUltimaModificacion=usernameModificacion;

	struct tm *tiempo;
	time_t fecha_sistema;
	time(&fecha_sistema);
	tiempo = localtime(&fecha_sistema);
	int anio = tiempo->tm_year + 1900;
	int mes = tiempo->tm_mon + 1;
	int dia = tiempo->tm_mday;
	std::string fecha = std::to_string(dia) + "/" + std::to_string(mes) + "/" + std::to_string(anio);
	metadato.fechaUltimaModificacion = fecha;
	std::string nuevosMetadatos = parser.serializarMetadatoArchivo(metadato);
	return nuevosMetadatos;
}

// El path recibido no debe contener el nombre de un archivo.
// En caso de que sea asi, se debera modificar este metodo.
bool ManejadorArchivosYMetadatos::crearCarpeta(std::string username, std::string path) {
	if ( verificarPermisos(username, path) ) {
		// Agrego el FileSystem para que sea la "raiz"
		string pathCompletoConFS = this->pathFileSystem + "/" + path;
		vector<string> directorios = ParserURI::parsear(pathCompletoConFS, '/');
		std::string directorioAcumulado = "";
		int size = directorios.size();
		for (int i = 0; i < size; i++){
			std::string directorio = directorios[i];
			std::string directorioPadre = directorioAcumulado;
			directorioAcumulado += (directorio + "/");
			// Me fijo si existe la carpeta, sino la creo
			if (not existeCarpeta(directorioAcumulado) ) {
				mkdir(directorioAcumulado.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				this->logInfo("La carpeta " + directorio + " no existe dentro de " + directorioPadre +" por lo que ha sido creada.");
			}
		}
		return true;
	} else
		return false;
}

bool ManejadorArchivosYMetadatos::crearCarpetaSegura(std::string username, std::string path) {
	if ( this->verificarPathValido(path) ) {
		return this->crearCarpeta(username, path);
	} else
		return false;
}

bool ManejadorArchivosYMetadatos::agregarPermisosABD(std::string username) {
	std::string key = permisos + "/" + username;
	if ( not this->dbMetadatos->contains(key) ) {
		this->dbMetadatos->put(key,"");
		return true;
	}
	return false; // No deberia pasar nunca, ya que implicaria que ya existe ese usuario
}

bool ManejadorArchivosYMetadatos::crearUsuario(std::string username) {
	//Creo tanto la carpeta del username como su papelera
	string pathTrash = username + "/" + trash;
	bool agregoPermisos = this->agregarPermisosABD(username);
	return ( agregoPermisos and this->crearCarpeta(username, pathTrash) );
}

//Borrara todos los archivos de la carpeta y, en caso de que quede vacia, la carpeta fisica del fileSystem tambien
bool ManejadorArchivosYMetadatos::eliminarCarpeta(std::string username, std::string path) {
	std::string pathConFS = this->pathFileSystem + "/" + path;
	DIR* dir;
	struct dirent* ent;
	if ( ( dir = opendir(pathConFS.c_str()) ) != NULL ) {
		while ( (ent = readdir (dir)) != NULL ) {
			if ( strcmp(ent->d_name, ".") == 0 ) continue;
			if ( strcmp(ent->d_name, "..") == 0 ) continue;
			std::string pathInterno = path + "/" + ent->d_name;
			std::string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;
			if ( this->existeCarpeta(pathInternoConFS) )
				this->eliminarCarpeta(username, pathInterno);
			else
				this->eliminarArchivo(username, pathInterno);
		}
		closedir (dir);
		if ( this->carpetaVacia(pathConFS) )
			return this->deleteCarpeta(pathConFS);
	} else
		this->logWarn("No existe el directorio " + path);
	return false;
}

// OJO porque el put tira excepciones
// En la base de datos se guarda el path sin la carpeta del FS
bool ManejadorArchivosYMetadatos::subirArchivo(std::string username,
		std::string filepath, const char* data, int dataLen, std::string jsonMetadatos) {
	if ( verificarPermisos(username, filepath) ) {
		std::string filepathCompleto = this->pathFileSystem + "/" + filepath;
		if ( not this->existeArchivo(filepathCompleto) ) {
			if ( dbMetadatos->contains(filepath) ){
				this->logWarn("Se quiso subir el archivo " + filepath + " pero este ya existe. Debe utilizar el metodo actualizarArchivo.");
				return false;
			}
			if ( not this->actualizarArchivo(username, filepath, data, dataLen) )
				return false;
			dbMetadatos->put(filepath, jsonMetadatos);
			return true;
		} else
			// TODO: Versionado
			return false;
	} else
		return false;
}

// El filename deberia venir con los path de carpetas tambien y dentro tambien el nombre de usuario
bool ManejadorArchivosYMetadatos::actualizarArchivo(std::string username,
		std::string filepath, const char* data, int dataLen) {
	//No le agrego el FileSystem porque se agrega despues en el metodo crearCarpeta
	if ( verificarPermisos(username, filepath) ) {
		unsigned long int folderSize = 0;
		if ( tamanioCarpeta(username, folderSize) ) {
			if ( folderSize + dataLen <= CUOTA ) {
				vector<string> directorios = ParserURI::parsear(filepath, '/');
				int size = directorios.size();
				std::string pathSinArchivo = "";
				for (int i = 0; i < size-1; i++){ //Saco el nombre del archivo
					std::string directorio = directorios[i];
					pathSinArchivo += directorio;
					if (i < size-2) pathSinArchivo += "/";
				}
				//Verifico que existan todas las carpetas y sino las creo
				if (pathSinArchivo != "") {
					if ( not crearCarpetaSegura(username, pathSinArchivo) ) {
						this->logWarn("Al querer actualizar el archivo " + filepath + " no se pudieron crear las carpetas.");
						return false ;
					}
				}
				std::string pathConFileSystem = this->pathFileSystem + "/" + filepath;

				if ( dbMetadatos->contains(filepath) ){ //Significa que no fui llamado desde el subirArchivo, por lo que la actualizacion se hará ahí
					string metadatos = dbMetadatos->get(filepath);
					string nuevosMetadatos = this->actualizarUsuarioFechaModificacion(metadatos, username);
					dbMetadatos->modify(filepath, nuevosMetadatos);
//					this->logWarn("Se quiso consultar los metadatos del archivo " + filepath + " pero este no existe.");
//					return false;
				}

				ofstream outFile(pathConFileSystem, std::ofstream::binary);
				outFile.write(data, dataLen);
				outFile.close();
				return true;
			}
			unsigned long int cuotaMB = CUOTA/1048576;
			std::string texto = "No se ha podido subir el archivo " + filepath + " debido a que se ha superado la cuota de ";// + cuotaMB + " MB.";
			texto += cuotaMB;
			texto += " MB.";
			this->logWarn(texto);
		}
	}
	return false;
}

// OJO porque el get tira excepciones
std::string ManejadorArchivosYMetadatos::consultarMetadatosArchivo(std::string username, std::string filepath) {
	if ( verificarPermisos(username, filepath) ) {
		if ( not dbMetadatos->contains(filepath) ){
			this->logWarn("Se quiso consultar los metadatos del archivo " + filepath + " pero este no existe.");
			return "";
		}
		return dbMetadatos->get(filepath);
	}
	return "";
}

// OJO porque el modify tira excepciones
// Lo que se va hacer es lo siguiente: Los nuevos metadatos vendran integramente por lo que se va a cambiar salvo por los usuarios habilitados.
// En ese caso, puedo recibir ese campo vacio o lleno:
// - Vacio significa que no se modificaron los usuarios
// - Lleno significa que esos son los nuevos usuarios habilitados -> En este caso me tengo que fijar los que ya habia comparados con los nuevos
//																	 a ver cual es el/los nuevo/s usuarios a dar permisos.
// Por ahora no se pueden borrar permisos.
bool ManejadorArchivosYMetadatos::actualizarMetadatos(std::string username,
		std::string filepath, std::string jsonNuevosMetadatos) {
	if ( verificarPermisos(username, filepath) ) {
		if ( not dbMetadatos->contains(filepath) ){
			this->logWarn("Se quiso actualizar los metadatos del archivo " + filepath + " pero este no existe.");
			return false;
		}
		ParserJson parser;
		std::string jsonMetadatosViejos = dbMetadatos->get(filepath);
		MetadatoArchivo metadatosViejos = parser.deserializarMetadatoArchivo(jsonMetadatosViejos);
		MetadatoArchivo metadatosNuevos = parser.deserializarMetadatoArchivo(jsonNuevosMetadatos);
		std::list<std::string> usuariosViejos =  metadatosViejos.usuariosHabilitados;
		std::list<std::string> usuariosNuevos =  metadatosNuevos.usuariosHabilitados;
		std::string nuevoJson;

		if ( usuariosNuevos.empty() ){
			metadatosNuevos.usuariosHabilitados = usuariosViejos;
			nuevoJson = parser.serializarMetadatoArchivo(metadatosNuevos);
		} else {
			std::list<string>::iterator itUsuNuevos = usuariosNuevos.begin();
			for ( ; itUsuNuevos != usuariosNuevos.end() ; itUsuNuevos++){
				std::string nuevoUsuario = (*itUsuNuevos);
				// Si todavia no tenia permisos
				if ( std::find( usuariosViejos.begin(), usuariosViejos.end(), nuevoUsuario ) == usuariosViejos.end() ) {
					this->agregarPermiso(username, filepath, nuevoUsuario);
				}
			}
			nuevoJson = jsonNuevosMetadatos;
		}

		dbMetadatos->modify(filepath, nuevoJson);
		return true;
	} else
		return false;
}

// OJO porque el get y modify tiran excepciones
bool ManejadorArchivosYMetadatos::agregarPermiso(std::string usernameOrigen,
		std::string filepath, std::string usernameDestino) {
	if ( verificarPermisos(usernameOrigen, filepath) ) {
		if ( not dbMetadatos->contains(filepath) ){
			this->logWarn("Se quiso agregar un permiso al archivo " + filepath + " pero este no existe.");
			return false;
		}
		std::string pathPermisos = permisos + "/" + usernameDestino;
		if ( not dbMetadatos->contains(pathPermisos) ) {
			this->logWarn("Se quiso agregar un permiso al usuario " + usernameDestino + " pero este no existe.");
			return false;
		}
		std::string jsonArchivo = dbMetadatos->get(filepath);
		ParserJson parser;
		MetadatoArchivo metadato = parser.deserializarMetadatoArchivo(jsonArchivo);
		metadato.usuariosHabilitados.push_back(usernameDestino);
		std::string jsonModificado = parser.serializarMetadatoArchivo(metadato);
		dbMetadatos->modify(filepath, jsonModificado);
//		Como este metodo se llama directamente desde actualizarMetadatos, ya la fecha viene modificada
//		std::string nuevosMetadatos = this->actualizarUsuarioFechaModificacion(jsonModificado, usernameOrigen);
//		dbMetadatos->modify(filepath, nuevosMetadatos);

		std::string archivosPermitidos = dbMetadatos->get(pathPermisos);
		if ( archivosPermitidos != "" )
			archivosPermitidos += "#";
		archivosPermitidos += filepath;
		dbMetadatos->modify(pathPermisos, archivosPermitidos);

		return true;
	} else
		return false;
}

// OJO porque el get tira excepciones
// Lo que se hace es moverlo a la papelera y cambiar el key de los metadatos por ese
// TODO: Mandar el archivo a la papelera del propietario y borrar todos los permisos de todos, salvo del propietario
bool ManejadorArchivosYMetadatos::eliminarArchivo(std::string username, std::string filepath) {
	if ( verificarPermisos(username, filepath) ) {
		std::string filepathCompleto = this->pathFileSystem + "/" + filepath;
		if ( this->existeArchivo(filepathCompleto) ) {
			if ( not dbMetadatos->contains(filepath) ){
				this->logWarn("Se quiso eliminar el archivo " + filepath + " pero este no existe en la base de datos.");
				return false;
			}
			vector<string> directorios = ParserURI::parsear(filepath, '/');
			int size = directorios.size();
			std::string filename = directorios[size-1];
			std::string pathCompletoPapelera = username + "/" + trash + "/" + filename;
			std::string pathArchivoPapelera = this->pathFileSystem + "/" + pathCompletoPapelera;

			int result = rename( filepathCompleto.c_str(), pathArchivoPapelera.c_str() );
			if ( result == 0 ) {
				this->logInfo("La eliminacion del archivo " + filepath + " fue correcta.");
				Batch batch;
				string metadatos = dbMetadatos->get(filepath);
				string nuevosMetadatos = this->actualizarUsuarioFechaModificacion(metadatos, username);
				batch.modify(filepath, nuevosMetadatos);
				batch.erase(filepath);
				batch.put(pathCompletoPapelera, nuevosMetadatos);
				// TODO: Tener en cuenta que hay que cambiar en #permisos
				if ( this->dbMetadatos->writeBatch(batch) )
					return true;
				this->logWarn("No se ha podido escribir el batch de eliminacion del archivo " + filepath + ".");
				return false;
			} else {
				this->logWarn("La eliminacion del archivo " + filepath + " no fue correcta.");
				return false;
			}
		} else
			return false;
	} else
		return false;
}

std::string ManejadorArchivosYMetadatos::obtenerEstructuraCarpeta(std::string path){
	std::string pathCompleto = this->pathFileSystem + "/" + path;
	std::map<std::string, std::string> mapa;
	DIR* dir;
	struct dirent* ent;
	if ( ( dir = opendir(pathCompleto.c_str()) ) != NULL ) {
		ParserJson parser;
		while ( (ent = readdir (dir)) != NULL ) {
			if ( strcmp(ent->d_name, ".") == 0 ) continue;
			if ( strcmp(ent->d_name, "..") == 0 ) continue;
			std::string pathInterno = path + "/" + ent->d_name;
			std::string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;
			if ( this->existeCarpeta(pathInternoConFS) ){
				vector<string> directorios = ParserURI::parsear(pathInterno, '/');
				int size = directorios.size();
				std::string foldername = directorios[size-1];
				mapa.insert(pair<string, string>(foldername, "#folder"));
			} else { //Es un archivo
				if ( not dbMetadatos->contains(pathInterno) ){
					this->logWarn("Se quiso obtener los metadatos del archivo " + path + " pero este no existe.");
					return "";
				}
				std::string jsonMetadatos = this->dbMetadatos->get(pathInterno);
				MetadatoArchivo metadato = parser.deserializarMetadatoArchivo(jsonMetadatos);
				string nombre = metadato.nombre;
				if ( metadato.extension != "none" )
					nombre += "." + metadato.extension;
				mapa.insert(pair<string, string>(nombre, metadato.extension));
			}
		}
		closedir (dir);
		std::string json = parser.serializarMapa(mapa);
		return json;
	} else
		this->logWarn("No existe el directorio " + path);
	return "";
//
}

std::string ManejadorArchivosYMetadatos::descargarArchivo(std::string username, std::string filepath) {
	//OJO porque si no se corre desde la carpeta build como ./udrive esto va a pinchar seguramente (Ya que la carpeta del FileSystem no va a existir)
	if ( verificarPermisos(username, filepath) ) {
		std::string filepathCompleto = this->pathFileSystem + "/" + filepath;
		if ( this->existeArchivo(filepathCompleto) ) {
			std::string pathADevolver(this->homeDirectory);
			pathADevolver += "/" + filepathCompleto;
			return pathADevolver;
		} else
			this->logWarn("Se ha querido descargar el archivo de path " + filepath + ", el cual no existe.");
	}
	return "";
}

bool ManejadorArchivosYMetadatos::deleteCarpeta(std::string path) {
	if ( this->existeCarpeta(path) ){
		string command = "exec rm -r '" + path + "'";
		system(command.c_str());
		Logger logger;
		logger.loggear("Se elimino permanentemente la carpeta " + path + "." , TRACE);
		return true;
	} else {
		this->logWarn("No se ha podido eliminar la carpeta " + path + " porque no existe." );
		return false;
	}
}

bool ManejadorArchivosYMetadatos::tamanioCarpeta(std::string path, unsigned long int & size) {
	std::string pathConFS = this->pathFileSystem + "/" + path;
	DIR* dir;
	struct dirent* ent;
	if ( ( dir = opendir(pathConFS.c_str()) ) != NULL ) {
		while ( (ent = readdir (dir)) != NULL ) {
			if ( strcmp(ent->d_name, ".") == 0 ) continue;
			if ( strcmp(ent->d_name, "..") == 0 ) continue;
			std::string pathInterno = path + "/" + ent->d_name;
			std::string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;
			if ( this->existeCarpeta(pathInternoConFS) )
				this->tamanioCarpeta(pathInterno, size);
			else {
				struct stat buffer;
				if ( stat ( pathInternoConFS.c_str(), &buffer ) == 0 )
					size += buffer.st_size;
				else
					this->logWarn("No se pudo encontrar el archivo " + pathInterno);
			}
		}
		closedir (dir);
		return true;
	}
	this->logWarn("No existe el directorio " + path);
	return false;
}

bool ManejadorArchivosYMetadatos::deleteFileSystem() {
	return this->deleteCarpeta(this->pathFileSystem);
}
