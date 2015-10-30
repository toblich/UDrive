#include "manejadorMetadatos.h"

ManejadorMetadatos::ManejadorMetadatos (BD* dbMetadatos, Validador* validador) {
	this->dbMetadatos = dbMetadatos;
	this->validador = validador;
}

string ManejadorMetadatos::actualizarUsuarioFechaModificacion (string jsonMetadatos, string usernameModificacion) {
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadatos);
	metadato.usuarioUltimaModificacion = usernameModificacion;

	struct tm *tiempo;
	time_t fecha_sistema;
	time(&fecha_sistema);
	tiempo = localtime(&fecha_sistema);
	int anio = tiempo->tm_year + 1900;
	int mes = tiempo->tm_mon + 1;
	int dia = tiempo->tm_mday;
	string fecha = to_string(dia) + "/" + to_string(mes) + "/" + to_string(anio);
	metadato.fechaUltimaModificacion = fecha;
	string nuevosMetadatos = ParserJson::serializarMetadatoArchivo(metadato);
	return nuevosMetadatos;
}
