============================================================================================================
Compilación y ejecución
============================================================================================================

Tanto la compilación como la ejecución del Servidor de UDrive se pueden realizar de forma manual
o de forma automática.

Compilación y Ejecución de forma Manual (del servidor, los tests unitarios y coverage)
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Habiendo instalado previamente todas las dependencias necesarias para la opción elegida,
ejecutar el script ``build.sh`` desde la raíz del proyecto con las opciones en cuestión.

Ejecutar ``build.sh -h`` para ver las distintas opciones de compilación. 

Compilación y Ejecución del servidor de forma automática con Docker
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
Una vez que se tiene instalado Docker, copiar el archivo ``Dockerfile`` a una carpeta vacía. Desde esa carpeta, ejecutar:

.. code-block:: bash

	sudo docker build -t udrive .
	sudo docker run -it udrive

Desde otra consola, ejecutar:

.. code-block:: bash

	sudo docker ps | grep udrive
	sudo docker inspect <containerID> | grep IPAddress

donde ``<containerID>`` es el ID del container obtenido en el primer comando. 
Finalmente con la IP obtenida en el último comando podrá interactuarse con el servidor (considerando que se lanza en el puerto 8080).
