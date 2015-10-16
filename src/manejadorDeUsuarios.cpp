#include "manejadorDeUsuarios.h"

using namespace std;

ManejadorDeUsuarios::ManejadorDeUsuarios(BD* perfiles, BD* sesiones, BD* passwords) {
	this->perfiles = perfiles;
	this->sesiones = sesiones;
	this->passwords = passwords;
}

ManejadorDeUsuarios::~ManejadorDeUsuarios() {}

bool ManejadorDeUsuarios::validarLogin(string username, string password) {
	string realPassword;
	try{
		realPassword = passwords->get(username);
	} catch (KeyNotFound& e) {
		return false;
	}
	return (realPassword == password);	//TODO: Comparacion segura :P
}

bool ManejadorDeUsuarios::registrarUsuario(string username, string password, string perfil) {
	if (not esUsernameValido(username)) return false;
	if (not esPasswordValida(password)) return false;
	if (not esPerfilValido(perfil)) return false;
	return passwords->put(username, password) and perfiles->put(username, perfil);
}

string ManejadorDeUsuarios::iniciarSesion(string username) {
	time_t timestamp;
	time(&timestamp);
	string token = generarToken(username, timestamp);
	if (sesiones->put(token, username))	//TODO agregar timestamp en la BD?
		return token;
	usleep(100000);	//Vuelve a probar 100 miliseg despues
	return iniciarSesion(username);
}

bool ManejadorDeUsuarios::autenticarToken(string token, string username) {
	try{
		return username == sesiones->get(token);
	} catch (KeyNotFound& e) {
		return false;
	}
}

bool ManejadorDeUsuarios::cerrarSesion(string token, string username) {
	if (autenticarToken(token, username)) {
		sesiones->erase(token);	//No lanza KeyNotFound por el autenticarToken
		return true;
	}
	return false;
}

string ManejadorDeUsuarios::generarToken(string username, time_t timestamp) {
	string timeAsString = to_string(timestamp);
	hash<string> funcionHash;
	return to_string(funcionHash(username + timeAsString));
}

bool ManejadorDeUsuarios::esPerfilValido(const string& perfilJson) {
	MetadatoUsuario perfil = ParserJson::deserializarMetadatoUsuario(perfilJson);
	return regex_match(perfil.nombre, REGEX_NOMBRE) and regex_match(perfil.email, REGEX_EMAIL);
}

bool ManejadorDeUsuarios::esUsernameValido(const string& username) {
	const size_t SIZE = username.size();
	for (size_t i = 0; i < SIZE; i++){
		if (esCaracterInvalido(username[i]))
			return false;
	}
	return true;
}

bool ManejadorDeUsuarios::esCaracterInvalido(char c) {
	return (CHARS_INVALIDOS.find(c) != string::npos);
}

bool ManejadorDeUsuarios::esPasswordValida(const string& password) {
	// TODO: forzar Alfanumerica?
	return (password.size() >= 8);
}

string ManejadorDeUsuarios::getPerfil(string username) {
	if (perfiles->contains(username)){
		return perfiles->get(username);
	} else {
		return string();
	}
}

bool ManejadorDeUsuarios::modifyPerfil(string username, string perfil){
	if (esPerfilValido(perfil)){
		if (perfiles->contains(username)){
			perfiles->modify(username, perfil);
			return true;
		}else {
			perfiles->put(username, perfil);
			return true;
		}
	}
	return false;
}
