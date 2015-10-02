#include "realizadorDeEventos.h"

RealizadorDeEventos::RealizadorDeEventos() {
	// TODO Auto-generated constructor stub

}

RealizadorDeEventos::~RealizadorDeEventos() {
	// TODO Auto-generated destructor stub
}

size_t RealizadorDeEventos::printfData(mg_connection* connection, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	size_t ret = mg_vprintf_data(connection, format, ap);
	va_end(ap);
	return ret;
}

string RealizadorDeEventos::getVar(mg_connection* connection, string varName){
	string variable;
	int maxLenght = 200;

	variable.resize(maxLenght);
	int result = mg_get_var(connection, varName.c_str(), (char*) variable.data(), maxLenght);
	//Si no existe una variable con ese varName, devuelvo un string vacio.
	if (result == -1) return string();
	variable.resize(strlen(variable.data()));

	return variable;
}

float RealizadorDeEventos::getVarFloat(mg_connection* connection, string varName) {
	return stof(getVar(connection, varName));
}

RealizadorDeEventos::DatosArchivo RealizadorDeEventos::getMultipartData(mg_connection* connection){
	DatosArchivo datosArch;
	const char* data;
	char varName[100], fileName[100];
	int dataLenght, n1 = 0, n2 = 0;

	while ((n2 = mg_parse_multipart(connection->content + n1, connection->content_len - n1,
			varName, sizeof(varName), fileName, sizeof(fileName), &data, &dataLenght)) > 0) {
		//mg_printf_data(connection, "var: %s, file_name: %s\n", varName, fileName);
		n1 += n2;
	}

	datosArch.fileData = data;
	datosArch.dataLenght = dataLenght;

	return datosArch;
}

mg_result RealizadorDeEventos::handler(mg_connection* connection) {
	string verb = string(connection->request_method);

	if 		(verb == "GET")		return this->GETHandler(connection);
	else if (verb == "POST")	return this->POSTHandler(connection);
	else if (verb == "PUT")		return this->PUTHandler(connection);
	else if (verb == "DELETE")	return this->DELETEHandler(connection);
	else {
		this->unsupportedMethod(connection);
		return MG_TRUE;
	}
}

mg_result RealizadorDeEventos::GETHandler(mg_connection* connection) {
	this->unsupportedMethod(connection);
	return MG_TRUE;
}

mg_result RealizadorDeEventos::PUTHandler(mg_connection* connection) {
	this->unsupportedMethod(connection);
	return MG_TRUE;
}

mg_result RealizadorDeEventos::POSTHandler(mg_connection* connection) {
	this->unsupportedMethod(connection);
	return MG_TRUE;
}

mg_result RealizadorDeEventos::DELETEHandler(mg_connection* connection) {
	this->unsupportedMethod(connection);
	return MG_TRUE;
}

void RealizadorDeEventos::unsupportedMethod(mg_connection* connection) {
	mg_send_status(connection, CODESTATUS_UNSUPPORTED_METHOD);
	mg_send_header(connection, contentType.c_str(), jsonType.c_str());
	printfData(connection, "{\"error\": \"metodo no soportado por esta clase\"}");
}
