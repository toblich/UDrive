#include "metadata.h"

Metadata::Metadata (ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorUs = manejadorUs;
	this->manejadorArchYMet = manejadorArchYMet;
}

Metadata::~Metadata () {
}

void Metadata::GETMetadatos(mg_connection* connection, vector<string> uris, string user) {
	string filepath = ParserURI::join(uris, '/', 1, uris.size());
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

void Metadata::busquedaEtiqueta(mg_connection* connection, string user, string etiqueta) {
	//TODO: queda la version para buscar de a una etiqueta por vez
	//despues si termina siendo una lista hay que ver como cambiarlo.
	string resultado = manejadorArchYMet->buscarPorEtiqueta(user, etiqueta);
	responderBusqueda(connection, resultado);
}

void Metadata::busquedaExtension(mg_connection* connection, string user, string extension) {
	string resultado = manejadorArchYMet->buscarPorExtension(user, extension);
	responderBusqueda(connection, resultado);
}

void Metadata::busquedaNombre(mg_connection* connection, string user, string nombre) {
	string resultado = manejadorArchYMet->buscarPorNombre(user, nombre);
	responderBusqueda(connection, resultado);
}

void Metadata::busquedaPropietario(mg_connection* connection, string user, string propietario) {
	string resultado = manejadorArchYMet->buscarPorPropietario(user, propietario);
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

		string etiqueta = getVar(connection, "etiqueta");
		Logger::logInfo("Se obtuvo la variable etiqueta con valor: " + etiqueta);
		string extension = getVar(connection, "extension");
		Logger::logInfo("Se obtuvo la variable extension con valor: " + extension);
		string nombre = getVar(connection, "nombre");
		Logger::logInfo("Se obtuvo la variable nombre con valor: " + nombre);
		string propietario = getVar(connection, "propietario");
		Logger::logInfo("Se obtuvo la variable propietario con valor: " + propietario);

		//Solo se puede realizar de a una busqueda por vez.
		if 		(etiqueta != "") 	busquedaEtiqueta(connection, user, etiqueta);
		else if (extension != "") 	busquedaExtension(connection, user, extension);
		else if (nombre != "") 		busquedaNombre(connection, user, nombre);
		else if (propietario != "") busquedaPropietario(connection, user, propietario);
		//Si no es ninguna de las busquedas estoy pidiendo los metadatos de algun archivo.
		else 						GETMetadatos(connection, uris, user);
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
		string filepath = ParserURI::join(uris, '/', 1, uris.size());
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
