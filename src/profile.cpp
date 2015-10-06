#include "profile.h"

Profile::Profile(ManejadorDeUsuarios* manejadorUsuarios, ManejadorArchivosYMetadatos* manejarAyM) {
	this->manejadorUs = manejadorUsuarios;
	this->manejadorAyM = manejarAyM;
}

Profile::~Profile() { }

mg_result Profile::GETHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	string token = getVar(connection, "token");

	if (manejadorUs->autenticarToken(token, uris[1])){
		string perfil = manejadorUs->getPerfil(uris[1]);

		if (perfil != ""){
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"perfil\": %s}", perfil.c_str());
		}else{
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El usuario no existe, no se pudo obtener el perfil\"}");
		}
	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario\"}");
	}

	return MG_TRUE;
}

mg_result Profile::PUTHandler(mg_connection* connection) {
	ParserURI parserURI;
	ParserJson parserJson;
	MetadatoUsuario nuevoPerfil;

	string uri = string(connection->uri);
	vector<string> uris = parserURI.parsear(uri, '/');

	string token = getVar(connection, "token");
	nuevoPerfil.nombre = getVar(connection, "nombre");
	nuevoPerfil.email = getVar(connection, "email");
	nuevoPerfil.pathFotoPerfil = getVar(connection, "pathFoto");
	nuevoPerfil.ultimaUbicacion.latitud = getVarFloat(connection, "latitud");
	nuevoPerfil.ultimaUbicacion.longitud = getVarFloat(connection, "longitud");
	string perfilActualizado = parserJson.serializarMetadatoUsuario(nuevoPerfil);

	if (manejadorUs->autenticarToken(token, uris[1])){

		manejadorUs->modifyPerfil(uris[1], perfilActualizado);
		mg_send_status(connection, CODESTATUS_SUCCES);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"success\": \"Se modifico el perfil exitosamente\"}");

	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario\"}");
	}

	return MG_TRUE;
}

mg_result Profile::POSTHandler(mg_connection* connection) {
	string username = getVar(connection, "user");
	string password = getVar(connection, "pass");
	string profile = getVar(connection, "profile");

	if( manejadorUs->registrarUsuario(username, password, profile) and manejadorAyM->crearUsuario(username)){
		mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"succes\": \"Cuenta creada correctamente\"}");
	}else{
		mg_send_status(connection, CODESTATUS_BAD_REQUEST);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"Error en la registracion\"}");
	}

	return MG_TRUE;
}
