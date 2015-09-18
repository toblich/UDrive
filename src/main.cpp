#include <iostream>
#include "manejadorArchivosYMetadatos.h"

using namespace std;

int main(int argc, char** argv) {
	cout << "Hola Mundo!" << endl;

	BaseDeDatos* db = new BaseDeDatos();
	ManejadorArchivosYMetadatos manejador(db);

	string directorio = "hola/puto/como estas";
	manejador.crearCarpeta(directorio);

	directorio = "hola/puto/como estas/bien vos?/bien";
	manejador.crearCarpeta(directorio);

	cout << "Terminado el creado de carpetas." << endl;

	cout << endl;
	manejador.subirArchivo("pablo", "hola/puto/juan.txt", "hola mi amor", 12);

	delete db;


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
