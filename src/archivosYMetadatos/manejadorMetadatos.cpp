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

bool ManejadorMetadatos::agregarPermisosABD (string username) {
	string key = PERMISOS + "/" + username;
	return this->dbMetadatos->put(key, ""); // PUT devuelve false si ya existia la clave, true si la agrega
}

bool ManejadorMetadatos::agregarPermiso (string usernameOrigen, string filepath, string usernameDestino) {
	if (not validador->verificarPermisos(usernameOrigen, filepath))
		return false;
	if (not dbMetadatos->contains(filepath)) {
		Logger::logWarn("Se quiso agregar un permiso al archivo " + filepath + " pero este no existe.");
		return false;
	}
	string pathPermisos = PERMISOS + "/" + usernameDestino;
	if (not dbMetadatos->contains(pathPermisos)) {
		Logger::logWarn("Se quiso agregar un permiso al usuario " + usernameDestino + " pero este no existe.");
		return false;
	}
	string jsonArchivo = dbMetadatos->get(filepath);

	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchivo);
	metadato.usuariosHabilitados.push_back(usernameDestino);
	string jsonModificado = ParserJson::serializarMetadatoArchivo(metadato);

	Batch batch;
	batch.modify(filepath, jsonModificado);
//	Como este metodo se llama directamente desde actualizarMetadatos, ya la fecha viene modificada

	string archivosPermitidos = dbMetadatos->get(pathPermisos);
	if (archivosPermitidos != "")
		archivosPermitidos += RESERVED_CHAR;
	archivosPermitidos += filepath;
	batch.modify(pathPermisos, archivosPermitidos);
	dbMetadatos->writeBatch(batch);

	return true;
}

bool ManejadorMetadatos::eliminarPermiso (string usernameOrigen, string filepath, string usernameDestino) {
	if (not validador->verificarPermisos(usernameOrigen, filepath))
		return false;
	if (not dbMetadatos->contains(filepath)) {
		Logger::logWarn("Se quiso eliminar un permiso del archivo " + filepath + " pero este no existe.");
		return false;
	}
	string pathPermisos = PERMISOS + "/" + usernameDestino;
	if (not dbMetadatos->contains(pathPermisos)) {
		Logger::logWarn("Se quiso eliminar un permiso del usuario " + usernameDestino + " pero este no existe.");
		return false;
	}

	string jsonModificado = metadatosConPermisosDepurados(filepath, usernameDestino);
	string archivosPermitidos = jsonArchivosPermitidos(pathPermisos, filepath);

	Batch batch;
	batch.modify(filepath, jsonModificado);
	batch.modify(pathPermisos, archivosPermitidos);
	dbMetadatos->writeBatch(batch);

	return true;
}

Batch ManejadorMetadatos::armarBatchEliminarArchivo (const string& jsonMetadatos, const string& username,
		const string& filepath, const string& pathCompletoPapelera) {
	Batch batch;
	string jsonMetadatosConFechaModif = this->actualizarUsuarioFechaModificacion(jsonMetadatos, username);
	MetadatoArchivo metadatoConFechaModif = ParserJson::deserializarMetadatoArchivo(jsonMetadatosConFechaModif);
	list<string> usuariosHabilitados = metadatoConFechaModif.usuariosHabilitados;
	list<string>::iterator itUsu = usuariosHabilitados.begin();

	for (; itUsu != usuariosHabilitados.end(); itUsu++) {
		string usuario = (*itUsu);
		if (usuario == metadatoConFechaModif.propietario)
			continue;

		string permisosUsuario = PERMISOS + "/" + usuario;
		if (dbMetadatos->contains(permisosUsuario)) {
			string archivosStr = dbMetadatos->get(permisosUsuario);
			vector<string> archivos = ParserURI::parsear(archivosStr, RESERVED_CHAR);
			archivos.erase(remove(archivos.begin(), archivos.end(), filepath), archivos.end());
			string joined = ParserURI::join(archivos, RESERVED_CHAR);
			batch.modify(permisosUsuario, joined);
		}
	}

	batch.erase(filepath);
	batch.put(pathCompletoPapelera, jsonMetadatosConFechaModif);
	return batch;
}

