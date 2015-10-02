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
	std::string path = "pablo/hola/como/estas";

	vector<string> directorios = manejador->parsearDirectorios(path);
	vector<string>::iterator itDir = directorios.begin();
	ASSERT_TRUE(directorios.size() == 4);
	int i = 0;
	for ( ; itDir != directorios.end() ; itDir++, i++){
		if (i==0) EXPECT_EQ("pablo", (*itDir));
		else {
			if (i==1) EXPECT_EQ("hola", (*itDir));
			else {
				if (i==2) EXPECT_EQ("como", (*itDir));
				else if (i==3) EXPECT_EQ("estas", (*itDir));
			}
		}
	}
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaParsearBienPathConEspacios) {
	std::string path = "pablo/hola/como estas";

	vector<string> directorios = manejador->parsearDirectorios(path);
	vector<string>::iterator itDir = directorios.begin();
	ASSERT_TRUE(directorios.size() == 3);
	int i = 0;
	for ( ; itDir != directorios.end() ; itDir++, i++){
		if (i==0) EXPECT_EQ("pablo", (*itDir));
		else {
			if (i==1) EXPECT_EQ("hola", (*itDir));
			else if (i==2) EXPECT_EQ("como estas", (*itDir));
		}
	}
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaCrearBienCarpetasSeguras) {
	struct stat sb;
	string path = "pablo/como estas/bien";
	manejador->crearCarpetaSegura("pablo", path);
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

TEST_F(ManejadorArchivosYMetadatosTest, deberiaHaberErrorEnPath) {
	string path = "pablo/como#estas/bien";
	EXPECT_FALSE( manejador->verificarPathValido(path) );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaHaberErrorEnPermisos) {
	string path = "pablo/como estas/bien";
	EXPECT_FALSE( manejador->verificarPermisos("juan",path) );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaCrearCarpetaTrashAlCrearUsuario) {
	struct stat sb;
	string usuario = "pablo";
	ASSERT_TRUE(manejador->crearUsuario(usuario));
	string trash = pathFS + "/" + usuario + "/#trash";
	EXPECT_TRUE(stat(trash.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaSubirBienArchivoDeTexto) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "");
	string pathCompleto = pathFS + "/" + filepath;
	ifstream archivo(pathCompleto.c_str());
	string texto;
	ASSERT_TRUE(archivo.is_open());
	while ( getline(archivo,texto) ) {
		EXPECT_EQ(texto, "hola pablo");
	}
	archivo.close();
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaEliminarBienArchivoDeTexto) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "");
	string filepathCompleto = pathFS + "/" + filepath;
	manejador->eliminarArchivo("pablo", filepath);
	string filepathCompletoTrash = pathFS + "/pablo/#trash/saludo.txt";
	struct stat buffer;
	EXPECT_FALSE(stat (filepathCompleto.c_str(), &buffer) == 0);
	EXPECT_TRUE(stat (filepathCompletoTrash.c_str(), &buffer) == 0);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderConsultarMetadatosDeArchivo) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	string metadato = manejador->consultarMetadatosArchivo("pablo", filepath);
	EXPECT_EQ("un metadato", metadato);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderActualizarMetadatosDeArchivo) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	manejador->actualizarMetadatos("pablo",filepath,"otro metadato");
	string metadato = manejador->consultarMetadatosArchivo("pablo", filepath);
	EXPECT_EQ("otro metadato", metadato);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderAgregarPermisoYAgregarloAlMetadato) {
	//Como todavia no se agregan los permisos propiamente dichos, me fijo que se agreguen al metadato nomas
	string metadatoArch = "{\n"
			"\t\"etiquetas\" : [ \"23\", true, \"juan\" ],\n"
			"\t\"extension\" : \"jpg\",\n"
			"\t\"fecha ultima modificacion\" : \"09/09/2015\",\n"
			"\t\"nombre\" : \"sol\",\n"
			"\t\"propietario\" : \"Pancheitor\",\n"
			"\t\"usuario ultima modificacion\" : \"Pepe\",\n"
			"\t\"usuarios\" : [ \"pablo\" ]\n"
			"}";
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, metadatoArch);
	manejador->agregarPermiso("pablo",filepath,"juan");
	string metadatoActualizado = manejador->consultarMetadatosArchivo("pablo",filepath);
	ParserJson parser;
	MetadatoArchivo asd = parser.deserializarMetadatoArchivo(metadatoActualizado);
	EXPECT_EQ("juan", asd.usuariosHabilitados.back());
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaDevolverStringVacioPorArchivoInexistenteAlDescargar) {
	string path = "vacio";
	path = manejador->descargarArchivo("pablo", "pablo/archivos/hola.txt");
	EXPECT_EQ("",path);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaDevolverPathCompletoPorArchivoExistenteAlDescargar) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	string pathCompleto = manejador->descargarArchivo("pablo", filepath);

	char homeDirectory[1024];
    getcwd(homeDirectory, sizeof(homeDirectory));
    string pathAComparar(homeDirectory);
    pathAComparar += "/" + pathFS + "/" + filepath;

	EXPECT_EQ(pathAComparar,pathCompleto);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBorrarElFileSystem) {
	struct stat sb;
	string path = "pablo/hola";
	manejador->crearCarpetaSegura("pablo", path); //Creo una carpeta para asegurarme que exista el FS
	//Me fijo si existe la carpeta del FS
	EXPECT_TRUE(stat(pathFS.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
	manejador->deleteFileSystem();
	//Ahora no deberia existir
	EXPECT_FALSE(stat(pathFS.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}
