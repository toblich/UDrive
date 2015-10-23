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
		if (i==0) 		EXPECT_EQ("pablo", (*itDir));
		else if (i==1) 	EXPECT_EQ("hola", (*itDir));
		else if (i==2) 	EXPECT_EQ("como", (*itDir));
		else if (i==3) 	EXPECT_EQ("estas", (*itDir));
	}
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaParsearBienPathConEspacios) {
	std::string path = "pablo/hola/como estas";
	vector<string> directorios = ParserURI::parsear(path, '/');
	vector<string>::iterator itDir = directorios.begin();
	ASSERT_TRUE(directorios.size() == 3);
	int i = 0;
	for ( ; itDir != directorios.end() ; itDir++, i++){
		if (i==0) 		EXPECT_EQ("pablo", (*itDir));
		else if (i==1) 	EXPECT_EQ("hola", (*itDir));
		else if (i==2) 	EXPECT_EQ("como estas", (*itDir));
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
	string path = "pablo/como" + RESERVED_STR + "estas/bien";
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
	string trash = pathFS + "/" + usuario + "/" + TRASH;
	EXPECT_TRUE(stat(trash.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaCrearCarpetaFotosAlCrearUsuario) {
	struct stat sb;
	string usuario = "pablo";
	ASSERT_TRUE(manejador->crearUsuario(usuario));
	string fotos = pathFS + "/" + FOTOS;
	EXPECT_TRUE(stat(fotos.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaCrearFotoDePerfilAlCrearUsuario) {
	struct stat buffer;
	string usuario = "pablo";
	ASSERT_TRUE(manejador->crearUsuario(usuario));
	string fotoDePerfil = pathFS + "/" + FOTOS + "/" + "pablo.jpg";
	EXPECT_TRUE(stat (fotoDePerfil.c_str(), &buffer) == 0);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderActualizarFotoDePerfil) {
	struct stat buffer;
	string usuario = "pablo";
	ASSERT_TRUE(manejador->crearUsuario(usuario));

	ASSERT_TRUE( manejador->actualizarFotoPerfil(FOTOS + "/pablo.jpg", FOTOS + "/pablo.txt", "hola pablo", 10) );

	string fotoDePerfil = pathFS + "/" + FOTOS + "/" + "pablo.txt";
	EXPECT_TRUE(stat (fotoDePerfil.c_str(), &buffer) == 0);

	ifstream archivo(fotoDePerfil.c_str());
	string texto;
	ASSERT_TRUE(archivo.is_open());
	while ( getline(archivo,texto) ) {
		EXPECT_EQ(texto, "hola pablo");
	}
	archivo.close();
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaEliminarArchivoViejoAlActualizarFotoDePerfil) {
	struct stat buffer;
	string usuario = "pablo";
	ASSERT_TRUE(manejador->crearUsuario(usuario));

	ASSERT_TRUE( manejador->actualizarFotoPerfil(FOTOS + "/pablo.jpg", FOTOS + "/pablo.txt", "hola pablo", 10) );

	string fotoDePerfil = pathFS + "/" + FOTOS + "/" + "pablo.jpg";
	EXPECT_FALSE(stat (fotoDePerfil.c_str(), &buffer) == 0);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaActualizarFotoDePerfilConMismoNombreDeArchivo) {
	struct stat buffer;
	string usuario = "pablo";
	ASSERT_TRUE(manejador->crearUsuario(usuario));

	ASSERT_TRUE( manejador->actualizarFotoPerfil(FOTOS + "/pablo.jpg", FOTOS + "/pablo.txt", "hola pablo", 10) );

	ASSERT_FALSE( manejador->actualizarFotoPerfil(FOTOS + "/pablo.txt", FOTOS + "/pablo.txt", "hola panch", 10) );

	string fotoDePerfil = pathFS + "/" + FOTOS + "/" + "pablo.txt";
	EXPECT_TRUE(stat (fotoDePerfil.c_str(), &buffer) == 0);

	ifstream archivo(fotoDePerfil.c_str());
	string texto;
	ASSERT_TRUE(archivo.is_open());
	while ( getline(archivo,texto) ) {
		EXPECT_EQ(texto, "hola panch");
	}
	archivo.close();
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaSubirBienArchivoDeTexto) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "", 2048);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	string filepathCompleto = pathFS + "/" + filepath;
	manejador->eliminarArchivo("pablo", filepath);
	string filepathCompletoTrash = pathFS + "/pablo/" + TRASH + "/archivos" + RESERVED_STR + "saludo.txt" + RESERVED_STR + "0";
	struct stat buffer;
	EXPECT_FALSE(stat (filepathCompleto.c_str(), &buffer) == 0); //No existe en el filesystem
	EXPECT_TRUE(stat (filepathCompletoTrash.c_str(), &buffer) == 0); //Si existe en el trash
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderConsultarMetadatosDeArchivo) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato", 2048);
	string metadato = manejador->consultarMetadatosArchivo("pablo", filepath);
	EXPECT_EQ("un metadato", metadato);
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderActualizarMetadatosDeArchivo) {
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	std::string nuevoJson = "{\n"
			"\t\"etiquetas\" : [ \"pepe\" ],\n"
			"\t\"extension\" : \"txt\",\n"
			"\t\"fecha ultima modificacion\" : \"10/09/2015\",\n"
			"\t\"nombre\" : \"saludo\",\n"
			"\t\"propietario\" : \"pablo\",\n"
			"\t\"usuario ultima modificacion\" : \"Juan\",\n"
			"\t\"usuarios\" : [ \"Pancheitor\", \"Juan\", \"Pepe\", \"Santi\", \"Pablo\" ]\n"
			"}";
	manejador->actualizarMetadatos("pablo",filepath, nuevoJson);
	string jsonNuevoMetadato = manejador->consultarMetadatosArchivo("pablo", filepath);
	MetadatoArchivo nuevoMetadato = ParserJson::deserializarMetadatoArchivo(jsonNuevoMetadato);

	EXPECT_EQ("txt", nuevoMetadato.extension);
	EXPECT_EQ("10/09/2015", nuevoMetadato.fechaUltimaModificacion);
	EXPECT_EQ("saludo", nuevoMetadato.nombre);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	std::string nuevoJson = "{\n"
			"\t\"etiquetas\" : [ \"pepe\" ],\n"
			"\t\"extension\" : \"txt\",\n"
			"\t\"fecha ultima modificacion\" : \"10/09/2015\",\n"
			"\t\"nombre\" : \"saludo\",\n"
			"\t\"propietario\" : \"pablo\",\n"
			"\t\"usuario ultima modificacion\" : \"Juan\",\n"
			"\t\"usuarios\" : []\n"
			"}";
	manejador->actualizarMetadatos("pablo",filepath, nuevoJson);
	string jsonNuevoMetadato = manejador->consultarMetadatosArchivo("pablo", filepath);
	MetadatoArchivo nuevoMetadato = ParserJson::deserializarMetadatoArchivo(jsonNuevoMetadato);

	EXPECT_EQ("pablo", nuevoMetadato.usuariosHabilitados.back());
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaPoderAgregarPermisoYAgregarloAlMetadato) {
	//Como todavia no se agregan los permisos propiamente dichos, me fijo que se agreguen al metadato nomas
	string path = "pablo/archivos";
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->crearCarpetaSegura("pablo", path);
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	manejador->agregarPermiso("pablo",filepath,"juan");
	string metadatoActualizado = manejador->consultarMetadatosArchivo("pablo",filepath);
	MetadatoArchivo asd = ParserJson::deserializarMetadatoArchivo(metadatoActualizado);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato", 2048);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, jsonArchOK, 2048);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, jsonArchOK, 2048);
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, jsonArchOK, 2048);
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
	MetadatoArchivo nuevoMetadato = ParserJson::deserializarMetadatoArchivo(jsonNuevoMetadato);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato", 2048);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, "un metadato", 2048);
	manejador->subirArchivo("pablo", filepath2, "hola tobi", 9, "un metadato", 2048);
	manejador->subirArchivo("pablo", filepath3, "hola pancho", 11, "un metadato", 2048);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, jsonArchOK, 2048);
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, jsonArchOK, 2048);
	manejador->eliminar("pablo", "pablo/como estas");

	string pathCarpetaEliminada = pathFS + "/pablo/como estas";
	struct stat sb;
	EXPECT_FALSE(stat(pathCarpetaEliminada.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaObtenerBienLaEstructuraDeLasCarpetas) {
	string filepath1 = "pablo/como estas/bien/saludo.txt";
	MetadatoArchivo metadato1;
	metadato1.nombre = "saludo";
	metadato1.extension = "txt";
	string metadatos1 = ParserJson::serializarMetadatoArchivo(metadato1);

	string filepath2 = "pablo/como estas/bien/juan";
	MetadatoArchivo metadato2;
	metadato2.nombre = "juan";
	metadato2.extension = "none";
	string metadatos2 = ParserJson::serializarMetadatoArchivo(metadato2);

	string filepath3 = "pablo/como estas/bien/pepe.hola";
	MetadatoArchivo metadato3;
	metadato3.nombre = "pepe";
	metadato3.extension = "hola";
	string metadatos3 = ParserJson::serializarMetadatoArchivo(metadato3);

	manejador->crearUsuario("pablo");
	string folderpath1 = "pablo/como estas/bien/vos?";
	string folderpath2 = "pablo/como estas/bien/hola";

	manejador->crearCarpetaSegura("pablo", folderpath1);
	manejador->crearCarpetaSegura("pablo", folderpath2);
	manejador->subirArchivo("pablo", filepath1, "hola pablo", 10, metadatos1, 2048);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, metadatos2, 2048);
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, metadatos3, 2048);
	string estructura = manejador->obtenerEstructuraCarpeta("pablo/como estas/bien");

	map<string, string> mapa = ParserJson::deserializarMapa(estructura);

	EXPECT_EQ(mapa.at(filepath1), metadato1.nombre + "." + metadato1.extension);
	EXPECT_EQ(mapa.at(filepath2), metadato2.nombre); // no tiene extension
	EXPECT_EQ(mapa.at(filepath3), metadato3.nombre + "." + metadato3.extension);
	EXPECT_EQ(mapa.at(folderpath1), "vos?." + FOLDER);
	EXPECT_EQ(mapa.at(folderpath2), "hola." + FOLDER);
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
	EXPECT_TRUE(manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048));
	EXPECT_FALSE(manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaObtenerEstructuraCorrectaDePermisos) {
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchOK);
	MetadatoArchivo metadato2 = ParserJson::deserializarMetadatoArchivo(jsonArchOK);

	metadato2.nombre = "saludo2";
	string jsonArchOK2 = ParserJson::serializarMetadatoArchivo(metadato2);

	metadato.usuariosHabilitados.push_back("juan");
	metadato2.usuariosHabilitados.push_back("juan");
	string jsonConJuanHabilitado = ParserJson::serializarMetadatoArchivo(metadato);
	string jsonConJuanHabilitado2 = ParserJson::serializarMetadatoArchivo(metadato2);

	string filepath1 = "pablo/archivos/saludo.txt";
	string filepath2 = "pablo/archivos/saludo2.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath1, "hola pablo", 10, jsonArchOK, 2048);
	manejador->actualizarMetadatos("pablo", filepath1, jsonConJuanHabilitado);
	manejador->subirArchivo("pablo", filepath2, "hola panch", 10, jsonArchOK2, 2048);
	manejador->actualizarMetadatos("pablo", filepath2, jsonConJuanHabilitado2);

	string jsonEstructura = manejador->obtenerEstructuraCarpeta(RESERVED_STR + "permisos/juan");

	map<string, string> mapa = ParserJson::deserializarMapa(jsonEstructura);

	EXPECT_EQ(mapa.at(filepath1), "saludo.txt");
	EXPECT_EQ(mapa.at(filepath2), "saludo2.txt");
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaNoPermitirSubirArchivoPorExcesoDeCuota) {
	manejador->crearUsuario("pablo");
	string path = "pablo/como estas/bien/saludo.txt";
	manejador->subirArchivo("pablo", path, "hola pablo", 10, jsonArchOK, 0);
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
	manejador->subirArchivo("pablo", path, "hola pablo", 10, jsonArchOK, 2048);
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
	string filename = "/archivos" + RESERVED_STR + "saludo.txt" + RESERVED_STR + "0";
	string filepathCompletoTrashProp = pathFS + "/pablo/"+ TRASH + "/" + filename;
	string filepathCompletoTrashJuan = pathFS + "/juan/" + TRASH + "/" + filename;
	struct stat buffer;
	EXPECT_FALSE(stat (filepathCompleto.c_str(), &buffer) == 0); //No existe en el filesystem
	EXPECT_TRUE(stat (filepathCompletoTrashProp.c_str(), &buffer) == 0); //Si existe en el trash del propietario
	EXPECT_FALSE(stat (filepathCompletoTrashJuan.c_str(), &buffer) == 0); //No existe en el trash del usuario con permisos
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaPoderConsultarMetadatosDeArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	string jsonMetadato = manejador->consultarMetadatosArchivo("juan", filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadato);
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
			"\t\"extension\" : \"txt\",\n"
			"\t\"fecha ultima modificacion\" : \"10/09/2015\",\n"
			"\t\"nombre\" : \"saludo\",\n"
			"\t\"propietario\" : \"pablo\",\n"
			"\t\"usuario ultima modificacion\" : \"juan\",\n"
			"\t\"usuarios\" : [ \"pablo\", \"juan\", \"Pepe\", \"Santi\", \"Pablo\" ]\n"
			"}";
	manejador->actualizarMetadatos("juan",filepath, nuevoJson);
	string jsonNuevoMetadato = manejador->consultarMetadatosArchivo("juan", filepath);
	MetadatoArchivo nuevoMetadato = ParserJson::deserializarMetadatoArchivo(jsonNuevoMetadato);

	EXPECT_EQ("txt", nuevoMetadato.extension);
	EXPECT_EQ("10/09/2015", nuevoMetadato.fechaUltimaModificacion);
	EXPECT_EQ("saludo", nuevoMetadato.nombre);
	EXPECT_EQ("pablo", nuevoMetadato.propietario);
	EXPECT_EQ("juan", nuevoMetadato.usuarioUltimaModificacion);
	EXPECT_EQ("pepe", nuevoMetadato.etiquetas.front());
	EXPECT_EQ("Pablo", nuevoMetadato.usuariosHabilitados.back());
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaPoderAgregarPermisoYAgregarloAlMetadato) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	manejador->crearUsuario("pepe");
	manejador->agregarPermiso("juan",filepath,"pepe");
	string metadatoActualizado = manejador->consultarMetadatosArchivo("pepe",filepath);
	MetadatoArchivo asd = ParserJson::deserializarMetadatoArchivo(metadatoActualizado);
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
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	manejador->subirArchivo("pablo", filepath2, "hola pablo", 10, jsonArchOK, 2048);
	manejador->subirArchivo("pablo", filepath3, "hola pablo", 10, jsonArchOK, 2048);
	manejador->agregarPermiso("pablo", filepath, "juan");
	manejador->agregarPermiso("pablo", filepath2, "juan");
	manejador->agregarPermiso("pablo", filepath3, "juan");
	EXPECT_FALSE( manejador->eliminar("juan","pablo/como estas") );
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioConPermisosDeberiaPoderActualizarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	ASSERT_TRUE(manejador->actualizarArchivo("juan", filepath, "hola juan", 9, 2048));

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
	ASSERT_TRUE(manejador->actualizarArchivo("juan", filepath, "hola juan", 9, 2048));

	string jsonMetadatos = manejador->consultarMetadatosArchivo("juan", filepath);
	MetadatoArchivo metadato = ParserJson().deserializarMetadatoArchivo(jsonMetadatos);

	EXPECT_EQ("juan", metadato.usuarioUltimaModificacion);
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderConsultarMetadatos) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	EXPECT_EQ(manejador->consultarMetadatosArchivo("juan", filepath), "");
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderActualizarMetadatos) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	EXPECT_FALSE(manejador->actualizarMetadatos("juan", filepath, jsonArchOK));
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderActualizarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	EXPECT_FALSE(manejador->actualizarArchivo("juan", filepath, "hola juan", 9, 2048));
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderDescargarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	EXPECT_EQ(manejador->descargarArchivo("juan", filepath), "");
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioSinPermisosNoDeberiaPoderEliminarArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	manejador->crearUsuario("pablo");
	manejador->crearUsuario("juan");
	manejador->subirArchivo("pablo", filepath, "hola pablo", 10, jsonArchOK, 2048);
	EXPECT_FALSE(manejador->eliminar("juan", filepath));
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioDeberiaPoderAgregarPermisosAlActualizarMetadatos) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	manejador->crearUsuario("pepe");
	string jsonMetadato = manejador->consultarMetadatosArchivo("juan", filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadato);
	metadato.usuariosHabilitados.push_back("pepe");
	string jsonMetadatoModificado = ParserJson::serializarMetadatoArchivo(metadato);
	ASSERT_TRUE(manejador->actualizarMetadatos("juan", filepath, jsonMetadatoModificado));

	string jsonPepe = manejador->consultarMetadatosArchivo("pepe", filepath);
	EXPECT_NE("", jsonPepe);
	MetadatoArchivo metadatoPepe = ParserJson::deserializarMetadatoArchivo(jsonPepe);
	EXPECT_EQ("txt", metadatoPepe.extension);
	EXPECT_EQ("saludo", metadatoPepe.nombre);
	EXPECT_EQ("pablo", metadatoPepe.propietario);
	EXPECT_EQ("pepe", metadatoPepe.usuariosHabilitados.back());
	EXPECT_EQ("pablo", metadatoPepe.usuariosHabilitados.front());
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioDeberiaPoderEliminarPermisosAlActualizarMetadatos) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	manejador->crearUsuario("pepe");
	string jsonMetadato = manejador->consultarMetadatosArchivo("juan", filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadato);
	metadato.usuariosHabilitados.push_back("pepe");
	string jsonMetadatoModificado = ParserJson::serializarMetadatoArchivo(metadato);
	ASSERT_TRUE(manejador->actualizarMetadatos("juan", filepath, jsonMetadatoModificado));

	string jsonPepe = manejador->consultarMetadatosArchivo("pepe", filepath);
	EXPECT_NE("", jsonPepe);
	MetadatoArchivo metadatoPepe = ParserJson::deserializarMetadatoArchivo(jsonPepe);
	metadatoPepe.usuariosHabilitados.remove("juan");
	string jsonPepeModif = ParserJson::serializarMetadatoArchivo(metadatoPepe);
	manejador->actualizarMetadatos("pepe", filepath, jsonPepeModif);

	string jsonVacio = manejador->consultarMetadatosArchivo("juan", filepath);

	EXPECT_EQ("", jsonVacio);
}

