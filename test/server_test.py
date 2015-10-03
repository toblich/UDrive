import unittest
import requests
from subprocess import Popen
from time import sleep


def definirConstantesGlobales():
	global BASE 
	BASE = "http://localhost:8080/"
	
	global PROFILE
	PROFILE = BASE + "profile/"
	
	global SESSION 
	SESSION = BASE + "session/"
	
	global FILE
	FILE = BASE + "file/"

	global USER_SIMPLE 
	USER_SIMPLE = {"user":"tobi", "pass" : "masdeocholetras", "profile":"perfil"}
	
	global LOG_USER_SIMPLE 
	LOG_USER_SIMPLE = USER_SIMPLE.copy()
	del LOG_USER_SIMPLE["profile"]



class ServerTest(unittest.TestCase):
	def setUp(self):
		print
		self.serverProcess = Popen(["./udrive", "&"])

	def tearDown(self):
		requests.get(BASE + "close")
		self.serverProcess.wait()


	def test_registrarUsuarioInvalido(self):
		userCroto = USER_SIMPLE.copy()
		userCroto['pass'] = "corta"
		r = requests.post(PROFILE, data=userCroto)
		self.assertEquals(r.status_code, 400)

	def test_registroyLogueoValido(self):
		r = requests.post(PROFILE, data=USER_SIMPLE)
		self.assertEquals(r.status_code, 201)
		s = requests.post(SESSION, LOG_USER_SIMPLE)
		self.assertEquals(s.status_code, 200)







if __name__ == '__main__':
	definirConstantesGlobales()
	unittest.main()