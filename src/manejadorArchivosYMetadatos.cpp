#include "manejadorArchivosYMetadatos.h"

// TODO: LOGUEAR!!!!!
// TODO: Ver lo de la actualizacion de metadatos (ultima modificacion, etc)
// TODO: El username que se recibe en la mayoria de los metodos es el del que pide. Luego, el path tiene el path posta. Por ejemplo:
//		 username: pablo		path: tobi/hola/asd.txt
//		 Me deberia fijar si username es igual al primero del path. De ser asi, tiene los permisos seguro.
//		 Sino, como en este caso, deberia fijarme si pablo tiene permisos para acceder a ese path mediante lo que habra en la DB
//		 bajo la key '~permisos/pablo' y cuyo value sera algo del estilo 'pancho/datos/pepe.jpg#tobi/hola/asd.txt#santi/chau.txt'
//		 lo cual habra que parsear y ver si esta o no.
//
//		 REVISAR Y TESTEAR TODO!!!

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos(BD* dbMetadatos): ManejadorArchivosYMetadatos(dbMetadatos, defaultFileSystem){}

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos(BD* dbMetadatos, std::string path) {
	this->dbMetadatos = dbMetadatos;
	this->pathFileSystem = path;
//    getcwd(homeDirectory, sizeof(homeDirectory));
}

ManejadorArchivosYMetadatos::~ManejadorArchivosYMetadatos() {
}

// Una carpeta no puede contener un #
bool ManejadorArchivosYMetadatos::verificarPathValido(std::string path) {
	if ( path.find('#') == std::string::npos ) return true;
	return false;
}

bool ManejadorArchivosYMetadatos::verificarPermisos(std::string username, std::string path) {
	//TODO
	return true;
}

std::vector<std::string> ManejadorArchivosYMetadatos::parsearDirectorios(std::string pathCompleto) {
	std::vector<std::string> directorios;
	unsigned int size = pathCompleto.size();
	char delim = '/';
	std::string buffer = "";
	for (unsigned int i = 0; i < size; i++){
		char n = pathCompleto[i];
		if (n != delim)
			buffer +=n;
		else if (n == delim && buffer != ""){ //No hay directorios con nombre vacio
			directorios.push_back(buffer);
			buffer = "";
		}
	}
	if(buffer != "") directorios.push_back(buffer);
	return directorios;
}

