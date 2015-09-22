#include <iostream>
#include "server.h"
#include "basededatos.h"

using namespace std;

int main(int argc, char** argv) {
	// LOG
	Logger* logger = new Logger();
	logger->escribirHoraEnElLog();
	delete logger;

	cout << "Hola Mundo!" << endl;

	// SERVER
	BD* perfiles = new BaseDeDatos("perfiles");		// Los deletes los hace el destructor del Server
	BD* sesiones = new BaseDeDatos("sesiones");
	BD* passwords = new BaseDeDatos("passwords");

	string puerto = "8080";
	Server server(puerto, perfiles, sesiones, passwords);
	cout << "Lanzando servidor en el puerto " << server.getListeningPort() << endl;
	for (;;) {
		server.pollServer(1000);
	}

	return 0;
}
