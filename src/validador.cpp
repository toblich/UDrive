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
	return false;
}

string Validador::obtenerNumeroSecuencia(string pathFileSystem, string propietario, string pathSinUsernameConHash) {
	string comando = "ls '" + pathFileSystem + "/" + propietario + "/" + trash + "' -1 | grep \"^" + pathSinUsernameConHash + "\\#[0-9]\\+$\" | rev | cut -d \\# -f 1 | rev | sort -nr | head -n1";
	string nuevoNroSecuencia;
	FILE *fp;
	char ultimoNroSecuencia[4];
	fp = popen(comando.c_str(), "r");
	if ( fgets(ultimoNroSecuencia, sizeof(ultimoNroSecuencia), fp) != NULL ) {
		string ultimoNroSecuenciaStr(ultimoNroSecuencia);
		stringstream str(ultimoNroSecuenciaStr);
		int numeroSecuencia;
		str >> numeroSecuencia;
		numeroSecuencia++;
		nuevoNroSecuencia = to_string(numeroSecuencia);
	} else {
		nuevoNroSecuencia = "0";
	}
	pclose(fp);
	return nuevoNroSecuencia;
}

bool Validador::puedoRestaurarA (string pathEnPapeleraSinFS, string pathRealSinFS, string pathFileSystem) {
	if (dbMetadatos->contains(pathRealSinFS)) {
		Logger::logWarn("La restauracion del archivo " + pathEnPapeleraSinFS
				+ " fallo porque ya existen sus metadatos en el filesystem");
		return false;
	}
	if (this->existeArchivo(pathFileSystem + "/" + pathRealSinFS)) {
		Logger::logWarn("La restauracion del archivo " + pathEnPapeleraSinFS
				+ " fallo porque ya existe el archivo en el filesystem");
		return false;
	}
	if (not dbMetadatos->contains(pathEnPapeleraSinFS)) {
		Logger::logWarn("La restauracion del archivo " + pathEnPapeleraSinFS
				+ " fallo porque no existen sus metadatos en la papelera");
		return false;
	}
	if (not this->existeArchivo(pathFileSystem + "/" + pathEnPapeleraSinFS)) {
		Logger::logWarn("La restauracion del archivo " + pathEnPapeleraSinFS
				+ " fallo porque no existe el archivo en la papelera");
		return false;
	}

	return true;
}
