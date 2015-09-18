#ifndef MANEJADORARCHIVOSYMETADATOS_H_
#define MANEJADORARCHIVOSYMETADATOS_H_

#include "server.h"
#include "parserJson.h"
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

class ManejadorArchivosYMetadatos {

private:
	BaseDeDatos* dbMetadatos;
	char homeDirectory[1024];
	std::vector<std::string> parsearDirectorios(std::string pathCompleto);

public:
	ManejadorArchivosYMetadatos(BaseDeDatos* dbMetadatos);
	virtual ~ManejadorArchivosYMetadatos();

	void crearUsuario(std::string username);
	void crearCarpeta(std::string path);
	void subirArchivo(std::string username, std::string filename, const char* data, int dataLen);
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
