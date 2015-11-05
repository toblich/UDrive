#ifndef UDRIVE_MANEJADORMETADATOS_H
#define UDRIVE_MANEJADORMETADATOS_H

#include "validador.h"

class ManejadorMetadatos {

public:
	ManejadorMetadatos(BD* dbMetadatos, Validador* validador);

	bool agregarPermisosABD(string username);
	bool actualizarPermisos(const string& filepath, const string& jsonNuevosMetadatos, const string& username,
			string& nuevoJson);	// devuelve si hay que renombrar
	void actualizarPermisosPorRenombrado(const string& filepath, const string& nuevoFilename,
			const MetadatoArchivo& metadatosNuevos, const string& nuevoJson);

	string getJsonMetadatos(const string& filepath);

	void eliminarDefinitivamente(const string& filepath);
	bool restaurarMetadatos (const string& pathEnPapeleraSinFS, const string& username, const string& pathRealSinFS);
	bool mandarATrash (const string& jsonMetadatos, const string& username, const string& filepath,
			 	   const string& pathCompletoPapelera);

	bool cargarMetadato(const string& filepath, const string& jsonMetadatos);

	void actualizarMetadatosPorActualizacionArchivo(const string& filepath, const string& username);

private:
	BD* dbMetadatos;
	Validador* validador;

	string actualizarUsuarioFechaModificacion(string jsonMetadatos, string usernameModificacion);

	bool agregarPermiso(string usernameOrigen, string filepath, string usernameDestino);
	bool eliminarPermiso(string usernameOrigen, string filepath, string usernameDestino);
	void actualizarPermisosMetadato (const MetadatoArchivo& metadatosViejos, MetadatoArchivo metadatosNuevos,
									   const string& username, const string& filepath);
	void actualizarPermisosPathArchivo(const string& filepath, const string& nuevoFilepath,
			const list<string>& usuariosHabilitados);
	string jsonArchivosPermitidos (const string& pathPermisos, const string& filepath);
	string metadatosConPermisosDepurados (const string& filepath, const string& usernameDestino);

	Batch armarBatchEliminarArchivo (const string& jsonMetadatos, const string& username, const string& filepath,
									 const string& pathCompletoPapelera);
};

#endif //UDRIVE_MANEJADORMETADATOS_H
