#ifndef LOGGER_H_
#define LOGGER_H_

#include <string.h>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

const int NIVEL_LOGGER = 0;
const int UN_MB = 1048576;

const int ERROR = 4;
const int WARN 	= 3;
const int INFO 	= 2;
const int DEBUG = 1;
const int TRACE = 0;

/**
 * @brief Se encarga del loggeo de las distintas actividades en el Servidor.
 *
 */
class Logger {

private:
	int nivelLogger;
	void verificarTamanioDelLog();
public:
	Logger();
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje
	 * para un determinado tipo de error
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 * @param tipoDeError   int con el nivel de error que se quiere escribir
	 *
	 */
	void loggear(string texto, int tipoDeError);
	void escribirHoraEnElLog();
	~Logger();
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como INFO
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logInfo(string texto);
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como WARN
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logWarn(string texto);
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como TRACE
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logTrace(string texto);
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como ERROR
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logError(string texto);
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como DEBUG
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logDebug(string texto);
};

#endif /* LOGGER_H_ */
