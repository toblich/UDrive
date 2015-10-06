#define private public

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
	EXPECT_FALSE(stat (filepathCompleto.c_str(), &buffer) == 0); //No existe en el filesystem
	EXPECT_TRUE(stat (filepathCompletoTrash.c_str(), &buffer) == 0); //Si existe en el trash
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

TEST_F(ManejadorArchivosYMetadatosTest, deberiaVerificarBienQueCarpetaEstaVacia) {
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien");
	EXPECT_TRUE( manejador->carpetaVacia(pathFS + "/pablo/como estas/bien") );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaVerificarBienQueCarpetaNoEstaVacia) {
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien");
	EXPECT_FALSE( manejador->carpetaVacia(pathFS + "/pablo/como estas") );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBorrarBienCarpetaSinArchivosNiCarpetas) {
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien");
	EXPECT_TRUE( manejador->eliminarCarpeta("pablo","pablo/como estas/bien") );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBorrarBienCarpetaConArchivosPeroSinCarpetas) {
	string filepath = "pablo/como estas/bien/saludo.txt";
	string filepath2 = "pablo/como estas/bien/juan";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, "un metadato");
	EXPECT_TRUE( manejador->eliminarCarpeta("pablo","pablo/como estas/bien") );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBorrarBienCarpetaSinArchivosPeroConCarpetas) {
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien/vos?");
	EXPECT_TRUE( manejador->eliminarCarpeta("pablo","pablo/como estas") );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBorrarBienCarpetaConArchivosYCarpetas) {
	string filepath = "pablo/como estas/bien/saludo.txt";
	string filepath2 = "pablo/como estas/bien/juan";
	string filepath3 = "pablo/como estas/bien/vos?/juan";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien/vos?");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, "un metadato");
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, "un metadato");
	EXPECT_TRUE( manejador->eliminarCarpeta("pablo","pablo/como estas") );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaCambiarBienFechaYUsuarioUltimaModificacion) {
	std::string jsonArchOK = "{\n"
			"\t\"etiquetas\" : [ \"23\", true, \"juan\" ],\n"
			"\t\"extension\" : \"jpg\",\n"
			"\t\"fecha ultima modificacion\" : \"09/09/2015\",\n"
			"\t\"nombre\" : \"sol\",\n"
			"\t\"propietario\" : \"Pancheitor\",\n"
			"\t\"usuario ultima modificacion\" : \"Pepe\",\n"
			"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\" ]\n"
			"}";
	struct tm *tiempo;
	time_t fecha_sistema;
	time(&fecha_sistema);
	tiempo = localtime(&fecha_sistema);
	int anio = tiempo->tm_year + 1900;
	int mes = tiempo->tm_mon + 1;
	int dia = tiempo->tm_mday;

	std::string jsonNuevoMetadato = manejador->actualizarUsuarioFechaModificacion(jsonArchOK, "juancito");
	ParserJson parser;
	MetadatoArchivo nuevoMetadato = parser.deserializarMetadatoArchivo(jsonNuevoMetadato);
	std::string fecha = std::to_string(dia) + "/" + std::to_string(mes) + "/" + std::to_string(anio);
	EXPECT_EQ(fecha, nuevoMetadato.fechaUltimaModificacion);
	EXPECT_EQ("juancito", nuevoMetadato.usuarioUltimaModificacion);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaTenerTamanioCeroCarpetaVacia) {
	unsigned long int size = 0;
	manejador->crearUsuario("pablo");
	manejador->tamanioCarpeta("pablo", size);
	EXPECT_EQ(0,size);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaTenerCarpetaTamanioIgualAlUnicoArchivo) {
	unsigned long int size = 0;
	string filepath = "pablo/como estas/bien/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	manejador->tamanioCarpeta("pablo", size);
	EXPECT_EQ(10, size);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaTenerCarpetaTamanioIgualASumaDeArchivos) {
	unsigned long int sizePablo = 0;
	unsigned long int sizeBien = 0;
	unsigned long int sizeVos = 0;
	string filepath  = "pablo/como estas/bien/vos?/saludo.txt";
	string filepath2 = "pablo/como estas/bien/vos?/juan";
	string filepath3 = "pablo/como estas/bien/juan";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien/vos?");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	manejador->subirArchivo("pablo", filepath2, "hola tobi", 9, "un metadato");
	manejador->subirArchivo("pablo", filepath3, "hola pancho", 11, "un metadato");
	manejador->tamanioCarpeta("pablo", sizePablo);
	manejador->tamanioCarpeta("pablo/como estas/bien", sizeBien);
	manejador->tamanioCarpeta("pablo/como estas/bien/vos?", sizeVos);
	EXPECT_EQ(30, sizePablo);
	EXPECT_EQ(30, sizeBien);
	EXPECT_EQ(19, sizeVos);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaEliminarArchivoConMetodoEliminar) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "");
	string filepathCompleto = pathFS + "/" + filepath;
	manejador->eliminar("pablo", filepath);
	struct stat buffer;
	EXPECT_FALSE(stat (filepathCompleto.c_str(), &buffer) == 0); //No existe en el filesystem
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaEliminarCarpetaConMetodoEliminar) {
	string filepath = "pablo/como estas/bien/saludo.txt";
	string filepath2 = "pablo/como estas/bien/juan";
	string filepath3 = "pablo/como estas/bien/vos?/juan";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien/vos?");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, "un metadato");
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, "un metadato");
	manejador->eliminar("pablo", "pablo/como estas");

	string pathCarpetaEliminada = pathFS + "/pablo/como estas";
	struct stat sb;
	EXPECT_FALSE(stat(pathCarpetaEliminada.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
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
