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
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)){
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		string estructura = manejadorArchYMet->obtenerEstructuraCarpeta(filepath);
		if(estructura != ""){
			this->logInfo("Se envió la estructura de la carpeta: " + filepath + " correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"estructura\": %s}", estructura.c_str());
		}else{
			this->logInfo("Path inválido, no se encontró la carpeta: " + filepath);
			mg_send_status(connection, CODESTATUS_BAD_REQUEST);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El path de la carpeta es invalido.\"}");
		}
	}else{
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Folder::PUTHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)){
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		if(manejadorArchYMet->crearCarpetaSegura(user, filepath)){
			this->logInfo("Se creo la carpeta: " + filepath + " correctamente.");
			mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"succes\": \"La carpeta se creo exitosamente.\"}");
		}else{
			this->logInfo("Path inválido, no se pudo crear la carpeta: " + filepath);
			mg_send_status(connection, CODESTATUS_BAD_REQUEST);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El path de la carpeta es invalido.\"}");
		}
	}else{
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Folder::DELETEHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)){
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		if (manejadorArchYMet->eliminar(user, filepath)){
			this->logInfo("Se eliminó la carpeta: " + filepath + " correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"success\": \"Se elimino la carpeta exitosamente.\"}");
		}else{
			this->logInfo("Path inválido, no se pudo crear la carpeta: " + filepath);
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"Path invalido, no se encontro la carpeta.\"}");
		}
	}else{
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}
