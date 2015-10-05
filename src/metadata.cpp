#include "metadata.h"

Metadata::Metadata(ManejadorArchivosYMetadatos* manejadorArchYMet) {
	this->manejadorArchYMet = manejadorArchYMet;
}

Metadata::~Metadata() {
}

mg_result Metadata::GETHandler(mg_connection* connection) {
	return MG_TRUE;
}

mg_result Metadata::PUTHandler(mg_connection* connection) {
	return MG_TRUE;
}
