#ifndef UDRIVE_BUSCADOR_H
#define UDRIVE_BUSCADOR_H

#include <functional>
#include "validador.h"
#include "bd.h"
#include "metadatosYConstantes.h"

/**
 * @brief Se encarga de obtener la estructura de las carpetas, los archivos compartidos
 * y de las busquedas de archivos dentro del FileSystem.
 */
class Buscador {
public:
	/**
	 * @brief Constructor: Crea un Buscador.
	 *
	 * @param dbMetadatos		BD* a la base de datos de los metadatos
	 * @param pathFileSystem 	string con el path raiz del FileSystem
	 */
	Buscador(BD* dbMetadatos, string pathFileSystem);
	virtual ~Buscador();
	/**
	 * @brief Se encarga de obtener la estructura interna de una determinada carpeta.
	 *
	 * @param path 			string con el path a la carpeta
	 * @param esRecursivo	bool indicando si la busqueda es recursiva o solo en la carpeta indicada
	 * @param predicate		funcion lambda para verificar si se esta cumpliendo con la busqueda
	 *
	 * @retval string con el json de la estructura de la carpeta
	 */
	string obtenerEstructuraCarpeta(string path, bool esRecursivo, function<bool(MetadatoArchivo&)> predicate);
	/**
	 * @brief Se encarga de obtener los archivos compartidos que tiene un determinado usuario.
	 *
	 * @param path 			string que se utiliza como key en la BD para obtener la lista de archivos compartidos
	 *
	 * @retval string con el json de los archivos compartidos con el usuario
	 */
	string obtenerEstructuraCompartidos(string path);
	/**
	 * @brief Se encarga de buscar por extensión dentro del FileSystem del usuario y los archivos que se le compartieron.
	 *
	 * @param user 			string con el nombre de usuario que está realizando la busqueda
	 * @param extension		string con la extensión que se quiere buscar
	 *
	 * @retval string con el resultado de la busqueda en formato json
	 */
	string buscarPorExtension(string username, string extension);
	/**
	 * @brief Se encarga de buscar por etiqueta dentro del FileSystem del usuario y los archivos que se le compartieron.
	 *
	 * @param user 			string con el nombre de usuario que está realizando la busqueda
	 * @param etiquetaBuscada		string con la etiqueta que se quiere buscar
	 *
	 * @retval string con el resultado de la busqueda en formato json
	 */
	string buscarPorEtiqueta(string username, string etiquetaBuscada);
	/**
	 * @brief Se encarga de buscar por nombre de archivo dentro del FileSystem del usuario y los archivos que se le compartieron.
	 *
	 * @param user 			string con el nombre de usuario que está realizando la busqueda
	 * @param nombre		string con el nombre (o parte de el) archivo que se quiere buscar
	 *
	 * @retval string con el resultado de la busqueda en formato json
	 */
	string buscarPorNombre(string username, string nombre);
	/**
	 * @brief Se encarga de buscar por propietario dentro del FileSystem del usuario y los archivos que se le compartieron.
	 *
	 * Esta busqueda implica mostrar todos los archivos que me compartio un determinado propietario.
	 *
	 * @param user 			string con el nombre de usuario que está realizando la busqueda
	 * @param propietario	string con el nombre del propietario que se quiere buscar
	 *
	 * @retval string con el resultado de la busqueda en formato json
	 */
	string buscarPorPropietario(string username, string propietario);

private:
	map<string, string> buscar(string username, function<bool(MetadatoArchivo&)> predicate);
	BD* dbMetadatos;
	Validador validador;
	const string pathFileSystem;

	static bool match(string original, string patron);
};


#endif //UDRIVE_BUSCADOR_H
