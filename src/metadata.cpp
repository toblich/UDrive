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
		Logger::logInfo("Se enviaron los metadatos del archivo: " + filepath + " correctamente.");
		mg_send_status(connection, CODESTATUS_SUCCESS);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"metadatos\": %s}", metadatosArch.c_str());
	} else {
		string mensaje = "No se encontraron los metadatos del archivo: " + filepath;
		this->responderResourceNotFound(connection, mensaje);
	}
}

void Metadata::GETBusquedas(mg_connection* connection, vector<string> uris, string query) {
	string path = getFilepathFrom(uris);
	//TODO: queda esto para que haga algo pero falta implementar bien
	mg_send_status(connection, CODESTATUS_SUCCESS);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"success\": %s}", query.c_str());
	//TODO: Ver a que funcion del manejar de archivos y metadatos llamar
//	string busqueda = manejadorArchYMet->buscarArchivos(user, filepath, query);
//	if (metadatosArch != "") {
//		Logger::logInfo("Se enviaron los metadatos del archivo: " + filepath + " correctamente.");
//		mg_send_status(connection, CODESTATUS_SUCCES);
//		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
//		printfData(connection, "{\"metadatos\": %s}", metadatosArch.c_str());
//	} else {
//		string mensaje = "No se encontraron los metadatos del archivo: " + filepath;
//		this->responderResourceNotFound(connection, mensaje);
//	}
}

mg_result Metadata::GETHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	Logger::logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");

		if (connection->query_string != NULL){
			string query = string(connection->query_string);
			Logger::logInfo("Se obtuvo la query correctamente.");
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
	Logger::logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);
	string nuevosMetadatos = getVar(connection, "metadatos");
	Logger::logInfo("Se obtuvo la variable con los nuevos metadatos.");

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");
		string filepath = getFilepathFrom(uris);
		if (manejadorArchYMet->actualizarMetadatos(user, filepath, nuevosMetadatos)) {
			string mensaje = "Se actualizaron los metadatos del archivo: " + filepath + " correctamente.";
			this->responderSucces(connection, mensaje);
		} else {
			string mensaje = "No se pudieron actualizar los metadatos del archivo: " + filepath;
			this->responderBadRequest(connection, mensaje);
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}
