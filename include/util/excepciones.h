#ifndef EXCEPCIONES_H_
#define EXCEPCIONES_H_

#include <stdexcept>
#include <string.h>
#include <iostream>

using namespace std;

/**
 * @brief Se encarga del manejo de la excepción KeyNotFound en la Base de Datos.
 *
 * Esta excepción es lanzada cuando se quiere buscar una clave en la BD y ésta no existe.
 */
class KeyNotFound : public exception {
  private:
    string err_msg;
  public:
    KeyNotFound() : err_msg("") {};
    KeyNotFound(const char *msg) : err_msg(msg) {};
    KeyNotFound(string msg) { err_msg = msg; };
    KeyNotFound(const KeyNotFound& original) : err_msg(original.what()) {};
    ~KeyNotFound() throw() {};
    const char *what() const throw() { return ("KeyNotFound: " + err_msg).c_str(); };
};

/**
 * @brief Se encarga del manejo de la excepción InvalidDBAction en la Base de Datos.
 *
 * Esta excepción es lanzada cuando se quiere realizar una acción inválida en alguno de los métodos de la BD.
 */
class InvalidDBAction : public exception {
  private:
    string err_msg;
  public:
    InvalidDBAction() : err_msg("") {};
    InvalidDBAction(const char *msg) : err_msg(msg) {};
    InvalidDBAction(string msg) { err_msg = msg; };
    InvalidDBAction(const InvalidDBAction& original) : err_msg(original.what()) {};
    ~InvalidDBAction() throw() {};
    const char *what() const throw() { return ("InvalidDBAction: " + err_msg).c_str(); };
};

/**
 * @brief Se encarga del manejo de la excepción InvalidVersion.
 *
 * Esta excepción es lanzada cuando se quiere subir (sin forzar) un archivo a partir de una versión que no es la última.
 */
class InvalidVersion : public exception {
  private:
	string err_msg;
  public:
	InvalidVersion() : err_msg("") {};
	InvalidVersion(const char *msg) : err_msg(msg) {};
	InvalidVersion(string msg) { err_msg = msg; };
	InvalidVersion(const InvalidVersion& original) : err_msg(original.what()) {};
	~InvalidVersion() throw() {};
	const char *what() const throw() { return ("InvalidVersion: " + err_msg).c_str(); };
};

#endif /* EXCEPCIONES_H_ */
