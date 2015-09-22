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
	BD* dbMetadatos;
//	char homeDirectory[1024];
	std::string pathFileSystem;

public:
	ManejadorArchivosYMetadatos(BD* dbMetadatos);
	ManejadorArchivosYMetadatos(BD* dbMetadatos, std::string path);
	virtual ~ManejadorArchivosYMetadatos();

	std::vector<std::string> parsearDirectorios(std::string pathCompleto);
	void crearUsuario(std::string username);
	void crearCarpeta(std::string path);
	void subirArchivo(std::string username, std::string filename, const char* data, int dataLen);

	void deleteFileSystem();
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
