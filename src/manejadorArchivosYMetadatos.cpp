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
		ManejadorArchivosYMetadatos(dbMetadatos, DEFAULT_FS) {
}

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos (BD* dbMetadatos, string path) : validador(dbMetadatos) {
	this->dbMetadatos = dbMetadatos;
	this->pathFileSystem = path;
	getcwd(homeDirectory, sizeof(homeDirectory));
}

ManejadorArchivosYMetadatos::~ManejadorArchivosYMetadatos () {
}

bool ManejadorArchivosYMetadatos::eliminar (string username, string path) {
	if (not validador.verificarPermisos(username, path))
		return false;

	string pathConFS = this->pathFileSystem + "/" + path;

	if (validador.existeCarpeta(pathConFS))
		return this->eliminarCarpeta(username, path);

	if (validador.existeArchivo(pathConFS))
		return this->eliminarArchivo(username, path);

	return false;
}

bool ManejadorArchivosYMetadatos::restaurarMetadatos (const string& pathEnPapeleraSinFS, const string& username,
		const string& pathRealSinFS) {
	string metadatosJson = dbMetadatos->get(pathEnPapeleraSinFS);
	string nuevosMetadatosJson = this->actualizarUsuarioFechaModificacion(metadatosJson, username);
	Batch batch;
	batch.erase(pathEnPapeleraSinFS);
	batch.put(pathRealSinFS, nuevosMetadatosJson);
	return dbMetadatos->writeBatch(batch);
}

bool ManejadorArchivosYMetadatos::restaurar(string username, string pathEnPapeleraSinFS) {
	if (not validador.verificarPermisos(username, pathEnPapeleraSinFS))	// username igual a la primera parte de la URI
		return false;

	string pathRealConHashYSecuencia = ParserURI::parsear(pathEnPapeleraSinFS, '/').back();
	vector<string> partes = ParserURI::parsear(pathRealConHashYSecuencia, RESERVED_CHAR);
	string pathRealSinFS = username + "/" + ParserURI::join(partes, '/', 0, partes.size()-1);	// descarta el numero de secuencia

	if (not validador.puedoRestaurarA(pathEnPapeleraSinFS, pathRealSinFS, pathFileSystem))
		return false;

	string pathRealConFS = pathFileSystem + "/" + pathRealSinFS;
	string pathEnPapeleraConFS = pathFileSystem + "/" + pathEnPapeleraSinFS;

	if (rename(pathEnPapeleraConFS.c_str(), pathRealConFS.c_str()) != 0) {
		Logger::logWarn("La restauracion del archivo " + pathEnPapeleraSinFS + " no fue correcta.");
		return false;
	}

	Logger::logInfo("La restauracion del archivo " + pathEnPapeleraSinFS + " fue correcta.");

	if (restaurarMetadatos(pathEnPapeleraSinFS, username, pathRealSinFS))
		return true;

	rename(pathRealConFS.c_str(), pathEnPapeleraConFS.c_str());	// deshace la eliminacion
	Logger::logWarn("No se ha podido escribir el batch de eliminacion del archivo "
			+ pathEnPapeleraSinFS + ", por lo que no fue restaurado");
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
			if (not tienePermisos)
				return false; // Si no existia la carpeta y no tiene permisos para crear

			mkdir(directorioAcumulado.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			Logger::logInfo("La carpeta " + directorio + " no existe dentro de " + directorioPadre
								+ " por lo que ha sido creada.");
		}
	}
	string directorioFotos = this->pathFileSystem + "/" + FOTOS;
	if (not validador.existeCarpeta(directorioFotos)) {
		mkdir(directorioFotos.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		Logger::logInfo("La carpeta " + FOTOS + " no existe dentro de " + this->pathFileSystem
							+ " por lo que ha sido creada.");
	}

	return true;
}

bool ManejadorArchivosYMetadatos::crearCarpetaSegura (string username, string path) {
	if (validador.esPathValido(path))
		return this->crearCarpeta(username, path);
	return false;
}

