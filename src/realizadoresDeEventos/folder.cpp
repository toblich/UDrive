#include "folder.h"

Folder::Folder (ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorUs = manejadorUs;
	this->manejadorArchYMet = manejadorArchYMet;
}

Folder::~Folder () {
}

mg_result Folder::GETHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, URI_DELIM);
	Logger::logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");
		string filepath = ParserURI::join(uris, URI_DELIM, 1, uris.size());
		string estructura = manejadorArchYMet->obtenerEstructuraCarpeta(filepath);
		if (estructura != "") {
			Logger::logInfo("Se envió la estructura de la carpeta: " + filepath + " correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCESS);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"estructura\": %s}", estructura.c_str());
		} else {
			string mensaje = "Path inválido, no se encontró la carpeta: " + filepath;
			this->responderBadRequest(connection, mensaje);
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Folder::PUTHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, URI_DELIM);
	Logger::logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");
		string filepath = ParserURI::join(uris, URI_DELIM, 1, uris.size());
		if (manejadorArchYMet->crearCarpetaSegura(user, filepath)) {
			string mensaje = "Se creo la carpeta: " + filepath + " correctamente.";
			this->responderResourceCreated(connection, mensaje);
		} else {
			string mensaje = "Path inválido, no se pudo crear la carpeta: " + filepath;
			this->responderBadRequest(connection, mensaje);
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Folder::DELETEHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, URI_DELIM);
	Logger::logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");
		string filepath = ParserURI::join(uris, URI_DELIM, 1, uris.size());
		if (manejadorArchYMet->eliminar(user, filepath)) {
			string mensaje = "Se eliminó la carpeta: " + filepath + " correctamente.";
			this->responderSuccess(connection, mensaje);
		} else {
			string mensaje = "Path inválido, no se pudo eliminar la carpeta: " + filepath;
			this->responderResourceNotFound(connection, mensaje);
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}
