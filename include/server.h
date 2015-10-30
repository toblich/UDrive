#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "logger.h"
#include "profile.h"
#include "session.h"
#include "metadata.h"
#include "folder.h"
#include "file.h"
#include "mongoose.h"
#include "bd.h"
#include "parserURI.h"
#include <thread>
#include <stdarg.h>

#define NOTFOUND 404

using namespace std;

#define POLL_MILLISEC 1
#define CANT_THREADS 10

/**
 * @brief Se encarga del manejo del Servidor y de la redirección de los request hacia las clases de la API REST.
 */

class Server {
public:
	/**
	 * @brief Constructor: Crea un Servidor.
	 *
	 * @param portNumber		string con el puerto a traves del cual se va a comunicar el servidor.
	 * @param perfiles			BD* a una base de datos de perfiles.
	 * @param sesiones			BD* a una base de datos de sesiones.
	 * @param passwords			BD* a una base de datos de passwords.
	 * @param metadatos			BD* a una base de datos de metadatos.
	 *
	 */
	Server(string portNumber, BD* perfiles, BD* sesiones, BD* passwords, BD* metadatos);
	virtual ~Server();

	/**
	 * @brief Función que se utiliza para redirigir el comportamiento del Servidor dependiendo el
	 * evento que le llegó.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param event				mg_event con el evento que le llegó al Servidor a partir de un request
	 *
	 * @retval MG_TRUE 			en caso de que el evento sea correcto
	 * @retval MG_FALSE 		en caso de que el evento no se pueda procesar
	 */
	mg_result eventHandler(mg_connection *connection, mg_event event);
	/**
	 * @brief Función estática que usa el mg_create_server para redirigir las conexiones.
	 *
	 * @param connection		mg_connection* con la conexion Cliente-Servidor.
	 * @param event				mg_event con el evento que le llegó al Servidor a partir de un request
	 *
	 */
	static int mgEventHandler(mg_connection *connection, mg_event event);
	/**
	 * @brief Función que realiza el poll en el Server para recibir las requests.
	 *
	 * @param server		mg_server* con el server que se está utilizando
	 *
	 */
	static void pollServer(mg_server* server);
	bool isRunning() { return running; }
	static void close(int signal) { running = false; };

private:
	static bool running;
	map<string, RealizadorDeEventos*> mapaURI; ///< Mapa que se utiliza para redirigir hacia la API REST
	vector<mg_server*> servers; ///< Vector de servers utilizados (existe uno por cada thread)
	vector<thread*> threads;    ///< Vector de threads corriendo
	ManejadorDeUsuarios* manejadorUsuarios;
	ManejadorArchivosYMetadatos* manejadorAYM;
	BD* perfiles;
	BD* sesiones;
	BD* passwords;
	BD* metadatos;

	mg_result closeHandler(mg_connection* connection);
	mg_result requestHandler(mg_connection* connection);
};

#endif /* SERVER_H_ */
