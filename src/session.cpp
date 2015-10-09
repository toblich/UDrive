#include "session.h"

Session::Session (ManejadorDeUsuarios* manejadorUsuarios) {
	this->manejadorUs = manejadorUsuarios;
}

Session::~Session () {
}

mg_result Session::POSTHandler (mg_connection* connection) {
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);
	string pass = getVar(connection, "pass");
	this->logInfo("Se obtuvo la variable pass con valor: " + pass);

	if (manejadorUs->validarLogin(user, pass)) {
		string token = manejadorUs->iniciarSesion(user);
		this->logInfo("Se inicio la sesion con token: " + token);
		mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"token\": \"%s\"}", token.c_str());
	} else {
		this->logInfo("No se pudo inicia sesion, usuario y/o contraseña incorrecto/s.");
		mg_send_status(connection, CODESTATUS_BAD_REQUEST);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\":\"No es posible conectar, usuario y/o contraseña incorrecto/s.\"}");
	}

	return MG_TRUE;
}

mg_result Session::DELETEHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri exitosamente.");
	string username = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + username);

	if (uris.size() == 2) {
		if (manejadorUs->cerrarSesion(uris[1], username)) {
			this->logInfo("La sesion se cerró correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"succes\": \"Sesion cerrada correctamente.\"}");
		} else {
			this->logInfo("La sesion que se quizo cerrar ya no existe");
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\":\"La sesión no existe o ya ha sido cerrada.\"}");
		}
	} else {
		this->logInfo("La uri a la que se quizo acceder es incorrecta.");
		mg_send_status(connection, CODESTATUS_BAD_REQUEST);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\":\"La uri es incorrecta, debe ser /session/token.\"}");
	}
	return MG_TRUE;
}
