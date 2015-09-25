#ifndef MANEJADORARCHIVOSYMETADATOS_H_
#define MANEJADORARCHIVOSYMETADATOS_H_

#include "server.h"
#include "parserJson.h"
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

const std::string defaultFileSystem = "FileSystem";
const std::string trash = "#trash";

class ManejadorArchivosYMetadatos {

private:
	BD* dbMetadatos;
//	char homeDirectory[1024];
	std::string pathFileSystem;
	bool verificarPathValido(std::string path);
	bool verificarPermisos(std::string username, std::string path);

public:
	ManejadorArchivosYMetadatos(BD* dbMetadatos);
	ManejadorArchivosYMetadatos(BD* dbMetadatos, std::string path);
	virtual ~ManejadorArchivosYMetadatos();

	std::vector<std::string> parsearDirectorios(std::string pathCompleto);
	void crearUsuario(std::string username);
	void crearCarpeta(std::string username, std::string path);
	bool crearCarpetaSegura(std::string username, std::string path);

	void subirArchivo(std::string username, std::string filepath, const char* data, int dataLen, std::string jsonMetadatos);
	std::string consultarMetadatosArchivo(std::string username, std::string filename);
	void eliminarArchivo(std::string username, std::string filepath);
	void actualizarMetadatos(std::string username, std::string filepath, std::string nuevosMetadatos);
	void actualizarArchivo(std::string username, std::string filepath, const char* data, int dataLen);
	void descargarArchivo(std::string username, std::string filepath);
	void agregarPermiso(std::string usernameOrigen, std::string filepath, std::string usernameDestino);

	void deleteFileSystem();
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