TEST_F(ManejadorArchivosYMetadatosTest, usuarioDeberiaMantenerPermisosAlActualizarMetadatos) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	string jsonMetadato = manejador->consultarMetadatosArchivo("juan", filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonMetadato);
	list<string> listaVacia;
	metadato.usuariosHabilitados = listaVacia;
	string jsonMetadatoModificado = ParserJson::serializarMetadatoArchivo(metadato);
	ASSERT_TRUE(manejador->actualizarMetadatos("juan", filepath, jsonMetadatoModificado));

	string jsonJuan = manejador->consultarMetadatosArchivo("juan", filepath);
	EXPECT_NE("", jsonJuan);
	string jsonPablo = manejador->consultarMetadatosArchivo("pablo", filepath);
	EXPECT_NE("", jsonPablo);
}

// TESTS RESTAURACION

TEST_F(ManejadorArchivosYMetadatosTest, deberiaRestaurarArchivoPropioBorrado) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/" + TRASH + "/subcarpeta" + RESERVED_STR + "archivo.txt" + RESERVED_STR + "0";

	ASSERT_TRUE(manejador->crearUsuario(propietario));
	ASSERT_TRUE(manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK, 2048));
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
	string pathEnPapelera = propietario + "/" + TRASH + "/subcarpeta" + RESERVED_STR + "archivo.txt" + RESERVED_STR + "0";

	manejador->crearUsuario(propietario);
	manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK, 2048);

	EXPECT_FALSE(manejador->restaurar(propietario, pathEnPapelera));
}

