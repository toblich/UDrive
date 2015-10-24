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

void Metadata::busquedaEtiqueta(mg_connection* connection, string user) {
	//TODO: queda la version para buscar de a una etiqueta por vez
	//despues si termina siendo una lista hay que ver como cambiarlo.
	string etiquetaABuscar = getVar(connection, "etiqueta");
	string resultado = manejadorArchYMet->buscarPorEtiqueta(user, etiquetaABuscar);
	responderBusqueda(connection, resultado);
}

void Metadata::busquedaExtension(mg_connection* connection, string user) {
	string extensionABuscar = getVar(connection, "extension");
	string resultado = manejadorArchYMet->buscarPorExtension(user, extensionABuscar);
	responderBusqueda(connection, resultado);
}

void Metadata::busquedaNombre(mg_connection* connection, string user) {
	string nombreABuscar = getVar(connection, "nombre");
	string resultado = manejadorArchYMet->buscarPorNombre(user, nombreABuscar);
	responderBusqueda(connection, resultado);
}

void Metadata::busquedaPropietario(mg_connection* connection, string user) {
	string propiestarioABuscar = getVar(connection, "propietario");
	string resultado = manejadorArchYMet->buscarPorPropietario(user, propiestarioABuscar);
	responderBusqueda(connection, resultado);
}

void Metadata::responderBusqueda(mg_connection* connection, string resultado) {
	Logger::logInfo("Se envio el resultado de la busqueda correctamente.");
	mg_send_status(connection, CODESTATUS_SUCCESS);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"busqueda\": %s}", resultado.c_str());
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

		string busqueda = getVar(connection, "busqueda");
		Logger::logInfo("Se obtuvo la variable busqueda con valor: " + busqueda);
		if (busqueda != ""){
			//TODO: No se como hacer un map o algo que dependiendo el tipo de busqueda mapee a la funcion.
			//Hago 4 ifs por ahora
			if (busqueda == "etiqueta") 	busquedaEtiqueta(connection, user);
			if (busqueda == "extension") 	busquedaExtension(connection, user);
			if (busqueda == "nombre") 		busquedaNombre(connection, user);
			if (busqueda == "propietario") 	busquedaPropietario(connection, user);
			//Si la busqueda no es ninguna de estas, BAD REQUEST
			string mensaje = "La busqueda que quiere realizar no existe.";
			this->responderBadRequest(connection, mensaje);
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
