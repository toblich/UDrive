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

private:
	void enviarArchivo(const string& completePath, mg_connection* connection);
	MetadatoArchivo extractMetadataFrom(const vector<string>& nombreYExtension,
			const string& user, const vector<string>& uris);
	void subirArchivo (const vector<string>& uris, const DatosArchivo& datosArch, const string& user, mg_connection* connection);
	void actualizarUltimaUbicacion(string user, string latitud, string longitud);
};

#endif /* FILE_H_ */