TEST_F(ManejadorArchivosYMetadatosTest, noDeberiaRestaurarArchivoPropioBorradoPeroVueltoACrear) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/" + TRASH + "/subcarpeta" + RESERVED_STR + "archivo.txt" + RESERVED_STR + "0";

	manejador->crearUsuario(propietario);
	manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK, 2048);
	manejador->eliminar(propietario, path);
	manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK, 2048);

	EXPECT_FALSE(manejador->restaurar(propietario, pathEnPapelera));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaRestaurarUnaVezArchivoBorradoDosVeces) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/" + TRASH + "/subcarpeta" + RESERVED_STR + "archivo.txt" + RESERVED_STR + "0";

	manejador->crearUsuario(propietario);
	for (int i = 0; i < 2; i++) {
		ASSERT_TRUE(manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK, 2048));
		ASSERT_TRUE(manejador->eliminar(propietario, path));
	}

	EXPECT_TRUE(manejador->restaurar(propietario, pathEnPapelera));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaEliminarDefinitivamenteArchivoAlBorrarloDeLaPapelera) {
	string contenido = "contenido";
	string path = propietario + "/subcarpeta/archivo.txt";
	string pathEnPapelera = propietario + "/" + TRASH + "/subcarpeta" + RESERVED_STR + "archivo.txt" + RESERVED_STR + "0";

	manejador->crearUsuario(propietario);
	manejador->subirArchivo(propietario, path, contenido.c_str(), contenido.size(), jsonArchOK, 2048);
	manejador->eliminar(propietario, path);

	ASSERT_TRUE(manejador->eliminar(propietario, pathEnPapelera)); // lo borra definitivamente
	EXPECT_EQ("", manejador->descargarArchivo(propietario, pathEnPapelera));
	EXPECT_EQ("", manejador->consultarMetadatosArchivo(propietario, pathEnPapelera));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaRenombrarArchivoAlActualizarMetadatosConOtroNombre) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);	// sube el archivo con metadatos jsonArchOK
	EXPECT_TRUE(validador->existeArchivo(pathFS + "/" + filepath));
	EXPECT_TRUE(db->contains(filepath));

	MetadatoArchivo metadatos = ParserJson::deserializarMetadatoArchivo(jsonArchOK);
	metadatos.nombre = "nuevoNombre";
	metadatos.extension = "nuevaExtension";
	string nuevoFilepath = "pablo/archivos/nuevoNombre.nuevaExtension";
	string jsonNuevo = ParserJson::serializarMetadatoArchivo(metadatos);
	manejador->actualizarMetadatos("pablo", filepath, jsonNuevo);

	EXPECT_FALSE(validador->existeArchivo(pathFS + "/" + filepath));
	EXPECT_FALSE(db->contains(filepath));
	EXPECT_TRUE(validador->existeArchivo(pathFS + "/" + nuevoFilepath));
	EXPECT_TRUE(db->contains(nuevoFilepath));
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaCambiarElPathDeLosArchivosConPermisoDeUnUsuarioAlRenombrarEseArchivo) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);	// sube el archivo con metadatos jsonArchOK, y juan tiene permiso sobre el arch de pablo

	MetadatoArchivo metadatos = ParserJson::deserializarMetadatoArchivo(jsonArchOK);
	metadatos.nombre = "nuevoNombre";
	metadatos.extension = "nuevaExtension";
	metadatos.usuariosHabilitados.clear();
	string nuevoFilepath = "pablo/archivos/nuevoNombre.nuevaExtension";
	string jsonNuevo = ParserJson::serializarMetadatoArchivo(metadatos);
	manejador->actualizarMetadatos("pablo", filepath, jsonNuevo);	// lo renombra

	string compartidosConJuan = manejador->obtenerEstructuraCarpeta(PERMISOS + "/juan");
	EXPECT_EQ(string::npos, compartidosConJuan.find(filepath));		// no tiene el archivo con el nombre viejo
	EXPECT_NE(string::npos, compartidosConJuan.find(nuevoFilepath));// si tiene el archivo con el nombre nuevo
}

