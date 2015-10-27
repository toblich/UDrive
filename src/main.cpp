#include <iostream>
#include "server.h"
#include "basededatos.h"
#include "lanzarServer.h"

using namespace std;

int main(int argc, char** argv) {
	if (argv[1] and string(argv[1]) == "--no-action") return 0;
	// BD
	// Los deletes los hace el destructor del Server
	BD* perfiles = new BaseDeDatos("perfiles");
	BD* sesiones = new BaseDeDatos("sesiones");
	BD* passwords = new BaseDeDatos("passwords");
	BD* metadatos = new BaseDeDatos("metadatos");

	lanzarServer(perfiles, sesiones, passwords, metadatos);

	Logger::logDebug("FIN MAIN");
	return 0;
}
