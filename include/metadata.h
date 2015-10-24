#ifndef METADATA_H_
#define METADATA_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

class Metadata : public RealizadorDeEventos {
public:
	Metadata(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet);
	virtual ~Metadata();

protected:
	ManejadorDeUsuarios* manejadorUs;
	ManejadorArchivosYMetadatos* manejadorArchYMet;

	void GETMetadatos(mg_connection* connection, vector<string> uris, string user);
	void busquedaEtiquetas(mg_connection* connection, string user);
	void busquedaExtension(mg_connection* connection, string user);
	void busquedaNombre(mg_connection* connection, string user);
	void busquedaPropietario(mg_connection* connection, string user);
	void responderBusqueda(mg_connection* connection, string resultado);
	mg_result GETHandler(mg_connection* connection);
	mg_result PUTHandler(mg_connection* connection);
};

#endif /* METADATA_H_ */