// TESTS DE BUSQUEDAS

void subirArchivoYAgregarPermiso(ManejadorArchivosYMetadatos* manejador, string propietario, string filepath, string nombreArch, string extensionArch,
		string permiso, string texto, list<string> etiquetas){
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchOK);

	MetadatoArchivo metadatoNuevo = metadato;
	metadatoNuevo.extension = extensionArch;
	metadatoNuevo.nombre = nombreArch;
	metadatoNuevo.propietario = propietario;
	metadatoNuevo.etiquetas = etiquetas;
	string jsonArchOKNuevo = ParserJson::serializarMetadatoArchivo(metadatoNuevo);
	manejador->subirArchivo(propietario, filepath, texto.c_str(), texto.size(), jsonArchOKNuevo, 2048);
	manejador->agregarPermiso(propietario, filepath, permiso);
}

string myStrings[] = {"hola", "true", "10"};
list<string> ETIQUETAS (&myStrings[0], &myStrings[2]);

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBuscarBienPorExtensionEnArchivosConPermisos) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchOK);

	string filepath2 = "pablo/archivos/saludo2.png";
	subirArchivoYAgregarPermiso(manejador, "pablo", filepath2, "saludo2", "png", "juan", "hola panch", ETIQUETAS);

	string filepath3 = "pablo/archivos/saludo3.txt";
	subirArchivoYAgregarPermiso(manejador, "pablo", filepath3, "saludo3", "txt", "juan", "hola tobis", ETIQUETAS);

	string jsonEstructura = manejador->buscarPorExtension("juan", "txt");
	map<string, string> busquedaExtension = ParserJson::deserializarMapa(jsonEstructura);

	EXPECT_EQ(busquedaExtension.at(filepath), metadato.nombre + "." + metadato.extension);
	EXPECT_FALSE(busquedaExtension.count(filepath2) > 0); // no existe en el mapa
	EXPECT_EQ(busquedaExtension.at(filepath3), "saludo3.txt");
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBuscarBienPorNombreEnArchivosConPermisos) {
	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchOK);

	string filepath2 = "pablo/archivos/saludo2.png";
	subirArchivoYAgregarPermiso(manejador, "pablo", filepath2, "saludo2", "png", "juan", "hola panch", ETIQUETAS);

	string filepath3 = "pablo/archivos/saludo3.txt";
	subirArchivoYAgregarPermiso(manejador, "pablo", filepath3, "saludo3", "txt", "juan", "hola tobis", ETIQUETAS);

	string filepath4 = "pablo/archivos/hola.txt";
	subirArchivoYAgregarPermiso(manejador, "pablo", filepath4, "hola", "txt", "juan", "hola santi", ETIQUETAS);

	string jsonEstructura = manejador->buscarPorNombre("juan", "saludo");
	map<string, string> busquedaNombre = ParserJson::deserializarMapa(jsonEstructura);

	EXPECT_EQ(busquedaNombre.at(filepath), metadato.nombre + "." + metadato.extension);
	EXPECT_EQ(busquedaNombre.at(filepath2), "saludo2.png");
	EXPECT_EQ(busquedaNombre.at(filepath3), "saludo3.txt");
	EXPECT_FALSE(busquedaNombre.count(filepath4) > 0); // no existe en el mapa
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBuscarBienPorPropietarioEnArchivosConPermisos) {
	manejador->crearUsuario("pepe");

	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchOK);

	string filepath2 = "pablo/archivos/saludo2.png";
	subirArchivoYAgregarPermiso(manejador, "pablo", filepath2, "saludo2", "png", "juan", "hola panch", ETIQUETAS);

	string filepath3 = "pepe/archivos/saludo3.txt";
	subirArchivoYAgregarPermiso(manejador, "pepe", filepath3, "saludo3", "txt", "juan", "hola tobis", ETIQUETAS);

	string jsonEstructura = manejador->buscarPorPropietario("juan", "pablo");
	map<string, string> busquedaNombre = ParserJson::deserializarMapa(jsonEstructura);

	EXPECT_EQ(busquedaNombre.at(filepath), metadato.nombre + "." + metadato.extension);
	EXPECT_EQ(busquedaNombre.at(filepath2), "saludo2.png");
	EXPECT_FALSE(busquedaNombre.count(filepath3) > 0); // no existe en el mapa
}

