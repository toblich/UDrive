#include "manejadorArchivos.h"

ManejadorArchivos::ManejadorArchivos (string pathFileSystem, Validador* validador) {
	this->pathFileSystem = pathFileSystem;
	this->validador = validador;
}

ManejadorArchivos::~ManejadorArchivos () {
}

bool ManejadorArchivos::tamanioCarpeta (string path, unsigned long int& size) {
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
		if (validador->existeCarpeta(pathInternoConFS))
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

// El path recibido no debe contener el nombre de un archivo.
// En caso de que sea asi, se debera modificar este metodo.
bool ManejadorArchivos::crearCarpeta (string username, string path) {
	bool tienePermisos = validador->verificarPermisos(username, path);
	// Agrego el FileSystem para que sea la "raiz"
	string pathCompletoConFS = this->pathFileSystem + "/" + path;
	vector<string> directorios = ParserURI::parsear(pathCompletoConFS, '/');
	string directorioAcumulado = "";
	const int SIZE = directorios.size();

	for (int i = 0; i < SIZE; i++) {
		string directorio = directorios[i];
		string directorioPadre = directorioAcumulado;
		directorioAcumulado += (directorio + "/");

		if ( not validador->existeCarpeta(directorioAcumulado) ) { // Me fijo si existe la carpeta, sino la creo
			if (not tienePermisos)
				return false; // Si no existia la carpeta y no tiene permisos para crear

			mkdir(directorioAcumulado.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			Logger::logInfo("La carpeta " + directorio + " no existe dentro de " + directorioPadre
								+ " por lo que ha sido creada.");
		}
	}
	string directorioFotos = this->pathFileSystem + "/" + FOTOS;
	if (not validador->existeCarpeta(directorioFotos)) {
		mkdir(directorioFotos.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		Logger::logInfo("La carpeta " + FOTOS + " no existe dentro de " + this->pathFileSystem
							+ " por lo que ha sido creada.");
	}

	return true;
}

//bool ManejadorArchivos::eliminarCarpeta (string username, string path) {
//	string pathConFS = this->pathFileSystem + "/" + path;
//	DIR* dir;
//	struct dirent* ent;
//	if ((dir = opendir(pathConFS.c_str())) == NULL) {
//		Logger::logWarn("No existe el directorio " + path);
//		return false;
//	}
//
//	while ((ent = readdir(dir)) != NULL) {
//		string dirName(ent->d_name);
//		if (dirName == "." or dirName == "..")
//			continue;
//
//		string pathInterno = path + "/" + dirName;
//		string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;
//
//		this->eliminar(username, pathInterno);
//	}
//	closedir(dir);
//
//	if (validador->carpetaVacia(pathConFS))
//		return this->deleteCarpeta(pathConFS);
//	else
//		return false;
//}


bool ManejadorArchivos::deleteCarpeta (string path) {
	if (not validador->existeCarpeta(path)) {
		Logger::logWarn("No se ha podido eliminar la carpeta " + path + " porque no existe.");
		return false;
	}
	string command = "exec rm -r '" + path + "'";
	system(command.c_str());
	Logger::logTrace("Se elimino permanentemente la carpeta " + path + ".");
	return true;
}

bool ManejadorArchivos::renombrarArchivo (const string& pathInterno, const string& nuevoFilename) {
	string nuevoPathInterno = ParserURI::pathConNuevoFilename(pathInterno, nuevoFilename);
	string pathConFS = pathFileSystem + "/" + pathInterno;
	string nuevoPathConFS = pathFileSystem + "/" + nuevoPathInterno;

	if (validador->existeArchivo(nuevoPathConFS) or validador->existeMetadato(nuevoPathInterno)) {
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
