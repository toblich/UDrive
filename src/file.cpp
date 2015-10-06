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
		string completePath = manejadorArchYMet->descargarArchivo(user, filepath);
		if (completePath != ""){
			if (sendFile(connection, completePath)){
				mg_send_status(connection, CODESTATUS_SUCCES);
				mg_send_header(connection, contentType.c_str(), jsonType.c_str());
				printfData(connection, "{\"success\": \"Se descargo el archivo exitosamente\"}");
			}else{
				mg_send_status(connection, CODESTATUS_INTERNAL_SERVER_ERROR);
				mg_send_header(connection, contentType.c_str(), jsonType.c_str());
				printfData(connection, "{\"error\": \"Hubo un problema y no se pudo enviar el archivo, intentelo nuevamente.\"}");
			}
		}else{
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El archivo al que quiere acceder no existe.\"}");
		}
	}else{
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

	string varFile = "file";
	datosArch = getMultipartData(connection, varFile);
	string token = datosArch.token;
	string user = datosArch.user;

	if (manejadorUs->autenticarToken(token, user)){
		string filepath = "";
		for (int i = 1; i <= uris.size() - 1; i++){
			filepath += uris[i];
			if (i != uris.size() - 1){
				filepath += "/";
			}
		}
		vector<string> nombreYExtension = parser.parsear(datosArch.fileName, '.');
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

		if(manejadorArchYMet->subirArchivo(user, filepath, datosArch.fileData, datosArch.dataLength, jsonMetadata)){
			mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
			//mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"success\": \"Se subio el archivo exitosamente\"}");
		}else{
			mg_send_status(connection, CODESTATUS_INTERNAL_SERVER_ERROR);
			//mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"Hubo un problema al subir el archivo, intentelo nuevamente\"}");
		}
	}else{
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
		if (manejadorArchYMet->eliminar(user, filepath)){
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"success\": \"Se elimino el archivo exitosamente.\"}");
		}else{
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"Path invalido, no se encontro el archivo.\"}");
		}
	}else{
		mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario.\"}");
	}
	return MG_TRUE;
}
