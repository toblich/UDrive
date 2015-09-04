#include "Server.h"

using namespace std;

Server::Server(std::string listeningPort) {
	server = mg_create_server(NULL, Server::eventHandler);
	mg_set_option(server, "listening_port", listeningPort.c_str());
}

Server::~Server() {
	mg_destroy_server(&server);
}

int Server::eventHandler(mg_connection* connection, mg_event event) {
	switch (event) {
		case MG_AUTH: return MG_TRUE;
		case MG_REQUEST: {
			string uri = string(connection->uri);
			mg_printf_data(connection, "Hola!: [%s]", Server::mensajeSegunURI(uri).c_str());
			return MG_TRUE;
		}
		default: return MG_FALSE;
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
