#ifndef MANEJADORARCHIVOSYMETADATOS_H_
#define MANEJADORARCHIVOSYMETADATOS_H_

//#include "validador.h"
#include "buscador.h"
#include "manejadorArchivos.h"
#include "manejadorMetadatos.h"
#include <functional>

using std::string;


class ManejadorArchivosYMetadatos {

private:
	BD* dbMetadatos;
	char homeDirectory[1024];
	string pathFileSystem;
	Validador validador;
	Buscador buscador;
	ManejadorArchivos manejadorArchivos;
	ManejadorMetadatos manejadorMetadatos;


	// Archivos y Carpetas
	bool eliminarArchivo(string username, string filepath); //*
	bool eliminarCarpeta(string username, string path);	//*
	bool mandarArchivoATrash(string username, string filepath); //* jodido, parte y parte
	bool guardarArchivo (const string& filepath, const string& username, const char* data, int dataLen); //* jodido, parte y parte


	bool actualizarMetadatosChequeados (const string& filepath, const string& jsonNuevosMetadatos, const string& username); // Queda

public:
	ManejadorArchivosYMetadatos(BD* dbMetadatos);
	ManejadorArchivosYMetadatos(BD* dbMetadatos, string path);
	virtual ~ManejadorArchivosYMetadatos();

	bool crearUsuario(string username);	// bajar copia de foto
	bool crearCarpetaSegura(string username, string path);
	string obtenerEstructuraCarpeta(string path);

	bool eliminar(string username, string path); //Sirve para carpetas y archivos
	bool restaurar(string username, string path); // bajar al menos un cacho

	bool subirArchivo(string username, string filepath, const char* data, int dataLen, string jsonMetadatos, int cuota); // dbMetadatos->put
	string descargarArchivo(string username, string filepath);
	bool actualizarArchivo(string username, string filepath, const char* data, int dataLen, int cuota);
	bool actualizarFotoPerfil(string filepathViejo, string filepathNuevo, const char* data, int dataLen); // a manejadorArchivos

	string consultarMetadatosArchivo(string username, string filename);
	bool actualizarMetadatos(string username, string filepath, string jsonNuevosMetadatos);

	bool deleteFileSystem();

	// Busquedas
	string buscarPorExtension(string username, string extension);
	string buscarPorEtiqueta(string username, string etiqueta);
	string buscarPorNombre(string username, string nombre);
	string buscarPorPropietario(string username, string propietario); // Da los archivos que me compartio ese propietario
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
