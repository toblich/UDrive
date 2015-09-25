#include "manejadorArchivosYMetadatos.h"
#include "mapDB.h"
#include <gtest/gtest.h>

using namespace std;

const string pathFS = "FileSystem_test";

class ManejadorArchivosYMetadatosTest : public ::testing::Test {

	protected:
		virtual void SetUp() {
			db = new MapDB();
			manejador = new ManejadorArchivosYMetadatos(db, pathFS);
		}

		virtual void TearDown() {
			manejador->deleteFileSystem();
			delete manejador;
			db->deleteBD();
			delete db;
		}

		BD* db;
		ManejadorArchivosYMetadatos* manejador;
};

TEST_F(ManejadorArchivosYMetadatosTest, deberiaParsearBienPathComun) {
	std::string path = "hola/pablo/como/estas";

	vector<string> directorios = manejador->parsearDirectorios(path);
	vector<string>::iterator itDir = directorios.begin();
	int i = 0;
	for ( ; itDir != directorios.end() ; itDir++, i++){
		if (i==0) EXPECT_EQ("hola", (*itDir));
		else {
			if (i==1) EXPECT_EQ("pablo", (*itDir));
			else {
				if (i==2) EXPECT_EQ("como", (*itDir));
				else if (i==3) EXPECT_EQ("estas", (*itDir));
					else EXPECT_TRUE(false); //Rompo
			}
		}
	}
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaParsearBienPathConEspacios) {
	std::string path = "hola/pablo/como estas";

	vector<string> directorios = manejador->parsearDirectorios(path);
	vector<string>::iterator itDir = directorios.begin();
	int i = 0;
	for ( ; itDir != directorios.end() ; itDir++, i++){
		if (i==0) EXPECT_EQ("hola", (*itDir));
		else {
			if (i==1) EXPECT_EQ("pablo", (*itDir));
			else if (i==2) EXPECT_EQ("como estas", (*itDir));
				else EXPECT_TRUE(false); //Rompo
		}
	}
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaCrearBienCarpetas) {
	struct stat sb;
	string path = "hola/como estas/pablo";
	manejador->crearCarpeta("hola", path);
	string pathCompleto = pathFS + "/" + path;
	std::vector<std::string> directorios = manejador->parsearDirectorios(pathCompleto);
	std::string directorioAcumulado = "";
	int size = directorios.size();
	for (int i = 0; i < size; i++){
		std::string directorio = directorios[i];
		directorioAcumulado += (directorio + "/");
		// Me fijo si existe la carpeta
		EXPECT_TRUE(stat(directorioAcumulado.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
	}
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaSubirBienArchivoDeTexto) {
	string path = "hola";
	manejador->crearCarpeta("pablo", path);
	manejador->subirArchivo("pablo", "hola/hola.txt", "hola pablo", 10, "");
	ifstream archivo;
	string texto;
	if ( archivo.is_open() ) {
		while ( getline(archivo,texto) ) {
			EXPECT_EQ(texto, "hola pablo");
		}
	}
	archivo.close();
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBorrarElFileSystem) {
	struct stat sb;
	string path = "hola";
	manejador->crearCarpeta("pablo", path); //Creo una carpeta para asegurarme que exista el FS
	//Me fijo si existe la carpeta del FS
	EXPECT_TRUE(stat(pathFS.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
	manejador->deleteFileSystem();
	//Ahora no deberia existir
	EXPECT_FALSE(stat(pathFS.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}
