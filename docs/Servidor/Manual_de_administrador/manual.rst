============================================================================================================
Compilación y ejecución manual
============================================================================================================
El servidor en cuestión requiere tener los algunos programas y librerías instalados previamente.
Esta guía de instalación está pensada para Ubuntu 14.04, aunque debería ser funcionar también
con otras versiones de Ubuntu y ser análoga (con los manejadores de paquetes correspondientes) 
en otras versiones de Linux.

Dependencias
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

G++ 4.9
------------------------------------------------------------------------------------------------------------
.. code-block:: bash

	sudo add-apt-repository ppa:ubuntu-toolchain-r/test
	sudo apt-get update
	sudo apt-get install g++-4.9
	sudo apt-get upgrade


CMake
------------------------------------------------------------------------------------------------------------
.. code-block:: bash

	sudo apt-get update
	sudo apt-get install cmake


RocksDB
------------------------------------------------------------------------------------------------------------
Ver `RocksDB <https://github.com/facebook/rocksdb>`_ y su guía de instalación 
(requiere a su vez **libsnappy-dev**, **zlib1g-dev** y **libbz2-dev**).

Compilar con ``make static_lib && make install``

Alternativamente, ejecutar el script ``docker/rocksdb.sh`` habiendo instalado previamente
*g++-4.9*, *libsnappy-dev*, *zlib1g-dev* y *libbz2-dev*.


GTest (para ejecutar los tests)
------------------------------------------------------------------------------------------------------------
.. code-block:: bash

	sudo apt-get install libgtest-dev
	cd /usr/src/gtest
	cmake CMakeLists.txt
	make
	cp *.a /usr/lib

Alternativamente, ejecutar el script ``docker/gtest.sh`` habiendo instalado previamente *CMake*

LCOV (para analizar coverage)
------------------------------------------------------------------------------------------------------------
.. code-block:: bash

	sudo apt-get install lcov

Requests (para tests de integración y coverage)
------------------------------------------------------------------------------------------------------------
Con **pip** instalado, ejecutar:

.. code-block:: bash

	sudo pip install requests

De no tener pip, instalarlo anteriormente con:

.. code-block:: bash

	sudo apt-get install python-pip

Compilación y Ejecución (del servidor, los tests unitarios y coverage)
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Ejecutar ``build.sh -h`` para ver las distintas opciones de compilación. 
Habiendo instalado previamente todas las dependencias necesarias para la opción elegida,
ejecutar el script ``build.sh`` desde la raíz del proyecto
con las opciones en cuestión.