bool ManejadorMetadatos::restaurarMetadatos (const string& pathEnPapeleraSinFS, const string& username,
		const string& pathRealSinFS) {
	string metadatosJson = dbMetadatos->get(pathEnPapeleraSinFS);
	string nuevosMetadatosJson = this->actualizarUsuarioFechaModificacion(metadatosJson, username);
	Batch batch;
	batch.erase(pathEnPapeleraSinFS);
	batch.put(pathRealSinFS, nuevosMetadatosJson);
	return dbMetadatos->writeBatch(batch);
}

string ManejadorMetadatos::metadatosConPermisosDepurados (const string& filepath, const string& usernameDestino) {
	string jsonArchivo = dbMetadatos->get(filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchivo);
	list<string> usuariosHabilitados = metadato.usuariosHabilitados;
	if (find(usuariosHabilitados.begin(), usuariosHabilitados.end(), usernameDestino) != usuariosHabilitados.end())
		usuariosHabilitados.remove(usernameDestino);

	return ParserJson::serializarMetadatoArchivo(metadato);
}

// Como este metodo se llama directamente desde actualizarMetadatos, ya la fecha viene modificada
string ManejadorMetadatos::jsonArchivosPermitidos (const string& pathPermisos, const string& filepath) {
	vector<string> vecArchivosPermitidos = ParserURI::parsear(dbMetadatos->get(pathPermisos), RESERVED_CHAR);
	vecArchivosPermitidos.erase(remove(vecArchivosPermitidos.begin(), vecArchivosPermitidos.end(), filepath),
			vecArchivosPermitidos.end());
	return ParserURI::join(vecArchivosPermitidos, RESERVED_CHAR);
}

string ManejadorMetadatos::actualizarPermisosMetadato (const MetadatoArchivo& metadatosViejos,
		MetadatoArchivo metadatosNuevos, const string& username, const string& filepath,
		const string& jsonNuevosMetadatos) {
	list<string> usuariosViejos = metadatosViejos.usuariosHabilitados;
	list<string> usuariosNuevos = metadatosNuevos.usuariosHabilitados;

	if (usuariosNuevos.empty()) {
		metadatosNuevos.usuariosHabilitados = usuariosViejos;
		return ParserJson::serializarMetadatoArchivo(metadatosNuevos);
	}
	list<string>::iterator itUsuNuevos = usuariosNuevos.begin();
	for (; itUsuNuevos != usuariosNuevos.end(); itUsuNuevos++) {
		string nuevoUsuario = (*itUsuNuevos);
		// Si todavia no tenia permisos
		if (find(usuariosViejos.begin(), usuariosViejos.end(), nuevoUsuario) == usuariosViejos.end()) {
			this->agregarPermiso(username, filepath, nuevoUsuario);
		}
	}
	// Si tenia permisos y ahora no los tiene, eliminarPermiso
	list<string>::iterator itUsuViejos = usuariosViejos.begin();
	for (; itUsuViejos != usuariosViejos.end(); itUsuViejos++) {
		string viejoUsuario = (*itUsuViejos);
		// Si tenia permisos y ahora ya no
		if (find(usuariosNuevos.begin(), usuariosNuevos.end(), viejoUsuario) == usuariosNuevos.end()) {
			this->eliminarPermiso(username, filepath, viejoUsuario);
		}
	}
	return jsonNuevosMetadatos;
}

void ManejadorMetadatos::actualizarPermisosPathArchivo (const string& filepath, const string& nuevoFilepath,
		const list<string>& usuariosHabilitados) {
	list<string>::const_iterator it = usuariosHabilitados.cbegin();
	const list<string>::const_iterator END = usuariosHabilitados.cend();
	for (; it != END; it++) {
		string usuario = *it;
		string archivos = dbMetadatos->get(PERMISOS + "/" + usuario);
		Logger::logDebug("Archivos compartidos a " + usuario + ": " + archivos);
		vector<string> partes = ParserURI::parsear(archivos, RESERVED_CHAR);
		partes.erase(remove(partes.begin(), partes.end(), filepath), partes.end());
		partes.push_back(nuevoFilepath);
		string nuevosArchivos = ParserURI::join(partes, RESERVED_CHAR);
		Logger::logDebug("nuevosArchivos de " + usuario + ": " + nuevosArchivos);
		dbMetadatos->modify(PERMISOS + "/" + usuario, nuevosArchivos);
	}
}
