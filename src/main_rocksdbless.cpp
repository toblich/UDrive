#include <iostream>
#include "server.h"
#include "mapDB.h"
#include "lanzarServer.h"

using namespace std;

int main(int argc, char** argv) {
	if (argv[1] and string(argv[1]) == "--no-action") return 0;
	
	// BD
	// Los deletes los hace el destructor del Server
	BD* perfiles = new MapDB();
	BD* sesiones = new MapDB();
	BD* passwords = new MapDB();
	BD* metadatos = new MapDB();

	lanzarServer(perfiles, sesiones, passwords, metadatos);

	Logger::logDebug("FIN MAIN");

	return 0;
}
