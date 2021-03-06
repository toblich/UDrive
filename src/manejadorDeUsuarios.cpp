#include "manejadorDeUsuarios.h"

using namespace std;

ManejadorDeUsuarios::ManejadorDeUsuarios (BD* perfiles, BD* sesiones, BD* passwords) {
	this->perfiles = perfiles;
	this->sesiones = sesiones;
	this->passwords = passwords;
}

ManejadorDeUsuarios::~ManejadorDeUsuarios () {
}

bool ManejadorDeUsuarios::validarLogin (string username, string password) {
	string realPassword;
	try {
		realPassword = passwords->get(username);
	} catch (KeyNotFound& e) {
		return false;
	}
	return (realPassword == password);
}

bool ManejadorDeUsuarios::registrarUsuario (string username, string password, string perfil) {
	if (not esUsernameValido(username)) return false;
	if (not esPasswordValida(password)) return false;
	if (not esPerfilValido(perfil))		return false;
	if (not passwords->put(username, password) or not perfiles->put(username, perfil))
		return false;

	try {
		string usuarios = perfiles->get(USERLIST);
		usuarios += (RESERVED_STR + username);
		perfiles->modify(USERLIST, usuarios);
	} catch (KeyNotFound& e) {
		perfiles->put(USERLIST, username);
	}
	return true;
}

string ManejadorDeUsuarios::iniciarSesion (string username) {
	time_t timestamp;
	time(&timestamp);
	string token = generarToken(username, timestamp);
	if (sesiones->put(token, username))
		return token;
	usleep(100000);	//Vuelve a probar 100 miliseg despues
	return iniciarSesion(username);
}

bool ManejadorDeUsuarios::autenticarToken (string token, string username) {
	try {
		return username == sesiones->get(token);
	} catch (KeyNotFound& e) {
		return false;
	}
}

bool ManejadorDeUsuarios::cerrarSesion (string token, string username) {
	if (autenticarToken(token, username)) {
		sesiones->erase(token);	//No lanza KeyNotFound por el autenticarToken
		return true;
	}
	return false;
}

string ManejadorDeUsuarios::generarToken (string username, time_t timestamp) {
	string timeAsString = to_string(timestamp);
	hash<string> funcionHash;
	return to_string(funcionHash(username + timeAsString));
}

bool ManejadorDeUsuarios::esPerfilValido (const string& perfilJson) {
	MetadatoUsuario perfil = ParserJson::deserializarMetadatoUsuario(perfilJson);
	return regex_match(perfil.nombre, REGEX_NOMBRE) and regex_match(perfil.email, REGEX_EMAIL);
}

bool ManejadorDeUsuarios::esUsernameValido (const string& username) {
	const size_t SIZE = username.size();
	for (size_t i = 0; i < SIZE; i++) {
		if (esCaracterInvalido(username[i]))
			return false;
	}
	return true;
}

bool ManejadorDeUsuarios::esCaracterInvalido (char c) {
	return (CHARS_INVALIDOS.find(c) != string::npos);
}

bool ManejadorDeUsuarios::esPasswordValida (const string& password) {
	return (password.size() >= 8);
}

string ManejadorDeUsuarios::getPerfil (string username) {
	if (perfiles->contains(username)) {
		return perfiles->get(username);
	} else {
		return string();
	}
}

bool ManejadorDeUsuarios::modifyPerfil (string username, string perfil) {
	if (esPerfilValido(perfil) and perfiles->contains(username)) {
		perfiles->modify(username, perfil);
		return true;
	}
	return false;
}

string ManejadorDeUsuarios::buscarUsuariosCon (const string& substring) {
	return "{\"usuarios\": \"" + usuariosConSubstring(substring) + "\"}";
}

string ManejadorDeUsuarios::usuariosConSubstring (const string& substring) {
	string usuarios;
	try {
		usuarios = perfiles->get(USERLIST);
	} catch (KeyNotFound& e) {
		return "";
	}

	string substringMinuscula(substring);
	transform(substringMinuscula.begin(), substringMinuscula.end(), substringMinuscula.begin(), ::tolower);

	vector<string> listaUsuarios = ParserURI::parsear(usuarios, RESERVED_CHAR);
	string usuariosConSubstring;
	for (auto &usuario : listaUsuarios) {
		string usuarioMinuscula(usuario);
		transform(usuarioMinuscula.begin(), usuarioMinuscula.end(), usuarioMinuscula.begin(), ::tolower);
		if (usuarioMinuscula.find(substringMinuscula) != string::npos)
			usuariosConSubstring += (usuariosConSubstring != "") ? (RESERVED_STR + usuario) : usuario;
	}
	return usuariosConSubstring;
}
