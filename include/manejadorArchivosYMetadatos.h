#ifndef MANEJADORARCHIVOSYMETADATOS_H_
#define MANEJADORARCHIVOSYMETADATOS_H_

#include "validador.h"

using std::string;


class ManejadorArchivosYMetadatos {

private:
	BD* dbMetadatos;
	char homeDirectory[1024];
	string pathFileSystem;
	Validador validador;

	bool tamanioCarpeta(string path, unsigned long int & size);
	bool crearCarpeta(string username, string path);
	bool eliminarCarpeta(string username, string path);
	bool deleteCarpeta(string path);

	string actualizarUsuarioFechaModificacion(string jsonMetadatos, string usernameModificacion);

	bool agregarPermisosABD(string username);
	bool eliminarArchivo(string username, string filepath);
	bool mandarArchivoATrash(string username, string filepath);
	void eliminarArchivoDefinitivamente(string filepath);
	bool agregarPermiso(string usernameOrigen, string filepath, string usernameDestino);
	bool guardarArchivo (const string& filepath, const string& username, const char* data, int dataLen);
	void actualizarMetadatosChequeados (const string& filepath, const string& jsonNuevosMetadatos, const string& username);
	Batch armarBatchEliminarArchivo (const string& jsonMetadatos, const string& username, const string& filepath,
			const string& pathCompletoPapelera);
	bool restaurarMetadatos (const string& pathEnPapeleraSinFS, const string& username, const string& pathRealSinFS);

public:
	ManejadorArchivosYMetadatos(BD* dbMetadatos);
	ManejadorArchivosYMetadatos(BD* dbMetadatos, string path);
	virtual ~ManejadorArchivosYMetadatos();

	bool crearUsuario(string username);
	bool crearCarpetaSegura(string username, string path);
	string obtenerEstructuraCarpeta(string path);

	bool eliminar(string username, string path); //Sirve para carpetas y archivos
	bool restaurar(string username, string path);

	bool subirArchivo(string username, string filepath, const char* data, int dataLen, string jsonMetadatos);
	string descargarArchivo(string username, string filepath);
	bool actualizarArchivo(string username, string filepath, const char* data, int dataLen);

	string consultarMetadatosArchivo(string username, string filename);
	bool actualizarMetadatos(string username, string filepath, string jsonNuevosMetadatos);

	bool deleteFileSystem();
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
