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
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = getFilepathFrom(uris);
		string completePath = manejadorArchYMet->descargarArchivo(user, filepath);
		enviarArchivo(completePath, connection);
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

MetadatoArchivo File::extractMetadataFrom (const vector<string>& nombreYExtension, const string& user, const vector<string>& uris) {
	MetadatoArchivo metArch;
	metArch.nombre = "";
	if (nombreYExtension.size() >= 2) {
		for (int i = 0; i <= nombreYExtension.size() - 2; i++) {
			metArch.nombre += nombreYExtension[i];
			if (i != nombreYExtension.size() - 2) {
				metArch.nombre += ".";
			}
		}
		metArch.extension = nombreYExtension[nombreYExtension.size() - 1];
	} else {
		metArch.nombre += nombreYExtension[0];
		metArch.extension = "none";
	}

	time_t fecha_sistema;
	time(&fecha_sistema);
	struct tm* tiempo = localtime(&fecha_sistema);
	int anio = tiempo->tm_year + 1900;
	int mes = tiempo->tm_mon + 1;
	int dia = tiempo->tm_mday;
	metArch.fechaUltimaModificacion = to_string(dia) + "/" + to_string(mes) + "/" + to_string(anio);
	;
	metArch.usuarioUltimaModificacion = user;
	metArch.propietario = uris[1];
	metArch.etiquetas = list<string>();		// TODO
	metArch.usuariosHabilitados.push_back(metArch.propietario);
	return metArch;
}

void File::subirArchivo (const vector<string>& uris, const DatosArchivo& datosArch, const string& user, mg_connection* connection) {
	string filepath = getFilepathFrom(uris);
	vector<string> nombreYExtension = ParserURI::parsear(datosArch.fileName, '.');
	this->logInfo("Se parseó el nombre del archivo correctamente.");
	MetadatoArchivo metArch = extractMetadataFrom(nombreYExtension, user, uris);
	string jsonMetadata = ParserJson::serializarMetadatoArchivo(metArch);
	this->logInfo("Se serializaron los metadatos del archivo correctamente.");
	int cuotaUsuario = ParserJson::deserializarMetadatoUsuario(manejadorUs->getPerfil(user)).cuota;

	if (manejadorArchYMet->subirArchivo(user, filepath, datosArch.fileData, datosArch.dataLength, jsonMetadata, cuotaUsuario)) {
		string mensaje = "Se subió el archivo: " + filepath + " correctamente.";
		this->responderResourceCreated(connection, mensaje);
	} else {
		string mensaje = "ERROR, no se pudo subir el archivo: " + filepath;
		this->responderInternalServerError(connection, mensaje);
	}
}

mg_result File::PUTHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");

	string varFile = "file";
	DatosArchivo datosArch = getMultipartData(connection, varFile);
	string token = datosArch.token;
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = datosArch.user;
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)) {
		this->logInfo("Se autenticó la sesión correctamente.");
		subirArchivo(uris, datosArch, user, connection);
	} else {
		this->responderAutenticacionFallida(connection);
	}

	return MG_TRUE;
}

mg_result File::DELETEHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);
	string restore = getVar(connection, "restore");
	this->logInfo("Se obtuvo la variable restore con valor: " + restore);

	if (manejadorUs->autenticarToken(token, user)) {
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = getFilepathFrom(uris);
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
