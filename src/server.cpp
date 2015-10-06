#include "server.h"

using namespace std;

Server::Server(string listeningPort, BD* perfiles, BD* sesiones, BD* passwords, BD* metadatos) {
	//Server
	server = mg_create_server((void *) this, Server::mgEventHandler);
	mg_set_option(server, "listening_port", listeningPort.c_str());
	mg_set_option(server, "document_root", ".");
	running = true;

	//BD
	this->perfiles = perfiles;
	this->sesiones = sesiones;
	this->passwords = passwords;
	this->metadatos = metadatos;

	//Manejadores
	manejadorUsuarios = new ManejadorDeUsuarios(perfiles, sesiones, passwords);
	manejadorAYM = new ManejadorArchivosYMetadatos(metadatos);

	//API REST
	mapaURI.insert(pair<string,RealizadorDeEventos*>("profile", new Profile(manejadorUsuarios, manejadorAYM)));
	mapaURI.insert(pair<string,RealizadorDeEventos*>("session", new Session(manejadorUsuarios)));
	mapaURI.insert(pair<string,RealizadorDeEventos*>("file", new File(manejadorUsuarios, manejadorAYM)));
	mapaURI.insert(pair<string,RealizadorDeEventos*>("metadata", new Metadata(manejadorUsuarios, manejadorAYM)));
}

Server::~Server() {
	//Server
	mg_destroy_server(&server);

	//TODO: Sacar estas instrucciones para que despues persistan los datos.
	perfiles->deleteBD(); //
	sesiones->deleteBD(); //
	passwords->deleteBD(); //
	metadatos->deleteBD(); //
	//BD
	delete perfiles;
	delete sesiones;
	delete passwords;
	delete metadatos;

	//Manejadores
	delete manejadorUsuarios;
	delete manejadorAYM;

	//API REST
	delete mapaURI.at("profile");
	delete mapaURI.at("session");
	delete mapaURI.at("file");
	delete mapaURI.at("metadata");
}

int Server::mgEventHandler(mg_connection* connection, mg_event event) {
	Server* _server = (Server*) connection->server_param;
	return _server->eventHandler(connection, event);
}

mg_result Server::eventHandler(mg_connection* connection, mg_event event) {
	switch (event) {

		case MG_AUTH: return MG_TRUE;

		case MG_REQUEST: return requestHandler(connection);

		case MG_CLOSE: return closeHandler(connection);

		default: return MG_FALSE;
	}
}

mg_result Server::closeHandler(mg_connection* connection){
	if(connection){
		free(connection->connection_param);
		connection->connection_param = NULL;
	}
	return MG_TRUE;
}

void Server::pollServer(int milliseconds) {
	mg_poll_server(server, milliseconds);
}

mg_result Server::requestHandler(mg_connection* connection) {
	ParserURI parser;
	string uri = string(connection->uri);
	vector<string> uris = parser.parsear(uri, '/');

	if (uris.size() > 0){
		if (uris[0] == "close"){
			running = false;
			mg_printf_data(connection, "Se cerro el servidor\n");
			return MG_TRUE;
		}
		try {
			RealizadorDeEventos* evento = mapaURI.at(uris[0]);
			return evento->handler(connection);

		}catch (const out_of_range& oor){
			mg_printf_data(connection, "Error, recurso no encontrado\n");
			return MG_TRUE;
		}
	}
	mg_printf_data(connection, "Error, URI incorrecta\n");
	return MG_TRUE;
}
