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
			this->responderSuccess(connection, mensaje);
		} else {
			string mensaje = "ERROR, no se pudo descargar el archivo: " + completePath;
			this->responderInternalServerError(connection, mensaje);
		}
	} else {
		string mensaje = "Path inválido, no se encontró el archivo deseado.";
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
		int version = ParserURI::obtenerNroVersion(uri);
		filepath = ParserURI::pathSinNroSecuencia(filepath);
		string completePath = manejadorArchYMet->descargarArchivo(user, filepath, version);
		enviarArchivo(completePath, connection);
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

MetadatoArchivo File::extractMetadataFrom (const string& filename, const string& user, const string& propietario) {
	MetadatoArchivo metArch;
	vector<string> nombreYExtension = ParserURI::parsear(filename, NAME_DELIM);
	Logger::logInfo("Se parseó el nombre del archivo correctamente.");
	if (nombreYExtension.size() >= 2) {
		metArch.nombre = ParserURI::join(nombreYExtension, NAME_DELIM, 0, nombreYExtension.size() - 1);
		metArch.extension = nombreYExtension[nombreYExtension.size() - 1];
	} else {
		metArch.extension = "";
		metArch.nombre = filename;
	}

	time_t fecha_sistema;
	time(&fecha_sistema);
	struct tm* tiempo = localtime(&fecha_sistema);
	int anio = tiempo->tm_year + 1900;
	int mes = tiempo->tm_mon + 1;
	int dia = tiempo->tm_mday;

	metArch.fechaUltimaModificacion = to_string(dia) + "/" + to_string(mes) + "/" + to_string(anio);
	metArch.usuarioUltimaModificacion = user;
	metArch.propietario = propietario;
	metArch.etiquetas = list<string>();
	metArch.usuariosHabilitados.push_back(metArch.propietario);
	metArch.ultimaVersion = FIRST;

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
	string propietario = uris[1];
	int cuotaUsuario = ParserJson::deserializarMetadatoUsuario(manejadorUs->getPerfil(user)).cuota;
	int version = ParserURI::obtenerNroVersion(filepath);
	filepath = ParserURI::pathSinNroSecuencia(filepath);

	MetadatoArchivo metArch = extractMetadataFrom(datosArch.fileName, user, propietario);
	string jsonMetadata = ParserJson::serializarMetadatoArchivo(metArch);
	Logger::logInfo("Se serializaron los metadatos del archivo correctamente.");
	bool force = datosArch.force == "true";
	Logger::logInfo("Se obtuvo la variable force con valor: " + force);


	try {
		if (manejadorArchYMet->subirArchivo(user, filepath, datosArch.fileData, datosArch.dataLength,
				jsonMetadata, cuotaUsuario, version, force)) {
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
			if (not manejadorArchYMet->tienePermisos(user, filepath))	// No mover a anter del manejador->subirArchivo
				this->responderAutenticacionFallida(connection);
			else {
				string mensaje = "ERROR, no se pudo subir el archivo: " + filepath;
				this->responderInternalServerError(connection, mensaje);
			}
		}
	} catch (InvalidVersion& e) {
		string mensaje = "ERROR, no se pudo subir el archivo: " + filepath;
		this->responderConflict(connection, mensaje, e.getUltimaVersion());
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
	bool hayArchivoParaSubir = (datosArch.fileName != "");

	if (not hayArchivoParaSubir){
		string mensaje = "Error, no se paso ningun archivo para subir por parametro.";
		this->responderBadRequest(connection, mensaje);
		return MG_TRUE;
	}

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
				this->responderSuccess(connection, mensaje);
			} else {
				string mensaje = "Path inválido, no se encontró el archivo: " + filepath;
				this->responderResourceNotFound(connection, mensaje);
			}
		} else {
			if (manejadorArchYMet->eliminar(user, filepath)) {
				string mensaje = "Se eliminó el archivo: " + filepath + " correctamente.";
				this->responderSuccess(connection, mensaje);
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
