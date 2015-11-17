Gestión
============================
Para la gestión del proyecto hemos utilizado **Git** como sistema de control de versiones, y más particularmente `GitHub <https://github.com>`_. Dentro de éste, hemos utilizado el sistema de ticketing y tareas que provee, ya que consideramos que era mejor tener todo centralizado en el mismo lugar.

Workflow
^^^^^^^^^^^^^^^^^^^^^
Dentro de **Git**, el workflow que se llevó a cabo fue más bien centralizado, donde todos los desarrolladores del equipo se manejaban haciendo commits en sus respectivos *working directories* para luego llevarlo al repositorio compartido, que estaba almacenado en `GitHub <https://github.com>`_. En la sección `Links a repositorios`_ se pueden ver los repositorios utlizados.
Por otro lado, se utilizó una estrategia de *branching* que consistió en tener una primera branch llamada *master*, donde estarán las versiones de *release* (O, dicho de otra forma, las versiones *alfa*, donde estarán todas las funcionalidades implementadas). Por otro lado, desprendiendo de ésta, está la branch *develop*, donde se acumulan las funcionalidades ya implementadas y testeadas. Por último, a medida que se iban desarrollando funcionalidades se llevaba cada una por separado a distintas branches (inestables) que indican el nombre de la funcionalidad con el fin de que cada desarrollador pueda programar libremente, sin que sus cambios influyan en el resto de los programadores. Una vez finalizada dicha funcionalidad e incluyendo los tests correspondientes, se volvía a la branch develop.

Por otro lado, todas las pruebas, tanto unitarias como de integración estaban subidas también a la plataforma de `integración continua <https://travis-ci.org>`_, lo cual permitió que todo *push* que se hiciera sobre cualquiera de las branches, hiciera que se corrieran todas las pruebas. De esta manera se tuvo un seguimiento del pasaje de las pruebas commit a commit, lo cual hizo que fueran quedando registradas cuándo y quién fue el responsable de que las pruebas fallaran. Además, se implementó un sistema provisto por `GitHub <https://github.com>`_, mediante el cual no se podían hacer commits directos a la branch develop, así como tampoco permitía *mergear* branches donde no estuvieran pasando las pruebas en `Travis <https://travis-ci.org>`_ a develop. Esto iba de la mano de mantener en develop una versión más o menos estable de la aplicación.

Por último, cabe mencionar que se trató de que toda tarea a llevar a cabo quede mencionada en algún ticket o issue dentro de lo que ya se mencionó como sistema de ticketing.

Links a repositorios
^^^^^^^^^^^^^^^^^^^^^
Cabe destacar que en un comienzo se iba a tener un sólo repositorio para ambas aplicaciones (Cliente y servidor), pero a medida que el proyecto fue creciendo y que se iban tomando caminos muy distintos, se decidió partir el repositorio original, creando aparte el repositorio del cliente.

 * `Repositorio del servidor <https://github.com/toblich/UDrive>`_
 * `Repositorio del cliente <https://github.com/plandino/clienteUdrive>`_