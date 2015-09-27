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
		mg_send_status(connection, CODESTATUS_SUCCES);
		//mg_send_header(connection, "Content-Type", "application/json");
		mg_printf_data(connection, "{\"token\":\"%s\"}\n", token.c_str());
	}else{
		mg_send_status(connection, CODESTATUS_BAD_REQUEST);
		//mg_send_header(connection, "Content-Type", "application/json");
		mg_printf_data(connection, "{\"error\":\"No es posible conectar, usuario o contraseña incorrecto\"}\n");
	}

	return MG_TRUE;
}

mg_result Session::DELETEHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri);
	string username = getVar(connection, "user");

	manejadorUs->cerrarSesion(uris[1],username);
	mg_send_status(connection, CODESTATUS_SUCCES);
	//mg_send_header(connection, "Content-Type", "application/json");
	mg_printf_data(connection, "{\"succes\":\"Sesion cerrada correctamente\"}\n");

	return MG_TRUE;
}
