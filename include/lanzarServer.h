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
	cout << "Lanzando servidor en el puerto " << puerto << endl;
	Server server(puerto, perfiles, sesiones, passwords, metadatos);

	while (server.isRunning()) {
		sleep(0.1);
	}

	Logger::logDebug("Cerrando servidor");
	cout << "Cerrando servidor" << endl;
	server.close();
}

#endif /* LANZAR_SERVER */
