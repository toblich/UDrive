#ifndef UDRIVE_MANEJADORARCHIVOS_H
#define UDRIVE_MANEJADORARCHIVOS_H

#include "validador.h"

class ManejadorArchivos {

public:
	ManejadorArchivos(string pathFileSystem, Validador* validador);
	virtual ~ManejadorArchivos();

	bool tamanioCarpeta(string path, unsigned long int & size);
	bool crearCarpeta(string username, string path);
	bool deleteCarpeta(string path);

	bool renombrarArchivo (const string& pathInterno, const string& nuevoFilename);
	bool restaurarArchivo (const string& pathRealSinFS, const string& pathEnPapeleraSinFS, int ultimaVersion);
	void deshacerRestaurado (const string& pathRealSinFS, const string& pathEnPapeleraSinFS);

	void eliminarArchivoDefinitivamente (const string& filepath);
	bool mandarArchivoATrash (const string& filepath, const string& pathCompletoPapelera, int ultimaVersion);
	void guardarArchivoEnFileSystem(const string& filepath, const char* data, int dataLen);

	void crearFotoPerfilDefault(string username);

private:
	bool deleteFileSystem();

	string pathFileSystem;
	Validador* validador;
};


#endif //UDRIVE_MANEJADORARCHIVOS_H
