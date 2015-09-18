#include <iostream>
#include "server.h"

using namespace std;

int main(int argc, char** argv) {
	cout << "Hola Mundo!" << endl;

	// LOG
	Logger* logger = new Logger();
	logger->escribirHoraEnElLog();
	delete logger;

	// SERVER
	string puerto = "8080";
	Server server(puerto);
	cout << "Lanzando servidor en el puerto " << server.getListeningPort() << endl;
	for (;;) {
		server.pollServer(1000);
	}

	return 0;
}