bool ManejadorArchivosYMetadatos::agregarPermisosABD (string username) {
	string key = PERMISOS + "/" + username;
	return this->dbMetadatos->put(key, ""); // PUT devuelve false si ya existia la clave, true si la agrega
}

bool ManejadorArchivosYMetadatos::crearUsuario (string username) {
	//Creo tanto la carpeta del username como su papelera y su foto de perfil por default
	string pathTrash = username + "/" + TRASH;
	bool agregoPermisos = this->agregarPermisosABD(username);
	if (not agregoPermisos) return false;
	bool creoCarpeta = this->crearCarpeta(username, pathTrash);
//	return (agregoPermisos and this->crearCarpeta(username, pathTrash));
	string filepathConFS = this->pathFileSystem + "/" + FOTOS + "/" + username + ".jpg";
	string command = "exec cp '" + PATH_DEFAULT_FOTO_PERFIL + "' '" + filepathConFS + "'";
	system(command.c_str());
	Logger::logInfo("Se copio la imagen default para el usuario " + username);
	return creoCarpeta;
}

//Borrara todos los archivos de la carpeta y, en caso de que quede vacia, la carpeta fisica del fileSystem tambien
bool ManejadorArchivosYMetadatos::eliminarCarpeta (string username, string path) {
	string pathConFS = this->pathFileSystem + "/" + path;
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(pathConFS.c_str())) == NULL) {
		Logger::logWarn("No existe el directorio " + path);
		return false;
	}

	while ((ent = readdir(dir)) != NULL) {
		string dirName(ent->d_name);
		if (dirName == "." or dirName == "..")
			continue;

		string pathInterno = path + "/" + dirName;
		string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;

		this->eliminar(username, pathInterno);
	}
	closedir(dir);

	if (validador.carpetaVacia(pathConFS))
		return this->deleteCarpeta(pathConFS);
	else
		return false;
}

bool ManejadorArchivosYMetadatos::actualizarFotoPerfil(string filepathViejo, string filepathNuevo, const char* data, int dataLen) {
	string filepathViejoConFS = this->pathFileSystem + "/" + filepathViejo;
	string command = "exec rm '" + filepathViejoConFS + "'";
	system(command.c_str());
	Logger::logInfo("Se borro definitivamente el archivo " + filepathViejo);

	string filepathNuevoConFS = this->pathFileSystem + "/" + filepathNuevo;
	ofstream outFile(filepathNuevoConFS, ofstream::binary);
	outFile.write(data, dataLen);
	outFile.close();

	if ( filepathViejo != filepathNuevo ) return true;
	return false;
}

// En la base de datos se guarda el path sin la carpeta del FS
bool ManejadorArchivosYMetadatos::subirArchivo (string username, string filepath, const char* data, int dataLen, string jsonMetadatos, int cuota) {
	if (not validador.verificarPermisos(username, filepath))
		return false;

	string filepathCompleto = this->pathFileSystem + "/" + filepath;
	if (validador.existeArchivo(filepathCompleto)) {
		// TODO: Versionado
		return false;
	}
	if (dbMetadatos->contains(filepath)) {
		Logger::logWarn("Se quiso subir el archivo " + filepath + " pero este ya existe. Debe utilizar el metodo actualizarArchivo.");
		return false;
	}
	if (not this->actualizarArchivo(username, filepath, data, dataLen, cuota))
		return false;
	return dbMetadatos->put(filepath, jsonMetadatos);
}

