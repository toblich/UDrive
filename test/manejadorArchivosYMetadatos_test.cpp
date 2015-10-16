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
			validador = new Validador(db);
		}

		virtual void TearDown() {
			manejador->deleteFileSystem();
			delete manejador;
			delete validador;
			db->deleteBD();
			delete db;
		}

		BD* db;
		ManejadorArchivosYMetadatos* manejador;
		Validador* validador;
};

const std::string jsonArchOK = "{\n"
		"\t\"etiquetas\" : [ \"23\", true, \"hola\" ],\n"
		"\t\"extension\" : \"txt\",\n"
		"\t\"fecha ultima modificacion\" : \"09/09/2015\",\n"
		"\t\"nombre\" : \"saludo\",\n"
		"\t\"propietario\" : \"pablo\",\n"
		"\t\"usuario ultima modificacion\" : \"pablo\",\n"
		"\t\"usuarios\" : [ \"pablo\" ]\n"
		"}";

const std::string propietario = "pablo";

TEST_F(ManejadorArchivosYMetadatosTest, deberiaParsearBienPathComun) {
	std::string path = "pablo/hola/como/estas";
	vector<string> directorios = ParserURI::parsear(path, '/');
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
	vector<string> directorios = ParserURI::parsear(path, '/');
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
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	string pathCompleto = pathFS + "/" + path;
	vector<string> directorios = ParserURI::parsear(pathCompleto, '/');
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
	EXPECT_FALSE( validador->esPathValido(path) );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaHaberErrorEnPermisos) {
	string path = "pablo/como estas/bien";
	EXPECT_FALSE( validador->verificarPermisos("juan",path) );
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
	manejador->crearUsuario("pablo");
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	string filepathCompleto = pathFS + "/" + filepath;
	manejador->eliminarArchivo("pablo", filepath);
	string filepathCompletoTrash = pathFS + "/pablo/#trash/archivos#saludo.txt#0";
	struct stat buffer;
	EXPECT_FALSE(stat (filepathCompleto.c_str(), &buffer) == 0); //No existe en el filesystem
	EXPECT_TRUE(stat (filepathCompletoTrash.c_str(), &buffer) == 0); //Si existe en el trash
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderConsultarMetadatosDeArchivo) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato");
	string metadato = manejador->consultarMetadatosArchivo("pablo", filepath);
	EXPECT_EQ("un metadato", metadato);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderActualizarMetadatosDeArchivo) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	std::string nuevoJson = "{\n"
			"\t\"etiquetas\" : [ \"pepe\" ],\n"
			"\t\"extension\" : \"png\",\n"
			"\t\"fecha ultima modificacion\" : \"10/09/2015\",\n"
			"\t\"nombre\" : \"luna\",\n"
			"\t\"propietario\" : \"pablo\",\n"
			"\t\"usuario ultima modificacion\" : \"Juan\",\n"
			"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\", \"Pablo\" ]\n"
			"}";
	manejador->actualizarMetadatos("pablo",filepath, nuevoJson);
	string jsonNuevoMetadato = manejador->consultarMetadatosArchivo("pablo", filepath);
	ParserJson parser;
	MetadatoArchivo nuevoMetadato = parser.deserializarMetadatoArchivo(jsonNuevoMetadato);

	EXPECT_EQ("png", nuevoMetadato.extension);
	EXPECT_EQ("10/09/2015", nuevoMetadato.fechaUltimaModificacion);
	EXPECT_EQ("luna", nuevoMetadato.nombre);
	EXPECT_EQ("pablo", nuevoMetadato.propietario);
	EXPECT_EQ("Juan", nuevoMetadato.usuarioUltimaModificacion);
	EXPECT_EQ("pepe", nuevoMetadato.etiquetas.front());
	EXPECT_EQ("Pablo", nuevoMetadato.usuariosHabilitados.back());
}

