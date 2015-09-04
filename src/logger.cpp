#include "logger.h"

const char* NOMBRE_ARCH_LOG = "files/logger/log.txt";

Logger::Logger() {
	nivelLogger = NIVEL_LOGGER;
}

Logger::~Logger() {

}

void Logger::loggear(string texto, int tipoDeError){
	if (this->nivelLogger <= tipoDeError){
        ofstream log(NOMBRE_ARCH_LOG, ios_base::app | ios::binary);	

        switch (tipoDeError){

        	case ERROR:
        		log << "ERROR: ";
        		break;

        	case WARN:
        		log << "WARNING: ";
        		break;

        	case INFO:
        		log << "INFO: ";
        		break;

        	case DEBUG:
        		log << "DEBUG: ";
        		break;

        	case TRACE:
        		log << "TRACE: ";
        		break;

        	default:
        		break;
        }
        log << texto << endl;
        log.close();
	}
}

void Logger::verificarTamanioDelLog(){
    ifstream log(NOMBRE_ARCH_LOG, ios_base::in);
	log.seekg(0, ios::end);
	int size = static_cast <int> (log.tellg());
	if (size > UN_MB){
		ofstream borrar(NOMBRE_ARCH_LOG, ios::trunc);
		borrar.close();
	}
	log.close();
}

void Logger::escribirHoraEnElLog(){
	struct tm *tiempo;
    time_t fecha_sistema;
    time(&fecha_sistema);
    tiempo = localtime(&fecha_sistema);
    int anio = tiempo->tm_year + 1900;
    int mes = tiempo->tm_mon + 1;
    int dia = tiempo->tm_mday;
    int hora = tiempo->tm_hour;
    int min = tiempo->tm_min;
    int seg = tiempo->tm_sec;
    this->verificarTamanioDelLog();
    ofstream log(NOMBRE_ARCH_LOG, ios_base::app | ios::binary);
	log << "===============EJECUCION DEL DIA ";
	if (dia <= 9){
		log << "0" << dia << "/";
	} else {
		log << dia << "/";
	}
	if (mes <= 9){
		log << "0" << mes << "/";
	} else {
		log << mes << "/";
	}
	log << anio << ", HORA ";
	if (hora <= 9){
		log << "0" << hora << ":";
	} else {
		log << hora << ":";
	}
	if (min <= 9){
		log << "0" << min << ":";
	} else {
		log << min << ":";
	}
	if (seg <= 9){
		log << "0" << seg;
	} else {
		log << seg;
	}
	log << "===============" << endl;
	log.close();
}
