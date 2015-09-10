#include "parserJson.h"

using std::string;
using namespace Json;

ParserJson::ParserJson() {

}

ParserJson::~ParserJson() {

}

string ParserJson::verificarString(string key, Value raiz) {
	Value parametro = raiz.get(key, "NO");
	string value;
	if (not parametro.isNull() and (parametro.isString() and parametro.asString() != "NO")){
		value = parametro.asString();
	} else {
		//TODO: Loguear como warning de valor vacio/invalido
		std::cout << "Parametro " << key << " invalido." << std::endl;
		value = "";
	}
	return value;
}

double ParserJson::verificarDouble(string key, Value raiz){
	Value parametro = raiz.get(key, "NO");
	double value;
	if (not parametro.isNull() and parametro.isDouble()){
		value = parametro.asDouble();
	} else {
		//TODO: Loguear como warning de valor vacio
		std::cout << "Parametro " << key << " invalido." << std::endl;
		value = 0;
	}
	return value;
}

string ParserJson::serializarMetadatoArchivo(MetadatoArchivo metadato) {
	Value archivo;

	//TODO: Habría que verificar que no esten vacios, o se podria asumir
	//		que como los escribimos nosotros van a estar bien??
	archivo["nombre"] = metadato.nombre;
	archivo["extension"] = metadato.extension;
	archivo["fecha ultima modificacion"] = metadato.fechaUltimaModificacion;
	archivo["usuario ultima modificacion"] = metadato.usuarioUltimaModificacion;
	archivo["propietario"] = metadato.propietario;

	Value etiquetas(arrayValue);
	std::list<string> etiq = metadato.etiquetas;
	std::list<string>::iterator itEtiq = etiq.begin();
	for ( ; itEtiq != etiq.end() ; itEtiq++){
		etiquetas.append(Value(*itEtiq));
	}
	archivo["etiquetas"] = etiquetas;

	Value usuarios(arrayValue);
	std::list<string> usu = metadato.usuariosHabilitados;
	std::list<string>::iterator itUsu = usu.begin();
	for ( ; itUsu != usu.end() ; itUsu++){
		usuarios.append(Value(*itUsu));
	}
	archivo["usuarios"] = usuarios;

	string serializado = archivo.toStyledString();
	std::cout << serializado << std::endl;
	return serializado;
}

string ParserJson::serializarMetadatoUsuario(MetadatoUsuario metadato) {
	Value archivo;

	//TODO: Habría que verificar que no esten vacios, o se podria asumir
	//		que como los escribimos nosotros van a estar bien??
	archivo["nombre"] = metadato.nombre;
	archivo["email"] = metadato.email;
	archivo["path foto de perfil"] = metadato.pathFotoPerfil;

	Value ubicacion;
	ubicacion["latitud"] = metadato.ultimaUbicacion.latitud;
	ubicacion["longitud"] = metadato.ultimaUbicacion.longitud;
	archivo["ultima ubicacion"] = ubicacion;

	string serializado = archivo.toStyledString();
	std::cout << serializado << std::endl;
	return serializado;
}

MetadatoArchivo ParserJson::deserializarMetadatoArchivo(string json) {
	Value raiz;
	Features f = Features::strictMode();
	Reader reader(f);
	MetadatoArchivo metadatos;
	bool parseadoExitoso = reader.parse(json, raiz);
	if (parseadoExitoso){
		metadatos.nombre = this->verificarString("nombre", raiz);
		metadatos.extension = this->verificarString("extension", raiz);
		metadatos.fechaUltimaModificacion = this->verificarString("fecha ultima modificacion", raiz);
		metadatos.usuarioUltimaModificacion = this->verificarString("usuario ultima modificacion", raiz);
		metadatos.propietario = this->verificarString("propietario", raiz);

		std::list<string> etiquetas;
		Value etiq = raiz.get("etiquetas", "NO");
		if (not etiq.isNull() and etiq.isArray()){
			unsigned int size = etiq.size();
			for (unsigned int i=0; i < size; i++){
				// No veo que sean un tipo en especial porque podrian ser cualquier
				// cosa las etiquetas (mismo numeros).
				etiquetas.push_back(etiq[i].asString());
			}
		} else {
			//TODO: Loguear como warning de invalido
			std::cout << "Parametro etiquetas invalido." << std::endl;
		}
		metadatos.etiquetas = etiquetas;

		std::list<string> usuarios;
		Value usu = raiz.get("usuarios", "NO");
		if (not usu.isNull() and usu.isArray()){
			unsigned int size = usu.size();
			for (unsigned int i=0; i < size; i++){
				// No veo que sean un tipo en especial porque podrian ser cualquier
				// cosa las etiquetas (mismo numeros).
				usuarios.push_back(usu[i].asString());
			}
		} else {
			//TODO: Loguear como warning de invalido
			std::cout << "Parametro usuarios invalido." << std::endl;
		}
		metadatos.usuariosHabilitados = usuarios;
	} else {
		//TODO: Loguear como warning de invalido
		std::cout << "Fallo el parseo del json" << std::endl;
	}
	return metadatos;
}

MetadatoUsuario ParserJson::deserializarMetadatoUsuario(string json) {
	Value raiz;
	Features f = Features::strictMode();
	Reader reader(f);
	MetadatoUsuario metadatos;
	bool parseadoExitoso = reader.parse(json, raiz);
	if (parseadoExitoso){
		metadatos.nombre = this->verificarString("nombre", raiz);
		metadatos.email = this->verificarString("email", raiz);
		metadatos.pathFotoPerfil = this->verificarString("path foto de perfil", raiz);

		Value ubicacion = raiz.get("ultima ubicacion", "NO");
		if (not ubicacion.isNull() and ubicacion.isObject()){
			metadatos.ultimaUbicacion.latitud = this->verificarDouble("latitud", ubicacion);
			metadatos.ultimaUbicacion.longitud = this->verificarDouble("longitud", ubicacion);
		} else {
			//TODO: Loguear como warning de invalido
		}
	}
	return metadatos;
}
