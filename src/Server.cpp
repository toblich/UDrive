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
	cout << "eventHandler" << endl;
	switch (event) {
		case MG_AUTH: return MG_TRUE;
		case MG_REQUEST:
			mg_printf_data(connection, "Hello! Requested URI is [%s]", connection->uri);
		return MG_TRUE;
		default: return MG_FALSE;
	}
}

void Server::pollServer(int milliseconds) {
	mg_poll_server(server, milliseconds);
}
