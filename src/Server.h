#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <iostream>
#include "mongoose.h"

class Server {
public:
	Server(std::string portNumber);
	virtual ~Server();

	static int eventHandler(mg_connection *connection, mg_event event);
	void pollServer(int milliseconds);
	const char* getListeningPort() { return mg_get_option(server, "listening_port");}

private:
	mg_server* server;
};

#endif /* SERVER_H_ */
