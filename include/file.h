#ifndef FILE_H_
#define FILE_H_

#include "realizadorDeEventos.h"

class File : public RealizadorDeEventos {
public:
	File(ManejadorDeUsuarios* manejadorUs, ManejadorArchivosYMetadatos* manejadorArchYMet);
	virtual ~File();

protected:
	ManejadorDeUsuarios* manejadorUs;
	ManejadorArchivosYMetadatos* manejadorArchYMet;

	mg_result GETHandler(mg_connection* connection);
	mg_result PUTHandler(mg_connection* connection);
	mg_result DELETEHandler(mg_connection* connection);

};

#endif /* FILE_H_ */
