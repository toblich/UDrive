#ifndef INCLUDE_PARSERJSON_H_
#define INCLUDE_PARSERJSON_H_

#include "json/json.h"
#include <map>

#include "metadatosYConstantes.h"

using std::string;

/**
 * @brief Encapsula integramente el manejo de archivos Json.
 *
 * Se encarga del parseo de archivos/strings del formato Json, así como también de
 * transformar determinados structs o mapas en un archivo/string Json válido.
 */

class ParserJson {

private:
	static string verificarString(string key, Json::Value raiz);
	static double verificarDouble(string key, Json::Value raiz);
	static int verificarInt(string key, Json::Value raiz, int porDefecto);
	ParserJson() {}

public:
	virtual ~ParserJson() {}

	/**
	 * @brief Lleva de un tipo MetadatoArchivo a su correspondiente representación en
	 * un formato Json.
	 *
	 * En caso de no tener algún atributo lleno, se lo serializa quedando el mismo vacío.
	 *
	 * @param metadato			MetadatoArchivo el cual se quiere transformar en Json.
	 *
	 * @retval string			string con el metadato transformado en Json.
	 */
	static string serializarMetadatoArchivo(MetadatoArchivo metadato);
	/**
	 * @brief Lleva de un tipo MetadatoUsuario a su correspondiente representación en
	 * un formato Json.
	 *
	 * En caso de no tener algún atributo lleno, se lo serializa quedando el mismo vacío.
	 *
	 * @param metadato			MetadatoUsuario el cual se quiere transformar en Json.
	 *
	 * @retval string			string con el metadato transformado en Json.
	 */
	static string serializarMetadatoUsuario(MetadatoUsuario metadato);
	/**
	 * @brief Lleva de un tipo MetadatoSesion a su correspondiente representación en
	 * un formato Json.
	 *
	 * En caso de no tener algún atributo lleno, se lo serializa quedando el mismo vacío.
	 *
	 * @param metadato			MetadatoSesion el cual se quiere transformar en Json.
	 *
	 * @retval string			string con el metadato transformado en Json.
	 */
	static string serializarMetadatoSesion(MetadatoSesion metadato);
	/**
	 * @brief Lleva de un tipo map<string, string> a su correspondiente representación en
	 * un formato Json.
	 *
	 * @param mapa				map<string, string> el cual se quiere transformar en Json.
	 *
	 * @retval string			string con el mapa transformado en Json. Éste tendrá el
	 * formato "clave": "valor", tal como el mapa lo indique.
	 * @retval ""				string vacío si el mapa estaba vacío.
	 */
	static string serializarMapa(std::map<string, string>& mapa);

	/**
	 * @brief Lleva de un tipo string con el formato Json a un MetadatoArchivo.
	 *
	 * En caso de no tener algún atributo (clave dentro del Json), se lo serializa quedando
	 * el mismo vacío.
	 *
	 * @param json				string con un formato Json.
	 *
	 * @retval MetadatoArchivo	MetadatoArchivo con los campos llenos según el Json.
	 * @retval MetadatoArchivo	MetadatoArchivo vacío si el string no era de formato Json.
	 */
	static MetadatoArchivo deserializarMetadatoArchivo(string json);
	/**
	 * @brief Lleva de un tipo string con el formato Json a un MetadatoUsuario.
	 *
	 * En caso de no tener algún atributo (clave dentro del Json), se lo serializa quedando
	 * el mismo vacío.
	 *
	 * @param json				string con un formato Json.
	 *
	 * @retval MetadatoUsuario	MetadatoUsuario con los campos llenos según el Json.
	 * @retval MetadatoUsuario	MetadatoUsuario vacío si el string no era de formato Json.
	 */
	static MetadatoUsuario deserializarMetadatoUsuario(string json);
	/**
	 * @brief Lleva de un tipo string con el formato Json a un MetadatoSesion.
	 *
	 * En caso de no tener algún atributo (clave dentro del Json), se lo serializa quedando
	 * el mismo vacío.
	 *
	 * @param json				string con un formato Json.
	 *
	 * @retval MetadatoSesion	MetadatoSesion con los campos llenos según el Json.
	 * @retval MetadatoSesion	MetadatoSesion vacío si el string no era de formato Json.
	 */
	static MetadatoSesion deserializarMetadatoSesion(string json);
	/**
	 * @brief Lleva de un tipo string con el formato Json a un map<string, string>.
	 *
	 * @param json				string con un formato Json.
	 *
	 * @retval map<string,string>	map<string,string> con el mismo formato clave-valor
	 * del Json.
	 * @retval map<string,string>	map<string,string> vacío si el string no era de
	 * formato Json.
	 */
	static std::map<string, string> deserializarMapa(string json);

	/**
	 * @brief Genera un solo Json a partir de un map<string, string> y un string de
	 * formato Json.
	 *
	 * @param estructuraPermisos		map<string,string> el cual se quiere mergear
	 * para formar un Json.
	 * @param jsonEstructuraFileSystem	string con un formato Json, el cual se quiere
	 * mergear para formar otro Json.
	 *
	 * @retval string			string con el mapa transformado en Json, mergeado con
	 * el otro Json.
	 */
	static string estructurasMerge(std::map<string, string> estructuraPermisos, string jsonEstructuraFileSystem);
};

#endif /* INCLUDE_PARSERJSON_H_ */
