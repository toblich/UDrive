#ifndef FILE_H_
#define FILE_H_

#include "realizadorDeEventos.h"

/**
 * @brief Se encarga de los requests relacionados al manejo de archivos dentro de la API REST.
 *
 * Para mas información acerca de la conexión para los distintos métodos leer el manual de la API REST.
 */

class File : public RealizadorDeEventos {
public:
	/**
	 * @brief Constructor: Crea un manejador para los requests relacionados con archivos.
	 *
	 * @param manejadorUs			ManejadorDeUsuarios* a un manejador de usuarios.
	 * @param manejadorArchYMet		ManejadorArchivosYMetadatos* a un manejador de archivos y metadatos.
	 */
	File(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet);
	virtual ~File();

protected:
	ManejadorDeUsuarios* manejadorUs;					///< Se encarga del manejo de usuarios y las distantas BD relacionadas.
	ManejadorArchivosYMetadatos* manejadorArchYMet;		///< Se encarga del manejo de archivos y de la BD de metadatos.

	/**
	 * @brief Se encarga de enviar los archivo desde el Servidor al Cliente.
	 *
	 * Este método es llamado cuando se hace un GET a /file/<username_de_propietario>/<path>
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
	 * @brief En caso de que el CODESTATUS sea igual a SUCCESS la respuesta será el archivo pedido.
	 *
	 * @brief Para cualquier otro CODESTATUS la respuesta será un mensaje el error correspondiente.
	 */
	mg_result GETHandler(mg_connection* connection);
	/**
	 * @brief Se encarga de enviar los archivos del Cliente al Servidor.
	 *
	 * Este método es llamado cuando se hace un PUT a /file/<username_de_propietario>/<path>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param token				string con el token de la sesión del cliente
	 * @param user				string con el nombre de usuario del cliente
	 * @param file				archivo que se quiere subir
	 * @param latitud			string con la latitud de la ubicación del usuario en caso de que se quiera actualizar
	 * @param longitud			string con la longitud de la ubicación del usuario en caso de que se quiera actualizar
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para cualquiera de los CODESTATUS la respuesta será un json con el mensaje correspondiente.
	 */
	mg_result PUTHandler(mg_connection* connection);
	/**
	 * @brief Se encarga de borrar los archivo en el Servidor.
	 *
	 * El borrado de archivos puede ser lógico (se manda a la papelera) o físico (se elimina definitivamente).
	 * También se pueden restaurar archivos que se encuentren en la papelera.
	 *
	 * Este método es llamado cuando se hace un DELETE a /file/<username_de_propietario>/<path>
	 *
	 * ## Método:
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 *
	 * ## Conexión:
	 * @param token				string con el token de la sesión del cliente
	 * @param user				string con el nombre de usuario del cliente
	 * @param restore 			string con "true" en caso de que se quiera restaurar un archivo de la papelera
	 *
	 * @retval CODESTATUS		int con el codigo de estado de la operación
	 * @retval respuesta		respuesta de la operación
	 *
	 * Para cualquiera de los CODESTATUS la respuesta será un json con el mensaje correspondiente.
	 */
	mg_result DELETEHandler(mg_connection* connection);

private:

	void enviarArchivo(const string& completePath, mg_connection* connection);
	MetadatoArchivo extractMetadataFrom(const vector<string>& nombreYExtension, const string& user, const vector<string>& uris);
	void subirArchivo (const vector<string>& uris, const DatosArchivo& datosArch, const string& user, mg_connection* connection);
	void actualizarUltimaUbicacion(string user, string latitud, string longitud);
};

#endif /* FILE_H_ */
