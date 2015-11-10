#ifndef UDRIVE_MANEJADORMETADATOS_H
#define UDRIVE_MANEJADORMETADATOS_H

#include "validador.h"

/**
 * @brief Se encarga del manejo de los metadatos de los archivos.
 */
class ManejadorMetadatos {
public:
	/**
	 * @brief Constructor: Crea un ManejadorMetadatos.
	 *
	 * @param pathFileSystem 	string con el path raiz del FileSystem
	 * @param validador			Validador* que se utilizara para las validaciones
	 */
	ManejadorMetadatos(BD* dbMetadatos, Validador* validador);
	/**
	 * @brief Se encarga de agregar los permisos de un determinado usuario a la base de datos.
	 */
	bool agregarPermisosABD(string username);
	/**
	 * @brief Se encarga de actualizar los permisos en la base de datos.
	 */
	bool actualizarPermisos(const string& filepath, const string& jsonNuevosMetadatos, const string& username,
			string& nuevoJson);	// devuelve si hay que renombrar
	/**
	 * @brief Se encarga de actualizar los permisos en la base de datos cuando se renombra algun archivo.
	 */
	void actualizarPermisosPorRenombrado(const string& filepath, const string& nuevoFilename,
			const MetadatoArchivo& metadatosNuevos, const string& nuevoJson);
	/**
	 * @brief Se encarga de obtener los metadatos de un archivo con un determinado filepath.
	 */
	string getJsonMetadatos(const string& filepath);
	/**
	 * @brief Se encarga de borrar definitivamente los metadatos de un archivo en la base de datos.
	 */
	void eliminarDefinitivamente(const string& filepath);
	/**
	 * @brief Se encarga de restaurar los metadatos de un archivo que fue restaurado.
	 */
	bool restaurarMetadatos (const string& pathEnPapeleraSinFS, const string& username, const string& pathRealSinFS);
	bool mandarATrash (const string& jsonMetadatos, const string& username, const string& filepath,
			 	   const string& pathCompletoPapelera);
	/**
	 * @brief Se encarga de asignarle los metadatos a un determinado archivo en la base de datos.
	 */
	bool cargarMetadato(const string& filepath, const string& jsonMetadatos);
	/**
	 * @brief Se encarga de obtener los metadatos de un determinado archivo.
	 */
	string obtenerMetadato(const string& filepath);

	/**
	 * @brief Se encarga de actualizar los metadatos luego de la actualización de algun archivo.
	 */
	void actualizarMetadatosPorActualizacionArchivo(const string& filepath, const string& username, int nuevaVersion);

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
