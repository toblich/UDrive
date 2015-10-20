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
	this->logInfo("Se parseó la uri correctamente.");
	string token = getVar(connection, "token");
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string user = uris[1];

	if (manejadorUs->autenticarToken(token, user)) {
		this->logInfo("Se autenticó la sesión correctamente.");

		string perfil = manejadorUs->getPerfil(user);
		if (perfil != "") {
			this->logInfo("Se envió el perfil correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"perfil\": %s}", perfil.c_str());
		} else {
			this->logInfo("El perfil al que se quizo acceder no existe, o bien el usuario no existe.");
			mg_send_status(connection, CODESTATUS_RESOURCE_NOT_FOUND);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"El usuario no existe, no se pudo obtener el perfil\"}");
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}

	return MG_TRUE;
}

mg_result Profile::PUTHandler (mg_connection* connection) {
	string uri = string(connection->uri);
	vector<string> uris = ParserURI::parsear(uri, '/');
	this->logInfo("Se parseó la uri correctamente.");
	string user = uris[1];

	string varFile = "picture";
	DatosArchivo datosArch = getMultipartData(connection, varFile);
	string token = datosArch.token;
	this->logInfo("Se obtuvo la variable token con valor: " + token);
	string nuevoPerfil = datosArch.perfil;
	this->logInfo("Se obtuvo la variable con el nuevo perfil.");

	if (manejadorUs->autenticarToken(token, user)) {
		this->logInfo("Se autenticó la sesión correctamente.");

		if (datosArch.dataLength != 0){ //Si se subio una nueva foto de perfil.
			//Obtengo el perfil del usuario y a partir de ahi el path foto de perfil viejo.
			string pathFotoViejo = ParserJson::deserializarMetadatoUsuario(manejadorUs->getPerfil(user)).pathFotoPerfil;

			vector<string> nombreYExtension = ParserURI::parsear(datosArch.fileName, '.');
			this->logInfo("Se parseó el nombre del archivo correctamente.");
			//Obtengo el nuevo path para la foto de perfil -> ^fotos/user.extension
			string pathFotoNuevo = FOTOS + "/" + user + "." + nombreYExtension[nombreYExtension.size() - 1];

			MetadatoUsuario perfilUsuario = ParserJson::deserializarMetadatoUsuario(nuevoPerfil);
			 //Si se cambio el path de la foto, la actualizo en el perfil del usuario.
			if (manejadorAyM->actualizarFotoPerfil(pathFotoViejo, pathFotoNuevo, datosArch.fileData, datosArch.dataLength)) {
				this->logInfo("Se actualizo la foto de perfil y cambio el path.");
				perfilUsuario.pathFotoPerfil = pathFotoNuevo;
			} else { //Sino queda como antes.
				this->logInfo("Se actualizo la foto de perfil pero no cambio el path.");
				perfilUsuario.pathFotoPerfil = pathFotoViejo;
			}
			string nuevoPerfil = ParserJson::serializarMetadatoUsuario(perfilUsuario);
		}

		if (manejadorUs->modifyPerfil(user, nuevoPerfil)) {
			this->logInfo("Se modificó el perfil del usuario correctamente.");
			mg_send_status(connection, CODESTATUS_SUCCES);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"success\": \"Se modifico el perfil exitosamente.\"}");
		} else {
			this->logInfo("No se pudo modificar el perfil debido a que es inválido.");
			mg_send_status(connection, CODESTATUS_BAD_REQUEST);
			mg_send_header(connection, contentType.c_str(), jsonType.c_str());
			printfData(connection, "{\"error\": \"No se pudo modificar el perfil debido a que es inválido.\"}");
		}
	} else {
		this->responderAutenticacionFallida(connection);
	}
	return MG_TRUE;
}

mg_result Profile::POSTHandler (mg_connection* connection) {
	string username = getVar(connection, "user");
	this->logInfo("Se obtuvo la variable user con valor: " + username);
	string password = getVar(connection, "pass");
	this->logInfo("Se obtuvo la variable pass con valor: " + password);
	string profile = getVar(connection, "profile");
	this->logInfo("Se obtuvo la variable profile.");
	string premium = getVar(connection, "premium");
	this->logInfo("Se obtuvo la variable premium con valor: " + premium);

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
		this->logInfo("Se registro la cuenta con usuario: " + username + " y pass: " + password + " exitosamente.");
		mg_send_status(connection, CODESTATUS_RESOURCE_CREATED);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"succes\": \"Cuenta creada correctamente\"}");
	} else {
		this->logInfo("No se pudo registrar la cuenta debido a algun error en las variables.");
		mg_send_status(connection, CODESTATUS_BAD_REQUEST);
		mg_send_header(connection, contentType.c_str(), jsonType.c_str());
		printfData(connection, "{\"error\": \"Error en la registracion\"}");
	}
	return MG_TRUE;
}
