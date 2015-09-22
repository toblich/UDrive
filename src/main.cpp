#include <iostream>
#include "manejadorArchivosYMetadatos.h"

using namespace std;

int main(int argc, char** argv) {
	// LOG
	Logger* logger = new Logger();
	logger->escribirHoraEnElLog();
	delete logger;


	cout << "Hola Mundo!" << endl;


	BaseDeDatos* db = new BaseDeDatos();
	ManejadorArchivosYMetadatos manejador(db, "fileSystem_test");

	string directorio = "hola/puto/como estas";
	manejador.crearCarpeta(directorio);

	directorio = "hola/puto/como estas/bien vos?/bien";
	manejador.crearCarpeta(directorio);

	cout << "Terminado el creado de carpetas." << endl;

	cout << endl;
	manejador.subirArchivo("pablo", "hola/puto/juan.txt", "hola mi amor", 12);

	delete db;


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
