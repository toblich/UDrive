===========================================================
Ejecución con Docker
===========================================================
Instalar docker ejecutando:

.. code-block:: bash

	wget -qO- https://get.docker.com/ | sh


Luego, copiar el archivo ``Dockerfile`` a una carpeta vacía. Desde esa carpeta, ejecutar:

.. code-block:: bash

	sudo docker build -t udrive .
	sudo docker run -it udrive

Desde otra consola, ejecutar:

.. code-block:: bash

	sudo docker ps | grep udrive
	sudo docker inspect <containerID> | grep IPAddress

donde ``<containerID>`` es el ID del container obtenido en el primer comando. Con la IP obtenida en 
el último comando podrá interactuarse con el servidor (considerando que se lanza en el puerto 8080).
