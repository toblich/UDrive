#ifndef PARSERURI_H_
#define PARSERURI_H_

#include <vector>
#include "metadatosYConstantes.h"

using namespace std;

/**
 * @brief Se encarga de algunas tareas que son repetitivas en el código.
 *
 * Particularmente se encarga de hacer separación o unión de strings
 * mediante algún delimitador.
 */

class ParserURI {

public:
	/**
	 * @brief Parsea un determinado string, separandolo por un caracter delimitador.
	 *
	 * @param uri				string a parsear.
	 * @param delim			char por el cual se quiere separar.
	 *
	 * @retval vector<string>	vector con las "partes" en las cuales fue separado el string.
	 */
	static vector<string> parsear(string uri, char delim) {
		vector<string> uris;
		unsigned int size = uri.size();
		string buffer = "";
		for (unsigned int i = 0; i < size; i++) {
			char n = uri[i];
			if (n != delim)
				buffer += n;
			else if (n == delim && buffer != "") { //No hay uris con nombre vacio
				uris.push_back(buffer);
				buffer = "";
			}
		}
		if (buffer != "")
			uris.push_back(buffer);
		return uris;
	}

	/**
	 * @brief Junta los elementos de un vector separandolos por el caracter delimitador.
	 *
	 * @param partes			vector de string a juntar.
	 * @param delim				char por el cual se quiere unir.
	 *
	 * @retval string			string con los elementos del vector unidos por el delimitador.
	 */
	static string join(const vector<string>& partes, char delim){
		return ParserURI::join(partes, delim, 0, partes.size());
	}

	/**
	 * @brief Junta desde el elemento 'desde' hasta el elemento 'hasta' de un vector, separandolos
	 * por el caracter delimitador.
	 *
	 * @param partes			vector de string a juntar.
	 * @param delim				char por el cual se quiere unir.
	 * @param desde				int que indica desde que elemento del vector se quiere unir.
	 * @param hasta				int que indica hasta que elemento del vector se quiere unir.
	 *
	 * @retval string			string con los elementos del vector unidos por el delimitador.
	 */
	static string join(const vector<string>& partes, char delim, int desde, int hasta){
		string salida = "";
		int size = partes.size();
		for (int i = desde; i < hasta and i < size; i++){
			salida += partes[i] + delim;
		}
		if ( salida.size() > 0)
			salida.pop_back();
		return salida;
	}

	/**
	 * @brief Esta función sirve para cuando se renombra un archivo reemplaza
	 * en el path interno del mismo el filename viejo por el nuevo.
	 *
	 * @param pathInterno		string con el path interno actual del archivo.
	 * @param nuevoFilename		string con el nuevo filename del archivo.
	 *
	 * @retval string			string del path interno del archivo con el nuevo filename.
	 */
	static string pathConNuevoFilename (const string& pathInterno, const string& nuevoFilename) {
		vector<string> partes = ParserURI::parsear(pathInterno, '/');
		partes.pop_back();
		partes.push_back(nuevoFilename);
		string nuevoPathInterno = ParserURI::join(partes, '/');
		return nuevoPathInterno;
	}

	/**
	 * @brief Quita el último campo que este luego del RESERVED_CHAR.
	 *
	 * @param filepath			string al cual se le quiere quitar el nro de secuencia.
	 *
	 * @retval string			string sin el numero de secuencia.
	 */
	static string pathSinNroSecuencia (const string& filepath) {
		if (filepath.find(RESERVED_CHAR) == string::npos)
			return filepath;	// si no tenia nro secuencia, lo devuelve como estaba
		vector<string> partes = ParserURI::parsear(filepath, RESERVED_CHAR);
		string filepathSinSecuencia = ParserURI::join(partes, RESERVED_CHAR, 0, partes.size() - 1);
		return filepathSinSecuencia;
	}

	/**
	 * @brief Devuelve el numero de secuencia o version final en un filepath o URI.
	 *
	 * @param filepath			string del que se obtendrá el nro de secuencia/version.
	 *
	 * @retval int				numero de secuencia/version.
	 */
	static int obtenerNroVersion (const string& filepath) {
		string version = parsear(filepath, RESERVED_CHAR).back();
		try {
			return stoi(version);
		} catch (invalid_argument& e) {
			return LATEST;
		}
	}
};

#endif /* PARSERURI_H_ */
