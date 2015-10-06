#ifndef INCLUDE_PARSERJSON_H_
#define INCLUDE_PARSERJSON_H_

#include "json/json.h"
#include "metadatos.h"
#include <map>


class ParserJson {

private:
	std::string verificarString(std::string key, Json::Value raiz);
	double verificarDouble(std::string key, Json::Value raiz);

public:
	ParserJson();
	virtual ~ParserJson();

	std::string serializarMetadatoArchivo(MetadatoArchivo metadato);
	std::string serializarMetadatoUsuario(MetadatoUsuario metadato);
	std::string serializarMetadatoSesion(MetadatosSesion metadato);
	std::string serializarMapa(std::map<std::string, std::string>& mapa);

	MetadatoArchivo deserializarMetadatoArchivo(std::string json);
	MetadatoUsuario deserializarMetadatoUsuario(std::string json);
	MetadatosSesion deserializarMetadatoSesion(std::string json);
	std::map<std::string, std::string> deserializarMapa(std::string json);
};

#endif /* INCLUDE_PARSERJSON_H_ */
