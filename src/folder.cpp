#include "folder.h"

Folder::Folder(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorUs = manejadorUs;
	this->manejadorArchYMet = manejadorArchYMet;
}

Folder::~Folder() { }

mg_result Folder::GETHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	string token = getVar(connection, "token");
	string user = getVar(connection, "user");

	if (manejadorUs->autenticarToken(token, user)){
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		string estructura = manejadorArchYMet->obtenerEstructuraCarpeta(filepath);
		if(estructura != ""){
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"estructura\": %s}", estructura.c_str());
		}else{
			mg_send_status(connection, CODESTATUS_BAD_REQUEST);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El path de la carpeta es invalido.\"}");
		}
	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario.\"}");
	}
	return MG_TRUE;
}

mg_result Folder::PUTHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	string token = getVar(connection, "token");
	string user = getVar(connection, "user");

	if (manejadorUs->autenticarToken(token, user)){
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		if(manejadorArchYMet->crearCarpetaSegura(user, filepath)){
			mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"succes\": \"La carpeta se creo exitosamente.\"}");
		}else{
			mg_send_status(connection, CODESTATUS_BAD_REQUEST);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El path de la carpeta es invalido.\"}");
		}
	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario.\"}");
	}
	return MG_TRUE;
}

mg_result Folder::DELETEHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	string token = getVar(connection, "token");
	string user = getVar(connection, "user");

	if (manejadorUs->autenticarToken(token, user)){
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		if (manejadorArchYMet->eliminar(user, filepath)){
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"success\": \"Se elimino la carpeta exitosamente.\"}");
		}else{
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"Path invalido, no se encontro la carpeta.\"}");
		}
	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario.\"}");
	}
	return MG_TRUE;
}
