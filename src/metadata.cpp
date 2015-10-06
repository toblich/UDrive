#include "metadata.h"

Metadata::Metadata(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorUs = manejadorUs;
	this->manejadorArchYMet = manejadorArchYMet;
}

Metadata::~Metadata() {
}

//Consultar: GET en /metadata/<username_de_propietario>/<filename>
mg_result Metadata::GETHandler(mg_connection* connection) {
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
		string metadatosArch = manejadorArchYMet->consultarMetadatosArchivo(user, filepath);
		if (metadatosArch != ""){
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"metadatos\": %s}", metadatosArch.c_str());
		}else{
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El usuario no tiene permisos o los metadatos del archivo no existen\"}");
		}
	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario\"}");
	}
	return MG_TRUE;
}

//Actualizar, si se usa: PUT en /metadata/<username_de_propietario>/<filename>
mg_result Metadata::PUTHandler(mg_connection* connection) {
	return MG_TRUE;
}
