#ifndef PROFILE_H_
#define PROFILE_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

/**
 * @brief Se encarga de los requests relacionados al manejo de perfiles dentro de la API REST.
 *
 * Para mas información acerca de la conexión para los distintos métodos leer el manual de la API REST.
 */

class Profile : public RealizadorDeEventos {
public:
	/**
	 * @brief Constructor: Crea un manejador para los requests relacionados con perfiles y usuarios.
	 *
	 * @param manejadorUsuarios		ManejadorDeUsuarios* a un manejador de usuarios.
	 * @param manejadorAyM			ManejadorArchivosYMetadatos* a un manejador de archivos y metadatos.
	 */
	Profile(ManejadorDeUsuarios* manejadorUsuarios, ManejadorArchivosYMetadatos* manejadorAyM);
	virtual ~Profile();

protected:
	ManejadorDeUsuarios* manejadorUs;			///< Se encarga del manejo de usuarios y las distantas BD relacionadas.
	ManejadorArchivosYMetadatos* manejadorAyM;  ///< Se encarga del manejo de archivos y de la BD de metadatos.

	/**
	 * @brief Función interna encargada de enviar el resultado de la busqueda de usuarios al Cliente.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param nombre 			string con el username (o parte de el) que se quiere buscar
	 *
	 */
	void enviarUsuariosCon(mg_connection* connection, string nombre);
	/**
	 * @brief Función interna encargada de enviar la foto de perfil al Cliente.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param filePath			string con el path a la foto de perfil
	 * @param user 				string con el username que hace la request
	 *
	 */
	void enviarFotoPerfil(mg_connection* connection, string filePath, string user);
	/**
	 * @brief Función interna encargada de enviar el perfil al Cliente.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param userPerfil		string con el username del que se quiere obtener el perfil
	 *
	 */
	void enviarPerfil(mg_connection* connection, string userPerfil);
	/**
	 * @brief Se encarga de: obtener el perfil de un Cliente, obtener su foto de perfil y
	 * buscar usuarios por (parte de) su username.
	 *
	 * Este método es llamado cuando se hace un GET a:
	 * 						+ /profile/<username> para obtener perfiles
	 * 						+ /profile/pathFotoPerfil para obtener la foto de perfil
	 * 						+ /profile para la busqueda de usuarios.
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param token				string con el token de la sesión del cliente
	 * @param user				string con el user del Cliente que realiza la request
	 * @param nombre			string con el username (o parte de el) que se quiere buscar
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para el caso de que el CODESTATUS sea SUCCESS, la respuesta puede ser:
	 * 			+ Un JSON con el perfil del usuario
	 * 			+ La foto de perfil del usuario
	 * 			+ Un JSON con la lista de usuarios que coincidieron con el buscado
	 */
	mg_result GETHandler(mg_connection* connection);
	/**
	 * @brief Se encarga de actualizar tanto el perfil del Cliente como su foto de perfil.
	 *
	 * Este método es llamado cuando se hace un PUT a /profile/<username>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param token				string con el token de la sesión del cliente
	 * @param nombre			string con el nombre viejo o actualizado
	 * @param email				string con el email viejo o actualizado
	 * @param picture			archivo, en caso de que se quiera, para actualizar la foto de perfil
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para cualquiera de los CODESTATUS la respuesta será un json con el mensaje correspondiente.
	 */
	mg_result PUTHandler(mg_connection* connection);
	/**
	 * @brief Se encarga de registrar nuevos Clientes en el Servidor.
	 *
	 * Este método es llamado cuando se hace un POST a /profile
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param user				string con el nombre de usuario del cliente
	 * @param pass				string con la contraseña del cliente
	 * @param profile			string con el perfil del cliente en formato JSON
	 * @param premium			string con valor "true" si el usuario es premium
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para cualquiera de los CODESTATUS la respuesta será un json con el mensaje correspondiente.
	 */
	mg_result POSTHandler(mg_connection* connection);
};

#endif /* PROFILE_H_ */
