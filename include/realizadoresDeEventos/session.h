#ifndef SESSION_H_
#define SESSION_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

/**
 * @brief Se encarga de los requests relacionados al manejo de sesiones dentro de la API REST.
 *
 * Para mas información acerca de la conexión para los distintos métodos leer el manual de la API REST.
 */

class Session : public RealizadorDeEventos {
public:
	/**
	 * @brief Constructor: Crea un manejador para los requests relacionados con sesiones de usuarios.
	 *
	 * @param manejadorUsuarios		ManejadorDeUsuarios* a un manejador de usuarios.
	 */
	Session(ManejadorDeUsuarios* manejadorUsuarios);
	virtual ~Session();

protected:
	ManejadorDeUsuarios* manejadorUs; ///< Se encarga del manejo de usuarios y las distantas BD relacionadas.

	/**
	 * @brief Se encarga de iniciar la sesión de un Cliente.
	 *
	 * Este método es llamado cuando se hace un POST a /session
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param user				string con el nombre de usuario del Cliente que quiere iniciar sesión
	 * @param pass				string con la password del Cliente que quiere iniciar sesión
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para el caso de que el CODESTATUS sea RESOURCE_CREATED la respuesta sera el token de la sesión
	 *
	 * Para cualquier otro CODESTATUS la respuesta será un mensaje el error correspondiente.
	 */
	mg_result POSTHandler(mg_connection* connection);
	/**
	 * @brief Se encarga de cerrar la sesión de un Cliente.
	 *
	 * Este método es llamado cuando se hace un DELETE a /session/<token>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param user				string con el nombre de usuario de la sesión que se quiere cerrar
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para cualquiera de los CODESTATUS la respuesta será un json con el mensaje correspondiente.
	 */
	mg_result DELETEHandler(mg_connection* connection);
};

#endif /* SESSION_H_ */
