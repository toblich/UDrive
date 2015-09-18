#ifndef MANEJADORARCHIVOSYMETADATOS_H_
#define MANEJADORARCHIVOSYMETADATOS_H_

#include "server.h"
#include "parserJson.h"
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

const std::string defaultFileSystem = "FileSystem";

class ManejadorArchivosYMetadatos {

private:
	BaseDeDatos* dbMetadatos;
	char homeDirectory[1024];
	std::string pathFileSystem;
	std::vector<std::string> parsearDirectorios(std::string pathCompleto);

public:
	ManejadorArchivosYMetadatos(BaseDeDatos* dbMetadatos);
	ManejadorArchivosYMetadatos(BaseDeDatos* dbMetadatos, std::string path);
	virtual ~ManejadorArchivosYMetadatos();

	void crearUsuario(std::string username);
	void crearCarpeta(std::string path);
	void subirArchivo(std::string username, std::string filename, const char* data, int dataLen);

	void deleteFileSystem();
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
