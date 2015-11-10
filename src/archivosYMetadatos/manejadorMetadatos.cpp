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
	for (auto &usuario : metadatoConFechaModif.usuariosHabilitados) {
		if (usuario == metadatoConFechaModif.propietario)
			continue;

		string permisosUsuario = PERMISOS + "/" + usuario;
		if (validador->existeMetadato(permisosUsuario)) {
			string archivosStr = dbMetadatos->get(permisosUsuario);
			vector<string> archivos = ParserURI::parsear(archivosStr, RESERVED_CHAR);
			archivos.erase(remove(archivos.begin(), archivos.end(), filepath), archivos.end());
			string joined = ParserURI::join(archivos, RESERVED_CHAR);
			batch.modify(permisosUsuario, joined);
		} else {
			Logger::logWarn("No existe en dbMetadatos " + permisosUsuario + " al armar batch de eliminar archivo");
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

void ManejadorMetadatos::actualizarPermisosMetadato (const MetadatoArchivo& metadatosViejos,
		MetadatoArchivo metadatosNuevos, const string& username, const string& filepath) {
	list<string> usuariosViejos = metadatosViejos.usuariosHabilitados;
	list<string> usuariosNuevos = metadatosNuevos.usuariosHabilitados;

	for (auto &nuevoUsuario: usuariosNuevos) {
		// Si todavia no tenia permisos
		if (find(usuariosViejos.begin(), usuariosViejos.end(), nuevoUsuario) == usuariosViejos.end()) {
			this->agregarPermiso(username, filepath, nuevoUsuario);
		}
	}
	// Si tenia permisos y ahora no los tiene, eliminarPermiso
	for (auto &viejoUsuario : usuariosViejos) {
		// Si tenia permisos y ahora ya no
		if (find(usuariosNuevos.begin(), usuariosNuevos.end(), viejoUsuario) == usuariosNuevos.end()) {
			this->eliminarPermiso(username, filepath, viejoUsuario);
		}
	}
}

void ManejadorMetadatos::actualizarPermisosPathArchivo (const string& filepath, const string& nuevoFilepath,
		const list<string>& usuariosHabilitados) {

	for (auto &usuario : usuariosHabilitados) {
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

bool ManejadorMetadatos::actualizarPermisos (const string& filepath, const string& jsonNuevosMetadatos,
		const string& username, string& nuevoJson) {
	string jsonMetadatosViejos = dbMetadatos->get(filepath);
	MetadatoArchivo metadatosViejos = ParserJson::deserializarMetadatoArchivo(jsonMetadatosViejos);
	MetadatoArchivo metadatosNuevos = ParserJson::deserializarMetadatoArchivo(jsonNuevosMetadatos);

	if (metadatosNuevos.usuariosHabilitados.empty()) {
		metadatosNuevos.usuariosHabilitados = metadatosViejos.usuariosHabilitados;
		nuevoJson = ParserJson::serializarMetadatoArchivo(metadatosNuevos);
	} else {
		actualizarPermisosMetadato(metadatosViejos, metadatosNuevos, username, filepath);	// actualiza quienes tienen permiso
		nuevoJson = jsonNuevosMetadatos;
	}

	bool deboRenombrar = metadatosViejos.nombre != metadatosNuevos.nombre or metadatosViejos.extension != metadatosNuevos.extension;
	if (not deboRenombrar) {
		dbMetadatos->modify(filepath, nuevoJson);
	}
	return deboRenombrar;
}

void ManejadorMetadatos::actualizarPermisosPorRenombrado (const string& filepath, const string& nuevoFilename,
		const MetadatoArchivo& metadatosNuevos, const string& nuevoJson) {
	MetadatoArchivo metadatosViejos = ParserJson::deserializarMetadatoArchivo(dbMetadatos->get(filepath));
	string nuevoFilepath = ParserURI::pathConNuevoFilename(filepath, nuevoFilename);
	list<string> usuariosHabilitados = (metadatosNuevos.usuariosHabilitados.empty()) ?
			metadatosViejos.usuariosHabilitados : metadatosNuevos.usuariosHabilitados;
	actualizarPermisosPathArchivo(filepath, nuevoFilepath, usuariosHabilitados);	// actualiza el path del archivo en quienes tienen permiso
	Batch batch;
	batch.erase(filepath);
	batch.put(nuevoFilepath, nuevoJson);
	dbMetadatos->writeBatch(batch);
}

string ManejadorMetadatos::getJsonMetadatos (const string& filepath) {
	if (not validador->existeMetadato(filepath)) {
		Logger::logWarn("Se quiso consultar los metadatos del archivo " + filepath + " pero este no existe.");
		return "";
	}
	return dbMetadatos->get(filepath);
}

void ManejadorMetadatos::eliminarDefinitivamente (const string& filepath) {
	// Las validaciones de existencia se hacen antes de llamarlo
	dbMetadatos->erase(filepath);
	Logger::logInfo("Se borraron definitivamente los metadatos del archivo " + filepath);
}

bool ManejadorMetadatos::cargarMetadato (const string& filepath, const string& jsonMetadatos) {
	// Las validaciones de existencia se hacen antes de llamarlo
	return dbMetadatos->put(filepath, jsonMetadatos);
}

void ManejadorMetadatos::actualizarMetadatosPorActualizacionArchivo (const string& filepath, const string& username, int nuevaVersion) {
	string metadatos = dbMetadatos->get(filepath);
	string jsonNuevosMetadatos = actualizarUsuarioFechaModificacion(metadatos, username);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonNuevosMetadatos);
	metadato.ultimaVersion = nuevaVersion;
	jsonNuevosMetadatos = ParserJson::serializarMetadatoArchivo(metadato);
	dbMetadatos->modify(filepath, jsonNuevosMetadatos);
}

bool ManejadorMetadatos::mandarATrash(const string& jsonMetadatos, const string& username,
		const string& filepath, const string& pathCompletoPapelera) {
	Batch batch = armarBatchEliminarArchivo(jsonMetadatos, username, filepath, pathCompletoPapelera);

	if (this->dbMetadatos->writeBatch(batch))
		return true;

	Logger::logWarn("No se ha podido escribir el batch de eliminacion del archivo " + filepath + ".");
	return false;
}

string ManejadorMetadatos::obtenerMetadato(const string& filepath) {
	// Las validaciones de existencia se hacen antes de llamarlo
	return dbMetadatos->get(filepath);
}
