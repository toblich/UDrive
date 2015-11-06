#include "manejadorArchivosYMetadatos.h"

// 		El username que se recibe en la mayoria de los metodos es el del que pide. Luego, el path tiene el path posta. Por ejemplo:
//		username: pablo		path: tobi/hola/asd.txt
//		Me deberia fijar si username es igual al primero del path. De ser asi, tiene los permisos seguro.
//		Sino, como en este caso, deberia fijarme si pablo tiene permisos para acceder a ese path mediante lo que habra en la DB
//		bajo la key '#permisos/pablo' y cuyo value sera algo del estilo 'pancho/datos/pepe.jpg#tobi/hola/asd.txt#santi/chau.txt'
//		lo cual habra que parsear y ver si esta o no.

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos (BD* dbMetadatos) :
		ManejadorArchivosYMetadatos(dbMetadatos, DEFAULT_FS) {
}

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos (BD* dbMetadatos, string path) :
		validador(dbMetadatos), buscador(dbMetadatos, path), manejadorArchivos(path, &validador), manejadorMetadatos(dbMetadatos, &validador) {
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

bool ManejadorArchivosYMetadatos::restaurar(string username, string pathEnPapeleraSinFS) {
	if (not validador.verificarPermisos(username, pathEnPapeleraSinFS))	// username igual a la primera parte de la URI
		return false;

	string pathRealConHashYSecuencia = ParserURI::parsear(pathEnPapeleraSinFS, '/').back();
	vector<string> partes = ParserURI::parsear(pathRealConHashYSecuencia, RESERVED_CHAR);
	string filepathRealSinFS = username + "/" + ParserURI::join(partes, '/', 0, partes.size()-1);	// descarta el numero de secuencia
	string pathRealSinFS = username + "/" + ParserURI::join(partes, '/', 0, partes.size()-2); //Solo me quedo con la/las carpeta/s

	if (not crearCarpetaSegura(username, pathRealSinFS)){
		Logger::logWarn("No se pudo crear la carpeta " + pathRealSinFS + " en la restauración del archivo");
		return false;
	}

	if (not validador.puedoRestaurarA(pathEnPapeleraSinFS, filepathRealSinFS, pathFileSystem))
		return false;

	if (not manejadorArchivos.restaurarArchivo(filepathRealSinFS, pathEnPapeleraSinFS))
		return false;

	if (manejadorMetadatos.restaurarMetadatos(pathEnPapeleraSinFS, username, filepathRealSinFS))
		return true;

	manejadorArchivos.deshacerRestaurado(filepathRealSinFS, pathEnPapeleraSinFS);
	return false;
}

bool ManejadorArchivosYMetadatos::crearCarpetaSegura (string username, string path) {
	if (validador.esPathValido(path))
		return manejadorArchivos.crearCarpeta(username, path);
	return false;
}

bool ManejadorArchivosYMetadatos::crearUsuario (string username) {
	//Creo tanto la carpeta del username como su papelera y su foto de perfil por default
	string pathTrash = username + "/" + TRASH;
	bool agregoPermisos = manejadorMetadatos.agregarPermisosABD(username);
	if (not agregoPermisos) return false;
	bool creoCarpeta = manejadorArchivos.crearCarpeta(username, pathTrash);
	manejadorArchivos.crearFotoPerfilDefault(username);
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

		if (pathInterno.find(RESERVED_CHAR) != string::npos)	// es un archivo con nro de version
			pathInterno = ParserURI::pathSinNroSecuencia(pathInterno);
		this->eliminar(username, pathInterno);
	}
	closedir(dir);

	if (validador.carpetaVacia(pathConFS))
		return manejadorArchivos.deleteCarpeta(pathConFS);
	else
		return false;
}

bool ManejadorArchivosYMetadatos::actualizarFotoPerfil(string filepathViejo, string filepathNuevo, const char* data, int dataLen) {
	manejadorArchivos.eliminarArchivoDefinitivamente(filepathViejo);
	manejadorArchivos.guardarArchivoEnFileSystem(filepathNuevo, data, dataLen);
	return ( filepathViejo != filepathNuevo );
}

// En la base de datos se guarda el path sin la carpeta del FS
bool ManejadorArchivosYMetadatos::subirArchivo (string username, string filepath, const char* data, int dataLen, string jsonMetadatos, int cuota) {
	if (not validador.verificarPermisos(username, filepath))
		return false;

	string filepathCompleto = this->pathFileSystem + "/" + filepath + RESERVED_FIRST;
	if (validador.existeArchivo(filepathCompleto)) {
		// TODO: Versionado
		return false;
	}
	if (validador.existeMetadato(filepath)) {
		Logger::logWarn("Se quiso subir el archivo " + filepath + " pero este ya existe. Debe utilizar el metodo actualizarArchivo.");
		return false;
	}
	if (not this->actualizarArchivo(username, filepath, data, dataLen, cuota, FIRST-1))
		return false;
	return manejadorMetadatos.cargarMetadato(filepath, jsonMetadatos);
}

