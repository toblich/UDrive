
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
#include <sstream>

using namespace std;

#define RESERVED_CHAR '^'	// Ojo, mover los tres juntos
const string RESERVED_STR = "^";
const string RESERVED_REGEX = "\\^";

const string TRASH = RESERVED_STR + "trash";
const string PERMISOS = RESERVED_STR + "permisos";
const string FOLDER = RESERVED_STR + "folder";

const string DEFAULT_FS = "FileSystem";
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
	bool puedoRestaurarA(string pathEnPapeleraSinFS, string pathRealSinFS, string pathFileSystem);
	string obtenerNumeroSecuencia(string pathFileSystem, string propietario, string pathSinUsernameConHash);
};

#endif /* INCLUDE_VALIDADOR_H_ */
