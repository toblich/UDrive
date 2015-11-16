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
	static void verificarTamanioDelLog();
	Logger();
public:
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje
	 * para un determinado tipo de error
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 * @param tipoDeError   int con el nivel de error que se quiere escribir
	 *
	 */
	static void loggear(const string& texto, int tipoDeError);
	static void escribirHoraEnElLog();
	~Logger();
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como INFO
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logInfo(const string& texto);
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como WARN
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logWarn(const string& texto);
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como TRACE
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logTrace(const string& texto);
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como ERROR
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logError(const string& texto);
	/**
	 * @brief Función que se utiliza para escribir en el log un determinado mensaje como DEBUG
	 *
	 * @param texto			string con el mensaje que se quiere loggear
	 *
	 */
	static void logDebug(const string& texto);
};

#endif /* LOGGER_H_ */