TEST_F(ManejadorArchivosYMetadatosTest, alActualizarMetadatoDeArchivoConUsuariosVaciosDeberiaConservarUsuarios) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	std::string nuevoJson = "{\n"
			"\t\"etiquetas\" : [ \"pepe\" ],\n"
			"\t\"extension\" : \"png\",\n"
			"\t\"fecha ultima modificacion\" : \"10/09/2015\",\n"
			"\t\"nombre\" : \"luna\",\n"
			"\t\"propietario\" : \"pablo\",\n"
			"\t\"usuario ultima modificacion\" : \"Juan\",\n"
			"\t\"usuarios\" : \n"
			"}";
	manejador->actualizarMetadatos("pablo",filepath, nuevoJson);
	string jsonNuevoMetadato = manejador->consultarMetadatosArchivo("pablo", filepath);
	ParserJson parser;
	MetadatoArchivo nuevoMetadato = parser.deserializarMetadatoArchivo(jsonNuevoMetadato);

	EXPECT_EQ("pablo", nuevoMetadato.usuariosHabilitados.back());
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderAgregarPermisoYAgregarloAlMetadato) {
	//Como todavia no se agregan los permisos propiamente dichos, me fijo que se agreguen al metadato nomas
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
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
	manejador->crearUsuario("pablo");
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
	EXPECT_TRUE( validador->carpetaVacia(pathFS + "/pablo/como estas/bien") );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaVerificarBienQueCarpetaNoEstaVacia) {
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien");
	EXPECT_FALSE( validador->carpetaVacia(pathFS + "/pablo/como estas") );
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, jsonArchOK);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, jsonArchOK);
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, jsonArchOK);
	EXPECT_TRUE( manejador->eliminarCarpeta("pablo","pablo/como estas") );
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaCambiarBienFechaYUsuarioUltimaModificacion) {
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, jsonArchOK);
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, jsonArchOK);
	manejador->eliminar("pablo", "pablo/como estas");

	string pathCarpetaEliminada = pathFS + "/pablo/como estas";
	struct stat sb;
	EXPECT_FALSE(stat(pathCarpetaEliminada.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaObtenerBienLaEstructuraDeLasCarpetas) {
	ParserJson parser;
	string filepath1 = "pablo/como estas/bien/saludo.txt";
	MetadatoArchivo metadato1;
	metadato1.nombre = "saludo";
	metadato1.extension = "txt";
	string metadatos1 = parser.serializarMetadatoArchivo(metadato1);

	string filepath2 = "pablo/como estas/bien/juan";
	MetadatoArchivo metadato2;
	metadato2.nombre = "juan";
	metadato2.extension = "none";
	string metadatos2 = parser.serializarMetadatoArchivo(metadato2);

	string filepath3 = "pablo/como estas/bien/pepe.hola";
	MetadatoArchivo metadato3;
	metadato3.nombre = "pepe";
	metadato3.extension = "hola";
	string metadatos3 = parser.serializarMetadatoArchivo(metadato3);

	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien/vos?");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien/hola");
	manejador->subirArchivo("pablo", filepath1, "hola pablo", 10, metadatos1);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, metadatos2);
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, metadatos3);
	string estructura = manejador->obtenerEstructuraCarpeta("pablo/como estas/bien");

	map<string, string> mapa = parser.deserializarMapa(estructura);

	EXPECT_EQ(mapa.at("saludo.txt"), "txt");
	EXPECT_EQ(mapa.at("juan"), "none");
	EXPECT_EQ(mapa.at("pepe.hola"), "hola");
	EXPECT_EQ(mapa.at("vos?"), "#folder");
	EXPECT_EQ(mapa.at("hola"), "#folder");
}

TEST_F(ManejadorArchivosYMetadatosTest, noDeberiaPoderEliminarArchivoInexistente) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", "pablo/archivos");
	EXPECT_FALSE(manejador->eliminar("pablo", filepath));
}

TEST_F(ManejadorArchivosYMetadatosTest, noDeberiaPoderActualizarMetadatoInexistente) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", "pablo/archivos");
	EXPECT_FALSE(manejador->actualizarMetadatos("pablo", filepath, jsonArchOK));
}

TEST_F(ManejadorArchivosYMetadatosTest, noDeberiaPoderConsultarMetadatoInexistente) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", "pablo/archivos");
	EXPECT_EQ(manejador->consultarMetadatosArchivo("pablo", filepath), "");
}

TEST_F(ManejadorArchivosYMetadatosTest, noDeberiaPoderSubirDosVecesUnArchivoConMetodoSubirArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", "pablo/archivos");
	EXPECT_TRUE(manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK));
	EXPECT_FALSE(manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK));
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

// TESTEO DE PERMISOS

