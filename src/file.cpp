#include "file.h"

File::File (ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorUs = manejadorUs;
	this->manejadorArchYMet = manejadorArchYMet;
}

File::~File () {
}

void File::enviarArchivo (const string& completePath, mg_connection* connection) {
	if (completePath != "") {
		if (sendFile(connection, completePath)) {
			string mensaje = "Se descargó el archivo: " + completePath + " correctamente.";
			this->responderSucces(connection, mensaje);
		} else {
			string mensaje = "ERROR, no se pudo descargar el archivo: " + completePath;
			this->responderInternalServerError(connection, mensaje);
		}
	} else {
		string mensaje = "Path inválido, no se encontró el archivo: " + completePath;
		this->responderResourceNotFound(connection, mensaje);
	}
}

mg_result File::GETHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, URI_DELIM);
	Logger::logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");
		string filepath = ParserURI::join(uris, URI_DELIM, 1, uris.size());
		string completePath = manejadorArchYMet->descargarArchivo(user, filepath);
		enviarArchivo(completePath, connection);
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

MetadatoArchivo File::extractMetadataFrom (const vector<string>& nombreYExtension, const string& user, const vector<string>& uris) {
	MetadatoArchivo metArch;

	if (nombreYExtension.size() >= 2) {
		metArch.nombre = ParserURI::join(nombreYExtension, NAME_DELIM, 0, nombreYExtension.size() - 1);
		metArch.extension = nombreYExtension[nombreYExtension.size() - 1];
	} else {
		metArch.nombre = nombreYExtension[0];
		metArch.extension = "none";
	}

	time_t fecha_sistema;
	time(&fecha_sistema);
	struct tm* tiempo = localtime(&fecha_sistema);
	int anio = tiempo->tm_year + 1900;
	int mes = tiempo->tm_mon + 1;
	int dia = tiempo->tm_mday;

	metArch.fechaUltimaModificacion = to_string(dia) + "/" + to_string(mes) + "/" + to_string(anio);
	metArch.usuarioUltimaModificacion = user;
	metArch.propietario = uris[1];
	metArch.etiquetas = list<string>();
	metArch.usuariosHabilitados.push_back(metArch.propietario);

	return metArch;
}

void File::actualizarUltimaUbicacion(string user, string latitud, string longitud) {
	//Se obtiene el perfil del usuario que subio el archivo y se deserializa
	MetadatoUsuario perfilUsuario = ParserJson::deserializarMetadatoUsuario(manejadorUs->getPerfil(user));

	//Se actualiza la longitud y latitud
	perfilUsuario.ultimaUbicacion.latitud = stod(latitud);
	perfilUsuario.ultimaUbicacion.longitud = stod(longitud);

	//Vuelvo a serializar el perfil
	string perfilActualizado = ParserJson::serializarMetadatoUsuario(perfilUsuario);

	//Lo actualizo en la BD
	manejadorUs->modifyPerfil(user, perfilActualizado);
}

void File::subirArchivo (const vector<string>& uris, const DatosArchivo& datosArch, const string& user, mg_connection* connection) {
	string filepath = ParserURI::join(uris, URI_DELIM, 1, uris.size());
	vector<string> nombreYExtension = ParserURI::parsear(datosArch.fileName, NAME_DELIM);
	Logger::logInfo("Se parseó el nombre del archivo correctamente.");
	MetadatoArchivo metArch = extractMetadataFrom(nombreYExtension, user, uris);
	string jsonMetadata = ParserJson::serializarMetadatoArchivo(metArch);
	Logger::logInfo("Se serializaron los metadatos del archivo correctamente.");
	int cuotaUsuario = ParserJson::deserializarMetadatoUsuario(manejadorUs->getPerfil(user)).cuota;

	if (manejadorArchYMet->subirArchivo(user, filepath, datosArch.fileData, datosArch.dataLength, jsonMetadata, cuotaUsuario)) {
		//Como el usuario subio un archivo se actualiza su ultima ubicacion
		string latitud = datosArch.latitud;
		Logger::logInfo("Se obtuvo la variable latitud con valor: " + latitud);
		string longitud = datosArch.longitud;
		Logger::logInfo("Se obtuvo la variable longitiud con valor: " + longitud);
		//Si se reciben ambos, se actualiza
		if (latitud != "" and longitud != "") actualizarUltimaUbicacion(user, latitud, longitud);

		string mensaje = "Se subió el archivo: " + filepath + " correctamente.";
		this->responderResourceCreated(connection, mensaje);
	} else {
		string mensaje = "ERROR, no se pudo subir el archivo: " + filepath;
		this->responderInternalServerError(connection, mensaje);
	}
}

mg_result File::PUTHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, URI_DELIM);
	Logger::logInfo("Se parseó la uri correctamente.");

	string varFile = "file";
	DatosArchivo datosArch = getMultipartData(connection, varFile);
	string token = datosArch.token;
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = datosArch.user;
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");
		subirArchivo(uris, datosArch, user, connection);
	} else {
		this->responderAutenticacionFallida(connection);
	}

	return MG_TRUE;
}

mg_result File::DELETEHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, URI_DELIM);
	Logger::logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);
	string restore = getVar(connection, "restore");
	Logger::logInfo("Se obtuvo la variable restore con valor: " + restore);

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");
		string filepath = ParserURI::join(uris, URI_DELIM, 1, uris.size());
		if (restore == "true"){
			if (manejadorArchYMet->restaurar(user, filepath)){
				string mensaje = "Se restauró el archivo: " + filepath + " correctamente.";
				this->responderSucces(connection, mensaje);
			} else {
				string mensaje = "Path inválido, no se encontró el archivo: " + filepath;
				this->responderResourceNotFound(connection, mensaje);
			}
		} else {
			if (manejadorArchYMet->eliminar(user, filepath)) {
				string mensaje = "Se eliminó el archivo: " + filepath + " correctamente.";
				this->responderSucces(connection, mensaje);
			} else {
				string mensaje = "Path inválido, no se encontró el archivo: " + filepath;
				this->responderResourceNotFound(connection, mensaje);
			}
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}
