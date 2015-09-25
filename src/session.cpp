#include "session.h"

Session::Session(ManejadorDeUsuarios* manejadorUsuarios) {
	this->manejadorUs = manejadorUsuarios;
}

Session::~Session() {
	// TODO Auto-generated destructor stub
}

mg_result Session::POSTHandler(mg_connection* connection) {
	string user = getVar(connection, "user");
	string pass = getVar(connection, "pass");

	if(manejadorUs->validarLogin(user, pass)){
		string token = manejadorUs->iniciarSesion(user);
		mg_printf_data(connection, "{\"Token de sesion\": \"%s\"}", token.c_str());
	}else{
		mg_printf_data(connection, "No es posible conectar, usuario o contraseña incorrecto\n");
	}

	return MG_TRUE;
}

mg_result Session::DELETEHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri);
	string username = getVar(connection, "user");

	manejadorUs->cerrarSesion(uris[1],username);
	mg_printf_data(connection, "Sesion cerrada correctamente\n");

	return MG_TRUE;
}
