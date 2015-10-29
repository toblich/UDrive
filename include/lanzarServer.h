#ifndef LANZAR_SERVER
#define LANZAR_SERVER 

#include <iostream>
#include <string>
#include "bd.h"
#include "server.h"
#include <csignal>

using namespace std;

void lanzarServer(BD* perfiles, BD* sesiones, BD* passwords, BD* metadatos) {
	Logger logger;
	logger.escribirHoraEnElLog();

	signal(SIGTERM, Server::close);
	signal(SIGINT, Server::close);

	string puerto = "8080";
	cout << "Lanzando servidor en el puerto " << puerto << endl;
	Server server(puerto, perfiles, sesiones, passwords, metadatos);

	while (server.isRunning()) {
		sleep(0.1);
	}

	Logger::logDebug("Cerrando servidor");
	cout << "Cerrando servidor" << endl;
}



#endif /* LANZAR_SERVER */
