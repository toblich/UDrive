#include "realizadorDeEventos.h"

RealizadorDeEventos::RealizadorDeEventos() {
	// TODO Auto-generated constructor stub

}

RealizadorDeEventos::~RealizadorDeEventos() {
	// TODO Auto-generated destructor stub
}

string RealizadorDeEventos::getVar(mg_connection* connection, string varName){
	string value;
	int max = 100;
	int n = 0;

	value.resize(max);
	mg_get_var_n(connection, varName.c_str(), (char*) value.data(), max, n);

	value.resize(strlen(value.data()));

	mg_printf_data(connection, "Variable: '%s'\n", value.c_str());

	return value;
}

void RealizadorDeEventos::getMultipartData(mg_connection* connection){
	const char *data;
	int data_len, n1, n2;
	char var_name[100], file_name[100];

	n1 = n2 = 0;

	while ((n2 = mg_parse_multipart(connection->content + n1, connection->content_len - n1,
				   var_name, sizeof(var_name), file_name, sizeof(file_name), &data, &data_len)) > 0) {
		mg_printf_data(connection, "var: %s, file_name: %s, size: %d bytes\n", var_name, file_name, data_len);
		n1 += n2;
	}
	ofstream outFile(file_name, ofstream::binary);
	outFile.write(data, data_len);
	outFile.close();
}

mg_result RealizadorDeEventos::handler(mg_connection* connection) {
	string verb = string(connection->request_method);

	if (verb == "GET")			return this->GETHandler(connection);
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
	mg_printf_data(connection, "Error: metodo no soportado por esta clase\n");
}
