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
	void busquedaEtiqueta(mg_connection* connection, string user, string etiqueta);
	void busquedaExtension(mg_connection* connection, string user, string extension);
	void busquedaNombre(mg_connection* connection, string user, string nombre);
	void busquedaPropietario(mg_connection* connection, string user, string propietario);
	void responderBusqueda(mg_connection* connection, string resultado);
	mg_result GETHandler(mg_connection* connection);
	mg_result PUTHandler(mg_connection* connection);
};

#endif /* METADATA_H_ */