bool ManejadorArchivosYMetadatos::guardarArchivo (const string& filepath, const string& username, const char* data, int dataLen) {
	vector<string> directorios = ParserURI::parsear(filepath, '/');
	string pathSinArchivo = ParserURI::join(directorios, '/', 0, directorios.size() - 1);

	//Verifico que existan todas las carpetas y sino las creo
	if (pathSinArchivo != "" and not crearCarpetaSegura(username, pathSinArchivo)) {
		Logger::logWarn("Al querer actualizar el archivo " + filepath + " no se pudieron crear las carpetas.");
		return false;
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
bool ManejadorArchivosYMetadatos::actualizarArchivo (string username, string filepath, const char* data, int dataLen, int cuota) {
	//No le agrego el FileSystem porque se agrega despues en el metodo crearCarpeta
	if (not validador.verificarPermisos(username, filepath))
		return false;

	unsigned long int folderSize = 0;
	if (tamanioCarpeta(username, folderSize)) {
		int cuotaBytes = cuota * 1024 * 1024;
		if (folderSize + dataLen <= cuotaBytes) { //TODO: Restar el tamanio del archivo viejo
			return guardarArchivo(filepath, username, data, dataLen);
		}
		Logger::logWarn("No se ha podido subir el archivo " + filepath + " debido a que se ha superado la cuota de " + to_string(cuota) + " MB.");
	}
	return false;
}

string ManejadorArchivosYMetadatos::consultarMetadatosArchivo (string username, string filepath) {
	if (not validador.verificarPermisos(username, filepath))
		return "";

	if (not dbMetadatos->contains(filepath)) {
		Logger::logWarn("Se quiso consultar los metadatos del archivo " + filepath + " pero este no existe.");
		return "";
	}
	return dbMetadatos->get(filepath);
}

string ManejadorArchivosYMetadatos::pathConNuevoFilename (const string& pathInterno, const string& nuevoFilename) {
	vector<string> partes = ParserURI::parsear(pathInterno, '/');
	partes.pop_back();
	partes.push_back(nuevoFilename);
	string nuevoPathInterno = ParserURI::join(partes, '/');
	return nuevoPathInterno;
}

bool ManejadorArchivosYMetadatos::renombrar(const string& pathInterno, const string& nuevoFilename) {
	string nuevoPathInterno = pathConNuevoFilename(pathInterno, nuevoFilename);
	string pathConFS = pathFileSystem + "/" + pathInterno;
	string nuevoPathConFS = pathFileSystem + "/" + nuevoPathInterno;

	if (validador.existeArchivo(nuevoPathConFS) or dbMetadatos->contains(nuevoPathInterno)) {
		Logger::logWarn("No se renombro el archivo " + pathInterno + " a " + nuevoFilename
				+ " porque ya existe un archivo allí con ese nombre en el FileSystem o la base de datos.");
		return false;
	}

	if (rename(pathConFS.c_str(), nuevoPathConFS.c_str())) {
		Logger::logError("Fallo el renombrado del archivo " + pathInterno + " a " + nuevoFilename);
		return false;
	}

	Logger::logInfo("Se renombro correctamente " + pathInterno + " a " + nuevoFilename);
	return true;
}

string ManejadorArchivosYMetadatos::actualizarPermisosMetadato (const MetadatoArchivo& metadatosViejos,
		MetadatoArchivo metadatosNuevos, const string& username, const string& filepath,
		const string& jsonNuevosMetadatos) {
	list<string> usuariosViejos = metadatosViejos.usuariosHabilitados;
	list<string> usuariosNuevos = metadatosNuevos.usuariosHabilitados;

	if (usuariosNuevos.empty()) {
		metadatosNuevos.usuariosHabilitados = usuariosViejos;
		return ParserJson::serializarMetadatoArchivo(metadatosNuevos);
	}
	list<string>::iterator itUsuNuevos = usuariosNuevos.begin();
	for (; itUsuNuevos != usuariosNuevos.end(); itUsuNuevos++) {
		string nuevoUsuario = (*itUsuNuevos);
		// Si todavia no tenia permisos
		if (find(usuariosViejos.begin(), usuariosViejos.end(), nuevoUsuario) == usuariosViejos.end()) {
			this->agregarPermiso(username, filepath, nuevoUsuario);
		}
	}
	// Si tenia permisos y ahora no los tiene, eliminarPermiso
	list<string>::iterator itUsuViejos = usuariosViejos.begin();
	for (; itUsuViejos != usuariosViejos.end(); itUsuViejos++) {
		string viejoUsuario = (*itUsuViejos);
		// Si tenia permisos y ahora ya no
		if (find(usuariosNuevos.begin(), usuariosNuevos.end(), viejoUsuario) == usuariosNuevos.end()) {
			this->eliminarPermiso(username, filepath, viejoUsuario);
		}
	}
	return jsonNuevosMetadatos;
}

bool ManejadorArchivosYMetadatos::actualizarMetadatosChequeados (const string& filepath,
		const string& jsonNuevosMetadatos, const string& username) {

	string jsonMetadatosViejos = dbMetadatos->get(filepath);
	MetadatoArchivo metadatosViejos = ParserJson::deserializarMetadatoArchivo(jsonMetadatosViejos);
	MetadatoArchivo metadatosNuevos = ParserJson::deserializarMetadatoArchivo(jsonNuevosMetadatos);

	string nuevoJson = actualizarPermisosMetadato(metadatosViejos, metadatosNuevos, username, filepath,
			jsonNuevosMetadatos);	// actualiza quienes tienen permiso

	bool deboRenombrar = metadatosViejos.nombre != metadatosNuevos.nombre or metadatosViejos.extension != metadatosNuevos.extension;
	if (not deboRenombrar) {
		dbMetadatos->modify(filepath, nuevoJson);
		return true;
	}

	string nuevoFilename = metadatosNuevos.nombre + "." + metadatosNuevos.extension;
	if (not renombrar(filepath, nuevoFilename)) {
		Logger::logError("Metadatos que pinchan el renombrado: \nViejos: "
				+ jsonMetadatosViejos +  "\nNuevos: " + jsonNuevosMetadatos);
		return false;
	}
	string nuevoFilepath = pathConNuevoFilename(filepath, nuevoFilename);
	list<string> usuariosHabilitados = (metadatosNuevos.usuariosHabilitados.empty()) ?
			metadatosViejos.usuariosHabilitados : metadatosNuevos.usuariosHabilitados;
	actualizarPermisosPathArchivo(filepath, nuevoFilepath, usuariosHabilitados);	// actualiza el path del archivo en quienes tienen permiso
	Batch batch;
	batch.erase(filepath);
	batch.put(nuevoFilepath, nuevoJson);
	dbMetadatos->writeBatch(batch);
	return true;
}

// Lo que se va hacer es lo siguiente: Los nuevos metadatos vendran integramente por lo que se va a cambiar salvo por los usuarios habilitados.
// En ese caso, puedo recibir ese campo vacio o lleno:
// - Vacio significa que no se modificaron los usuarios
// - Lleno significa que esos son los nuevos usuarios habilitados -> En este caso me tengo que fijar los que ya habia comparados con los nuevos
//																	 a ver cual es el/los nuevo/s usuarios a dar permisos.
// Por ahora no se pueden borrar permisos.
bool ManejadorArchivosYMetadatos::actualizarMetadatos (string username, string filepath, string jsonNuevosMetadatos) {
	if (not validador.verificarPermisos(username, filepath))
		return false;

	if (not dbMetadatos->contains(filepath)) {
		Logger::logWarn("Se quiso actualizar los metadatos del archivo " + filepath + " pero este no existe.");
		return false;
	}
	return actualizarMetadatosChequeados(filepath, jsonNuevosMetadatos, username);
}

bool ManejadorArchivosYMetadatos::agregarPermiso (string usernameOrigen, string filepath, string usernameDestino) {
	if (not validador.verificarPermisos(usernameOrigen, filepath))
		return false;
	if (not dbMetadatos->contains(filepath)) {
		Logger::logWarn("Se quiso agregar un permiso al archivo " + filepath + " pero este no existe.");
		return false;
	}
	string pathPermisos = PERMISOS + "/" + usernameDestino;
	if (not dbMetadatos->contains(pathPermisos)) {
		Logger::logWarn("Se quiso agregar un permiso al usuario " + usernameDestino + " pero este no existe.");
		return false;
	}
	string jsonArchivo = dbMetadatos->get(filepath);

	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchivo);
	metadato.usuariosHabilitados.push_back(usernameDestino);
	string jsonModificado = ParserJson::serializarMetadatoArchivo(metadato);

	Batch batch;
	batch.modify(filepath, jsonModificado);
//		Como este metodo se llama directamente desde actualizarMetadatos, ya la fecha viene modificada

	string archivosPermitidos = dbMetadatos->get(pathPermisos);
	if (archivosPermitidos != "")
		archivosPermitidos += RESERVED_CHAR;
	archivosPermitidos += filepath;
	batch.modify(pathPermisos, archivosPermitidos);
	dbMetadatos->writeBatch(batch);

	return true;
}

string ManejadorArchivosYMetadatos::metadatosConPermisosDepurados (const string& filepath, const string& usernameDestino) {
	string jsonArchivo = dbMetadatos->get(filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchivo);
	list<string> usuariosHabilitados = metadato.usuariosHabilitados;
	if (find(usuariosHabilitados.begin(), usuariosHabilitados.end(), usernameDestino) != usuariosHabilitados.end())
		usuariosHabilitados.remove(usernameDestino);

	return ParserJson::serializarMetadatoArchivo(metadato);
}

// Como este metodo se llama directamente desde actualizarMetadatos, ya la fecha viene modificada
string ManejadorArchivosYMetadatos::jsonArchivosPermitidos (const string& pathPermisos, const string& filepath) {
	vector<string> vecArchivosPermitidos = ParserURI::parsear(dbMetadatos->get(pathPermisos), RESERVED_CHAR);
	vecArchivosPermitidos.erase(remove(vecArchivosPermitidos.begin(), vecArchivosPermitidos.end(), filepath),
			vecArchivosPermitidos.end());
	return ParserURI::join(vecArchivosPermitidos, RESERVED_CHAR);
}

bool ManejadorArchivosYMetadatos::eliminarPermiso(string usernameOrigen, string filepath, string usernameDestino) {
	if (not validador.verificarPermisos(usernameOrigen, filepath))
		return false;
	if (not dbMetadatos->contains(filepath)) {
		Logger::logWarn("Se quiso eliminar un permiso del archivo " + filepath + " pero este no existe.");
		return false;
	}
	string pathPermisos = PERMISOS + "/" + usernameDestino;
	if (not dbMetadatos->contains(pathPermisos)) {
		Logger::logWarn("Se quiso eliminar un permiso del usuario " + usernameDestino + " pero este no existe.");
		return false;
	}

	string jsonModificado = metadatosConPermisosDepurados(filepath, usernameDestino);
	string archivosPermitidos = jsonArchivosPermitidos(pathPermisos, filepath);

	Batch batch;
	batch.modify(filepath, jsonModificado);
	batch.modify(pathPermisos, archivosPermitidos);
	dbMetadatos->writeBatch(batch);

	return true;
}

Batch ManejadorArchivosYMetadatos::armarBatchEliminarArchivo (const string& jsonMetadatos, const string& username,
		const string& filepath, const string& pathCompletoPapelera) {
	Batch batch;
	string jsonMetadatosConFechaModif = this->actualizarUsuarioFechaModificacion(jsonMetadatos, username);
	MetadatoArchivo metadatoConFechaModif = ParserJson::deserializarMetadatoArchivo(jsonMetadatosConFechaModif);
	list<string> usuariosHabilitados = metadatoConFechaModif.usuariosHabilitados;
	list<string>::iterator itUsu = usuariosHabilitados.begin();

	for (; itUsu != usuariosHabilitados.end(); itUsu++) {
		string usuario = (*itUsu);
		if (usuario == metadatoConFechaModif.propietario)
			continue;

		string permisosUsuario = PERMISOS + "/" + usuario;
		if (dbMetadatos->contains(permisosUsuario)) {
			string archivosStr = dbMetadatos->get(permisosUsuario);
			vector<string> archivos = ParserURI::parsear(archivosStr, RESERVED_CHAR);
			archivos.erase(remove(archivos.begin(), archivos.end(), filepath), archivos.end());
			string joined = ParserURI::join(archivos, RESERVED_CHAR);
			batch.modify(permisosUsuario, joined);
		}
	}

	batch.erase(filepath);
	batch.put(pathCompletoPapelera, jsonMetadatosConFechaModif);
	return batch;
}

// Lo que se hace es moverlo a la papelera y cambiar el key de los metadatos por ese
// Manda el archivo a la papelera del propietario y borra todos los permisos de todos, salvo el del propietario
bool ManejadorArchivosYMetadatos::eliminarArchivo (string username, string filepath) {
	if (not validador.verificarPermisos(username, filepath))
		return false;

	string filepathCompleto = this->pathFileSystem + "/" + filepath;

	if (not validador.existeArchivo(filepathCompleto)) {
		Logger::logWarn("Se quiso eliminar el archivo " + filepath + " pero este no existe en el filesystem.");
		return false;
	}
	if (not dbMetadatos->contains(filepath)) {
		Logger::logWarn("Se quiso eliminar el archivo " + filepath + " pero este no existe en la base de datos.");
		return false;
	}

	if (TRASH == ParserURI::parsear(filepath, '/')[1]) {
		this->eliminarArchivoDefinitivamente(filepath);
		return true;
	} else {
		return this->mandarArchivoATrash(username, filepath);
	}
}

bool ManejadorArchivosYMetadatos::mandarArchivoATrash(string username, string filepath) {
	string filepathCompleto = this->pathFileSystem + "/" + filepath;
	vector<string> directorios = ParserURI::parsear(filepath, '/');
	string jsonMetadatos = dbMetadatos->get(filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadatos);
	string pathSinUsernameConHash = ParserURI::join(directorios, RESERVED_CHAR, 1, directorios.size());

	if (metadato.propietario != directorios.front()) {	// DEBUG: no deberia suceder nunca
		cout << "El propietario del metadato no coincide con la carpeta del FileSystem. EXIT." << endl;
		Logger::logError("El propietario del metadato no coincide con la carpeta del FileSystem. EXIT.");
		exit(1);
	}

	string pathCompletoPapelera = metadato.propietario + "/" + TRASH + "/" + pathSinUsernameConHash;
	string nroSecuencia = this->validador.obtenerNumeroSecuencia(this->pathFileSystem, metadato.propietario, pathSinUsernameConHash);
	pathCompletoPapelera += RESERVED_CHAR + nroSecuencia;
	string pathCompletoPapeleraConFS = this->pathFileSystem + "/" + pathCompletoPapelera;

	if (rename(filepathCompleto.c_str(), pathCompletoPapeleraConFS.c_str())) {
		Logger::logWarn("La eliminacion del archivo " + filepath + " no fue correcta.");
		return false;
	}
	Logger::logInfo("La eliminacion del archivo " + filepath + " fue correcta.");

	Batch batch = armarBatchEliminarArchivo(jsonMetadatos, username, filepath, pathCompletoPapelera);

	if (this->dbMetadatos->writeBatch(batch))
		return true;

	Logger::logWarn("No se ha podido escribir el batch de eliminacion del archivo " + filepath + ".");
	return false;
}

string ManejadorArchivosYMetadatos::obtenerEstructuraCompartidos(string path) {
	if ( not dbMetadatos->contains(path) )
		return "";
	string compartidosConUsuario = dbMetadatos->get(path);
	Logger::logDebug("Compartidos con " + path + " " + compartidosConUsuario + " al obtener estructura de Compartidos");
	vector<string> archivosCompartidos = ParserURI::parsear(compartidosConUsuario, RESERVED_CHAR);
	map<string, string> mapa;
	vector<string>::iterator it = archivosCompartidos.begin();
	for ( ; it != archivosCompartidos.end(); it++) {
		string archivoCompartido = (*it);
		if (not dbMetadatos->contains(archivoCompartido)) {	// debug
			Logger::logError("En la base de datos no esta el archivo compartido " + archivoCompartido);
		}
		string jsonMetadatoArchivoCompartido = dbMetadatos->get(archivoCompartido);
		MetadatoArchivo metadatoArchivoCompartido = ParserJson::deserializarMetadatoArchivo(jsonMetadatoArchivoCompartido);
		string nombre = metadatoArchivoCompartido.nombre;
		if (metadatoArchivoCompartido.extension != "none")
			nombre += "." + metadatoArchivoCompartido.extension;
		mapa.insert(pair<string, string>(archivoCompartido, nombre));
	}
	string json = ParserJson::serializarMapa(mapa);
	return json;
}

string ManejadorArchivosYMetadatos::obtenerEstructuraCarpeta (string path) {
	if ( path.find(RESERVED_STR + "permisos") != string::npos )
		return this->obtenerEstructuraCompartidos(path);
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
				//if (esRecursivo)
				//	obtenerEstructuraCarpeta(pathInterno)
				// else {
				vector<string> directorios = ParserURI::parsear(pathInterno, '/');
				int size = directorios.size();
				string foldername = directorios[size - 1];
				mapa.insert(pair<string, string>(pathInterno, foldername + "." + FOLDER));
			} else { //Es un archivo
				if (not dbMetadatos->contains(pathInterno)) {
					Logger::logWarn("Se quiso obtener los metadatos del archivo " + path + " pero este no existe.");
					return "";
				}
				string jsonMetadatos = this->dbMetadatos->get(pathInterno);
				MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadatos);
				string nombre = metadato.nombre;
				if (metadato.extension != "none")
					nombre += "." + metadato.extension;
				// if (match con lambda function)
				mapa.insert(pair<string, string>(pathInterno, nombre));
			}
		}
		closedir(dir);
		string json = ParserJson::serializarMapa(mapa);
		return json;
	} else
		Logger::logWarn("No existe el directorio " + path);
	return "";
