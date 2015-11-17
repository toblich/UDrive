#ifndef UDRIVE_MANEJADORARCHIVOS_H
#define UDRIVE_MANEJADORARCHIVOS_H

#include "validador.h"

/**
 * @brief Se encarga del manejo de las carpetas y los archivos dentro del FileSystem.
 */
class ManejadorArchivos {
public:
	/**
	 * @brief Constructor: Crea un manejador de archivos y carpetas.
	 *
	 * @param pathFileSystem 	string con el path raiz del FileSystem
	 * @param validador			Validador* que se utilizara para las validaciones
	 */
	ManejadorArchivos(string pathFileSystem, Validador* validador);
	virtual ~ManejadorArchivos();
	/**
	 * @brief Se encarga de calcular el tamaño de una determinada carpeta.
	 *
	 * @param path 		string con el path a la carpeta
	 * @param size 		int pasado por referencia donde se guarda el tamaño de la carpeta
	 *
	 * @retval true 	en caso de que se haya podido calcular el tamaño
	 * @retval false 	en caso de que el path a la carpeta no existe
	 */
	bool tamanioCarpeta(string path, unsigned long int & size);

	/**
	 * @brief Se encarga de crear una nueva carpeta en el path indicado dentro del FileSystem del user.
	 *
	 * @retval true 	en caso de que se haya podido crear la carpet
	 * @retval false 	en caso de que no tenga los permisos para crear la carpeta
	 */
	bool crearCarpeta(string username, string path);

	/**
	 * @brief Se encarga de eliminar la carpeta con el path indicado.
	 *
	 * @retval true 	en caso de que se haya podido eliminar la carpeta
	 * @retval false 	en caso de que la carpeta no exista o no se haya podido eliminar
	 */
	bool deleteCarpeta(string path);

	/**
	 * @brief Se encarga de renombrar el archivo pathInterno (Todas las versiones) con el nuevoFilename.
	 *
	 * @retval true 	en caso de que se haya podido renombrar el archivo
	 * @retval false 	en caso de que el archivo no exista o no se haya podido renombrar
	 */
	bool renombrarArchivo (const string& pathInterno, const string& nuevoFilename, int ultimaVersion);

	/**
	 * @brief Se encarga de restaurar el archivo del trash a la ubicacion original (Todas las versiones).
	 *
	 * @retval true 	en caso de que se haya podido restaurar el archivo
	 * @retval false 	en caso de que el archivo no se haya podido restaurar
	 */
	bool restaurarArchivo (const string& pathRealSinFS, const string& pathEnPapeleraSinFS, int ultimaVersion);

	/**
	 * @brief Se encarga de, en caso que haya fallado el restaurado, volver al estado anterior.
	 */
	void deshacerRestaurado (const string& pathRealSinFS, const string& pathEnPapeleraSinFS, int ultimaVersion);

	/**
	 * @brief Se encarga de eliminar un archivo definitivamente, junto con todas sus versiones
	 * (Deberia encontrarse en el trash).
	 */
	void eliminarArchivoDefinitivamente (const string& filepath, bool usarWildcardFinal = false);

	/**
	 * @brief Se encarga de eliminar el archivo, mandandolo a la papelera (Todas las versiones).
	 *
	 * @retval true 	en caso de que se haya podido mandar el archivo al trash
	 * @retval false 	en caso de que no se haya podido mandar el archivo al trash
	 */
	bool mandarArchivoATrash (const string& filepath, const string& pathCompletoPapelera, int ultimaVersion);

	/**
	 * @brief Guarda el archivo directamente en el FileSystem (Es decir, escribe los bits en un
	 * archivo propiamente dicho).
	 */
	void guardarArchivoEnFileSystem(const string& filepath, const char* data, int dataLen);

	/**
	 * @brief Se encarga de asignarle la foto de perfil por default a un determinado user.
	 */
	void crearFotoPerfilDefault(string username);

private:
	bool deleteFileSystem();

	string pathFileSystem;
	Validador* validador;
};


#endif //UDRIVE_MANEJADORARCHIVOS_H