TEST_F(ManejadorArchivosYMetadatosTest, deberiaBuscarBienPorEtiquetasEnArchivosConPermisos) {
	manejador->crearUsuario("pepe");

	string filepath = "pablo/archivos/saludo.txt";
	inic(manejador, filepath);
	MetadatoArchivo metadato = ParserJson::deserializarMetadatoArchivo(jsonArchOK);

	string myStrings2[] = {"chau", "false", "01"};
	list<string> etiquetas2 (&myStrings2[0], &myStrings2[2]);
	string filepath2 = "pablo/archivos/saludo2.png";
	subirArchivoYAgregarPermiso(manejador, "pablo", filepath2, "saludo2", "png", "juan", "hola panch", etiquetas2);

	string myStrings3[] = {"pepe", "perro", "chauchi", "juan"};
	list<string> etiquetas3 (&myStrings3[0], &myStrings3[3]);
	string filepath3 = "pepe/archivos/saludo3.txt";
	subirArchivoYAgregarPermiso(manejador, "pepe", filepath3, "saludo3", "txt", "juan", "hola tobis", etiquetas3);

	string myStrings4[] = {"chauchi", "hola", "tobi"};
	list<string> etiquetas4 (&myStrings4[0], &myStrings4[2]);
	string filepath4 = "pablo/archivos/saludo4.txt";
	subirArchivoYAgregarPermiso(manejador, "pablo", filepath4, "saludo4", "txt", "juan", "hola santi", etiquetas4);

	string jsonEstructura = manejador->buscarPorEtiqueta("juan", "chauchi");
	Logger::logDebug(jsonEstructura);
	map<string, string> busquedaEtiqueta = ParserJson::deserializarMapa(jsonEstructura);

	EXPECT_FALSE(busquedaEtiqueta.count(filepath) > 0); // no existe en el mapa
	EXPECT_FALSE(busquedaEtiqueta.count(filepath2) > 0); // no existe en el mapa
	EXPECT_EQ(busquedaEtiqueta.at(filepath3), "saludo3.txt");
	EXPECT_EQ(busquedaEtiqueta.at(filepath4), "saludo4.txt");

	string jsonEstructura2 = manejador->buscarPorEtiqueta("juan", "hola");
	map<string, string> busquedaEtiqueta2 = ParserJson::deserializarMapa(jsonEstructura2);

	EXPECT_EQ(busquedaEtiqueta2.at(filepath), "saludo.txt");
	EXPECT_FALSE(busquedaEtiqueta2.count(filepath2) > 0); // no existe en el mapa
	EXPECT_FALSE(busquedaEtiqueta2.count(filepath3) > 0); // no existe en el mapa
	EXPECT_EQ(busquedaEtiqueta2.at(filepath4), "saludo4.txt");
}
