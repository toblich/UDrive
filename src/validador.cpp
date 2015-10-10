#include "validador.h"

Validador::Validador (BD* dbMetadatos) {
	this->dbMetadatos = dbMetadatos;
}

Validador::~Validador () {
}


bool Validador::existeArchivo (string filepath) {
	struct stat buffer;
	return (stat(filepath.c_str(), &buffer) == 0);
}

bool Validador::existeCarpeta (string path) {
	struct stat sb;
	return (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

bool Validador::carpetaVacia (string pathConFS) {
	DIR* dir;
	struct dirent* ent;
	bool vacia = true;
	if ((dir = opendir(pathConFS.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL && vacia) {
			string dirName(ent->d_name);
			if (dirName != "." and dirName != "..")
				vacia = false;
		}
		closedir(dir);
	} else
		return false;
	return vacia;
}

// Una carpeta no puede contener un #
bool Validador::esPathValido (string path) {
	if (path.find('#') == string::npos)
		return true;
	Logger::logWarn("El path " + path + " no es valido.");
	return false;
}

bool Validador::tienePermisos (string username, string path) {
	string key = permisos + "/" + username;
	if (this->dbMetadatos->contains(key)) {
		string archivosPermitidos = this->dbMetadatos->get(key);
		vector<string> archivos = ParserURI::parsear(archivosPermitidos, '#');
		return (find(archivos.begin(), archivos.end(), path) != archivos.end());
	}
	return false; //Significa que no se creo el usuario porque no existe esa key basicamente. No deberia pasar.
}

bool Validador::verificarPermisos (string username, string path) {
	vector<string> directorios = ParserURI::parsear(path, '/');
	if (directorios.size() > 0) {
		string fileOwner = directorios[0];
		if (username == fileOwner or this->tienePermisos(username, path))
			return true;
		Logger::logWarn("El usuario " + username + " no posee los permisos para el archivo " + path + ".");
	}
	return false; //Esto no deberia pasar jamás, pero bueno
}
