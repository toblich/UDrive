#ifndef INCLUDE_PARSERJSON_H_
#define INCLUDE_PARSERJSON_H_

#include "json/json.h"
#include <map>

#include "metadatosYConstantes.h"

using std::string;


class ParserJson {

private:
	static string verificarString(string key, Json::Value raiz);
	static double verificarDouble(string key, Json::Value raiz);
	static int verificarInt(string key, Json::Value raiz);
	ParserJson() {}

public:
	virtual ~ParserJson() {}

	static string serializarMetadatoArchivo(MetadatoArchivo metadato);
	static string serializarMetadatoUsuario(MetadatoUsuario metadato);
	static string serializarMetadatoSesion(MetadatosSesion metadato);
	static string serializarMapa(std::map<string, string>& mapa);

	static MetadatoArchivo deserializarMetadatoArchivo(string json);
	static MetadatoUsuario deserializarMetadatoUsuario(string json);
	static MetadatosSesion deserializarMetadatoSesion(string json);
	static std::map<string, string> deserializarMapa(string json);

	static string estructurasMerge(std::map<string, string> estructuraPermisos, string jsonEstructuraFileSystem);
};

#endif /* INCLUDE_PARSERJSON_H_ */
