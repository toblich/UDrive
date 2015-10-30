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

class RealizadorDeEventos {
public:

	typedef enum CodeStatus {
		// 2xx Exito
		CODESTATUS_SUCCESS = 200,
		CODESTATUS_RESOURCE_CREATED = 201,

		// 4xx Errores del Cliente
		CODESTATUS_BAD_REQUEST = 400, 			// Falta informacion en el request.
		CODESTATUS_UNAUTHORIZED_CLIENT = 401, 	// Cliente no autorizado
		CODESTATUS_RESOURCE_NOT_FOUND = 404, 	// Recurso no encontrado
		CODESTATUS_UNSUPPORTED_METHOD = 405, 	// Metodo no implementado para esa clase

		// 5xx Error de Servidor
		CODESTATUS_INTERNAL_SERVER_ERROR = 500
	}CodeStatus;

	typedef struct{
		const char* fileData;
		int	dataLength;
		string fileName;
		string user;
		string token;
		string nombre;
		string email;
		string latitud;
		string longitud;
	}DatosArchivo;

	RealizadorDeEventos();
	virtual ~RealizadorDeEventos();
	mg_result handler(mg_connection* connection);

protected:

	string contentType = "Content-Type";
	string jsonType = "application/json";

	//Log
	void logInfo(std::string mensaje);
	void logWarn(std::string mensaje);
	void logError(std::string mensaje);

	//API REST
	virtual mg_result POSTHandler(mg_connection* connection);
	virtual mg_result GETHandler(mg_connection* connection);
	virtual mg_result PUTHandler(mg_connection* connection);
	virtual mg_result DELETEHandler(mg_connection* connection);

	//Funciones extra de la API REST
	size_t printfData(mg_connection* connection, const char* format, ...);
	bool sendFile(mg_connection* connection, string filePath);
	DatosArchivo getMultipartData(mg_connection* connection, string variable);
	string getVar(mg_connection* connection, string varName);

	//Funciones para contestarle al usuario
	void responderUnsupportedMethod(mg_connection* connection);
	void responderAutenticacionFallida(mg_connection* connection);
	void responderBadRequest(mg_connection* connection, string errMsg);
	void responderResourceNotFound(mg_connection* connection, string errMsg);
	void responderInternalServerError(mg_connection* connection, string errMsg);
	void responderSucces(mg_connection* connection, string msg);
	void responderResourceCreated(mg_connection* connection, string msg);

};

#endif /* REALIZADORDEEVENTOS_H_ */