//
}

string ManejadorArchivosYMetadatos::descargarArchivo (string username, string filepath) {
	//OJO porque si no se corre desde la carpeta build como ./udrive esto va a pinchar seguramente (Ya que la carpeta del FileSystem no va a existir)
	if (not validador.verificarPermisos(username, filepath))
		return "";
	string filepathCompleto = this->pathFileSystem + "/" + filepath;
	if (not validador.existeArchivo(filepathCompleto)) {
		Logger::logWarn("Se ha querido descargar el archivo de path " + filepath + ", el cual no existe.");
		return "";
	}
	string pathADevolver(this->homeDirectory);
	pathADevolver += "/" + filepathCompleto;
	return pathADevolver;
}

bool ManejadorArchivosYMetadatos::deleteCarpeta (string path) {
	if (not validador.existeCarpeta(path)) {
		Logger::logWarn("No se ha podido eliminar la carpeta " + path + " porque no existe.");
		return false;
	}
	string command = "exec rm -r '" + path + "'";
	system(command.c_str());
	Logger::logTrace("Se elimino permanentemente la carpeta " + path + ".");
	return true;
}

bool ManejadorArchivosYMetadatos::tamanioCarpeta (string path, unsigned long int & size) {
	string pathConFS = this->pathFileSystem + "/" + path;
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(pathConFS.c_str())) == NULL) {
		Logger::logWarn("No existe el directorio " + path);
		return false;
	}

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