bool ManejadorArchivosYMetadatos::guardarArchivo (const string& filepath, const string& username, const char* data,
		int dataLen, int nuevaVersion) {
	vector<string> directorios = ParserURI::parsear(filepath, '/');
	string pathSinArchivo = ParserURI::join(directorios, '/', 0, directorios.size() - 1);

	//Verifico que existan todas las carpetas y sino las creo
	if (pathSinArchivo != "" and not crearCarpetaSegura(username, pathSinArchivo)) {
		Logger::logWarn("Al querer actualizar el archivo " + filepath + " a la version " + to_string(nuevaVersion)
				+ " no se pudieron crear las carpetas.");
		return false;
	}

	if (validador.existeMetadato(filepath)) {
		//Significa que no fui llamado desde el subirArchivo, por lo que la actualizacion se hará ahí
		manejadorMetadatos.actualizarMetadatosPorActualizacionArchivo(filepath, username);
	}

	manejadorArchivos.guardarArchivoEnFileSystem(filepath + RESERVED_STR + to_string(nuevaVersion), data, dataLen);
	return true;
}

// El filename deberia venir con los path de carpetas tambien y dentro tambien el nombre de usuario
bool ManejadorArchivosYMetadatos::actualizarArchivo (string username, string filepath, const char* data, int dataLen,
		int cuota, int versionAnterior) {
	//No le agrego el FileSystem porque se agrega despues en el metodo crearCarpeta
	if (not validador.verificarPermisos(username, filepath))
		return false;

	unsigned long int folderSize = 0;
	if (manejadorArchivos.tamanioCarpeta(username, folderSize)) {
		int cuotaBytes = cuota * 1024 * 1024;
		if (folderSize + dataLen <= cuotaBytes) { //TODO: Restar el tamanio del archivo viejo
			return guardarArchivo(filepath, username, data, dataLen, versionAnterior + 1);
		}
		Logger::logWarn( "No se ha podido subir el archivo " + filepath +
				" debido a que se ha superado la cuota de " + to_string(cuota) + " MB.");
	}
	return false;
}

string ManejadorArchivosYMetadatos::consultarMetadatosArchivo (string username, string filepath) {
	if (not validador.verificarPermisos(username, filepath))
		return "";
	if (filepath.find(TRASH) != string::npos)	// Los metadatos de la papelera son inaccesibles
		return "";

	return manejadorMetadatos.getJsonMetadatos(filepath);
}

bool ManejadorArchivosYMetadatos::actualizarMetadatosChequeados (const string& filepath,
		const string& jsonNuevosMetadatos, const string& username) {

	string nuevoJson;
	bool deboRenombrar = manejadorMetadatos.actualizarPermisos(filepath, jsonNuevosMetadatos, username, nuevoJson);
	if (not deboRenombrar) return true;

	MetadatoArchivo metadatosNuevos = ParserJson::deserializarMetadatoArchivo(nuevoJson);
	string nuevoFilename = metadatosNuevos.nombre + "." + metadatosNuevos.extension;
	if (not manejadorArchivos.renombrarArchivo(filepath, nuevoFilename)) {
		Logger::logError("Metadatos que pinchan el renombrado: " + jsonNuevosMetadatos);
		return false;
	}

	manejadorMetadatos.actualizarPermisosPorRenombrado(filepath, nuevoFilename, metadatosNuevos, nuevoJson);
	return true;
}

// Lo que se va hacer es lo siguiente: Los nuevos metadatos vendran integramente por lo que se va a cambiar salvo por los usuarios habilitados.
// En ese caso, puedo recibir ese campo vacio o lleno:
// - Vacio significa que no se modificaron los usuarios
// - Lleno significa que esos son los nuevos usuarios habilitados -> En este caso me tengo que fijar los que ya habia comparados con los nuevos
//																	 a ver cual es el/los nuevo/s usuarios a dar permisos.
bool ManejadorArchivosYMetadatos::actualizarMetadatos (string username, string filepath, string jsonNuevosMetadatos) {
	if (not validador.verificarPermisos(username, filepath))
		return false;

	if (not validador.existeMetadato(filepath)) {
		Logger::logWarn("Se quiso actualizar los metadatos del archivo " + filepath + " pero este no existe.");
		return false;
	}
	return actualizarMetadatosChequeados(filepath, jsonNuevosMetadatos, username);
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
	if (not validador.existeMetadato(filepath)) {
		Logger::logWarn("Se quiso eliminar el archivo " + filepath + " pero este no existe en la base de datos.");
		return false;
	}

	vector<string> partes = ParserURI::parsear(filepath, '/');
	if (partes.size() <= 1)
		return false;
	if (TRASH == partes[1]) {
		manejadorMetadatos.eliminarDefinitivamente(filepath);
		manejadorArchivos.eliminarArchivoDefinitivamente(filepath + RESERVED_STR + "*");
		return true;
	} else {
		return this->mandarArchivoATrash(username, filepath);
	}
}

