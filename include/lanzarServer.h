#ifndef LANZAR_SERVER
#define LANZAR_SERVER 

#include <iostream>
#include <string>
#include "bd.h"
#include "server.h"
#include <csignal>

using namespace std;

/**
 * @brief Se encarga de lanzar el servidor y cerrarlo en caso de que reciba alguna señal.
 */
void lanzarServer(BD* perfiles, BD* sesiones, BD* passwords, BD* metadatos) {
	Logger::escribirHoraEnElLog();

	signal(SIGTERM, Server::close);
	signal(SIGINT, Server::close);

	string puerto = "8080";
	cout << "Lanzando servidor en el puerto " << puerto << endl;
	Server server(puerto, perfiles, sesiones, passwords, metadatos);

	while (server.isRunning()) {
		usleep(1000);
	}

	Logger::logDebug("Cerrando servidor");
	cout << "Cerrando servidor" << endl;
}

#endif /* LANZAR_SERVER */
