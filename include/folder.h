#ifndef FOLDER_H_
#define FOLDER_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

/**
 * @brief Se encarga de los requests relacionados al manejo de carpetas dentro de la API REST.
 *
 * Para mas información acerca de la conexión para los distintos métodos leer el manual de la API REST.
 */

class Folder : public RealizadorDeEventos {
public:
	/**
	 * @brief Constructor: Crea un manejador para los requests relacionados con carpetas.
	 *
	 * @param manejadorUs			ManejadorDeUsuarios* a un manejador de usuarios.
	 * @param manejadorArchYMet		ManejadorArchivosYMetadatos* a un manejador de archivos y metadatos.
	 */
	Folder(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet);
	virtual ~Folder();

protected:
	ManejadorDeUsuarios* manejadorUs;				///< Se encarga del manejo de usuarios y las distantas BD relacionadas.
	ManejadorArchivosYMetadatos* manejadorArchYMet; ///< Se encarga del manejo de archivos y de la BD de metadatos.

	/**
	 * @brief Se encarga de enviarle al Cliente la estructura de una determinada carpeta.
	 *
	 * Este método es llamado cuando se hace un GET a /folder/<username_de_propietario>/<path>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param token				string con el token de la sesión del cliente
	 * @param user				string con el nombre de usuario del cliente
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		json con la respuesta de la operación
	 *
	 * @brief En caso de que el CODESTATUS sea igual a SUCCESS la respuesta será la estructura de la carpeta pedida.
	 *
	 * @brief Para cualquier otro CODESTATUS la respuesta será un mensaje el error correspondiente.
	 */
	mg_result GETHandler(mg_connection* connection);
	/**
	 * @brief Se encarga de crear una nueva carpeta en el FileSystem del Cliente.
	 *
	 * Este método es llamado cuando se hace un PUT a /folder/<username_de_propietario>/<path>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param token				string con el token de la sesión del cliente
	 * @param user				string con el nombre de usuario del cliente
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para cualquiera de los CODESTATUS la respuesta será un json con el mensaje correspondiente.
	 */
	mg_result PUTHandler(mg_connection* connection);
	/**
	 * @brief Se encarga de borrar una carpeta en el FileSystem del Cliente, como asi todos los elementos dentro de ella.
	 *
	 * Este método es llamado cuando se hace un DELETE a /folder/<username_de_propietario>/<path>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param token				string con el token de la sesión del cliente
	 * @param user				string con el nombre de usuario del cliente
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para cualquiera de los CODESTATUS la respuesta será un json con el mensaje correspondiente.
	 */
	mg_result DELETEHandler(mg_connection* connection);
};

#endif /* FOLDER_H_ */
