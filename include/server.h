#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <iostream>
#include <fstream>
#include "mongoose.h"
#include "basededatos.h"
#include "manejadorDeUsuarios.h"

using namespace std;

class Server {
public:
	Server(std::string portNumber);
	virtual ~Server();

	static int eventHandler(mg_connection *connection, mg_event event);
	static string mensajeSegunURI(string uri);
	void pollServer(int milliseconds);
	const char* getListeningPort() { return mg_get_option(server, "listening_port");}

private:
	mg_server* server;
	ManejadorDeUsuarios* manejador;
	BaseDeDatos* perfiles;
	BaseDeDatos* sesiones;
	BaseDeDatos* passwords;

	static void closeHandler(mg_connection* connection);
	static enum mg_result requestHandler(mg_connection* connection);
	static enum mg_result GETHandler(mg_connection* connection);
	static enum mg_result PUTHandler(mg_connection* connection);
	static enum mg_result POSTHandler(mg_connection* connection);
	static enum mg_result DELETEHandler(mg_connection* connection);
};

#endif /* SERVER_H_ */
