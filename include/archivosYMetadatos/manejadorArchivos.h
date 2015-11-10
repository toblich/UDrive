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

	bool renombrarArchivo (const string& pathInterno, const string& nuevoFilename, int ultimaVersion);
	bool restaurarArchivo (const string& pathRealSinFS, const string& pathEnPapeleraSinFS, int ultimaVersion);
	void deshacerRestaurado (const string& pathRealSinFS, const string& pathEnPapeleraSinFS);

	void eliminarArchivoDefinitivamente (const string& filepath, bool usarWildcardFinal = false);
	bool mandarArchivoATrash (const string& filepath, const string& pathCompletoPapelera, int ultimaVersion);
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
