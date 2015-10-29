#ifndef PROFILE_H_
#define PROFILE_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

class Profile : public RealizadorDeEventos {
public:
	Profile(ManejadorDeUsuarios* manejadorUsuarios, ManejadorArchivosYMetadatos* manejarAyM);
	virtual ~Profile();

protected:
	ManejadorDeUsuarios* manejadorUs;
	ManejadorArchivosYMetadatos* manejadorAyM;


	void enviarUsuariosCon(mg_connection* connection, string nombre);
	void enviarFotoPerfil(mg_connection* connection, string filePath, string user);
	void enviarPerfil(mg_connection* connection, string userPerfil);
	mg_result GETHandler(mg_connection* connection);
	mg_result PUTHandler(mg_connection* connection);
	mg_result POSTHandler(mg_connection* connection);
};

#endif /* PROFILE_H_ */
