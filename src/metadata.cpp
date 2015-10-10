#include "metadata.h"

Metadata::Metadata (ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorUs = manejadorUs;
	this->manejadorArchYMet = manejadorArchYMet;
}

Metadata::~Metadata () {
}

void Metadata::GETMetadatos(mg_connection* connection, vector<string> uris, string user) {
	string filepath = getFilepathFrom(uris);
	string metadatosArch = manejadorArchYMet->consultarMetadatosArchivo(user, filepath);
	if (metadatosArch != "") {
		this->logInfo("Se enviaron los metadatos del archivo: " + filepath + " correctamente.");
		mg_send_status(connection, CODESTATUS_SUCCES);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"metadatos\": %s}", metadatosArch.c_str());
	} else {
		this->logInfo("No se encontraron los metadatos del archivo: " + filepath);
		mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El usuario no tiene permisos o los metadatos del archivo no existen\"}");
	}
}

void Metadata::GETBusquedas(mg_connection* connection, vector<string> uris, string query) {
	string path = getFilepathFrom(uris);
	//TODO: queda esto para que haga algo pero falta implementar bien
	mg_send_status(connection, CODESTATUS_SUCCES);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"success\": %s}", query.c_str());
	//TODO: Ver a que funcion del manejar de archivos y metadatos llamar
//	string busqueda = manejadorArchYMet->buscarArchivos(user, filepath, query);
//	if (metadatosArch != "") {
//		this->logInfo("Se enviaron los metadatos del archivo: " + filepath + " correctamente.");
//		mg_send_status(connection, CODESTATUS_SUCCES);
//		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
//		printfData(connection, "{\"metadatos\": %s}", metadatosArch.c_str());
//	} else {
//		this->logInfo("No se encontraron los metadatos del archivo: " + filepath);
//		mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
//		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
//		printfData(connection, "{\"error\": \"El usuario no tiene permisos o los metadatos del archivo no existen\"}");
//	}
}

mg_result Metadata::GETHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		this->logInfo("Se autenticó la sesión correctamente.");

		if (connection->query_string != NULL){
			string query = string(connection->query_string);
			this->logInfo("Se obtuvo la query correctamente.");
			this->GETBusquedas(connection, uris, query);
		} else {
			this->GETMetadatos(connection, uris, user);
		}

	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Metadata::PUTHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);
	string nuevosMetadatos = getVar(connection, "metadatos");
	this->logInfo("Se obtuvo la variable con los nuevos metadatos.");

	if (manejadorUs->autenticarToken(token, user)) {
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = getFilepathFrom(uris);
		if (manejadorArchYMet->actualizarMetadatos(user, filepath, nuevosMetadatos)) {
			this->logInfo("Se actualizaron los metadatos del archivo: " + filepath + " correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"success\": \"Se actualizo el archivo con sus nuevos metadatos.\"}");
		} else {
			this->logInfo("No se pudieron actualizar los metadatos del archivo: " + filepath);
			mg_send_status(connection, CODESTATUS_BAD_REQUEST);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"No se pudieron actualizar los metadatos del archivo.\"}");
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}
