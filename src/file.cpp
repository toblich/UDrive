#include "file.h"

File::File(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorUs = manejadorUs;
	this->manejadorArchYMet = manejadorArchYMet;
}

File::~File() {
	// TODO Auto-generated destructor stub
}

mg_result File::GETHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri);
	string token = getVar(connection, "token");
	string user = getVar(connection, "user");

	if (manejadorUs->autenticarToken(token, user)){
		//TODO: Ver si esta bien usar ese path, sino hay que cambiarlo
		string filepath = uris[1] + "/" + uris[2];
		string completePath = manejadorArchYMet->descargarArchivo(user, filepath);
		mg_send_status(connection, CODESTATUS_SUCCES);
		mg_send_file(connection, completePath.c_str(), NULL);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"success\": \"Se envio el archivo exitosamente\"}");
	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario\"}");
	}

	return MG_TRUE;
}

mg_result File::PUTHandler(mg_connection* connection) {
	DatosArchivo datosArch;
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri);
	string token = getVar(connection, "token");

	if (manejadorUs->autenticarToken(token, uris[1])){
		datosArch = getMultipartData(connection);
		string filepath = uris[1] + "/" + uris[2];
		string jsonMet = "{\"metadatos\": \"esto es un metadato\"}";
		manejadorArchYMet->subirArchivo(uris[1], filepath, datosArch.fileData, datosArch.dataLenght, jsonMet);
		mg_send_status(connection, CODESTATUS_SUCCES);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"success\": \"Se subio el archivo exitosamente\"}");
	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario\"}");
	}

	return MG_TRUE;
}

mg_result File::DELETEHandler(mg_connection* connection) {
	//Ver si tambien va DELETE para borrar los archivos.
	return MG_TRUE;
}
