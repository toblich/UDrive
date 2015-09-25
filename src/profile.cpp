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
		mg_printf_data(connection, "%s\n", perfil.c_str());
	}else{
		mg_printf_data(connection, "El usuario no existe, imposible obtener el perfil\n");
	}

	return MG_TRUE;
}

mg_result Profile::PUTHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri);
	string newProfile = getVar(connection, "profile");

	manejadorUs->modifyPerfil(uris[1], newProfile);
	mg_printf_data(connection, "Se modifico el perfil exitosamente\n");

	return MG_TRUE;
}

mg_result Profile::POSTHandler(mg_connection* connection) {
	string username = getVar(connection, "user");
	string password = getVar(connection, "pass");
	string profile = getVar(connection, "profile");

	//TODO: falta revisar si hay errores
	if( manejadorUs->registrarUsuario(username, password, profile)){
		mg_printf_data(connection, "Cuenta creada correctamente\n");
	}else{
		mg_printf_data(connection, "Error al registrar nuevo usuario\n");
	}

	return MG_TRUE;
}