string ManejadorArchivosYMetadatos::actualizarUsuarioFechaModificacion (string jsonMetadatos,
		string usernameModificacion) {

	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadatos);
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
	string nuevosMetadatos = ParserJson::serializarMetadatoArchivo(metadato);
	return nuevosMetadatos;
}

void ManejadorArchivosYMetadatos::eliminarArchivoDefinitivamente (string filepath) {
	// Las validaciones de existencia se hacen antes de llamarlo
	dbMetadatos->erase(filepath);
	Logger::logInfo("Se borraron definitivamente los metadatos del archivo " + filepath);
	string filepathConFS = this->pathFileSystem + "/" + filepath;
	string command = "exec rm '" + filepathConFS + "'";
	system(command.c_str());
	Logger::logInfo("Se borro definitivamente el archivo " + filepath);
}

void ManejadorArchivosYMetadatos::actualizarPermisosPathArchivo (const string& filepath, const string& nuevoFilepath,
		const list<string>& usuariosHabilitados) {

	list<string>::const_iterator it = usuariosHabilitados.cbegin();
	const list<string>::const_iterator END = usuariosHabilitados.cend();
	for (; it != END; it++) {
		string usuario = *it;
		string archivos = dbMetadatos->get(PERMISOS + "/" + usuario);
		Logger::logDebug("Archivos compartidos a " + usuario + ": " + archivos);
		vector<string> partes = ParserURI::parsear(archivos, RESERVED_CHAR);
		partes.erase(remove(partes.begin(), partes.end(), filepath), partes.end());
		partes.push_back(nuevoFilepath);
		string nuevosArchivos = ParserURI::join(partes, RESERVED_CHAR);
		Logger::logDebug("nuevosArchivos de " + usuario + ": " + nuevosArchivos);
		dbMetadatos->modify(PERMISOS + "/" + usuario, nuevosArchivos);
	}
}

