import unittest
import requests
import json
from subprocess import Popen
from time import sleep
from shutil import rmtree
from os.path import exists
from ast import literal_eval


def definirConstantesGlobales():

	global RESERVED_STR
	RESERVED_STR = "!"

	global FIRST
	FIRST = "1"

	global FOLDER_TYPE
	FOLDER_TYPE = RESERVED_STR + "folder"

	global TRASH_TYPE
	TRASH_TYPE = RESERVED_STR + "trash/"

	global FOTOS_TYPE
	FOTOS_TYPE = RESERVED_STR + "fotos/"

	global BASE 
	BASE = "http://localhost:8080/"
	
	global PROFILE
	PROFILE = BASE + "profile/"
	
	global SESSION 
	SESSION = BASE + "session/"
	
	global FILE
	FILE = BASE + "file/"

	global METADATA
	METADATA = BASE + "metadata/"

	global FOLDER
	FOLDER = BASE + "folder/"

	global PERFIL 
	PERFIL = '{ "nombre": "Tobias", "email": "santi@pancho.pablo"}' # TODO: completar
	
	global USER_SIMPLE 
	USER_SIMPLE = {"user":"tobi", "pass" : "masdeocholetras", "profile": PERFIL}
	
	global LOG_USER_SIMPLE 
	LOG_USER_SIMPLE = USER_SIMPLE.copy()
	del LOG_USER_SIMPLE["profile"]

	global BAD_REQUEST
	BAD_REQUEST = 400

	global NOT_FOUND
	NOT_FOUND = 404

	global UNAUTHORIZED
	UNAUTHORIZED = 401

	global RESOURCE_CREATED
	RESOURCE_CREATED = 201

	global SUCCESS
	SUCCESS = 200

	global UNSUPPORTED_METHOD
	UNSUPPORTED_METHOD = 405


def registrarYLoguear(username, password, profile):
	requests.post(PROFILE, data={"user": username, "pass": password, "profile": profile})
	r = requests.post(SESSION, data={"user": username, "pass": password})
	token = r.json().get("token")
	return str(token)

def registrarYLoguearUser(dictUser):
	return registrarYLoguear(dictUser["user"], dictUser["pass"], dictUser["profile"])

def logTest(testname):
	with open('files/log.txt', 'a') as file:
		file.write("TEST: " + testname + "\n")