bool ManejadorArchivosYMetadatos::mandarArchivoATrash(string username, string filepath) {
	vector<string> directorios = ParserURI::parsear(filepath, '/');
	if (not validador.existeMetadato(filepath)) {
		Logger::logWarn("Se quiso mandar a la papelera el archivo " + filepath + " pero este no existe.");
		return false;
	}
	string jsonMetadatos = manejadorMetadatos.obtenerMetadato(filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadatos);
	string pathSinUsernameConReserved = ParserURI::join(directorios, RESERVED_CHAR, 1, directorios.size());

	if (metadato.propietario != directorios.front()) {	// DEBUG: no deberia suceder nunca
		cout << "El propietario del metadato no coincide con la carpeta del FileSystem. EXIT." << endl;
		Logger::logError("El propietario del metadato no coincide con la carpeta del FileSystem. EXIT.");
		exit(1);
	}

	string pathCompletoPapelera = metadato.propietario + "/" + TRASH + "/" + pathSinUsernameConReserved;
	string nroSecuencia = this->validador.obtenerNumeroSecuencia(this->pathFileSystem, metadato.propietario, pathSinUsernameConReserved);
	pathCompletoPapelera += RESERVED_CHAR + nroSecuencia;

	if (not manejadorMetadatos.mandarATrash(jsonMetadatos, username, filepath, pathCompletoPapelera))
		return false;

	return manejadorArchivos.mandarArchivoATrash(filepath, pathCompletoPapelera, metadato.ultimaVersion);
}

string ManejadorArchivosYMetadatos::obtenerEstructuraCarpeta (string path) {
	auto predicate = [&] (MetadatoArchivo& metadato) -> bool {return true;};
	return buscador.obtenerEstructuraCarpeta(path, false, predicate);
}

/* DEFAULT: version = LATEST */
string ManejadorArchivosYMetadatos::descargarArchivo (string username, string filepath, int version) {
	//OJO porque si no se corre desde la carpeta build como ./udrive esto va a pinchar seguramente (Ya que la carpeta del FileSystem no va a existir)
	string filepathCompleto = this->pathFileSystem + "/" + filepath;
	if (not validador.verificarPermisos(username, filepath) or not validador.existeArchivo(filepathCompleto, FIRST) ) {
		return "";
	}

	if (version == LATEST)
		version = getLatestVersion(filepath);

	if ( not validador.existeArchivo(filepathCompleto, version) ) {
		Logger::logWarn("Se ha querido descargar el archivo de path " + filepath + " en la version "
				+ to_string(version) + ", el cual no existe.");
		return "";
	}
	string strVersion = RESERVED_STR + to_string(version);
	string pathADevolver(this->homeDirectory);
	pathADevolver += "/" + filepathCompleto + strVersion;
	return pathADevolver;
}

string ManejadorArchivosYMetadatos::buscarPorExtension(string username, string extension) {
	return buscador.buscarPorExtension(username, extension);
}

string ManejadorArchivosYMetadatos::buscarPorEtiqueta(string username, string etiqueta) {
	return buscador.buscarPorEtiqueta(username, etiqueta);
}

string ManejadorArchivosYMetadatos::buscarPorNombre(string username, string nombre) {
	return buscador.buscarPorNombre(username, nombre);
}

int ManejadorArchivosYMetadatos::getLatestVersion (const string& filepath) {
	if (not validador.existeMetadato(filepath)) {
		Logger::logError("No existe el metadato " + filepath + " al buscar su ultima version");
		return -2;
	}
	string json = manejadorMetadatos.obtenerMetadato(filepath);
	return ParserJson::deserializarMetadatoArchivo(json).ultimaVersion;
}

string ManejadorArchivosYMetadatos::buscarPorPropietario(string username, string propietario) {
	return buscador.buscarPorPropietario(username, propietario);
}
