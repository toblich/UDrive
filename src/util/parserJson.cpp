#include "parserJson.h"
#include "logger.h"

using std::string;
using namespace Json;

//ParserJson::ParserJson() {
//
//}
//
//ParserJson::~ParserJson() {
//
//}

string ParserJson::verificarString(string key, Value raiz, string porDefecto) {
	Value parametro = raiz.get(key, "NO");
	string value;
	if (not parametro.isNull() and (parametro.isString() and parametro.asString() != "NO")){
		value = parametro.asString();
	} else {
		string warning = "Parametro de tipo string  \"" + key + "\" invalido o inexistente en un Json. Se asigna el valor \"" + porDefecto + "\" para esta key.";
		Logger::logWarn(warning);
		value = porDefecto;
	}
	return value;
}

double ParserJson::verificarDouble(string key, Value raiz){
	Value parametro = raiz.get(key, "NO");
	double value;
	if (not parametro.isNull() and parametro.isDouble()){
		value = parametro.asDouble();
	} else {
		string warning = "Parametro de tipo double \"" + key + "\" invalido o inexistente en un Json. Se asigna el valor \"0\" (cero) para esta key.";
		Logger::logWarn(warning);
		value = 0;
	}
	return value;
}

int ParserJson::verificarInt(std::string key, Json::Value raiz, int porDefecto) {
	Value parametro = raiz.get(key, "NO");
	int value;
	if (not parametro.isNull() and parametro.isInt() and parametro.asInt() > 0){
		value = parametro.asInt();
	} else {
		string error = "Parametro de tipo int \"" + key + "\" invalido, inexistente o menor que 0 en un Json. Se asigna el valor " + to_string(porDefecto) + " para esta key.";
		Logger::logWarn(error);
		value = porDefecto;
	}
	return value;
}

string ParserJson::serializarMetadatoArchivo(MetadatoArchivo metadato) {
	Value archivo;
	archivo["nombre"] = metadato.nombre;
	archivo["extension"] = metadato.extension;
	archivo["fecha ultima modificacion"] = metadato.fechaUltimaModificacion;
	archivo["usuario ultima modificacion"] = metadato.usuarioUltimaModificacion;
	archivo["propietario"] = metadato.propietario;
	archivo["ultima version"] = metadato.ultimaVersion;

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
	return serializado;
}

string ParserJson::serializarMetadatoUsuario(MetadatoUsuario metadato) {
	Value archivo;
	archivo["nombre"] = metadato.nombre;
	archivo["email"] = metadato.email;
	archivo["path foto de perfil"] = metadato.pathFotoPerfil;
	archivo["cuota"] = metadato.cuota;

	Value ubicacion;
	ubicacion["latitud"] = metadato.ultimaUbicacion.latitud;
	ubicacion["longitud"] = metadato.ultimaUbicacion.longitud;
	archivo["ultima ubicacion"] = ubicacion;

	return archivo.toStyledString();
}

string ParserJson::serializarMetadatoSesion(MetadatoSesion metadato){
	Value archivo;
	archivo["username"] = metadato.username;
	archivo["password"] = metadato.password;
	archivo["token"] = metadato.token;

	string serializado = archivo.toStyledString();
	return serializado;
}

string ParserJson::serializarMapa(map<string, string>& mapa){
	Value json;

	map<string, string>::iterator it = mapa.begin();
	for( ; it != mapa.end(); it++) {
		json[it->first] = it->second;
	}

	if (mapa.empty()) {
		Logger::logWarn("Se quiso serializar un mapa vacio.");
		return "{}";
	}

	string serializado = json.toStyledString();
	return serializado;
}


MetadatoArchivo ParserJson::deserializarMetadatoArchivo(string json) {
	Value raiz;
	Features f = Features::strictMode();
	Reader reader(f);
	MetadatoArchivo metadatos;
	bool parseadoExitoso = reader.parse(json, raiz);
	if (parseadoExitoso){
		metadatos.nombre = ParserJson::verificarString("nombre", raiz);
		metadatos.extension = ParserJson::verificarString("extension", raiz);
		metadatos.fechaUltimaModificacion = ParserJson::verificarString("fecha ultima modificacion", raiz);
		metadatos.usuarioUltimaModificacion = ParserJson::verificarString("usuario ultima modificacion", raiz);
		metadatos.propietario = ParserJson::verificarString("propietario", raiz);
		metadatos.ultimaVersion = ParserJson::verificarInt("ultima version", raiz, 1);

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
			string error = "Parametro \"etiquetas\" de un metadato de archivo invalido o inexistente en un Json. No se le asignan etiquetas al mismo.";
			Logger::logWarn(error);
		}
		metadatos.etiquetas = etiquetas;

		std::list<string> usuarios;
		Value usu = raiz.get("usuarios", "NO");
		if (not usu.isNull() and usu.isArray()){
			unsigned int size = usu.size();
			for (unsigned int i=0; i < size; i++){
				// No veo que sean un tipo en especial porque podrian ser cualquier
				// cosa los usernames (mismo numeros).
				usuarios.push_back(usu[i].asString());
			}
		} else {
			string error = "Parametro \"usuarios habilitados\" de un metadato de archivo invalido o inexistente en un Json. No se le asignan usuarios habilitados al mismo.";
			Logger::logWarn(error);
		}
		metadatos.usuariosHabilitados = usuarios;
	} else {
		string error = "Fallo el parseo de un Json metadato de archivo.";
		Logger::logWarn(error);
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
		metadatos.nombre = ParserJson::verificarString("nombre", raiz);
		metadatos.email = ParserJson::verificarString("email", raiz);
		metadatos.pathFotoPerfil = ParserJson::verificarString("path foto de perfil", raiz);
		metadatos.cuota = ParserJson::verificarInt("cuota", raiz, CUOTA_DEFAULT);

		Value ubicacion = raiz.get("ultima ubicacion", "NO");
		if (not ubicacion.isNull() and ubicacion.isObject()){
			metadatos.ultimaUbicacion.latitud = ParserJson::verificarDouble("latitud", ubicacion);
			metadatos.ultimaUbicacion.longitud = ParserJson::verificarDouble("longitud", ubicacion);
		} else {
			string error = "Parametro \"ultima ubicacion\" de un metadato de usuario invalido o inexistente en un Json. Se le asigna la ubicacion (0,0) al mismo.";
			Logger::logWarn(error);
			metadatos.ultimaUbicacion.latitud = 0;
			metadatos.ultimaUbicacion.longitud = 0;
		}
	} else {
		string error = "Fallo el parseo de un Json metadato de usuario.";
		Logger::logWarn(error);
	}
	return metadatos;
}

