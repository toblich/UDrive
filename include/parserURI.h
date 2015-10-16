#ifndef PARSERURI_H_
#define PARSERURI_H_

#include <vector>

using namespace std;

class ParserURI {

public:
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

	static string join(const vector<string>& partes, char delim){
		return ParserURI::join(partes, delim, 0, partes.size());
	}

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
};

#endif /* PARSERURI_H_ */
