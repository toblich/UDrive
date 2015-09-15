#include "manejadorDeUsuarios.h"

using namespace std;

ManejadorDeUsuarios::ManejadorDeUsuarios(BaseDeDatos* perfiles, BaseDeDatos* sesiones, BaseDeDatos* passwords) {
	this->perfiles = perfiles;
	this->sesiones = sesiones;
	this->passwords = passwords;
}

ManejadorDeUsuarios::~ManejadorDeUsuarios() {
	// TODO Auto-generated destructor stub
}

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
	//TODO validar datos de entrada
	return passwords->put(username, password) and perfiles->put(username, perfil);
}

string ManejadorDeUsuarios::iniciarSesion(string username) {
	time_t timestamp;
	time(&timestamp);
	string token = generarToken(username, timestamp);
	if (sesiones->put(token, username))	//TODO agregar timestamp en la BD?
		return token;
	usleep(1000);	//Vuelve a probar 1 miliseg despues. TODO: revisar si esta bien.
	iniciarSesion(username);
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
	string time = to_string(timestamp);
	hash<string> funcionHash;
	return to_string(funcionHash(username + time));
}
