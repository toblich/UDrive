#include <iostream>
#include "server.h"
#include "basededatos.h"
#include "lanzarServer.h"

using namespace std;

int main(int argc, const char** argv) {
	if (argv[1] and string(argv[1]) == "--no-action") return 0;

	char* pathArchConf;
	if ( argv[1] )
		pathArchConf = argv[1];
	else
		pathArchConf = "";

	Configuracion conf = ParserJson::leerArchivoConfiguracion(pathArchConf);
	Logger::setNivelLogger(conf.nivelLog);
	Logger::setTamanoMaximo(conf.tamMaxLog);

	// Creo la carpeta de las DBs
	mkdir(conf.pathDB.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	// BD
	// Los deletes los hace el destructor del Server
	BD* perfiles = new BaseDeDatos("perfiles", conf.pathDB);
	BD* sesiones = new BaseDeDatos("sesiones", conf.pathDB);
	BD* passwords = new BaseDeDatos("passwords", conf.pathDB);
	BD* metadatos = new BaseDeDatos("metadatos", conf.pathDB);

	lanzarServer(perfiles, sesiones, passwords, metadatos, conf);

	Logger::logDebug("FIN MAIN");
	return 0;
}
