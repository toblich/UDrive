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

class Logger {

private:
	int nivelLogger;
	void verificarTamanioDelLog();
public:
	Logger();
	void loggear(string texto, int tipoDeError);
	void escribirHoraEnElLog();
	~Logger();

	static void logInfo(string texto);
	static void logWarn(string texto);
	static void logTrace(string texto);
	static void logError(string texto);
	static void logDebug(string texto);
};

#endif /* LOGGER_H_ */
