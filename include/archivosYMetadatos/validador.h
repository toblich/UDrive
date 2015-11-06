
#ifndef INCLUDE_VALIDADOR_H_
#define INCLUDE_VALIDADOR_H_

#include "parserJson.h"
#include "parserURI.h"
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include "bd.h"
#include "logger.h"
#include <dirent.h>
#include <algorithm>
#include <sstream>

using namespace std;

/**
 * @brief Se encarga de efectuar ciertas validaciones.
 */

class Validador {
private:
	BD* dbMetadatos;
	bool tienePermisos(string username, string path);

public:
	/**
	 * @brief Crea un Validador.
	 *
	 * @param dbMetadatos		BD* a una base de datos de metadatos.
	 */
	Validador(BD* dbMetadatos);
	virtual ~Validador();

	/**
	 * @brief Verifica existencia de una version de un archivo (por default, la primera).
	 *
	 * @param filepath			string con la ruta del archivo relativa a donde se ejecutó
	 * (Es decir, que debe incluir el nombre del FileSystem en él).
	 * @param version			numero de version a buscar
	 *
	 * @retval true				si existe el archivo.
	 * @retval false			si no existe el archivo.
	 */
	bool existeArchivo(string filepath, int version = FIRST);

	/**
	 * @brief Verifica existencia de una carpeta.
	 *
	 * @param path				string con la ruta de la carpeta relativa a donde se ejecutó
	 * (Es decir, que debe incluir el nombre del FileSystem en él).
	 *
	 * @retval true				si existe la carpeta.
	 * @retval false			si no existe la carpeta.
	 */
	bool existeCarpeta(string path);

	/**
	 * @brief Verifica si una carpeta contiene o no elementos.
	 *
	 * @param path				string con la ruta de la carpeta relativa a donde se ejecutó
	 * (Es decir, que debe incluir el nombre del FileSystem en él).
	 *
	 * @retval true				si la carpeta está vacía.
	 * @retval false			si la carpeta contiene algún elemento (no está vacía).
	 */
	bool carpetaVacia(string path);

	bool existeMetadato(const string& path) {return dbMetadatos->contains(path); }

	/**
	 * @brief Verifica si un determinado path contiene o no un determinado caracter.
	 *
	 * @param path				string con la ruta a validar.
	 *
	 * @retval true				si el path no contiene el char RESERVED_CHAR.
	 * @retval false			si el path contiene el char RESERVED_CHAR.
	 */
	bool esPathValido(string path);

	/**
	 * @brief Verifica si un usuario tiene o no permisos sobre un determinado path (Generalmente
	 * será un path a un archivo).
	 *
	 * El tener permisos significa que el username sea el dueño del path/archivo, o bien se le
	 * hayan cedido los permisos correspondientes, lo cual debería estar reflejado en la base
	 * de datos.
	 *
	 * @param path				string con la ruta sobre la cual se quieren verificar los permisos.
	 * Esta no debe contener la ruta al FileSystem, sino que debe comenzar directamente con un
	 * nombre de usuario.
	 *
	 * @retval true				si el usuario tiene permisos sobre ese path.
	 * @retval false			si el usuario no tiene permisos sobre ese path.
	 */
	bool verificarPermisos(string username, string path);

	/**
	 * @brief Verifica si un archivo puede ser restaurado de la papelera.
	 *
	 * Un archivo podrá ser restaurado si:
	 * - En la base de datos no existe el path que tendría el archivo al ser restaurado.
	 * - No existe el path al archivo en el FileSystem a donde sería restaurado.
	 * - Existe en la base de datos el path al archivo dentro de la papelera.
	 * - Existe el path al archivo en el FileSystem dentro de la papelera.
	 *
	 * @param pathEnPapeleraSinFS	string con la ruta que tiene ese archivo en la papelera, sin
	 * incluir la carpeta del File System.
	 * @param pathRealSinFS			string con la ruta que tendría ese archivo al ser restaurado,
	 * sin incluir la carpeta del File System.
	 * @param pathFileSystem		string con la ruta de la carpeta del FileSystem.
	 *
	 * @retval true				si es posible la restauración del archivo.
	 * @retval false			si no es posible la restauración del archivo.
	 */
	bool puedoRestaurarA(string pathEnPapeleraSinFS, string pathRealSinFS, string pathFileSystem);

	/**
	 * @brief Obtiene el número de secuencia que debe tener el archivo.
	 *
	 * Para obtener el número de secuencia se fija todos los archivos que haya con ese nombre y
	 * se queda con el último número de secuencia + 1. Si no hay ningún archivo con ese nombre
	 * se le asigna el número de secuencia 0.
	 *
	 * @param pathFileSystem				string con la ruta de la carpeta del FileSystem.
	 * @param propietario					string con el nombre del propietario del archivo.
	 * @param pathSinUsernameConReserved	string con el path al archivo, reemplazando las '/'
	 * que separan en el path por RESERVED_CHAR, de forma de poder luego restaurar a la ubicación
	 * original.
	 *
	 * @retval string			string con el siguiente número de secuencia.
	 * @retval "0"				string con el valor numérico 0, indicando que es el primer archivo
	 * con ese nombre.
	 */
	string obtenerNumeroSecuencia(string pathFileSystem, string propietario, string pathSinUsernameConReserved);
};

#endif /* INCLUDE_VALIDADOR_H_ */
