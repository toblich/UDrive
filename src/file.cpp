#include "file.h"

File::File(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorUs = manejadorUs;
	this->manejadorArchYMet = manejadorArchYMet;
}

File::~File() { }

mg_result File::GETHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)){
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		string completePath = manejadorArchYMet->descargarArchivo(user, filepath);
		if (completePath != ""){
			if (sendFile(connection, completePath)){
				this->logInfo("Se descargó el archivo: " + filepath + " correctamente.");
				mg_send_status(connection, CODESTATUS_SUCCES);
				mg_send_header(connection, contentType.c_str(), jsonType.c_str());
				printfData(connection, "{\"success\": \"Se descargo el archivo exitosamente\"}");
			}else{
				this->logError("ERROR, no se pudo descargar el archivo: " + filepath);
				mg_send_status(connection, CODESTATUS_INTERNAL_SERVER_ERROR);
				mg_send_header(connection, contentType.c_str(), jsonType.c_str());
				printfData(connection, "{\"error\": \"Hubo un problema y no se pudo enviar el archivo, intentelo nuevamente.\"}");
			}
		}else{
			this->logInfo("Path inválido, no se encontró el archivo: " + filepath);
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El archivo al que quiere acceder no existe.\"}");
		}
	}else{
		this->logInfo("No se pudo autenticar la sesión.");
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario\"}");
	}
	return MG_TRUE;
}

mg_result File::PUTHandler(mg_connection* connection) {
	DatosArchivo datosArch;
	MetadatoArchivo metArch;
	ParserURI parser;
	ParserJson parserJson;
	list<string> habilitados;

	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");

	string varFile = "file";
	datosArch = getMultipartData(connection, varFile);
	this->logInfo("Se obtuvo el archivo correctamente con tamaño: " + datosArch.dataLength);
	string token = datosArch.token;
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = datosArch.user;
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)){
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		vector<string> nombreYExtension = parser.parsear(datosArch.fileName, '.');
		this->logInfo("Se parseó el nombre del archivo correctamente.");
		metArch.nombre = "";
		if (nombreYExtension.size() >= 2){
			for (int i = 0; i <= nombreYExtension.size() - 2; i++){
				metArch.nombre += nombreYExtension[i];
				if (i != nombreYExtension.size() - 2){
					metArch.nombre += ".";
				}
			}
			metArch.extension = nombreYExtension[nombreYExtension.size() - 1];
		}else{
			metArch.nombre += nombreYExtension[0];
			metArch.extension = "none";
		}

		struct tm *tiempo;
		time_t fecha_sistema;
		time(&fecha_sistema);
		tiempo = localtime(&fecha_sistema);
		int anio = tiempo->tm_year + 1900;
		int mes = tiempo->tm_mon + 1;
		int dia = tiempo->tm_mday;
		string fecha = to_string(dia) + "/" + to_string(mes) + "/" + to_string(anio);
		metArch.fechaUltimaModificacion = fecha;

		metArch.usuarioUltimaModificacion = user;
		metArch.propietario = uris[1];
		metArch.etiquetas = list<string>();
		habilitados.push_back(metArch.propietario);
		metArch.usuariosHabilitados = habilitados;

		string jsonMetadata = parserJson.serializarMetadatoArchivo(metArch);

		this->logInfo("Se serializaron los metadatos del archivo correctamente.");

		if(manejadorArchYMet->subirArchivo(user, filepath, datosArch.fileData, datosArch.dataLength, jsonMetadata)){
			this->logInfo("Se subió el archivo: " + filepath + " correctamente.");
			mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
			//mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"success\": \"Se subio el archivo exitosamente\"}");
		}else{
			this->logError("ERROR, no se pudo subir el archivo: " + filepath);
			mg_send_status(connection, CODESTATUS_INTERNAL_SERVER_ERROR);
			//mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"Hubo un problema al subir el archivo, intentelo nuevamente\"}");
		}
	}else{
		this->logInfo("No se pudo autenticar la sesión.");
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		//mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario\"}");
	}

	return MG_TRUE;
}

mg_result File::DELETEHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + user);

	if (manejadorUs->autenticarToken(token, user)){
		this->logInfo("Se autenticó la sesión correctamente.");
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		if (manejadorArchYMet->eliminar(user, filepath)){
			this->logInfo("Se eliminó el archivo: " + filepath + " correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"success\": \"Se elimino el archivo exitosamente.\"}");
		}else{
			this->logInfo("Path inválido, no se encontró el archivo: " + filepath);
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"Path invalido, no se encontro el archivo.\"}");
		}
	}else{
		this->logInfo("No se pudo autenticar la sesión.");
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario.\"}");
	}
	return MG_TRUE;
}
