#ifndef REALIZADORDEEVENTOS_H_
#define REALIZADORDEEVENTOS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <list>
#include <ctime>
#include "logger.h"
#include "mongoose.h"
#include "manejadorDeUsuarios.h"
#include "manejadorArchivosYMetadatos.h"
#include "parserURI.h"

using namespace std;

#define MAX_LENGTH 1024
#define URI_DELIM '/'
#define NAME_DELIM '.'
#define CUOTA_PREMIUM 5120 //5 GB
#define CUOTA_NORMAL  2048 //2 GB

/**
 * @brief Clase abstracta de la cual heredan las clases que manejan la API REST.
 */

class RealizadorDeEventos {
public:
	/**
	 * @brief Enumerado encargado de manejar todos los CodeStatus de las requests Cliente-Servidor
	 */
	typedef enum CodeStatus {
		// 2xx Exito
		CODESTATUS_SUCCESS = 200, 				///< Exito
		CODESTATUS_RESOURCE_CREATED = 201,		///< Nuevo recurso creado

		// 4xx Errores del Cliente
		CODESTATUS_BAD_REQUEST = 400, 			///< Falta informacion en el request.
		CODESTATUS_UNAUTHORIZED_CLIENT = 401, 	///< Cliente no autorizado
		CODESTATUS_RESOURCE_NOT_FOUND = 404, 	///< Recurso no encontrado
		CODESTATUS_UNSUPPORTED_METHOD = 405, 	///< Metodo no implementado para esa clase
		CODESTATUS_CONFLICT = 409,				///< Conflicto de versiones de archivo subido

		// 5xx Error de Servidor
		CODESTATUS_INTERNAL_SERVER_ERROR = 500  ///< Error interno del servidor al querer subir o bajar un archivo
	}CodeStatus;

	/**
	 * @brief Struct encargado de guardar datos que se cargan a partir del getMultipartData de Mongoose
	 */
	typedef struct{
		const char* fileData; ///< Contenido del archivo que se quiere subir
		int	dataLength;		  ///< Tamaño del archivo que se quiere subir
		string fileName;	  ///< Nombre del archivo que se quiere subir
		string user; 		  ///< Nombre de usuario que esta haciendo la request
		string token;		  ///< Token de sesión del usuario que hace la request
		string nombre;		  ///< Nombre del usuario que quiere actualizar su perfil
		string email;  		  ///< Email del usuario que quiere actualizar su perfil
		string latitud;		  ///< Latitud de la ubicación del usuario que quiere actualizar su ubicación
		string longitud;	  ///< Longitud de la ubicación del usuario que quiere actualizar su ubicación
		string force;		  ///< Para para saber si se fuerza o no la subida de un archivo
	}DatosArchivo;

	RealizadorDeEventos();
	virtual ~RealizadorDeEventos();
	/**
	 * @brief Función que redirige según el metodo o verbo HTTP utilizado
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 */
	mg_result handler(mg_connection* connection);

protected:

	string contentType = "Content-Type";  ///< String con el nombre del header que se le manda al Cliente
	string jsonType = "application/json"; ///< String con el tipo de "Content-Type" que se le mando al Cliente

	//API REST
	/**
	 * @brief Función que se utiliza cuando el verbo HTTP utilizado es POST.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 */
	virtual mg_result POSTHandler(mg_connection* connection);
	/**
	 * @brief Función que se utiliza cuando el verbo HTTP utilizado es GET.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 */
	virtual mg_result GETHandler(mg_connection* connection);
	/**
	 * @brief Función que se utiliza cuando el verbo HTTP utilizado es PUT.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 */
	virtual mg_result PUTHandler(mg_connection* connection);
	/**
	 * @brief Función que se utiliza cuando el verbo HTTP utilizado es DELETE.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 * @retval MG_TRUE			en cualquiera de los casos.
	 */
	virtual mg_result DELETEHandler(mg_connection* connection);

	//Funciones extra de la API REST
	/**
	 * @brief Función que se utiliza para enviar los JSON al Cliente.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor
	 * @param format			const char* con el formato de la informacion que se le envia al Cliente
	 *
	 * @retval ret				size_t con la cantidad de datos que se le enviaron al Cliente
	 */
	size_t printfData(mg_connection* connection, const char* format, ...);
	/**
	 * @brief Función que se utiliza para enviarle un archivo al Cliente.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param filePath 			string con el path al archivo que se le manda al Cliente
	 *
	 * @retval true				en caso de que el envio sea exitoso
	 * @retval false 			en caso de que no se haya podido enviar el archivo
	 */
	bool sendFile(mg_connection* connection, string filePath);
	/**
	 * @brief Función que se utiliza para cargar los datos que se le envian al Servidor.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param variable 			string con el nombre de la variable que se quiere cargar, puede ser
	 * 						*file* cuando se quiere subir un archivo y *picture* en caso de la foto de perfil
	 *
	 * @retval datosArch		DatosArchivo con los datos que se cargaron de la request, ya sea el archivo
	 * 							y los demas parámetros
	 */
	DatosArchivo getMultipartData(mg_connection* connection, string variable);
	/**
	 * @brief Función que se utiliza para obtener los parámetros enviados por el Cliente.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param varName			string con el nombre del parámetro que se quiere cargar
	 *
	 * @retval variable			string con el parámetro correspondiente al varName
	 * @retval ""				string vacío cuando el parámetro no existe
	 */
	string getVar(mg_connection* connection, string varName);
	/**
	 * @brief Función que se utiliza para avisarle al Cliente que el método HTTP de la request no
	 * está implementada por esta clase.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 */
	void responderUnsupportedMethod(mg_connection* connection);
	/**
	 * @brief Función que se utiliza para avisarle al Cliente que falló la autenticación,
	 * debido a que el username o el token de sesión son erroneos.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 */
	void responderAutenticacionFallida(mg_connection* connection);
	/**
	 * @brief Función que se utiliza para enviarle un mensaje de error al Cliente
	 * debido a un BAD_REQUEST
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 */
	void responderBadRequest(mg_connection* connection, string errMsg);
	/**
	 * @brief Función que se utiliza para enviarle un mensaje de error al Cliente
	 * debido a un RESOURCE_NOT_FOUND
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 */
	void responderResourceNotFound(mg_connection* connection, string errMsg);
	/**
	 * @brief Función que se utiliza para enviarle un mensaje de error al Cliente
	 * debido a un INTERNAL_SERVER_ERROR
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 */
	void responderInternalServerError(mg_connection* connection, string errMsg);
	/**
	 * @brief Función que se utiliza para enviarle un mensaje al Cliente
	 * debido a un SUCCESS
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 */
	void responderSuccess(mg_connection* connection, string msg);
	/**
	 * @brief Función que se utiliza para enviarle un mensaje al Cliente
	 * debido a un RESOURCE_CREATED
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 */
	void responderResourceCreated(mg_connection* connection, string msg);

	/**
	 * @brief Función que se utiliza para enviarle un mensaje al Cliente
	 * debido a un CONFLICT
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 *
	 */
	void responderConflict(mg_connection* connection, string msg, int ultimaVersion);

};

#endif /* REALIZADORDEEVENTOS_H_ */