void inic(ManejadorArchivosYMetadatos* manejador, string path){
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", path, "hola pablo", 10, jsonArchOK);
	manejador->agregarPermiso("pablo", path, "juan");
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaNoHaberErrorEnPermisos) {
	string path = "pablo/como estas/bien/saludo.txt";
	inic(manejador, path);
	EXPECT_TRUE( validador->verificarPermisos("juan",path) );
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaEliminarBienArchivoDeTexto) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	string filepathCompleto = pathFS + "/" + filepath;
	ASSERT_TRUE( manejador->eliminarArchivo("juan", filepath) );
	string filepathCompletoTrashProp = pathFS + "/pablo/#trash/archivos#saludo.txt#0";
	string filepathCompletoTrashJuan = pathFS + "/juan/#trash/archivos#saludo.txt#0";
	struct stat buffer;
	EXPECT_FALSE(stat (filepathCompleto.c_str(), &buffer) == 0); //No existe en el filesystem
	EXPECT_TRUE(stat (filepathCompletoTrashProp.c_str(), &buffer) == 0); //Si existe en el trash del propietario
	EXPECT_FALSE(stat (filepathCompletoTrashJuan.c_str(), &buffer) == 0); //No existe en el trash del usuario con permisos
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaPoderConsultarMetadatosDeArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	string jsonMetadato = manejador->consultarMetadatosArchivo("juan", filepath);
	ParserJson parser;
	MetadatoArchivo metadato = parser.deserializarMetadatoArchivo(jsonMetadato);
	EXPECT_EQ("txt", metadato.extension);
	EXPECT_EQ("saludo", metadato.nombre);
	EXPECT_EQ("pablo", metadato.propietario);
	EXPECT_EQ("juan", metadato.usuariosHabilitados.back());
	EXPECT_EQ("pablo", metadato.usuariosHabilitados.front());
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaPoderActualizarMetadatosDeArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	std::string nuevoJson = "{\n"
			"\t\"etiquetas\" : [ \"pepe\" ],\n"
			"\t\"extension\" : \"png\",\n"
			"\t\"fecha ultima modificacion\" : \"10/09/2015\",\n"
			"\t\"nombre\" : \"luna\",\n"
			"\t\"propietario\" : \"pablo\",\n"
			"\t\"usuario ultima modificacion\" : \"Juan\",\n"
			"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\", \"Pablo\" ]\n"
			"}";
	manejador->actualizarMetadatos("juan",filepath, nuevoJson);
	string jsonNuevoMetadato = manejador->consultarMetadatosArchivo("juan", filepath);
	ParserJson parser;
	MetadatoArchivo nuevoMetadato = parser.deserializarMetadatoArchivo(jsonNuevoMetadato);

	EXPECT_EQ("png", nuevoMetadato.extension);
	EXPECT_EQ("10/09/2015", nuevoMetadato.fechaUltimaModificacion);
	EXPECT_EQ("luna", nuevoMetadato.nombre);
	EXPECT_EQ("pablo", nuevoMetadato.propietario);
	EXPECT_EQ("Juan", nuevoMetadato.usuarioUltimaModificacion);
	EXPECT_EQ("pepe", nuevoMetadato.etiquetas.front());
	EXPECT_EQ("Pablo", nuevoMetadato.usuariosHabilitados.back());
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaPoderAgregarPermisoYAgregarloAlMetadato) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	manejador->crearUsuario("pepe");
	manejador->agregarPermiso("juan",filepath,"pepe");
	string metadatoActualizado = manejador->consultarMetadatosArchivo("pepe",filepath);
	ParserJson parser;
	MetadatoArchivo asd = parser.deserializarMetadatoArchivo(metadatoActualizado);
	EXPECT_EQ("pepe", asd.usuariosHabilitados.back());
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaPoderDescargarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	string pathCompleto = manejador->descargarArchivo("juan", filepath);

	char homeDirectory[1024];
	getcwd(homeDirectory, sizeof(homeDirectory));
	string pathAComparar(homeDirectory);
	pathAComparar += "/" + pathFS + "/" + filepath;

	EXPECT_EQ(pathAComparar,pathCompleto);
}

