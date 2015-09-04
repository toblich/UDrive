#ifndef LOGGER_H_
#define LOGGER_H_

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <ctime>

using namespace std;

const int NIVEL_LOGGER = 0;
const int UN_MB = 1048576;

const int ERROR = 0;
const int WARN 	= 1;
const int INFO 	= 2;
const int DEBUG = 3;
const int TRACE = 4;


class Logger {

private:
	int nivelLogger;
	void verificarTamanioDelLog();
public:
	Logger();
	void loggear(string texto, int tipoDeError);
	void escribirHoraEnElLog();
	~Logger();
};

#endif /* LOGGER_H_ */
