#include "server.h"

using namespace std;

bool Server::running = true;
int Server::pollMilisec = POLL_SERVER_DEFAULT;

Server::Server (string listeningPort, BD* perfiles, BD* sesiones, BD* passwords, BD* metadatos, Configuracion conf) {

	//BDs
	this->perfiles = perfiles;
	this->sesiones = sesiones;
	this->passwords = passwords;
	this->metadatos = metadatos;

	//Manejadores
	manejadorUsuarios = new ManejadorDeUsuarios(perfiles, sesiones, passwords);
	manejadorAYM = new ManejadorArchivosYMetadatos(metadatos, conf.pathFS);

	//API REST
	mapaURI.insert(pair<string, RealizadorDeEventos*>("profile", new Profile(manejadorUsuarios, manejadorAYM)));
	mapaURI.insert(pair<string, RealizadorDeEventos*>("session", new Session(manejadorUsuarios)));
	mapaURI.insert(pair<string, RealizadorDeEventos*>("file", new File(manejadorUsuarios, manejadorAYM)));
	mapaURI.insert(pair<string, RealizadorDeEventos*>("metadata", new Metadata(manejadorUsuarios, manejadorAYM)));
	mapaURI.insert(pair<string, RealizadorDeEventos*>("folder", new Folder(manejadorUsuarios, manejadorAYM)));

	//Atributos
	this->cantThreads = conf.cantThreads;
	pollMilisec = conf.tiempoPollServer;

	//Servers
	mg_server* primerServer = mg_create_server((void *) this, Server::mgEventHandler);
	mg_set_option(primerServer, "listening_port", listeningPort.c_str());
	servers.push_back(primerServer);
	for (int i = 1; i < this->cantThreads; i++) {
		mg_server* server = mg_create_server((void *) this, Server::mgEventHandler);
		mg_copy_listeners(primerServer, server);
		servers.push_back(server);
	}

	//Threads
	for (auto &server : servers) {
		threads.push_back(new thread(pollServer, server));
	}
}

Server::~Server () {
	//Threads
	for (auto &thread : threads) {
		thread->join();
		delete thread;
	}

	//Servers
	for (auto &server : servers) {
		mg_destroy_server(&server);
	}

//	perfiles->deleteBD(); //
//	sesiones->deleteBD(); //
//	passwords->deleteBD(); //
//	metadatos->deleteBD(); //
	//BDs
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
	delete mapaURI.at("folder");

	Logger::logDebug("Se cerro el servidor");
}

int Server::mgEventHandler (mg_connection* connection, mg_event event) {
	Server* _server = (Server*) connection->server_param;
	return _server->eventHandler(connection, event);
}

mg_result Server::eventHandler (mg_connection* connection, mg_event event) {
	switch (event) {

	case MG_AUTH:
		return MG_TRUE;

	case MG_REQUEST:
		return requestHandler(connection);

	case MG_CLOSE:
		return closeHandler(connection);

	default:
		return MG_FALSE;
	}
}

mg_result Server::closeHandler (mg_connection* connection) {
	if (connection) {
		free(connection->connection_param);
		connection->connection_param = NULL;
	}
	return MG_TRUE;
}

void Server::pollServer (mg_server* server) {
	while (running) {
		mg_poll_server(server, pollMilisec);
	}
}

mg_result Server::requestHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	Logger::logTrace("Se accedio a la URI: " + uri);
	vector<string> uris = ParserURI::parsear(uri, URI_DELIM);

	if (uris.size() > 0) {
		try {
			RealizadorDeEventos* evento = mapaURI.at(uris[0]);
			return evento->handler(connection);
		}catch (const out_of_range& oor){
			mg_send_status(connection, NOTFOUND);
			mg_printf_data(connection, "Error, recurso no encontrado\n");
			return MG_TRUE;
		}
	}
	mg_send_status(connection, NOTFOUND);
	mg_printf_data(connection, "Error, recurso no encontrado\n");
	return MG_TRUE;
}
