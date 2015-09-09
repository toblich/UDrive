#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <iostream>
#include <fstream>
#include "mongoose.h"

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

	static void requestHandler(mg_connection* connection);
	static void GETHandler(mg_connection* connection);
	static void PUTHandler(mg_connection* connection);
	static void POSTHandler(mg_connection* connection);
	static void DELETEHandler(mg_connection* connection);
};

#endif /* SERVER_H_ */
