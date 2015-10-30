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
	bool tamanioCarpeta(string path, unsigned long int & size);
	bool eliminarCarpeta(string username, string path);
	bool eliminarArchivo(string username, string filepath); //*
	void eliminarArchivoDefinitivamente(string filepath); //*
	bool mandarArchivoATrash(string username, string filepath); //*
	bool guardarArchivo (const string& filepath, const string& username, const char* data, int dataLen); //*
	bool renombrarArchivo (const string& pathInterno, const string& nuevoFilename);

	// Buscador
	string obtenerEstructuraCarpeta(string path, bool esRecursivo, function<bool(MetadatoArchivo&)> predicate);
	string obtenerEstructuraCompartidos(string path);

	// Metadatos
	string actualizarUsuarioFechaModificacion(string jsonMetadatos, string usernameModificacion);
	bool agregarPermiso(string usernameOrigen, string filepath, string usernameDestino);

	bool actualizarMetadatosChequeados (const string& filepath, const string& jsonNuevosMetadatos, const string& username); //*

public:
	ManejadorArchivosYMetadatos(BD* dbMetadatos);
	ManejadorArchivosYMetadatos(BD* dbMetadatos, string path);
	virtual ~ManejadorArchivosYMetadatos();

	bool crearUsuario(string username);
	bool crearCarpetaSegura(string username, string path);
	string obtenerEstructuraCarpeta(string path);

	bool eliminar(string username, string path); //Sirve para carpetas y archivos
	bool restaurar(string username, string path);

	bool subirArchivo(string username, string filepath, const char* data, int dataLen, string jsonMetadatos, int cuota);
	string descargarArchivo(string username, string filepath);
	bool actualizarArchivo(string username, string filepath, const char* data, int dataLen, int cuota);
	bool actualizarFotoPerfil(string filepathViejo, string filepathNuevo, const char* data, int dataLen);

	string consultarMetadatosArchivo(string username, string filename);
	bool actualizarMetadatos(string username, string filepath, string jsonNuevosMetadatos);

	bool deleteFileSystem();

	//busquedas
	string buscarPorExtension(string username, string extension);
	string buscarPorEtiqueta(string username, string etiqueta);
	string buscarPorNombre(string username, string nombre);
	//Esta busqueda implica mostrar todos los archivos que me compartio ese propietario
	string buscarPorPropietario(string username, string propietario);
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
