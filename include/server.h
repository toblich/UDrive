#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <iostream>
#include <fstream>
#include "logger.h"
#include "mongoose.h"
#include "bd.h"
#include "manejadorDeUsuarios.h"
#include "parserJson.h"

using namespace std;

class Server {
public:
	Server(string portNumber, BD* perfiles, BD* sesiones, BD* passwords);
	virtual ~Server();

	enum mg_result eventHandler(mg_connection *connection, mg_event event);
	static int mgEventHandler(mg_connection *connection, mg_event event);
	string mensajeSegunURI(string uri);
	void pollServer(int milliseconds);
	const char* getListeningPort() { return mg_get_option(server, "listening_port"); }
	enum mg_result getMultipartData(mg_connection* connection);
	string getVar(mg_connection* connection, string varName);

private:
	ParserJson parser;
	mg_server* server;
	ManejadorDeUsuarios* manejador;
	BD* perfiles;
	BD* sesiones;
	BD* passwords;

	enum mg_result closeHandler(mg_connection* connection);
	enum mg_result requestHandler(mg_connection* connection);
	enum mg_result GETHandler(mg_connection* connection);
	enum mg_result PUTHandler(mg_connection* connection);
	enum mg_result POSTHandler(mg_connection* connection);
	enum mg_result DELETEHandler(mg_connection* connection);
};

#endif /* SERVER_H_ */
