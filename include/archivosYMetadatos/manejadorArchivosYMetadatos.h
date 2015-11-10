#ifndef MANEJADORARCHIVOSYMETADATOS_H_
#define MANEJADORARCHIVOSYMETADATOS_H_

#include "buscador.h"
#include "manejadorArchivos.h"
#include "manejadorMetadatos.h"
#include <functional>

using std::string;

/**
 * @brief Se encarga de proveer una interfaz entre las clases de la API REST y los diferentes manejadores.
 */

class ManejadorArchivosYMetadatos {

private:
	char homeDirectory[1024];
	string pathFileSystem;
	Validador validador;
	Buscador buscador;
	ManejadorArchivos manejadorArchivos;
	ManejadorMetadatos manejadorMetadatos;

	// Archivos y Carpetas
	bool eliminarArchivo(string username, string filepath);
	bool eliminarCarpeta(string username, string path);
	bool mandarArchivoATrash(string username, string filepath);
	bool guardarArchivo (const string& filepath, const string& username, const char* data, int dataLen, int nuevaVersion);

	bool actualizarMetadatosChequeados (const string& filepath, const string& jsonNuevosMetadatos, const string& username);

	int getLatestVersion(const string& filepath);

public:
	/**
	 * @brief Constructor: Crea un ManejadorArchivosYMetadatos.
	 *
	 * @param dbMetadatos		BD* a la base de datos de los metadatos
	 */
	ManejadorArchivosYMetadatos(BD* dbMetadatos);
	/**
	 * @brief Constructor: Crea un ManejadorArchivosYMetadatos.
	 *
	 * @param dbMetadatos		BD* a la base de datos de los metadatos
	 * @param pathFileSystem 	string con el path raiz del FileSystem
	 */
	ManejadorArchivosYMetadatos(BD* dbMetadatos, string path);
	virtual ~ManejadorArchivosYMetadatos();
	/**
	 * @brief Se encarga crear un nuevo usuario, creandole una carpeta propia dentro del FileSystem.
	 */
	bool crearUsuario(string username);
	/**
	 * @brief Se encarga de crear una carpeta dentro del FileSystem de forma segura verificando los permisos.
	 */
	bool crearCarpetaSegura(string username, string path);
	/**
	 * @brief Se encarga de obtener la estructura interna de una determinada carpeta.
	 */
	string obtenerEstructuraCarpeta(string path);
	/**
	 * @brief Se encarga de eliminar tanto archivos como carpetas dentro del FileSystem.
	 */
	bool eliminar(string username, string path);
	/**
	 * @brief Se encarga de restaurar tanto archivos como carpetas que se encuentres en el trash.
	 */
	bool restaurar(string username, string path);

	bool subirArchivo (string username, string filepath, const char* data, int dataLen, string jsonMetadatos, int cuota,
			int version = FIRST);
	string descargarArchivo(string username, string filepath, int version = LATEST);
	bool actualizarArchivo (string username, string filepath, const char* data, int dataLen, int cuota,
			int versionAnterior = FIRST-1);	// TODO SACAR EL DEFAULT

	/**
	 * @brief Se encarga de actualizar la foto de perfil de los usuarios.
	 */
	bool actualizarFotoPerfil(string filepathViejo, string filepathNuevo, const char* data, int dataLen);
	/**
	 * @brief Se encarga de obtener los metadatos de un determinado archivo.
	 */
	string consultarMetadatosArchivo(string username, string filename);
	/**
	 * @brief Se encarga de actualizar los metadatos de un determinado archivo.
	 */
	bool actualizarMetadatos(string username, string filepath, string jsonNuevosMetadatos);
	/**
	 * @brief Se encarga de buscar por extensión dentro del FileSystem del usuario y los archivos que se le compartieron.
	 */
	string buscarPorExtension(string username, string extension);
	/**
	 * @brief Se encarga de buscar por etiqueta dentro del FileSystem del usuario y los archivos que se le compartieron.
	 */
	string buscarPorEtiqueta(string username, string etiqueta);
	/**
	 * @brief Se encarga de buscar por nombre de archivo dentro del FileSystem del usuario y los archivos que se le compartieron.
	 */
	string buscarPorNombre(string username, string nombre);
	/**
	 * @brief Se encarga de buscar por propietario dentro del FileSystem del usuario y los archivos que se le compartieron.
	 */
	string buscarPorPropietario(string username, string propietario);
};

#endif /* MANEJADORARCHIVOSYMETADATOS_H_ */