bool ManejadorArchivosYMetadatos::deleteFileSystem () {
	return this->deleteCarpeta(this->pathFileSystem);
}

string ManejadorArchivosYMetadatos::buscarPorExtension(string username, string extension) {
	auto predicate = [&] (MetadatoArchivo& metadato) -> bool {return metadato.extension == extension;};
	return buscar(username, predicate);
}

string ManejadorArchivosYMetadatos::buscarPorEtiqueta(string username, string etiqueta) {
	auto predicate = [&] (MetadatoArchivo& metadato) -> bool {
		bool contieneEtiqueta = false;
		list<string> etiquetas = metadato.etiquetas;
		list<string>::iterator it = etiquetas.begin();
		for ( ; it != etiquetas.end() ; it++ ) {
			string etiquetaArchivo = (*it);
			if ( etiquetaArchivo == etiqueta ) {
				contieneEtiqueta = true;
				break;
			}
		}
		return contieneEtiqueta;
	};
	return buscar(username, predicate);
}

string ManejadorArchivosYMetadatos::buscarPorNombre(string username, string nombre) {
	auto predicate = [&] (MetadatoArchivo& metadato) -> bool {return (metadato.nombre.find(nombre) != string::npos);};
	return buscar(username, predicate);
}

string ManejadorArchivosYMetadatos::buscarPorPropietario(string username, string propietario) {
	if ( username != propietario ) {
		auto predicate = [&] (MetadatoArchivo& metadato) -> bool {return metadato.propietario == propietario;};
		return buscar(username, predicate);
	}
	return ""; //TODO: Deberia buscar solo en el FS
}

// Tiene sentido buscar en los archivos propios del user?
string ManejadorArchivosYMetadatos::buscar(string username, function<bool(MetadatoArchivo&)> predicate) {
	string permisos = PERMISOS + "/" + username;
	string archivosPermitidos = dbMetadatos->get(permisos);
	vector<string> archivos = ParserURI::parsear(archivosPermitidos, RESERVED_CHAR);
	map<string, string> estructura;
	vector<string>::iterator it = archivos.begin();
	for ( ; it != archivos.end(); it++){
		string archivoCompartido = (*it);
		string jsonMetadato = dbMetadatos->get(archivoCompartido);
		MetadatoArchivo metadatoArchivoCompartido = ParserJson::deserializarMetadatoArchivo(jsonMetadato);
		if ( predicate(metadatoArchivoCompartido) ) {
			string nombre = metadatoArchivoCompartido.nombre;
			if (metadatoArchivoCompartido.extension != "none")
				nombre += "." + metadatoArchivoCompartido.extension;
			estructura.insert(pair<string, string>(archivoCompartido, nombre));
		}
	}
	string json = ParserJson::serializarMapa(estructura);
	return json;
}
