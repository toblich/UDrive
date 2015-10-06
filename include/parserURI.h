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
};

#endif /* PARSERURI_H_ */
