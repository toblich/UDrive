#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "logger.h"
#include "profile.h"
#include "session.h"
#include "metadata.h"
#include "folder.h"
#include "file.h"
#include "mongoose.h"
#include "bd.h"
#include "parserURI.h"

#define NOTFOUND 404

using namespace std;

#define POLL_MILLISEC 1000

class Server {
public:
	Server(string portNumber, BD* perfiles, BD* sesiones, BD* passwords, BD* metadatos);
	virtual ~Server();

	mg_result eventHandler(mg_connection *connection, mg_event event);
	static int mgEventHandler(mg_connection *connection, mg_event event);
	static void *pollServer(void* server);
//	const char* getListeningPort() { return mg_get_option(server, "listening_port"); }
	bool isRunning() { return running; }

private:
	static bool running;
	map<string, RealizadorDeEventos*> mapaURI;
//	mg_server* server;
	ManejadorDeUsuarios* manejadorUsuarios;
	ManejadorArchivosYMetadatos* manejadorAYM;
	BD* perfiles;
	BD* sesiones;
	BD* passwords;
	BD* metadatos;

	mg_result closeHandler(mg_connection* connection);
	mg_result requestHandler(mg_connection* connection);
};

#endif /* SERVER_H_ */