// No se cuanto sentido tiene este test
TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosNoDeberiaPoderBorrarCarpetaConArchivosYCarpetas) {
	string filepath = "pablo/como estas/bien/saludo.txt";
	string filepath2 = "pablo/como estas/bien/juan";
	string filepath3 = "pablo/como estas/bien/vos?/juan";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->crearCarpetaSegura("pablo","pablo/como estas/bien/vos?");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, jsonArchOK);
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, jsonArchOK);
	manejador->agregarPermiso("pablo", filepath, "juan");
	manejador->agregarPermiso("pablo", filepath2, "juan");
	manejador->agregarPermiso("pablo", filepath3, "juan");
	EXPECT_FALSE( manejador->eliminar("juan","pablo/como estas") );
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaPoderActualizarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	ASSERT_TRUE(manejador->actualizarArchivo("juan", filepath, "hola juan", 9));

	string pathCompleto = pathFS + "/" + filepath;
	ifstream archivo(pathCompleto.c_str());
	string texto;
	ASSERT_TRUE(archivo.is_open());
	while ( getline(archivo,texto) ) {
		EXPECT_EQ(texto, "hola juan");
	}
	archivo.close();
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaSerElUltimoQueModifico) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	ASSERT_TRUE(manejador->actualizarArchivo("juan", filepath, "hola juan", 9));

	string jsonMetadatos = manejador->consultarMetadatosArchivo("juan", filepath);
	MetadatoArchivo metadato = ParserJson().deserializarMetadatoArchivo(jsonMetadatos);

	EXPECT_EQ("juan", metadato.usuarioUltimaModificacion);
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderConsultarMetadatos) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	EXPECT_EQ(manejador->consultarMetadatosArchivo("juan", filepath), "");
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderActualizarMetadatos) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	EXPECT_FALSE(manejador->actualizarMetadatos("juan", filepath, jsonArchOK));
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderActualizarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	EXPECT_FALSE(manejador->actualizarArchivo("juan", filepath, "hola juan", 9));
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderDescargarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	EXPECT_EQ(manejador->descargarArchivo("juan", filepath), "");
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderEliminarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK);
	EXPECT_FALSE(manejador->eliminar("juan", filepath));
}

// TESTS RESTAURACION

TEST_F(ManejadorArchivosYMetadatosTest, deberiaRestaurarArchivoPropioBorrado) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/#trash/subcarpeta#archivo.txt#0";

	ASSERT_TRUE(manejador->crearUsuario(propietario));
	ASSERT_TRUE(manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK));
	ASSERT_TRUE(manejador->eliminar(propietario, path));

	EXPECT_TRUE(manejador->restaurar(propietario, pathEnPapelera));
	EXPECT_NE("", manejador->consultarMetadatosArchivo(propietario, path));	// Los metadatos existen (no son iguales por la fecha y los permisos)
	EXPECT_NE("", manejador->descargarArchivo(propietario, path));	// El archivo existe (da el path)
	EXPECT_EQ("", manejador->consultarMetadatosArchivo(propietario, pathEnPapelera));
	EXPECT_EQ("", manejador->descargarArchivo(propietario, pathEnPapelera));
}

TEST_F(ManejadorArchivosYMetadatosTest, noDeberiaRestaurarArchivoPropioNuncaBorrado) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/#trash/subcarpeta#archivo.txt#0";

	manejador->crearUsuario(propietario);
	manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK);

	EXPECT_FALSE(manejador->restaurar(propietario, pathEnPapelera));
}

TEST_F(ManejadorArchivosYMetadatosTest, noDeberiaRestaurarArchivoPropioBorradoPeroVueltoACrear) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/#trash/subcarpeta#archivo.txt#0";

	manejador->crearUsuario(propietario);
	manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK);
	manejador->eliminar(propietario, path);
	manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK);

	EXPECT_FALSE(manejador->restaurar(propietario, pathEnPapelera));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaRestaurarUnaVezArchivoBorradoDosVeces) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/#trash/subcarpeta#archivo.txt#0";

	manejador->crearUsuario(propietario);
	for (int i = 0; i < 2; i++) {
		ASSERT_TRUE(manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK));
		ASSERT_TRUE(manejador->eliminar(propietario, path));
	}

	EXPECT_TRUE(manejador->restaurar(propietario, pathEnPapelera));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaEliminarDefinitivamenteArchivoAlBorrarloDeLaPapelera) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/#trash/subcarpeta#archivo.txt#0";

	manejador->crearUsuario(propietario);
	manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK);
	manejador->eliminar(propietario, path);

	ASSERT_TRUE(manejador->eliminar(propietario, pathEnPapelera)); // lo borra definitivamente
	EXPECT_EQ("", manejador->descargarArchivo(propietario, pathEnPapelera));
	EXPECT_EQ("", manejador->consultarMetadatosArchivo(propietario, pathEnPapelera));
}
