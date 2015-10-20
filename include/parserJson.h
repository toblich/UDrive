#ifndef INCLUDE_PARSERJSON_H_
#define INCLUDE_PARSERJSON_H_

#include "json/json.h"
#include <map>

#include "metadatosYConstantes.h"


class ParserJson {

private:
	static std::string verificarString(std::string key, Json::Value raiz);
	static double verificarDouble(std::string key, Json::Value raiz);
	static int verificarInt(std::string key, Json::Value raiz);
	ParserJson() {}

public:
	virtual ~ParserJson() {}

	static std::string serializarMetadatoArchivo(MetadatoArchivo metadato);
	static std::string serializarMetadatoUsuario(MetadatoUsuario metadato);
	static std::string serializarMetadatoSesion(MetadatosSesion metadato);
	static std::string serializarMapa(std::map<std::string, std::string>& mapa);

	static MetadatoArchivo deserializarMetadatoArchivo(std::string json);
	static MetadatoUsuario deserializarMetadatoUsuario(std::string json);
	static MetadatosSesion deserializarMetadatoSesion(std::string json);
	static std::map<std::string, std::string> deserializarMapa(std::string json);
};

#endif /* INCLUDE_PARSERJSON_H_ */
