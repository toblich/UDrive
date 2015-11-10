#include "buscador.h"

Buscador::Buscador(BD* dbMetadatos, string pathFileSystem) : validador(dbMetadatos), pathFileSystem(pathFileSystem) {
	this->dbMetadatos = dbMetadatos;
}

Buscador::~Buscador() {

}

string Buscador::obtenerEstructuraCarpeta (string path, bool esRecursivo, function<bool (MetadatoArchivo&)> predicate) {
	if ( path.find(RESERVED_STR + "permisos") != string::npos )
			return this->obtenerEstructuraCompartidos(path);
	string pathCompleto = this->pathFileSystem + "/" + path;
	DIR* dir;
	if ((dir = opendir(pathCompleto.c_str())) == NULL) {
		Logger::logWarn("No existe el directorio " + path);
		return "";
	}

	map<string, string> mapa;
	struct dirent* ent;
	while ((ent = readdir(dir)) != NULL) {
		string dirName(ent->d_name);
		if (dirName == "." or dirName == "..")
			continue;

		string pathInterno = path + "/" + ent->d_name;
		string pathInternoConFS = this->pathFileSystem + "/" + pathInterno;
		if (validador.existeCarpeta(pathInternoConFS)) {
			if (esRecursivo) {
				string jsonEstructura = obtenerEstructuraCarpeta(pathInterno, true, predicate);
				mapa = ParserJson::deserializarMapa( ParserJson::estructurasMerge( mapa, jsonEstructura ) );
			} else {
				vector<string> directorios = ParserURI::parsear(pathInterno, '/');
				int size = directorios.size();
				string foldername = directorios[size - 1];
				mapa.insert(pair<string, string>(pathInterno, foldername + "." + FOLDER));
			}
		} else { //Es un archivo
			pathInterno = ParserURI::pathSinNroSecuencia(pathInterno);
			if (not dbMetadatos->contains(pathInterno)) {
				Logger::logWarn("Se quiso obtener los metadatos del archivo " + path + " pero este no existe.");
				return "";
			}
			string jsonMetadatos = this->dbMetadatos->get(pathInterno);
			MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadatos);
			string nombre = metadato.nombre;
			if (metadato.extension != "none")
				nombre += "." + metadato.extension;
			nombre += RESERVED_STR + to_string(metadato.ultimaVersion);

			if ( predicate(metadato) ) {
				mapa.insert(pair<string, string>(pathInterno, nombre));
			}
		}
	}
	closedir(dir);
	return ParserJson::serializarMapa(mapa);
}

string Buscador::obtenerEstructuraCompartidos (string path) {
	if ( not dbMetadatos->contains(path) )
		return "";
	string compartidosConUsuario = dbMetadatos->get(path);
	Logger::logDebug("Compartidos con " + path + " " + compartidosConUsuario + " al obtener estructura de Compartidos");
	vector<string> archivosCompartidos = ParserURI::parsear(compartidosConUsuario, RESERVED_CHAR);
	map<string, string> mapa;

	for (auto &archivoCompartido : archivosCompartidos) {
		if (not dbMetadatos->contains(archivoCompartido)) {	// debug
			Logger::logError("En la base de datos no esta el archivo compartido " + archivoCompartido);
		}
		string jsonMetadatoArchivoCompartido = dbMetadatos->get(archivoCompartido);
		MetadatoArchivo metadatoArchivoCompartido = ParserJson::deserializarMetadatoArchivo(jsonMetadatoArchivoCompartido);
		string nombre = metadatoArchivoCompartido.nombre;
		if (metadatoArchivoCompartido.extension != "none")
			nombre += "." + metadatoArchivoCompartido.extension;
		nombre += RESERVED_STR + to_string(metadatoArchivoCompartido.ultimaVersion);
		mapa.insert(pair<string, string>(archivoCompartido, nombre));
	}
	return ParserJson::serializarMapa(mapa);
}

map<string, string> Buscador::buscar (string username, function<bool (MetadatoArchivo&)> predicate) {
	string permisos = PERMISOS + "/" + username;
	string archivosPermitidos = dbMetadatos->get(permisos);
	vector<string> archivos = ParserURI::parsear(archivosPermitidos, RESERVED_CHAR);
	map<string, string> estructura;

	for (auto & archivoCompartido : archivos) {
		string jsonMetadato = dbMetadatos->get(archivoCompartido);
		MetadatoArchivo metadatoArchivoCompartido = ParserJson::deserializarMetadatoArchivo(jsonMetadato);
		if ( predicate(metadatoArchivoCompartido) ) {
			string nombre = metadatoArchivoCompartido.nombre;
			if (metadatoArchivoCompartido.extension != "none")
				nombre += "." + metadatoArchivoCompartido.extension;
			nombre += RESERVED_STR + to_string(metadatoArchivoCompartido.ultimaVersion);
			estructura.insert(pair<string, string>(archivoCompartido, nombre));
		}
	}
	return estructura;
}

string Buscador::buscarPorExtension(string username, string extension) {
	auto predicate = [&] (MetadatoArchivo& metadato) -> bool {return metadato.extension == extension;};
	map<string, string> estructuraPermisos = buscar(username, predicate);
	string jsonEstructuraFileSystem = obtenerEstructuraCarpeta(username, true, predicate);
	return ParserJson::estructurasMerge(estructuraPermisos, jsonEstructuraFileSystem);
}

string Buscador::buscarPorEtiqueta(string username, string etiqueta) {
	auto predicate = [&] (MetadatoArchivo& metadato) -> bool {
		list<string>& etiquetas = metadato.etiquetas;
		return find(etiquetas.begin(), etiquetas.end(), etiqueta) != etiquetas.end();
	};
	map<string, string> estructuraPermisos = buscar(username, predicate);
	string jsonEstructuraFileSystem = obtenerEstructuraCarpeta(username, true, predicate);
	return ParserJson::estructurasMerge(estructuraPermisos, jsonEstructuraFileSystem);
}

string Buscador::buscarPorNombre(string username, string nombre) {
	auto predicate = [&] (MetadatoArchivo& metadato) -> bool {return (metadato.nombre.find(nombre) != string::npos);};
	map<string, string> estructuraPermisos = buscar(username, predicate);
	string jsonEstructuraFileSystem = obtenerEstructuraCarpeta(username, true, predicate);
	return ParserJson::estructurasMerge(estructuraPermisos, jsonEstructuraFileSystem);
}

string Buscador::buscarPorPropietario(string username, string propietario) {
	auto predicate = [&] (MetadatoArchivo& metadato) -> bool {return metadato.propietario == propietario;};
	if ( username != propietario ) {
		map<string, string> estructuraPermisos = buscar(username, predicate);
		return ParserJson::serializarMapa(estructuraPermisos);
	}
	return obtenerEstructuraCarpeta(username, true, predicate);
}