MetadatoSesion ParserJson::deserializarMetadatoSesion(std::string json){
	Value raiz;
	Features f = Features::strictMode();
	Reader reader(f);
	MetadatoSesion metadatos;
	bool parseadoExitoso = reader.parse(json, raiz);
	if (parseadoExitoso){
		metadatos.username = ParserJson::verificarString("username", raiz);
		metadatos.password = ParserJson::verificarString("password", raiz);
		metadatos.token = ParserJson::verificarString("token", raiz);
	} else {
		string error = "Fallo el parseo de un Json metadato de sesion.";
		Logger::logWarn(error);
	}
	return metadatos;
}

std::map<std::string, std::string> ParserJson::deserializarMapa(std::string json) {
	Value raiz;
	Features f = Features::strictMode();
	Reader reader(f);
	map<string, string> mapa;
	bool parseadoExitoso = reader.parse(json, raiz);
	if (parseadoExitoso){
		if( raiz.size() > 0 ) {
	        for( Json::ValueIterator itr = raiz.begin() ; itr != raiz.end() ; itr++ ) {
	        	string key = itr.key().asString();
	        	string value = (*itr).asString();
	        	mapa.insert(pair<string, string>(key, value));
	        }
	    }
	} else {
		string error = "Fallo el parseo de un Json metadato de mapa.";
		Logger::logWarn(error);
	}
	return mapa;
}

Configuracion ParserJson::leerArchivoConfiguracion(const char* pathArchConf) {
	// Asumo los valores por defecto, sino se cambiaran cuando se parsee correctamente
	Configuracion config;
	config.nivelLog = NIVEL_LOG_DEFAULT;
	config.tamMaxLog = TAM_MAX_LOG_DEFAULT * 1024;
	config.pathFS = DEFAULT_FS;
	config.pathDB = DEFAULT_DB;
	config.cantThreads = CANT_THREADS_DEFAULT;
	config.tiempoPollServer = POLL_SERVER_DEFAULT;

	//Seteo por ahora para que el logger pueda loguear
	Logger::setNivelLogger(config.nivelLog);
	Logger::setTamanoMaximo(config.tamMaxLog);
	Logger::escribirHoraEnElLog();

	ifstream entrada(pathArchConf, ios::binary );
	if ( not entrada.good() ) {
		Logger::logWarn("El archivo de configuracion no existe. Se asumen los valores por defecto.");
		return config;
	}

	Value raiz;
	Features f = Features::strictMode();
	Reader reader(f);
	bool parseadoExitoso = reader.parse(entrada, raiz);
	if (parseadoExitoso) {
		int nivelLog = verificarInt("Nivel Log", raiz, NIVEL_LOG_DEFAULT);
		config.nivelLog = (nivelLog <= 4) ? nivelLog : NIVEL_LOG_DEFAULT;
		config.tamMaxLog = verificarInt("Tamano max Log", raiz, TAM_MAX_LOG_DEFAULT) * 1024;
		config.cantThreads = verificarInt("Cantidad threads", raiz, CANT_THREADS_DEFAULT);
		config.tiempoPollServer = verificarInt("Tiempo Poll Server", raiz, POLL_SERVER_DEFAULT);

		string pathFS = verificarString("FileSystem", raiz, DEFAULT_FS);
		config.pathFS = (pathFS.find('/') == string::npos) ? pathFS : DEFAULT_FS;
		string pathDB = verificarString("Base de datos", raiz, DEFAULT_DB);
		config.pathDB = (pathDB.find('/') == string::npos) ? pathDB : DEFAULT_DB;
	} else {
		Logger::logWarn("Fallo el parseo del archivo de configuración. Se asumen todos los valores por defecto.");
	}
	return config;
}

string ParserJson::estructurasMerge(map<string, string> estructuraPermisos, string jsonEstructuraFileSystem) {
	map<string, string> estructuraFileSystem = ParserJson::deserializarMapa(jsonEstructuraFileSystem);
	estructuraFileSystem.insert(estructuraPermisos.begin(), estructuraPermisos.end());
	return ParserJson::serializarMapa(estructuraFileSystem);
}
