#ifndef METADATA_H_
#define METADATA_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

/**
 * @brief Se encarga de los requests relacionados al manejo de metadatos dentro de la API REST.
 *
 * Para mas información acerca de la conexión para los distintos métodos leer el manual de la API REST.
 */

class Metadata : public RealizadorDeEventos {
public:
	/**
	 * @brief Constructor: Crea un manejador para los requests relacionados con metadatos.
	 *
	 * @param manejadorUs			ManejadorDeUsuarios* a un manejador de usuarios.
	 * @param manejadorArchYMet		ManejadorArchivosYMetadatos* a un manejador de archivos y metadatos.
	 */
	Metadata(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet);
	virtual ~Metadata();

protected:
	ManejadorDeUsuarios* manejadorUs;				///< Se encarga del manejo de usuarios y las distantas BD relacionadas.
	ManejadorArchivosYMetadatos* manejadorArchYMet; ///< Se encarga del manejo de archivos y de la BD de metadatos.

	/**
	 * @brief Función interna encargada de enviar los metadatos al Cliente.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param uris				vector<string> vector donde cada elemento es una parte de la uri
	 * @param user 				string con el nombre de usuario del cliente
	 *
	 */
	void GETMetadatos(mg_connection* connection, vector<string> uris, string user);
	/**
	 * @brief Función interna encargada de las busquedas por etiqueta
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param user 				string con el nombre de usuario del cliente
	 * @param etiqueta			string con la etiqueta que se quiere buscar.
	 *
	 */
	void busquedaEtiqueta(mg_connection* connection, string user, string etiqueta);
	/**
	 * @brief Función interna encargada de las busquedas por extensión
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param user 				string con el nombre de usuario del cliente
	 * @param extension			string con la extensión que se quiere buscar.
	 *
	 */
	void busquedaExtension(mg_connection* connection, string user, string extension);
	/**
	 * @brief Función interna encargada de las busquedas por nombre
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param user 				string con el nombre de usuario del cliente
	 * @param nombre			string con el nombre o parte del nombre del archivo que se quiere buscar.
	 *
	 */
	void busquedaNombre(mg_connection* connection, string user, string nombre);
	/**
	 * @brief Función interna encargada de las busquedas por propietario
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param user 				string con el nombre de usuario del cliente
	 * @param propietario		string con el nombre del propiestario que se quiere buscar.
	 *
	 */
	void busquedaPropietario(mg_connection* connection, string user, string propietario);
	/**
	 * @brief Función interna encargada de enviar el resultado de la busqueda al Cliente.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param resultado			string con el resultado de la busqueda que se le enviará al Cliente.
	 *
	 */
	void responderBusqueda(mg_connection* connection, string resultado);
	/**
	 * @brief Se encarga de enviarle al Cliente los metadatos de los archivos tanto propios como compartidos.
	 * Además se pueden realizar busquedas sobre estos metadatos.
	 *
	 * Este método es llamado cuando se hace un GET a /metadata/<username_de_propietario>/<filename>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * En el caso de que se quieran obtener los metadatos de un archivo:
	 * @param token				string con el token de la sesión del cliente
	 * @param user				string con el nombre de usuario del cliente
	 *
	 * En el caso de que se quiera realizar alguna busqueda hay que agregar alguno de estos parámetros:
	 * @param etiqueta			string con la etiqueta que se quiere buscar.
	 * @param extension			string con la extensión que se quiere buscar.
	 * @param nombre			string con el nombre o parte del nombre del archivo que se quiere buscar.
	 * @param propietario		string con el nombre del propiestario que se quiere buscar.
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		json con la respuesta de la operación
	 *
	 * @brief En caso de que el CODESTATUS sea igual a SUCCESS la respuesta seran:
	 * 					+ Los metadatos pedidos
	 * 					+ El resultado de la busqueda.
	 *
	 * @brief Para cualquier otro CODESTATUS la respuesta será un mensaje el error correspondiente.
	 */
	mg_result GETHandler(mg_connection* connection);
	/**
	 * @brief Se encarga de actualizar los metadatos de los archivos del Cliente, tanto propios como compartidos.
	 *
	 * Este método es llamado cuando se hace un PUT a /metadata/<username_de_propietario>/<filename>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param token				string con el token de la sesión del cliente
	 * @param user				string con el nombre de usuario del cliente
	 * @param metadatos			string con un json con los metadatos actualizados
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para cualquiera de los CODESTATUS la respuesta será un json con el mensaje correspondiente.
	 */
	mg_result PUTHandler(mg_connection* connection);
};

#endif /* METADATA_H_ */
