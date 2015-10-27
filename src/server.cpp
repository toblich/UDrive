#include "server.h"

using namespace std;


bool Server::running = true;

Server::Server (string listeningPort, BD* perfiles, BD* sesiones, BD* passwords, BD* metadatos) {

	//BD
	this->perfiles = perfiles;
	this->sesiones = sesiones;
	this->passwords = passwords;
	this->metadatos = metadatos;

	//Manejadores
	manejadorUsuarios = new ManejadorDeUsuarios(perfiles, sesiones, passwords);
	manejadorAYM = new ManejadorArchivosYMetadatos(metadatos);

	//API REST
	mapaURI.insert(pair<string, RealizadorDeEventos*>("profile", new Profile(manejadorUsuarios, manejadorAYM)));
	mapaURI.insert(pair<string, RealizadorDeEventos*>("session", new Session(manejadorUsuarios)));
	mapaURI.insert(pair<string, RealizadorDeEventos*>("file", new File(manejadorUsuarios, manejadorAYM)));
	mapaURI.insert(pair<string, RealizadorDeEventos*>("metadata", new Metadata(manejadorUsuarios, manejadorAYM)));
	mapaURI.insert(pair<string, RealizadorDeEventos*>("folder", new Folder(manejadorUsuarios, manejadorAYM)));

	//Server
	mg_server* primerServer = mg_create_server((void *) this, Server::mgEventHandler);
	mg_set_option(primerServer, "listening_port", listeningPort.c_str());
//	mg_set_option(primerServer, "document_root", ".");
	servers.push_back(primerServer);
	for (int i = 1; i < 2; i++) {
		mg_server* server = mg_create_server((void *) this, Server::mgEventHandler);
		mg_copy_listeners(primerServer, server);
//		mg_set_option(server, "document_root", ".");
		servers.push_back(server);
	}
	for (auto &server : servers) {
		threads.push_back(new thread(pollServer, server));
//		mg_start_thread(serv_func, (void*) server);
	}
}

Server::~Server () {
	Logger::logDebug("DESTRUCTOR");
	//Server
//	for (auto &thread : threads) {
//		thread->join();
//		delete thread;
//		cout << "Se cerro un thread" << endl;
//	}
//	for (auto &server : servers) {
//		Logger::logDebug("Se destuye un servidor");
//		mg_destroy_server(&server);
//	}

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
	delete mapaURI.at("folder");
	Logger::logDebug("FIN DESTRUCTOR");
}

void Server::close() {
	running = false;
	for (auto &thread : threads) {
		thread->join();
		delete thread;
		cout << "Se cerro un thread" << endl;
	}
	for (auto &server : servers) {
		Logger::logDebug("Se destuye un servidor");
		mg_destroy_server(&server);
	}
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
		mg_poll_server(server, POLL_MILLISEC);
	}
}

void* Server::serv_func(void* server) {
	pollServer((mg_server*) server);
	return NULL;
}


size_t sarasa(mg_connection* connection, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	size_t ret = mg_vprintf_data(connection, format, ap);
	va_end(ap);
	return ret;
}

mg_result Server::requestHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');

	if (uris.size() > 0) {
		if (uris[0] == "close") {
//			mg_printf_data(connection, "Se cerro el servidor\n");
			mg_send_status(connection, 200);
			mg_send_header(connection, "Content-Type", "text/plain");
			sarasa(connection, "Se cerro el servidor\n");
			Logger::logDebug("CLOSE");
			running = false;
			return MG_TRUE;
		}
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
