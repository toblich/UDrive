#include "profile.h"

Profile::Profile (ManejadorDeUsuarios* manejadorUsuarios, ManejadorArchivosYMetadatos* manejarAyM) {
	this->manejadorUs = manejadorUsuarios;
	this->manejadorAyM = manejarAyM;
}

Profile::~Profile () {
}

mg_result Profile::GETHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	Logger::logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string user = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + user);

	if (uris.size() != 2 and uris.size() !=3){
		string mensaje = "El recurso al que se quiso acceder no existe.";
		this->responderBadRequest(connection, mensaje);
		return MG_TRUE;
	}
	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");

		//Si uris[1] == ^fotos se quiere descargar una foto de perfil
		if (uris[1] == FOTOS){
			string filePath = getFilepathFrom(uris);
			string filePathCompleto = manejadorAyM->descargarArchivo(user, filePath);
			if (filePathCompleto != "") {
				if (sendFile(connection, filePathCompleto)) {
					string mensaje = "Se descargó el archivo: " + filePathCompleto + " correctamente.";
					this->responderSucces(connection, mensaje);
				} else {
					string mensaje = "ERROR, no se pudo descargar el archivo: " + filePathCompleto;
					this->responderInternalServerError(connection, mensaje);
				}
			} else {
				string mensaje = "Path inválido, no se encontró el archivo: " + filePathCompleto;
				this->responderResourceNotFound(connection, mensaje);
			}
		} else { //Si no se quiere descargar el perfil de un usuario
			string userPerfil = uris[1];
			string perfil = manejadorUs->getPerfil(userPerfil);
			if (perfil != "") {
				Logger::logInfo("Se envió el perfil correctamente.");
				mg_send_status(connection, CODESTATUS_SUCCESS);
				mg_send_header(connection, contentType.c_str(), jsonType.c_str());
				printfData(connection, "{\"perfil\": %s}", perfil.c_str());
			} else {
				string mensaje = "El perfil al que se quizo acceder no existe, o bien el usuario no existe.";
				this->responderResourceNotFound(connection, mensaje);
			}
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Profile::PUTHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	Logger::logInfo("Se parseó la uri correctamente.");

	if (uris.size() != 2){
		string mensaje = "El recurso al que se quiso acceder no existe.";
		this->responderBadRequest(connection, mensaje);
		return MG_TRUE;
	}

	string user = uris[1];
	string varFile = "picture";
	DatosArchivo datosArch = getMultipartData(connection, varFile);
	string token = datosArch.token;
	Logger::logInfo("Se obtuvo la variable token con valor: " + token);
	string nombre = datosArch.nombre;
	Logger::logInfo("Se obtuvo la variable nombre con valor: " + nombre);
	string email = datosArch.email;
	Logger::logInfo("Se obtuvo la variable email con valor: " + email);

	if (manejadorUs->autenticarToken(token, user)) {
		Logger::logInfo("Se autenticó la sesión correctamente.");

		//Deserializo el perfil viejo.
		MetadatoUsuario viejoPerfil = ParserJson::deserializarMetadatoUsuario(manejadorUs->getPerfil(user));

		//Actualizo el perfil viejo con el nuevo email y nombre en caso de que se hayan modificado.
		if (nombre != "") viejoPerfil.nombre = nombre;
		if (email != "") viejoPerfil.email = email;

		//Verifico si ambos son validos antes de seguir, sino hay problema en que se actualiza la foto
		//y tal vez el perfil no es valido.
		if (not manejadorUs->esPerfilValido(ParserJson::serializarMetadatoUsuario(viejoPerfil))){
			string mensaje = "No se pudo modificar el perfil debido a que es inválido.";
			this->responderBadRequest(connection, mensaje);
			return MG_TRUE;
		}

		if (datosArch.dataLength != 0){ //Si se subio una nueva foto de perfil.
			//Obtengo el perfil del usuario y a partir de ahi el path foto de perfil viejo.
			string pathFotoViejo = viejoPerfil.pathFotoPerfil;

			//Parseo el nombre y extension del archivo para ver si cambio la extension.
			vector<string> nombreYExtension = ParserURI::parsear(datosArch.fileName, '.');
			Logger::logInfo("Se parseó el nombre del archivo correctamente.");

			//Obtengo el nuevo path para la foto de perfil -> ^fotos/user.extension
			string pathFotoNuevo = FOTOS + "/" + user + "." + nombreYExtension[nombreYExtension.size() - 1];

			//Si se cambio el path de la foto, la actualizo en el perfil del usuario. Sino queda como antes.
			if (manejadorAyM->actualizarFotoPerfil(pathFotoViejo, pathFotoNuevo, datosArch.fileData, datosArch.dataLength)) {
				Logger::logInfo("Se actualizo la foto de perfil y cambio el path.");
				viejoPerfil.pathFotoPerfil = pathFotoNuevo;
			}
		}

		//Se vuelve a serializar el perfil pero con los datos actualizados.
		string nuevoPerfil = ParserJson::serializarMetadatoUsuario(viejoPerfil);

		if (manejadorUs->modifyPerfil(user, nuevoPerfil)) {
			string mensaje = "Se modificó el perfil del usuario exitosamente.";
			this->responderSucces(connection, mensaje);
		} else {
			string mensaje = "No se pudo modificar el perfil debido a que es inválido.";
			this->responderBadRequest(connection, mensaje);
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Profile::POSTHandler (mg_connection* connection) {
	string username = getVar(connection, "user");
	Logger::logInfo("Se obtuvo la variable user con valor: " + username);
	string password = getVar(connection, "pass");
	Logger::logInfo("Se obtuvo la variable pass con valor: " + password);
	string profile = getVar(connection, "profile");
	Logger::logInfo("Se obtuvo la variable profile.");
	string premium = getVar(connection, "premium");
	Logger::logInfo("Se obtuvo la variable premium con valor: " + premium);

	//Le agrego al perfil del usuario la foto por defecto
	MetadatoUsuario perfil = ParserJson::deserializarMetadatoUsuario(profile);
	perfil.pathFotoPerfil = FOTOS + "/" + username + ".jpg";

	//Le agrego al perfil su cuota dependiendo si es premium o no
	if (premium == "true"){
		perfil.cuota = 5 * 1024; // 5 GB
	} else {
		perfil.cuota = 2 * 1024; // 2 GB
	}

	string perfilActualizado = ParserJson::serializarMetadatoUsuario(perfil);

	if (manejadorUs->registrarUsuario(username, password, perfilActualizado) and manejadorAyM->crearUsuario(username)) {
		string mensaje = "Se registro la cuenta con usuario: " + username + " y pass: " + password + " exitosamente.";
		this->responderResourceCreated(connection, mensaje);
	} else {
		string mensaje = "No se pudo registrar, el username/password/perfil es invalido.";
		this->responderBadRequest(connection, mensaje);
	}
	return MG_TRUE;
}
