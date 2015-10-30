#include "session.h"

Session::Session (ManejadorDeUsuarios* manejadorUsuarios) {
	this->manejadorUs = manejadorUsuarios;
}

Session::~Session () {
}

mg_result Session::POSTHandler (mg_connection* connection) {
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);
	string pass = getVar(connection, "pass");
	Logger::logInfo("Se obtuvo la variable pass con valor: " + pass);

	if (manejadorUs->validarLogin(user, pass)) {
		string token = manejadorUs->iniciarSesion(user);
		Logger::logInfo("Se inicio la sesion con token: " + token);
		mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"token\": \"%s\"}", token.c_str());
	} else {
		string mensaje = "No se pudo inicia sesion, usuario y/o contraseña incorrecto/s.";
		this->responderBadRequest(connection, mensaje);
	}

	return MG_TRUE;
}

mg_result Session::DELETEHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, URI_DELIM);
	Logger::logInfo("Se parseó la uri exitosamente.");
	string username = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + username);

	if (uris.size() == 2) {
		if (manejadorUs->cerrarSesion(uris[1], username)) {
			string mensaje = "La sesion se cerró correctamente.";
			this->responderSucces(connection, mensaje);
		} else {
			string mensaje = "La sesion que se quizo cerrar ya no existe";
			this->responderResourceNotFound(connection, mensaje);
		}
	} else {
		string mensaje = "La uri es incorrecta, debe ser /session/token.";
		this->responderBadRequest(connection, mensaje);
	}
	return MG_TRUE;
}
