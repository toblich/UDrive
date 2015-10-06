#ifndef FOLDER_H_
#define FOLDER_H_

#include "realizadorDeEventos.h"
#include "parserURI.h"

using namespace std;

class Folder : public RealizadorDeEventos {
public:
	Folder(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet);
	virtual ~Folder();

protected:
	ManejadorDeUsuarios* manejadorUs;
	ManejadorArchivosYMetadatos* manejadorArchYMet;

	mg_result GETHandler(mg_connection* connection);
	mg_result PUTHandler(mg_connection* connection);
};

#endif /* FOLDER_H_ */
