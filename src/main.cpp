#include <iostream>
#include "server.h"
#include "basededatos.h"

using namespace std;

int main(int argc, char** argv) {

	// LOG
	Logger* logger = new Logger();
	logger->escribirHoraEnElLog();
	delete logger;

	// BD
	// Los deletes los hace el destructor del Server
	BD* perfiles = new BaseDeDatos("perfiles");
	BD* sesiones = new BaseDeDatos("sesiones");
	BD* passwords = new BaseDeDatos("passwords");
	BD* metadatos = new BaseDeDatos("metadatos");

	//SERVER
	string puerto = "8080";
	Server server(puerto, perfiles, sesiones, passwords, metadatos);
	cout << "Lanzando servidor en el puerto " << server.getListeningPort() << endl;

	while (server.isRunning()) {
		server.pollServer(1000);
	}

	cout << "Cerrando servidor" << endl;

	return 0;
}
