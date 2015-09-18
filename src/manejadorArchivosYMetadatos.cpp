#include "manejadorArchivosYMetadatos.h"

ManejadorArchivosYMetadatos::ManejadorArchivosYMetadatos(BaseDeDatos* dbMetadatos) {
	this->dbMetadatos = dbMetadatos;
    getcwd(homeDirectory, sizeof(homeDirectory));
    cout << "Home del proyecto: " << homeDirectory << endl;
}

ManejadorArchivosYMetadatos::~ManejadorArchivosYMetadatos() {
}

void ManejadorArchivosYMetadatos::crearUsuario(std::string username) {
	this->crearCarpeta(username);
}

std::queue<std::string> ManejadorArchivosYMetadatos::parsearDirectorios(std::string pathCompleto) {
	std::queue<std::string> directorios;
	unsigned int size = pathCompleto.size();
	char delim = '/';
	std::string buffer = "";
	for (unsigned int i = 0; i < size; i++){
		char n = pathCompleto[i];
		if (n != delim)
			buffer +=n;
		else if (n == delim && buffer != ""){
			directorios.push(buffer);
			buffer = "";
		}
	}
	if(buffer != "") directorios.push(buffer);
	return directorios;
}

// El path recibido no debe contener el nombre de un archivo.
// En caso de que sea asi, se debera modificar este metodo.
void ManejadorArchivosYMetadatos::crearCarpeta(std::string path) {
	//Vuelvo a la raiz por las dudas
	chdir(homeDirectory);

	char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    cout << "Current directory: " << cwd << endl;

	std::string fileSystem = "FileSystem";
	if ( chdir(fileSystem.c_str()) == -1 ){
		cout << "El directorio FileSystem no existe. Creandolo ..." << endl;
		mkdir(fileSystem.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		chdir(fileSystem.c_str());
	}

    getcwd(cwd, sizeof(cwd));
    cout << "Current directory: " << cwd << endl;

	std::queue<std::string> directorios = parsearDirectorios(path);
	int size = directorios.size();
	for (int i = 0; i < size; i++){
		std::string directorio = directorios.front();
		directorios.pop();
		if ( chdir(directorio.c_str()) == -1 ){
			cout << "El directorio " << directorio << " no existe. Creandolo ..." << endl;
			mkdir(directorio.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			chdir(directorio.c_str());
		}
	}

    getcwd(cwd, sizeof(cwd));
    cout << "Current directory: " << cwd << endl;
}

void ManejadorArchivosYMetadatos::subirArchivo(std::string username,
		std::string filename, const char* data, int dataLen) {
	//El filename deberia venir con los path de carpetas tambien
	ofstream outFile(filename, std::ofstream::binary);
	outFile.write(data, dataLen);
	outFile.close();
}
