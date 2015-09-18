#include "server.h"

using namespace std;

Server::Server(std::string listeningPort) {
	server = mg_create_server(NULL, Server::eventHandler);
	mg_set_option(server, "listening_port", listeningPort.c_str());
	mg_set_option(server, "document_root", ".");
}

Server::~Server() {
	mg_destroy_server(&server);
}

int Server::eventHandler(mg_connection* connection, mg_event event) {
	switch (event) {

		case MG_AUTH:  return MG_TRUE;

		case MG_REQUEST:
			requestHandler(connection);
			return MG_TRUE;

		case MG_CLOSE:
			closeHandler(connection);
			return MG_TRUE;

		default: return MG_FALSE;
	}
}

void Server::closeHandler(mg_connection* connection){
	if(connection){
		free(connection->connection_param);
		connection->connection_param = NULL;
	}
}

void Server::pollServer(int milliseconds) {
	mg_poll_server(server, milliseconds);
}

string Server::mensajeSegunURI(string uri) {
	if (uri == "/")
		return "Root";
	if (uri == "/santi")
		return "Roro";
	else return "Te equivocaste MUAJAJAJAJA";
}

void Server::requestHandler(mg_connection* connection) {
	string verb = string(connection->request_method);
	if (verb == "GET"){
		GETHandler(connection);
	}
	else if (verb == "POST"){
		POSTHandler(connection);
	}
	else if (verb == "PUT"){
		PUTHandler(connection);
	}
	else if (verb == "DELETE"){
		DELETEHandler(connection);
	}
	else
		cout << "WTF? No existe este verbo: " << verb << endl;
}

void Server::GETHandler(mg_connection* connection) {
	string uri(connection->uri);
	mg_printf_data(connection, "Hola!: [%s]\n", Server::mensajeSegunURI(uri).c_str());
}

void Server::PUTHandler(mg_connection* connection) {
	//Recibe un archivo de texto
	string uri(connection->uri);
	string filename = uri.substr(1, uri.length()-1); // Supone que no hay carpetas
	ofstream outFile(filename.data(), std::ofstream::binary);
	outFile.write(connection->content, connection->content_len);
	outFile.close();
	mg_printf_data(connection, "PUT [%s]\n", Server::mensajeSegunURI(uri).c_str());
}

void Server::POSTHandler(mg_connection* connection) {
	//Recibe un archivo binario
	string uri(connection->uri);
	const char *data;
	int data_len, n1, n2;
	char var_name[100], file_name[100];
	n1 = n2 = 0;
	while ((n2 = mg_parse_multipart(connection->content + n1, connection->content_len - n1,
							  var_name, sizeof(var_name), file_name, sizeof(file_name), &data, &data_len)) > 0) {
		mg_printf_data(connection, "var: %s, file_name: %s, size: %d bytes<br>",
				   var_name, file_name, data_len);
		n1 += n2;
	}
	ofstream outFile(file_name, std::ofstream::binary);
	outFile.write(data, data_len);
	outFile.close();
}

void Server::DELETEHandler(mg_connection* connection) {
	string uri(connection->uri);
	mg_printf_data(connection, "DELETE [%s]\n", Server::mensajeSegunURI(uri).c_str());
}
