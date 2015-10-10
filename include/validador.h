
#ifndef INCLUDE_VALIDADOR_H_
#define INCLUDE_VALIDADOR_H_

#include "parserJson.h"
#include "parserURI.h"
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include "bd.h"
#include "logger.h"
#include <dirent.h>
#include <algorithm>

using namespace std;

const string defaultFileSystem = "FileSystem";
const string trash = "#trash";
const string permisos = "#permisos";
const unsigned long int CUOTA = 2147483648; // 2GB

class Validador {
private:
	BD* dbMetadatos;

public:
	Validador(BD*);
	virtual ~Validador();
	bool existeArchivo(string filepath);
	bool existeCarpeta(string path);
	bool carpetaVacia(string path);
	bool esPathValido(string path);
	bool tienePermisos(string username, string path);
	bool verificarPermisos(string username, string path);
};

#endif /* INCLUDE_VALIDADOR_H_ */
