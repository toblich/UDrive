#ifndef UDRIVE_MANEJADORMETADATOS_H
#define UDRIVE_MANEJADORMETADATOS_H

#include "validador.h"

class ManejadorMetadatos {

public:
	ManejadorMetadatos(BD* dbMetadatos, Validador* validador);

	string actualizarUsuarioFechaModificacion(string jsonMetadatos, string usernameModificacion);
	bool agregarPermisosABD(string username);
	bool agregarPermiso(string usernameOrigen, string filepath, string usernameDestino);
	bool eliminarPermiso(string usernameOrigen, string filepath, string usernameDestino);										//*
	Batch armarBatchEliminarArchivo (const string& jsonMetadatos, const string& username, const string& filepath,
									 const string& pathCompletoPapelera);
	bool restaurarMetadatos (const string& pathEnPapeleraSinFS, const string& username, const string& pathRealSinFS);
	string metadatosConPermisosDepurados (const string& filepath, const string& usernameDestino);
	string jsonArchivosPermitidos (const string& pathPermisos, const string& filepath);
	string actualizarPermisosMetadato (const MetadatoArchivo& metadatosViejos, MetadatoArchivo metadatosNuevos,
									   const string& username, const string& filepath, const string& jsonNuevosMetadatos);		//*
	void actualizarPermisosPathArchivo(const string& filepath, const string& nuevoFilepath,
			const list<string>& usuariosHabilitados);

private:
	BD* dbMetadatos;
	Validador* validador;
};


#endif //UDRIVE_MANEJADORMETADATOS_H