class ServerTest(unittest.TestCase):
	def setUp(self):
		print
		if exists("FileSystem"):
			rmtree("FileSystem")
		self.serverProcess = Popen(["./udrive", "&"])
		sleep(0.05)


	def tearDown(self):
		self.serverProcess.terminate()


	def test_registroYLogueoValido(self):
		logTest("Registro y Logueo Valido")
		r = requests.post(PROFILE, data=USER_SIMPLE)
		self.assertEquals(r.status_code, RESOURCE_CREATED)
		s = requests.post(SESSION, LOG_USER_SIMPLE)
		self.assertEquals(s.status_code, RESOURCE_CREATED)


	def test_registrarUsuarioConPasswordInvalida(self):
		logTest("Registrar Usuario con Password Invalida")
		userCroto = USER_SIMPLE.copy()
		userCroto['pass'] = "corta"
		r = requests.post(PROFILE, data=userCroto)
		self.assertEquals(r.status_code, BAD_REQUEST)


	def test_logueoInvalido(self):
		logTest("Logueo Invalido")
		requests.post(PROFILE, data=USER_SIMPLE) #registro
		log_erroneo = LOG_USER_SIMPLE.copy()
		log_erroneo["pass"] = "equivocada"
		r = requests.post(SESSION, log_erroneo)
		self.assertEquals(r.status_code, BAD_REQUEST)


	def test_loguearYDesloguearMultiplesSesiones(self):
		logTest("Loguear y Desloguear Multiples Sesiones")
		r = requests.post(PROFILE, data=USER_SIMPLE)	# registro
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.post(SESSION, LOG_USER_SIMPLE)		# 1er log
		self.assertEquals(s.status_code, RESOURCE_CREATED)
		s.token = str(s.json().get('token'))

		t = requests.post(SESSION, LOG_USER_SIMPLE)		# 2do log
		self.assertEquals(t.status_code, RESOURCE_CREATED)
		t.token = str(t.json().get('token'))

		self.assertNotEquals(s.token, t.token)	# tokens distintos por ser sesiones distintas
		
		u = requests.delete(SESSION + s.token, data={"user" : USER_SIMPLE["user"]})	# 1er logout
		self.assertEquals(u.status_code, SUCCESS)
		
		v = requests.delete(SESSION + s.token, data={"user" : USER_SIMPLE["user"]})	# 1er logout repetido
		self.assertEquals(v.status_code, NOT_FOUND)
		
		w = requests.delete(SESSION + t.token, data={"user" : USER_SIMPLE["user"]})	# 2do logout
		self.assertEquals(w.status_code, SUCCESS)


	def test_perfilSubidoYActualizadoConFotoDePerfil(self):
		logTest("Perfil subido y actualizado con Foto de perfil")
		username = "hola"
		password = "masdeocholetras"
		token = registrarYLoguear(username, password, PERFIL)
		perfilOriginal = json.loads(PERFIL)
		FOTO = "default.jpg"

		r = requests.get(PROFILE + username, data={"token": token, "user": username})	# obtener perfil
		self.assertEquals(r.status_code, SUCCESS)
		perfilObtenido = r.json().get("perfil")
		self.assertEquals(perfilObtenido.get("nombre"), perfilOriginal.get("nombre"))
		self.assertEquals(perfilObtenido.get("email"), perfilOriginal.get("email"))

		s = requests.put(PROFILE + username, files={"nombre": "otroNombre", "email": "otro@e.mail",
			"token": token, 'picture': open(FOTO, 'rb')}) # actualizar perfil con foto
		self.assertEquals(s.status_code, SUCCESS)
		
		t = requests.get(PROFILE + username, data={"token": token, "user": username})	# obtener nuevo perfil
		self.assertEquals(t.status_code, SUCCESS)
		nuevoPerfilObtenido = t.json().get("perfil")
		self.assertEquals(nuevoPerfilObtenido.get("nombre"), "otroNombre")
		self.assertEquals(nuevoPerfilObtenido.get("email"), "otro@e.mail")

		pathFoto = nuevoPerfilObtenido.get("path foto de perfil") #Veo si es la misma foto que subi
		uriAFoto = PROFILE + pathFoto
		uriRealAFoto = PROFILE + FOTOS_TYPE + username + ".jpg"
		self.assertEquals(uriAFoto, uriRealAFoto)

		u = requests.get(uriAFoto, data={"token": token, "user": username})
		contenido = open(FOTO, 'rb').read()	# compara el archivo bajado con el original
		self.assertEquals(u.content, contenido)


	def test_subirBajarYBorrarArchivoTextoActualizandoUltimaUbicacion(self):
		logTest("Subir, Bajar y Borrar Archivo actualizando Ultima Ubicacion")
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "Makefile"

		uri = FILE + USER_SIMPLE["user"] + "/" + FILENAME
		uriConVersion = uri + RESERVED_STR + FIRST
		r = requests.put(uriConVersion, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"], 
							"latitud": "10.5", "longitud":"20.0"})	# lo sube pasandole la ubicacion para que la actualice
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.get(uriConVersion, data={"user": USER_SIMPLE["user"], "token": token})	# lo baja
		self.assertEquals(s.status_code, SUCCESS)

		contenido = open(FILENAME, 'rb').read()	# compara el archivo bajado con el original
		self.assertEquals(s.content, contenido)

		t = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token}) # lo borra
		self.assertEquals(t.status_code, SUCCESS)

		u = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token}) # trata de borrar otra vez
		self.assertEquals(u.status_code, NOT_FOUND)

		v = requests.get(uriConVersion, data={"user": USER_SIMPLE["user"], "token": token})	# trata de bajarlo
		self.assertEquals(v.status_code, NOT_FOUND)

		w = requests.get(PROFILE + USER_SIMPLE["user"], data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(w.status_code, SUCCESS)
		perfilObtenido = w.json().get("perfil")
		ultimaUbicacion = perfilObtenido.get("ultima ubicacion")
		self.assertEquals(ultimaUbicacion.get("latitud"), 10.5)
		self.assertEquals(ultimaUbicacion.get("longitud"), 20.0)


	def test_obtenerYActualizarMetadatos(self):
		logTest("Obtener y Actualizar Metadatos")
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "../src/db/batch.cpp"

		internalUri = USER_SIMPLE["user"] + FILENAME[2:] # saca los '..'
		requests.put(FILE + internalUri, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# sube archivo

		r = requests.get(METADATA + internalUri, data={"user": USER_SIMPLE["user"], "token": token})	# consulta metadatos
		self.assertEquals(r.status_code, SUCCESS)

		metadata = r.json().get("metadatos")
		self.assertEquals(metadata.get("propietario"), USER_SIMPLE["user"])
		self.assertEquals(metadata.get("nombre"), "batch")
		self.assertEquals(metadata.get("usuario ultima modificacion"), USER_SIMPLE["user"])
		self.assertEquals(metadata.get("extension"), "cpp")
		self.assertEquals(metadata.get("etiquetas"), [])
		self.assertEquals(metadata.get("usuarios"), [USER_SIMPLE["user"]])

		nuevosMetadatos = '{"propietario" : "' +  USER_SIMPLE["user"] + '" , "extension" : "cpp", "nombre" : "batch", "etiquetas" : ["nuevaEtiqueta"], "usuarios" : [], ' \
			+ '"usuario ultima modificacion" : "' + USER_SIMPLE["user"] + '" , "fecha ultima modificacion" : "23/10/2015"}'

		s = requests.put(METADATA + internalUri, data={"token" : token, "user": USER_SIMPLE["user"], "metadatos" : nuevosMetadatos})
		self.assertEquals(s.status_code, SUCCESS)

		t = requests.get(METADATA + internalUri, data={"user": USER_SIMPLE["user"], "token": token})
		self.assertEquals(t.status_code, SUCCESS)
		nuevaMetadata = t.json().get("metadatos")
		self.assertEquals(nuevaMetadata.get("propietario"), USER_SIMPLE["user"])
		self.assertEquals(nuevaMetadata.get("nombre"), "batch")
		self.assertEquals(nuevaMetadata.get("usuario ultima modificacion"), USER_SIMPLE["user"])
		self.assertEquals(nuevaMetadata.get("extension"), "cpp")
		self.assertEquals(nuevaMetadata.get("etiquetas"), ["nuevaEtiqueta"])	# este se actualizo
		self.assertEquals(nuevaMetadata.get("usuarios"), [USER_SIMPLE["user"]])

		u = requests.delete(FILE + internalUri, data={"user": USER_SIMPLE["user"], "token": token}) # lo borra
		self.assertEquals(u.status_code, SUCCESS)


	def test_obtenerEstructuraDeCarpetaYBorrarCarperta(self):
		logTest("Obtener Estructura de carpeta y Borrar carpeta")
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "../src/db/batch.cpp"
		BASE_FOLDER = FOLDER + USER_SIMPLE["user"] + "/src/"

		internalUri = USER_SIMPLE["user"] + FILENAME[2:] # saca los '..'
		requests.put(FILE + internalUri, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# sube archivo

		r = requests.put(BASE_FOLDER + "subcarpeta", data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.get(BASE_FOLDER, data={"token": token, "user": USER_SIMPLE["user"]})	#obtiene la estructura de /CMakeFiles
		self.assertEquals(s.status_code, SUCCESS)

		estructura = literal_eval(s.content)
		estructuraEsperada = {USER_SIMPLE["user"] + "/src/db" : "db." + FOLDER_TYPE, USER_SIMPLE["user"] + "/src/subcarpeta": "subcarpeta." + FOLDER_TYPE}
		expected = {"estructura" : estructuraEsperada }
		self.assertDictEqual(expected, estructura) 

		t = requests.delete(BASE_FOLDER, data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(t.status_code, SUCCESS)

		u = requests.delete(BASE_FOLDER, data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(u.status_code, NOT_FOUND)

		v = requests.delete(BASE_FOLDER, data={"token": 1234567890, "user": USER_SIMPLE["user"]})
		self.assertEquals(v.status_code, UNAUTHORIZED)


	def test_DeberiaCambiarEstructuraCarpetaAlRenombrar(self):
		logTest("Deberia cambiar la estructura de carpeta al renombrar")
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "../src/db/batch.cpp"
		internalUri = USER_SIMPLE["user"] + FILENAME[2:] # saca los '..'
		uriConVersion = internalUri + RESERVED_STR + FIRST
		requests.put(FILE + uriConVersion, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# sube archivo

		nuevosMetadatos = '{"propietario" : "' +  USER_SIMPLE["user"] + '" , "extension" : "nuevaExtension", "nombre" : "nuevoNombre", ' \
			+ '"etiquetas" : ["nuevaEtiqueta"], "usuarios" : [], "usuario ultima modificacion" : "' + USER_SIMPLE["user"] + '" , ' \
			+ '"fecha ultima modificacion" : "23/10/2015"}'

		s = requests.put(METADATA + internalUri, data={"token" : token, "user": USER_SIMPLE["user"], "metadatos" : nuevosMetadatos})
		self.assertEquals(s.status_code, SUCCESS)

		t = requests.get(METADATA + internalUri, data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(t.status_code, NOT_FOUND)	# como se renombro, existe pero bajo otra uri

		newInternalUri = USER_SIMPLE["user"] + "/src/db/nuevoNombre.nuevaExtension"
		u = requests.get(METADATA + newInternalUri, data={"token": token, "user": USER_SIMPLE["user"]})
		self.assertEquals(u.status_code, SUCCESS)

		estructuraEsperada = {USER_SIMPLE["user"] + "/src/db/nuevoNombre.nuevaExtension" : "nuevoNombre.nuevaExtension" + RESERVED_STR + FIRST}
		expected = {"estructura" : estructuraEsperada}
		v = requests.get(FOLDER + USER_SIMPLE["user"] + "/src/db", data={"token" : token, "user": USER_SIMPLE["user"]})
		estructuraRecibida = literal_eval(v.content)
		self.assertDictEqual(expected, estructuraRecibida)


	def test_DeberiaDarErrorAlIngresarAUnRecursoInexistente(self):
		logTest("Deberia dar error al ingresar un recurso inexistente")
		r = requests.get(BASE)
		self.assertEquals(r.status_code, NOT_FOUND)

		INEXISTENTE = BASE + "/sarasa"
		s = requests.get(INEXISTENTE)
		self.assertEquals(s.status_code, NOT_FOUND)


	def test_DeberiaDarErrorAlUsarUnMetodoNoSoportado(self):
		logTest("Deberia dar error al usar un metodo no soportado")
		r = requests.get(SESSION)
		self.assertEquals(r.status_code, UNSUPPORTED_METHOD)

		s = requests.put(SESSION)
		self.assertEquals(s.status_code, UNSUPPORTED_METHOD)

		t = requests.post(FOLDER)
		self.assertEquals(t.status_code, UNSUPPORTED_METHOD)

		u = requests.delete(METADATA)
		self.assertEquals(u.status_code, UNSUPPORTED_METHOD)


	def test_DeberiaDarErrorAlAccederConTokenErroneo(self):
		logTest("Deberia dar error al acceder con token erroneo")
		token = registrarYLoguearUser(USER_SIMPLE)
		r = requests.get(PROFILE + USER_SIMPLE["user"], data={"user": USER_SIMPLE["user"], "token": "123456789"})
		self.assertEquals(r.status_code, UNAUTHORIZED)


	def test_SubirBorrarYRestaurarUnArchivo(self):
		logTest("Subir, Borrar y Restaurar Archivo")
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "Makefile"

		uri = FILE + USER_SIMPLE["user"] + "/" + FILENAME
		r = requests.put(uri, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# lo sube
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token}) # lo borra
		self.assertEquals(s.status_code, SUCCESS)

		uri = FILE + USER_SIMPLE["user"] + "/" + TRASH_TYPE + FILENAME + RESERVED_STR + "0"
		t = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token, "restore": "true"}) # lo restaura
		self.assertEquals(t.status_code, SUCCESS)


	def test_SubirBorrarYEliminarDefinitivamenteUnArchivo(self):	
		logTest("Subir, Borrar y Eliminar Definitivamente un Archivo")
		token = registrarYLoguearUser(USER_SIMPLE)
		FILENAME = "Makefile"

		uri = FILE + USER_SIMPLE["user"] + "/" + FILENAME
		r = requests.put(uri + RESERVED_STR + FIRST, files={'file': open(FILENAME, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# lo sube
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token}) # lo borra
		self.assertEquals(s.status_code, SUCCESS)

		uri = FILE + USER_SIMPLE["user"] + "/" + TRASH_TYPE + FILENAME + RESERVED_STR + "0"
		t = requests.delete(uri, data={"user": USER_SIMPLE["user"], "token": token, "restore": "false"}) # lo borra de la papelera
		self.assertEquals(t.status_code, SUCCESS)


	def test_DeberiaDarErrorAlActualizarElPerfilYQueSeaInvalido(self):
		logTest("Deberia dar error al actualizar perfil invalido")
		username = "hola"
		password = "masdeocholetras"
		token = registrarYLoguear(username, password, PERFIL)

		r = requests.put(PROFILE + username, data={"nombre": "otroNombre", "email": "otro@email",
			"token": token}) # actualizar perfil con email invalido
		self.assertEquals(r.status_code, BAD_REQUEST)


	def test_DeberiaFiltrarBienLosUsuariosDadoUnString(self):
		logTest("Deberia filtrar bien los usuarios dado un string")
		#Primero registro tres usuarios
		username1 = "hola"
		password = "masdeocholetras"
		token = registrarYLoguear(username1, password, PERFIL)
		username2 = "como"
		registrarYLoguear(username2, password, PERFIL)
		username3 = "estas"
		registrarYLoguear(username3, password, PERFIL)
		username4 = "COmi"
		registrarYLoguear(username4, password, PERFIL)

		#Busco usuario que tengan la letra o
		t = requests.get(PROFILE, data={"user": username1, "token": token, "busqueda": "o"})
		self.assertEquals(t.status_code, SUCCESS)
		resultado = t.json().get("busqueda")
		usuariosMacheados = resultado.get("usuarios").split(RESERVED_STR)
		self.assertEquals(usuariosMacheados[0], username1)
		self.assertEquals(usuariosMacheados[1], username2)
		self.assertEquals(usuariosMacheados[2], username4)

		#Busco usuario que tengan la letra u
		u = requests.get(PROFILE, data={"user": username1, "token": token, "busqueda": "u"})
		self.assertEquals(u.status_code, SUCCESS)
		resultado = u.json().get("busqueda")
		usuariosMacheados = resultado.get("usuarios").split(RESERVED_STR)
		self.assertEquals(usuariosMacheados, [''])

		#Busco usuario que tengan com
		s = requests.get(PROFILE, data={"user": username1, "token": token, "busqueda": "com"})
		self.assertEquals(s.status_code, SUCCESS)
		resultado = s.json().get("busqueda")
		usuariosMacheados = resultado.get("usuarios").split(RESERVED_STR)
		self.assertEquals(usuariosMacheados[0], username2)
		self.assertEquals(usuariosMacheados[1], username4)

		#Trato de buscar usuarios que tengan string vacio
		r = requests.get(PROFILE, data={"user": username1, "token": token, "busqueda": ""})
		self.assertEquals(r.status_code, BAD_REQUEST)


	def test_deberiaBuscarBienArchivosPorExtensionYNombre(self):
		logTest("Deberia buscar bine archivos por extension y nombre")
		token = registrarYLoguearUser(USER_SIMPLE)
		
		TXT_FILENAME_ROOT = 'CMakeCache.txt'
		JPG_FILENAME_ROOT = 'default.jpg'
		TXT_FILENAME_SUBFOLDER = 'files/log.txt'

		requests.put(FILE + USER_SIMPLE["user"] + "/" + TXT_FILENAME_ROOT, \
			files={'file': open(TXT_FILENAME_ROOT, 'rb'), "token": token, "user": USER_SIMPLE["user"]})
		requests.put(FILE + USER_SIMPLE["user"] + "/" + JPG_FILENAME_ROOT, \
			files={'file': open(JPG_FILENAME_ROOT, 'rb'), "token": token, "user": USER_SIMPLE["user"]})
		requests.put(FILE + USER_SIMPLE["user"] + "/" + TXT_FILENAME_SUBFOLDER, \
			files={'file': open(TXT_FILENAME_SUBFOLDER, 'rb'), "token": token, "user": USER_SIMPLE["user"]})

		# Busco por extension txt
		r = requests.get(METADATA, data={"user": USER_SIMPLE["user"], "token": token, "extension": "txt"})
		self.assertEquals(r.status_code, SUCCESS)
		resultado = r.json().get("busqueda")
		esperado = { USER_SIMPLE["user"] +"/"+ TXT_FILENAME_ROOT : TXT_FILENAME_ROOT, \
			USER_SIMPLE["user"] + "/" + TXT_FILENAME_SUBFOLDER : TXT_FILENAME_SUBFOLDER.split("/")[-1]}	# CMakeCache.txt y el log
		self.assertDictEqual(resultado, esperado)

		# Busco por nombre con g -> no busca sobre la extension, asi que no toma al jpg
		s = requests.get(METADATA, data={"user": USER_SIMPLE["user"], "token": token, "nombre": "g"})
		self.assertEquals(s.status_code, SUCCESS)
		resultado = s.json().get("busqueda")
		esperado = { USER_SIMPLE["user"] + "/" + TXT_FILENAME_SUBFOLDER : TXT_FILENAME_SUBFOLDER.split("/")[-1]} # el log
		self.assertDictEqual(resultado, esperado)

		# Busco por nombre con s -> no busca sobre el path (las carpetas), asi que no toma al log
		t = requests.get(METADATA, data={"user": USER_SIMPLE["user"], "token": token, "nombre": "s"})
		self.assertEquals(t.status_code, SUCCESS)
		resultado = t.json().get("busqueda")
		esperado = { } # ninguno
		self.assertDictEqual(resultado, esperado)


	def test_DeberiaRestaurarArchivoHabiendoBorradoSuCarpetaOriginal(self):
		logTest("Deberia Restaurar Archivo Habiendo Borrado Su Carpeta Original")
		token = registrarYLoguearUser(USER_SIMPLE)

		TXT_FILENAME_SUBFOLDER = 'files/log.txt'

		r = requests.put(FILE + USER_SIMPLE["user"] + "/" + TXT_FILENAME_SUBFOLDER + RESERVED_STR + FIRST, \
			files={'file': open(TXT_FILENAME_SUBFOLDER, 'rb'), "token": token, "user": USER_SIMPLE["user"]})	# sube 'log.txt' en carpeta 'files'
		self.assertEquals(r.status_code, RESOURCE_CREATED)

		s = requests.delete(FOLDER + USER_SIMPLE["user"] + "/files", data = {"token" : token, "user": USER_SIMPLE["user"]})
		self.assertEquals(s.status_code, SUCCESS)

		restaurarUri = FILE + USER_SIMPLE["user"] + "/" + TRASH_TYPE + "files" + RESERVED_STR + "log.txt" + RESERVED_STR + "0"
		t = requests.delete(restaurarUri, data={"restore" : "true", "token" : token, "user" : USER_SIMPLE["user"]})
		self.assertEquals(t.status_code, SUCCESS)


if __name__ == '__main__':
	definirConstantesGlobales()
	unittest.main()