// El path recibido no debe contener el nombre de un archivo.
// En caso de que sea asi, se debera modificar este metodo.
void ManejadorArchivosYMetadatos::crearCarpeta(std::string username, std::string path) {
	struct stat sb;
	// Agrego el FileSystem para que sea la "raiz"
//	string pathCompletoConFS = this->pathFileSystem + "/" + username + "/" + path;
	string pathCompletoConFS = this->pathFileSystem + "/" + path;
	std::vector<std::string> directorios = parsearDirectorios(pathCompletoConFS);
	std::string directorioAcumulado = "";
	int size = directorios.size();
	//TODO Ojo con que se cree una carpeta .trash por parte del usuario
	for (int i = 0; i < size; i++){
		std::string directorio = directorios[i];
		std::string directorioPadre = directorioAcumulado;
		directorioAcumulado += (directorio + "/");
		// Me fijo si existe la carpeta, sino la creo
		if (! (stat(directorioAcumulado.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) ){
			mkdir(directorioAcumulado.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			Logger logger;
			logger.loggear("La carpeta " + directorio + " no existe dentro de " + directorioPadre +" por lo que ha sido creada.", INFO);
		}
	}
}

bool ManejadorArchivosYMetadatos::crearCarpetaSegura(std::string username, std::string path) {
	if ( this->verificarPathValido(path) ) {
		this->crearCarpeta(username, path);
		return true;
	}
	return false;
}

void ManejadorArchivosYMetadatos::crearUsuario(std::string username) {
	//Creo tanto la carpeta del username como su papelera
	this->crearCarpeta(username, trash);
}



// En la base de datos se guarda el path sin la carpeta del FS
void ManejadorArchivosYMetadatos::subirArchivo(std::string username,
		std::string filepath, const char* data, int dataLen, std::string jsonMetadatos) {
	if ( verificarPermisos(username, filepath) ) {
		this->actualizarArchivo(username, filepath, data, dataLen);
		dbMetadatos->put(filepath, jsonMetadatos);
	}
}

// OJO porque el get tira excepciones
// TODO: Ver si lanzar una excepcion mas especifica del tipo Metadato no encontrado
std::string ManejadorArchivosYMetadatos::consultarMetadatosArchivo(std::string username, std::string filepath) {
	if ( verificarPermisos(username, filepath) ) {
		return dbMetadatos->get(filepath);
	}
	return "";
}

void ManejadorArchivosYMetadatos::actualizarMetadatos(std::string username,
		std::string filepath, std::string nuevosMetadatos) {
	if ( verificarPermisos(username, filepath) ) {
		dbMetadatos->modify(filepath, nuevosMetadatos);
	}
}

void ManejadorArchivosYMetadatos::agregarPermiso(std::string usernameOrigen,
		std::string filepath, std::string usernameDestino) {
	if ( verificarPermisos(usernameOrigen, filepath) ) {
		//TODO Falta agregar el hecho de agregar al archivo de permisos que esta descripto en el issue
		std::string pathCompleto = usernameOrigen + "/" + filepath;
		std::string jsonArchivo = dbMetadatos->get(pathCompleto);
		ParserJson parser;
		MetadatoArchivo metadato = parser.deserializarMetadatoArchivo(jsonArchivo);
		metadato.usuariosHabilitados.push_back(usernameDestino);
		std::string jsonModificado = parser.serializarMetadatoArchivo(metadato);
		dbMetadatos->modify(pathCompleto,jsonModificado);
	}
}

// Lo que se hace es moverlo a la papelera y cambiar el key de los metadatos por ese
void ManejadorArchivosYMetadatos::eliminarArchivo(std::string username, std::string filepath) {
	if ( verificarPermisos(username, filepath) ) {
		std::string pathArchivo = this->pathFileSystem + "/" + filepath;
		std::vector<std::string> directorios = parsearDirectorios(filepath);
		int size = directorios.size();
		std::string filename = directorios[size-1];
		std::string pathCompletoPapelera = username + "/" + trash + "/" + filename;
		std::string pathArchivoPapelera = this->pathFileSystem + "/" + pathCompletoPapelera;

		int result = rename( pathArchivo.c_str(), pathArchivoPapelera.c_str() );
		if ( result == 0 ) {
			// TODO Loguear eliminado ok
			std::string json = this->dbMetadatos->get(filepath);
			Batch batch;
			batch.erase(filepath);
			batch.put(pathCompletoPapelera, json);
			// Tener en cuenta que hay que cambiar en ~permisos
			this->dbMetadatos->writeBatch(batch);
		}
		else
			;// TODO Loguear eliminado no ok
	}
}

// Tambien se podria sacar el username y que venga en el filename. En ese caso habria que modificar una boludez.
// El filename deberia venir con los path de carpetas tambien (Pero sin el del usuario)
void ManejadorArchivosYMetadatos::actualizarArchivo(std::string username,
		std::string filepath, const char* data, int dataLen) {
	//No le agrego el FileSystem porque se agrega despues en el metodo crearCarpeta
	if ( verificarPermisos(username, filepath) ) {
		std::vector<std::string> directorios = parsearDirectorios(filepath);
		int size = directorios.size();
		std::string pathSinArchivo = "";
		for (int i = 0; i < size-1; i++){ //Saco el nombre del archivo
			std::string directorio = directorios[i];
			pathSinArchivo += directorio;
			if (i < size-2) pathSinArchivo += "/";
		}
		//Verifico que existan todas las carpetas y sino las creo
		if (pathSinArchivo != "") {
			if ( crearCarpetaSegura(username, pathSinArchivo) ) ;
		}
		std::string pathConFileSystem = this->pathFileSystem + "/" + username + "/" + filepath;

		ofstream outFile(pathConFileSystem, std::ofstream::binary);
		outFile.write(data, dataLen);
		outFile.close();
	}
}

void ManejadorArchivosYMetadatos::descargarArchivo(std::string username, std::string filepath) {
	//TODO
}

void ManejadorArchivosYMetadatos::deleteFileSystem() {
	struct stat sb;
	if (stat(pathFileSystem.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)){
		string command = "exec rm -r " + this->pathFileSystem;
		system(command.c_str());
		Logger logger;
		logger.loggear("Se eliminaron los datos persistentes del file system." , TRACE);
	}
}
