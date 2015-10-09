#ifndef MANEJADORARCHIVOSYMETADATOS_H_
#define MANEJADORARCHIVOSYMETADATOS_H_

#include "parserJson.h"
#include "parserURI.h"
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include "bd.h"
#include "logger.h"
#include <dirent.h>
#include <algorithm>

using std::string;

const string defaultFileSystem = "FileSystem";
const string trash = "#trash";
const string permisos = "#permisos";
const unsigned long int CUOTA = 2147483648; // 2GB

class ManejadorArchivosYMetadatos {

private:
	BD* dbMetadatos;
	char homeDirectory[1024];
	string pathFileSystem;

	bool existeArchivo(string filepath);
	bool existeCarpeta(string path);

	bool verificarPathValido(string path);
	bool tienePermisos(string username, string path);
	bool verificarPermisos(string username, string path);

	bool tamanioCarpeta(string path, unsigned long int & size);
	bool crearCarpeta(string username, string path);
	bool carpetaVacia(string path);
	bool eliminarCarpeta(string username, string path);
	bool deleteCarpeta(string path);

	string actualizarUsuarioFechaModificacion(string jsonMetadatos, string usernameModificacion);
	bool agregarPermisosABD(string username);
	bool eliminarArchivo(string username, string filepath);
	bool agregarPermiso(string usernameOrigen, string filepath, string usernameDestino);
	bool guardarArchivo (const string& filepath, const string& username, const char* data, int dataLen);
	void actualizarMetadatosChequeados (const string& filepath, const string& jsonNuevosMetadatos, const string& username);

public:
	ManejadorArchivosYMetadatos(BD* dbMetadatos);
	ManejadorArchivosYMetadatos(BD* dbMetadatos, string path);
	virtual ~ManejadorArchivosYMetadatos();

	bool crearUsuario(string username);
	bool crearCarpetaSegura(string username, string path);
	string obtenerEstructuraCarpeta(string path);

	bool eliminar(string username, string path); //Sirve para carpetas y archivos

	bool subirArchivo(string username, string filepath, const char* data, int dataLen, string jsonMetadatos);
	string descargarArchivo(string username, string filepath);
	bool actualizarArchivo(string username, string filepath, const char* data, int dataLen);

	string consultarMetadatosArchivo(string username, string filename);
	bool actualizarMetadatos(string username, string filepath, string jsonNuevosMetadatos);

	bool deleteFileSystem();
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
