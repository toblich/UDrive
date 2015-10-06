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

bool ManejadorArchivosYMetadatos::verificarPermisos(std::string username, std::string path) {
	std::vector<std::string> directorios = this->parsearDirectorios(path);
	if ( directorios.size() > 0){
		std::string fileOwner = directorios[0];
		if ( username == fileOwner )
			return true;
		else {
			//TODO: Verificar que el username tenga permisos en la parte de ~permisos
			//TODO: Sino, loguear que no tiene permisos
			this->logWarn("El usuario " + username + " no posee los permisos para el archivo " + path + ".");
			return false; //Este return deberia depender de si tiene permisos o no
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
	if (buffer != "") directorios.push_back(buffer);
	return directorios;
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
		std::vector<std::string> directorios = parsearDirectorios(pathCompletoConFS);
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

bool ManejadorArchivosYMetadatos::crearUsuario(std::string username) {
	//Creo tanto la carpeta del username como su papelera
	string pathTrash = username + "/" + trash;
	return this->crearCarpeta(username, pathTrash);
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
					if ( not crearCarpetaSegura(username, pathSinArchivo) ) {
						this->logWarn("Al querer actualizar el archivo " + filepath + " no se pudieron crear las carpetas.");
						return false ;
					}
				}
				std::string pathConFileSystem = this->pathFileSystem + "/" + filepath;

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
// TODO: Ver si lanzar una excepcion mas especifica del tipo Metadato no encontrado
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
// TODO: Tiene sentido? Me parece mejor el hecho de agarrar y tener una "modificacion" por cada tipo como "darPermiso", "agregarEtiqueta", etc
bool ManejadorArchivosYMetadatos::actualizarMetadatos(std::string username,
		std::string filepath, std::string nuevosMetadatos) {
	if ( verificarPermisos(username, filepath) ) {
		if ( not dbMetadatos->contains(filepath) ){
			this->logWarn("Se quiso actualizar los metadatos del archivo " + filepath + " pero este no existe.");
			return false;
		}
		dbMetadatos->modify(filepath, nuevosMetadatos);
		return true;
	} else
		return false;
}

// OJO porque el get y modify tiran excepciones
bool ManejadorArchivosYMetadatos::agregarPermiso(std::string usernameOrigen,
		std::string filepath, std::string usernameDestino) {
	if ( verificarPermisos(usernameOrigen, filepath) ) {
		//TODO Falta agregar el hecho de agregar al archivo de permisos que esta descripto en el issue
		if ( not dbMetadatos->contains(filepath) ){
			this->logWarn("Se quiso agregar un permiso al archivo " + filepath + " pero este no existe.");
			return false;
		}
		std::string jsonArchivo = dbMetadatos->get(filepath);
		ParserJson parser;
		MetadatoArchivo metadato = parser.deserializarMetadatoArchivo(jsonArchivo);
		metadato.usuariosHabilitados.push_back(usernameDestino);
		std::string jsonModificado = parser.serializarMetadatoArchivo(metadato);
		dbMetadatos->modify(filepath,jsonModificado);
		return true;
	} else
		return false;
}

// OJO porque el get tira excepciones
// Lo que se hace es moverlo a la papelera y cambiar el key de los metadatos por ese
bool ManejadorArchivosYMetadatos::eliminarArchivo(std::string username, std::string filepath) {
	if ( verificarPermisos(username, filepath) ) {
		std::string filepathCompleto = this->pathFileSystem + "/" + filepath;
		if ( this->existeArchivo(filepathCompleto) ) {
			if ( not dbMetadatos->contains(filepath) ){
				this->logWarn("Se quiso eliminar el archivo " + filepath + " pero este no existe en la base de datos.");
				return false;
			}
			std::vector<std::string> directorios = parsearDirectorios(filepath);
			int size = directorios.size();
			std::string filename = directorios[size-1];
			std::string pathCompletoPapelera = username + "/" + trash + "/" + filename;
			std::string pathArchivoPapelera = this->pathFileSystem + "/" + pathCompletoPapelera;

			int result = rename( filepathCompleto.c_str(), pathArchivoPapelera.c_str() );
			if ( result == 0 ) {
				this->logInfo("La eliminacion del archivo " + filepath + " fue correcta.");
				std::string json = this->dbMetadatos->get(filepath);
				Batch batch;
				batch.erase(filepath);
				batch.put(pathCompletoPapelera, json);
				// TODO: Tener en cuenta que hay que cambiar en ~permisos
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
				std::vector<std::string> directorios = this->parsearDirectorios(pathInterno);
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
		cout << json << endl;
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
