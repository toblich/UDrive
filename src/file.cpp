#include "file.h"

File::File(ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorArchYMet = manejadorArchYMet;
}

File::~File() {
	// TODO Auto-generated destructor stub
}

mg_result File::GETHandler(mg_connection* connection) {

	return MG_TRUE;
}

mg_result File::PUTHandler(mg_connection* connection) {

	return MG_TRUE;
}

mg_result File::DELETEHandler(mg_connection* connection) {

	return MG_TRUE;
}
