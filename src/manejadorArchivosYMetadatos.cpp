#include "manejadorArchivosYMetadatos.h"

//TODO: LOGUEAR!!!!!

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

std::vector<std::string> ManejadorArchivosYMetadatos::parsearDirectorios(std::string pathCompleto) {
	std::vector<std::string> directorios;
	unsigned int size = pathCompleto.size();
	char delim = '/';
	std::string buffer = "";
	for (unsigned int i = 0; i < size; i++){
		char n = pathCompleto[i];
		if (n != delim)
			buffer +=n;
		else if (n == delim && buffer != ""){ //No hay directorios vacios
			directorios.push_back(buffer);
			buffer = "";
		}
	}
	if(buffer != "") directorios.push_back(buffer);
	return directorios;
}

// El path recibido no debe contener el nombre de un archivo.
// En caso de que sea asi, se debera modificar este metodo.
// Lo que si deberia tener el path es el nombre de usuario "incluido".
void ManejadorArchivosYMetadatos::crearCarpeta(std::string path) {
	struct stat sb;
	// Agrego el FileSystem para que sea la "raiz"
	string pathCompleto = "FileSystem/" + path;
	std::vector<std::string> directorios = parsearDirectorios(pathCompleto);
	std::string acum = "";
	int size = directorios.size();
	for (int i = 0; i < size; i++){
		std::string directorio = directorios[i];
		acum += (directorio + "/");
		// Me fijo si existe la carpeta, sino la creo
		if (! (stat(acum.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) ){
			cout << "El directorio " << directorio << " no existe. Creandolo ..." << endl;
			mkdir(acum.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}
	}
}

// Tambien se podria sacar el username y que venga en el filename.
// En ese caso habria que modificar una boludez.
// El filename deberia venir con los path de carpetas tambien
void ManejadorArchivosYMetadatos::subirArchivo(std::string username,
		std::string filename, const char* data, int dataLen) {
	//No le agrego el FileSystem porque se agrega despues en el metodo crearCarpeta
	std::string pathCompleto = username + "/" + filename;
	std::vector<std::string> directorios = parsearDirectorios(pathCompleto);
	int size = directorios.size();
	std::string pathSinArchivo = "";
	for (int i = 0; i < size-1; i++){ //Saco el nombre del archivo
		std::string directorio = directorios[i];
		pathSinArchivo += directorio;
		if (i < size-2) pathSinArchivo += "/";
	}
	//Verifico que existan todas las carpetas y sino las creo
	crearCarpeta(pathSinArchivo);
	std::string pathConFileSystem = "FileSystem/" + pathCompleto;

	ofstream outFile(pathConFileSystem, std::ofstream::binary);
	outFile.write(data, dataLen);
	outFile.close();
}
