#ifndef METADATA_H_
#define METADATA_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

class Metadata : public RealizadorDeEventos {
public:
	Metadata(ManejadorArchivosYMetadatos* manejadorArchYMet);
	virtual ~Metadata();

protected:
	ManejadorArchivosYMetadatos* manejadorArchYMet;

	mg_result GETHandler(mg_connection* connection);
	mg_result PUTHandler(mg_connection* connection);
};

#endif /* METADATA_H_ */
