#ifndef LANZAR_SERVER
#define LANZAR_SERVER 

#include <iostream>
#include <string>
#include "bd.h"
#include "server.h"

using namespace std;

void lanzarServer(BD* perfiles, BD* sesiones, BD* passwords, BD* metadatos) {
	Logger logger;
	logger.escribirHoraEnElLog();

	string puerto = "8080";
	Server server(puerto, perfiles, sesiones, passwords, metadatos);
	cout << "Lanzando servidor en el puerto " << server.getListeningPort() << endl;

	while (server.isRunning()) {
		server.pollServer(1000);
	}

	cout << "Cerrando servidor" << endl;
}

#endif /* LANZAR_SERVER */