#include "profile.h"

Profile::Profile (ManejadorDeUsuarios* manejadorUsuarios, ManejadorArchivosYMetadatos* manejarAyM) {
	this->manejadorUs = manejadorUsuarios;
	this->manejadorAyM = manejarAyM;
}

Profile::~Profile () {
}

mg_result Profile::GETHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);

	if (manejadorUs->autenticarToken(token, uris[1])) {
		this->logInfo("Se autenticó la sesión correctamente.");

		string perfil = manejadorUs->getPerfil(uris[1]);
		if (perfil != "") {
			this->logInfo("Se envió el perfil correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"perfil\": %s}", perfil.c_str());
		} else {
			this->logInfo("El perfil al que se quizo acceder no existe, o bien el usuario no existe.");
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El usuario no existe, no se pudo obtener el perfil\"}");
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}

	return MG_TRUE;
}

mg_result Profile::PUTHandler (mg_connection* connection) {
	ParserJson parserJson;
	MetadatoUsuario nuevoPerfil;

	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");

	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	nuevoPerfil.nombre = getVar(connection, "nombre");
	this->logInfo("Se obtuvo la variable nombre con valor: " + nuevoPerfil.nombre);
	nuevoPerfil.email = getVar(connection, "email");
	this->logInfo("Se obtuvo la variable email con valor: " + nuevoPerfil.email);
	nuevoPerfil.pathFotoPerfil = getVar(connection, "pathFoto");
	this->logInfo("Se obtuvo la variable pathFoto con valor: " + nuevoPerfil.pathFotoPerfil);
	nuevoPerfil.ultimaUbicacion.latitud = getVarFloat(connection, "latitud");
	nuevoPerfil.ultimaUbicacion.longitud = getVarFloat(connection, "longitud");

	string perfilActualizado = parserJson.serializarMetadatoUsuario(nuevoPerfil);

	if (manejadorUs->autenticarToken(token, uris[1])) {
		this->logInfo("Se autenticó la sesión correctamente.");

		manejadorUs->modifyPerfil(uris[1], perfilActualizado);
		this->logInfo("Se modificó el perfil del usuario correctamente.");
		mg_send_status(connection, CODESTATUS_SUCCES);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"success\": \"Se modifico el perfil exitosamente\"}");
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Profile::POSTHandler (mg_connection* connection) {
	string username = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + username);
	string password = getVar(connection, "pass");
	this->logInfo("Se obtuvo la variable pass con valor: " + password);
	string profile = getVar(connection, "profile");
	this->logInfo("Se obtuvo la variable profile.");

	if (manejadorUs->registrarUsuario(username, password, profile) and manejadorAyM->crearUsuario(username)) {
		this->logInfo("Se registro la cuenta con usuario: " + username + " y pass: " + password + " exitosamente.");
		mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"succes\": \"Cuenta creada correctamente\"}");
	} else {
		this->logInfo("No se pudo registrar la cuenta debido a algun error en las variables.");
		mg_send_status(connection, CODESTATUS_BAD_REQUEST);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"Error en la registracion\"}");
	}
	return MG_TRUE;
}
