#ifndef PARSERURI_H_
#define PARSERURI_H_

#include <vector>

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
	 * @brief Reemplaza todas las ocurrencias de un char en un string por otro.
	 *
	 * @param str				string al cual se le quieren reemplazar un caracter.
	 * @param original			char que se quiere reemplazar.
	 * @param nuevo				char por el cual se quiere reemplazar.
	 *
	 * @retval string			string con los caracteres correspondientes reemplazados.
	 */
	static string replaceAll(string str, char original, char nuevo) {
		const size_t SIZE = str.size();
		for (size_t i = 0; i < SIZE; i++) {
			if (str[i] == original)
				str[i] = nuevo;
		}
		return str;
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
};

#endif /* PARSERURI_H_ */
