============================================================================================================
Compilación y ejecución
============================================================================================================

Tanto la compilación como la ejecución del Servidor de UDrive se pueden realizar de forma manual
o de forma automática.

De todas formas, para ambos casos se precisa tener un archivo *server.conf* dentro de donde se vaya a ejecutar *udrive*. Éste permite configurar algunos aspectos del servidor. El formato del mismo debe ser de tipo Json y debe contener los siguientes campos:

 * "Nivel Log" : Indica con qué nivel de log se quiere ejecutar. Éste es un número entre 0 y 4, siendo 0 el nivel más bajo (Se logueará todo tipo de ) y 4 el nivel más alto (Se loguearán sólo errores graves), donde el resto de los niveles van en escala ascendente. Por defecto, el valor es 0.
 * "Tamano max Log" : Indica qué tamaño, expresado en KB, será el máximo que soporte el Log. Una vez que supere esta capacidad, el Log se vacía y vuelve a comenzar. Por defecto, el valor es 1024 (Un MB).
 * "FileSystem" : Indica cuál será la carpeta raíz del FileSystem. Por defecto, el valor es "FileSystem".
 * "Base de datos" : Indica cuál será la carpeta raíz de las bases de datos. Por defecto, el valor es "db".
 * "Cantidad threads" : Indica cuál es la cantidad de threads que se quiere que contenga el servidor. Su valor por defecto es 10.
 * "Tiempo Poll Server" : Indica cuál es el tiempo que el server espera hasta leer próximos eventos. Su valor está en milisegundos, y por defecto es 1.

 A continuación mostramos un ejemplo de cómo quedaría el archivo *server.conf*:

 .. code-block:: bash

	{
		"Nivel Log" : 4, 
		"Tamano max Log" : 1024,
		"FileSystem" : "FileSystem",
		"Base de datos" : "db",
		"Cantidad threads" : 10,
		"Tiempo Poll Server" : 1
	}


Compilación y Ejecución Manual (servidor, tests unitarios y code coverage)
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Con ejecución inmediata
------------------------------------------------------------------------------------------------------------

Habiendo instalado previamente todas las dependencias necesarias, indicadas en la sección `Instalación y configuración <instalacion_y_configuracion.html>`_ , ejecutar el script ``build.sh`` con la opción *-h* desde la raíz del proyecto. Este comando desplagará las distintas opciones de compilación del *Servidor* de *UDrive*

.. figure:: _static/buildHelp.png
	:scale: 90%
	:align: center

	Opciones que desplega la opción *-h* del script *build.sh*

Sin ejecución inmediata
------------------------------------------------------------------------------------------------------------

Crear una carpeta ``build`` dentro de la raíz del proyecto. Dentro de esa carpeta, crear las carpetas ``files`` y ``db``.
Luego, desde allí ejecutar 

.. code-block:: bash

	cmake [-D<flag>=<ON|OFF>] ../
	make <target>

Donde los *flags* pueden ser:
 + *rocksdb* - usar bases de datos persistentes en vez de mapas (volátiles) en memoria
 + *test* - habilita target **unitTest**
 + *coverage* - realizar análisis de coverage (habilita targets **unitTest** y **coverage**).    Esta opción desactiva RocksDB y activa los tests unitarios. Prepara la compilación el servidor con mapas en memoria y de los tests unitarios.

Los *targets* pueden ser:
 + *udrive* - compila el servidor
 + *unitTests* - compila los tests unitarios
 + *coverage* - compila el servidor y los tests unitarios; ejecuta estos últimos y los de integración, guardando los resultados del análisis de coverage en la carpeta ``coverage`` dentro de la carpeta ``build``.


Compilación y Ejecución con Docker (servidor)
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
Una vez que se tiene instalado Docker, tal como se indicada en la sección `Instalación y configuración <instalacion_y_configuracion.html>`_, copiar el archivo ``Dockerfile`` a una carpeta vacía. Desde esa carpeta, ejecutar:

.. code-block:: bash

	sudo docker build -t udrive .
	sudo docker run -it -p 8080:8080 udrive

La opción ``-p`` redirige el puerto 8080 de la computadora al 8080 de Docker.

Estos dos comandos inicializan el entorno de Docker y compilan, ejecutan los test unitarios y finalmente lanzan el servidor de UDrive.

Para poder conectarse con el Servidor, desde otra consola ejecutar:

.. code-block:: bash

	sudo docker ps | grep udrive
	sudo docker inspect <containerID> | grep IPAddress 

donde ``<containerID>`` es el ID del container obtenido en el primer comando. 

Finalmente con la IP obtenida en el último comando se podrá interactuar con el servidor (considerando que se lanza en el puerto 8080).

.. note::
	Para saber como interactuar con el Servidor ver el `Manual de la API REST <../../../Documentacion_Tecnica/_build/html/api_rest.html>`_ dentro de la `Documentación Técnica del Servidor. <../../../Documentacion_Tecnica/_build/html/index.html>`_


Para ver el log o el filesystem, o interactuar de cualquier otra forma con el contenedor que ejecuta el servidor, se sugiere iniciar una terminal del container de la siguiente manera::

	sudo docker ps | grep udrive
	sudo docker exec -it <containerID> /bin/bash

donde ``<containerID>`` es el ID del container obtenido en el primer comando.

