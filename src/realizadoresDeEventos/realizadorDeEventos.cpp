#include "realizadorDeEventos.h"

RealizadorDeEventos::RealizadorDeEventos() { }

RealizadorDeEventos::~RealizadorDeEventos() { }

size_t RealizadorDeEventos::printfData(mg_connection* connection, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	size_t ret = mg_vprintf_data(connection, format, ap);
	va_end(ap);
	return ret;
}

string RealizadorDeEventos::getVar(mg_connection* connection, string varName){
	string variable;

	variable.resize(MAX_LENGTH);
	int result = mg_get_var(connection, varName.c_str(), (char*) variable.data(), MAX_LENGTH);
	//Si no existe una variable con ese varName, devuelvo un string vacio.
	if (result == -1) return string();
	variable.resize(strlen(variable.data()));

	return variable;
}

RealizadorDeEventos::DatosArchivo RealizadorDeEventos::getMultipartData(mg_connection* connection, string variable){
	DatosArchivo datosArch;
	datosArch.fileName = "";
	const char* data;
	char varName[MAX_LENGTH], fileName[MAX_LENGTH];
	int dataLength, n1 = 0, n2 = 0;

	while ((n2 = mg_parse_multipart(connection->content + n1, connection->content_len - n1,
			varName, sizeof(varName), fileName, sizeof(fileName), &data, &dataLength)) > 0) {
		n1 += n2;
		if (string(varName) == variable){
			datosArch.fileData = data;
			datosArch.dataLength = dataLength;
			datosArch.fileName = string(fileName);
		}
		if (string(varName) == "user") 	   datosArch.user     = string(data, dataLength);
		if (string(varName) == "token")    datosArch.token    = string(data, dataLength);
		if (string(varName) == "nombre")   datosArch.nombre   = string(data, dataLength);
		if (string(varName) == "email")    datosArch.email    = string(data, dataLength);
		if (string(varName) == "latitud")  datosArch.latitud  = string(data, dataLength);
		if (string(varName) == "longitud") datosArch.longitud = string(data, dataLength);
		if (string(varName) == "force")    datosArch.force    = string(data, dataLength);
	}

	return datosArch;
}

bool RealizadorDeEventos::sendFile(mg_connection* connection, string filePath) {
	char buf[MAX_LENGTH];
	int n;

	FILE* fp = fopen(filePath.c_str(), "rb");
	if (fp == NULL)	return false;

	while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
		mg_send_data(connection,buf,n);
	}
	fclose(fp);
	mg_send_data(connection,"",0);

	return true;
}

mg_result RealizadorDeEventos::handler(mg_connection* connection) {
	string verb = string(connection->request_method);

	if 		(verb == "GET")		return this->GETHandler(connection);
	else if (verb == "POST")	return this->POSTHandler(connection);
	else if (verb == "PUT")		return this->PUTHandler(connection);
	else if (verb == "DELETE")	return this->DELETEHandler(connection);
	else {
		this->responderUnsupportedMethod(connection);
		return MG_TRUE;
	}
}

mg_result RealizadorDeEventos::GETHandler(mg_connection* connection) {
	this->responderUnsupportedMethod(connection);
	return MG_TRUE;
}

mg_result RealizadorDeEventos::PUTHandler(mg_connection* connection) {
	this->responderUnsupportedMethod(connection);
	return MG_TRUE;
}

mg_result RealizadorDeEventos::POSTHandler(mg_connection* connection) {
	this->responderUnsupportedMethod(connection);
	return MG_TRUE;
}

mg_result RealizadorDeEventos::DELETEHandler(mg_connection* connection) {
	this->responderUnsupportedMethod(connection);
	return MG_TRUE;
}

void RealizadorDeEventos::responderUnsupportedMethod(mg_connection* connection) {
	Logger::logWarn("El método al que se intentó acceder no está permitido por ésta clase.");
	mg_send_status(connection, CODESTATUS_UNSUPPORTED_METHOD);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"error\": \"metodo no soportado por esta clase\"}");
}

void RealizadorDeEventos::responderAutenticacionFallida(mg_connection* connection) {
	Logger::logInfo("No se pudo autenticar la sesión.");
	mg_send_status(connection, CODESTATUS_UNAUTHORIZED_CLIENT);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"error\": \"El token no corresponde con la sesion del usuario\"}");
}

void RealizadorDeEventos::responderBadRequest(mg_connection* connection, string errMsg) {
	Logger::logInfo(errMsg);
	mg_send_status(connection, CODESTATUS_BAD_REQUEST);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"error\": \"%s\"}", errMsg.c_str());
}

void RealizadorDeEventos::responderResourceNotFound(mg_connection* connection, string errMsg){
	Logger::logInfo(errMsg);
	mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"error\": \"%s\"}", errMsg.c_str());
}

void RealizadorDeEventos::responderInternalServerError(mg_connection* connection, string errMsg){
	Logger::logError(errMsg);
	mg_send_status(connection, CODESTATUS_INTERNAL_SERVER_ERROR);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"error\": \"%s\"}", errMsg.c_str());
}

void RealizadorDeEventos::responderSuccess(mg_connection* connection, string msg){
	Logger::logInfo(msg);
	mg_send_status(connection, CODESTATUS_SUCCESS);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"success\": \"%s\"}", msg.c_str());
}

void RealizadorDeEventos::responderResourceCreated(mg_connection* connection, string msg){
	Logger::logInfo(msg);
	mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"success\": \"%s\"}", msg.c_str());
}

void RealizadorDeEventos::responderConflict (mg_connection* connection, string msg, int ultimaVersion) {
	Logger::logInfo(msg);
	mg_send_status(connection, CODESTATUS_CONFLICT);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"error\": \"%s\", \"ultima version\": %i}", msg.c_str(), ultimaVersion);
}
