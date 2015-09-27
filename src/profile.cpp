#include "profile.h"

Profile::Profile(ManejadorDeUsuarios* manejadorUsuarios) {
	this->manejadorUs = manejadorUsuarios;
}

Profile::~Profile() {
}

mg_result Profile::GETHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri);

	string perfil = manejadorUs->getPerfil(uris[1]);

	if (perfil != ""){
		mg_send_status(connection, CODESTATUS_SUCCES);
		//mg_send_header(connection, "Content-Type", "application/json");
		mg_printf_data(connection, "{\"perfil\":\"%s\"}\n", perfil.c_str());
	}else{
		mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
		//mg_send_header(connection, "Content-Type", "application/json");
		mg_printf_data(connection, "{\"error\":\"El usuario no existe, imposible obtener el perfil\"}\n");
	}
	return MG_TRUE;
}

mg_result Profile::PUTHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri);
	string newProfile = getVar(connection, "profile");

	manejadorUs->modifyPerfil(uris[1], newProfile);
	mg_send_status(connection, CODESTATUS_SUCCES);
	//mg_send_header(connection, "Content-Type", "application/json");
	mg_printf_data(connection, "{\"succes\":\"Se modifico el perfil exitosamente\"}\n");

	return MG_TRUE;
}

mg_result Profile::POSTHandler(mg_connection* connection) {
	string username = getVar(connection, "user");
	string password = getVar(connection, "pass");
	string profile = getVar(connection, "profile");

	//TODO: falta revisar si hay errores
	if( manejadorUs->registrarUsuario(username, password, profile)){
		mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
		//mg_send_header(connection, "Content-Type", "application/json");
		mg_printf_data(connection, "{\"succes\":\"Cuenta creada correctamente\"}\n");
	}else{
		mg_send_status(connection, CODESTATUS_BAD_REQUEST);
		//mg_send_header(connection, "Content-Type", "application/json");
		mg_printf_data(connection, "{\"error\":\"Error en la registracion\"}\n");
	}

	return MG_TRUE;
}
