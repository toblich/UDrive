Pruebas
===============================================================================

.. _testdeps:

Dependencias
*******************************************************************************

Es necesario tener :ref:`gtest`, :ref:`requests` con Python y :ref:`lcov`. También se requieren :ref:`gpp` y :ref:`cmake` para compilar cualquier tipo de test.


Tests unitarios
*******************************************************************************

Desde la raíz del proyecto, ejecutar:

.. code-block:: bash

	./build.sh -t               # "-t" equivale a "--test"

Agregar el flag ``-r`` si se quiere usar bases de datos con :ref:`rocks` en vez de los mocks de bases de datos en memoria y volátiles (ver :ref:`basesdedatos`).

Para compilarlos y ejecutarlos manualmente, ejecutar en cambio desde una carpeta ``build`` vacía en la raíz del proyecto:

.. code-block:: bash
	
	mkdir files
	mkdir db                    # si se usa rocksdb
	cmake -Dtest=ON ../         # agregar el flag "-Drocksdb=ON" para usar RocksDB
	make unitTest               # compila los tests unitarios
	./unitTest                  # ejecuta los tests unitarios

Dentro de la carpeta ``build`` quedarán los *filesystems* de todos los tests unitarios en su estado final, cada uno en una carpeta con el mismo nombre del test en cuestión. También estará el archivo ``build/files/log.txt`` con un log con información sobre todo lo ocurrido dentro del servidor durante la ejecución de los tests.


Tests de integración
*******************************************************************************

Desde la raíz del proyecto, ejecutar:

.. code-block:: bash

	./build.sh                       # agregar el flag "-r" o "--rocksdb" si se quiere usar rocksdb
	cd build
	make udrive                      # compila el servidor
	python ../test/server_test.py    # ejecuta los tests de integración sin versionado de archivos
	python ../test/version_test.py   # ejecuta aquellos tests de integración con versionado

La carpeta ``build/FileSystem`` será el *filesystem* del último test que se haya ejecutado. Al igual que en los test unitario, también estará el archivo ``build/files/log.txt`` con un log con información sobre todo lo ocurrido dentro del servidor durante la ejecución de los tests.


.. _coverage:

Code Coverage
*******************************************************************************

Con todas las :ref:`testdeps` instaladas, ejecutar desde la raíz del proyecto:

.. code-block:: bash

	./build.sh -c               # "-c" equivale a "--coverage"

Al finalizar (exitosamente) la ejecución, abrir el archivo ``build/coverage/index.html`` con un navegador para ver el reporte de *code coverage* resultante.

Alternativamente al script anterior, puede ejecutarse la siguiente secuencia de comandos desde una carpeta ``build`` vacía en la raíz del proyecto, logrando el mismo resultado:

.. code-block:: bash

	mkdir files
	cmake -Dcoverage=ON ../    # prepara un Makefile para compilar y ejecutar el análisis de coverage
	make coverage              # compila y ejecuta el análisis de coverage

Dado que este análisis ejecuta tanto los tests unitarios como los de integración (con y sin versionado), se generaran todos los *filesystems* y quedará todo registrado en el log.

Haga click `aquí`_ para ver su último reporte de coverage. [#reportecoverage]_

.. _aquí: ../../../../../build/coverage/index.html

.. rubric:: Footnotes

.. [#reportecoverage] Si el link está roto, regenere su reporte de coverage ejecutando cualquiera de las alternativas de la sección :ref:`coverage`.