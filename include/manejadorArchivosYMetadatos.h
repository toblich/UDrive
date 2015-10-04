#ifndef MANEJADORARCHIVOSYMETADATOS_H_
#define MANEJADORARCHIVOSYMETADATOS_H_

#include "server.h"
#include "parserJson.h"
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

const std::string defaultFileSystem = "FileSystem";
const std::string trash = "#trash";

class ManejadorArchivosYMetadatos {

private:
	BD* dbMetadatos;
	char homeDirectory[1024];
	std::string pathFileSystem;
	void logInfo(std::string mensaje);
	void logWarn(std::string mensaje);
	void logError(std::string mensaje);
	bool existeArchivo(std::string filepath);
	bool existeCarpeta(std::string path);
	bool deleteCarpeta(std::string path);

public:
	ManejadorArchivosYMetadatos(BD* dbMetadatos);
	ManejadorArchivosYMetadatos(BD* dbMetadatos, std::string path);
	virtual ~ManejadorArchivosYMetadatos();


	bool verificarPathValido(std::string path);
	bool verificarPermisos(std::string username, std::string path);

	bool tamanioCarpeta(std::string path, unsigned long int & size);

	std::vector<std::string> parsearDirectorios(std::string pathCompleto);
	bool carpetaVacia(std::string path);
	bool crearUsuario(std::string username);
	bool crearCarpeta(std::string username, std::string path);
	bool crearCarpetaSegura(std::string username, std::string path);
	bool borrarCarpeta(std::string username, std::string path);

	std::string actualizarUsuarioFechaModificacion(std::string jsonMetadatos, std::string usernameModificacion);
	bool subirArchivo(std::string username, std::string filepath, const char* data, int dataLen, std::string jsonMetadatos);
	std::string consultarMetadatosArchivo(std::string username, std::string filename);
	bool eliminarArchivo(std::string username, std::string filepath);
	bool actualizarMetadatos(std::string username, std::string filepath, std::string nuevosMetadatos);
	bool actualizarArchivo(std::string username, std::string filepath, const char* data, int dataLen);
	std::string descargarArchivo(std::string username, std::string filepath);
	bool agregarPermiso(std::string usernameOrigen, std::string filepath, std::string usernameDestino);

	bool deleteFileSystem();
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
