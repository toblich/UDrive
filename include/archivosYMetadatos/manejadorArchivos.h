#ifndef UDRIVE_MANEJADORARCHIVOS_H
#define UDRIVE_MANEJADORARCHIVOS_H

#include "validador.h"

class ManejadorArchivos {

public:
	ManejadorArchivos(string pathFileSystem, Validador* validador);
	virtual ~ManejadorArchivos();

	bool tamanioCarpeta(string path, unsigned long int & size);
	bool crearCarpeta(string username, string path);
	bool eliminarCarpeta(string username, string path); //*
	bool deleteCarpeta(string path);
	bool renombrarArchivo (const string& pathInterno, const string& nuevoFilename);

private:
	string pathFileSystem;
	Validador* validador;
};


#endif //UDRIVE_MANEJADORARCHIVOS_H
