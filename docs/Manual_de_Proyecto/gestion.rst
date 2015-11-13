Gestión
============================
Para la gestión del proyecto hemos utilizado **Git** como sistema de control de versiones, y más particularmente `GitHub <https://github.com>`_. Dentro de este, hemos utilizado el sistema de ticketing y tareas que provee, ya que consideramos que era mejor tener todo centralizado en el mismo lugar.

Workflow
^^^^^^^^^^^^^^^^^^^^^
Dentro de **Git**, el workflow que se llevó a cabo fue más bien centralizado, donde todos los desarrolladores del equipo se manejaban haciendo commits en sus respectivos *working directories* para luego llevarlo al repositorio compartido, que estaba almacenado en `GitHub <https://github.com>`_. En la sección `Links a repositorios`_ se pueden ver los repositorios utlizados.
Por otro lado, se utilizó una estrategia de *branching* que consistió en tener una primera branch llamada *master*, donde estarán las versiones completamente estables (O, dicho de otra forma, las versiones *alfa*). Por otro lado, desprendiendo de ésta, está la branch *develop*, donde están las versiones *beta* o estables (Pero no completamente testeadas). Por último, a medida que se iban desarrollando funcionalidades se llevaba cada una por separado a distintas branches que indican el nombre de la funcionalidad con el fin de desarrollar libremente (seguir)

Comentar lo de que no se podia commitear a develop sin que pasen los tests en travis.

Links a repositorios
^^^^^^^^^^^^^^^^^^^^^
 * `Repositorio del servidor <https://github.com/toblich/UDrive>`_
 * `Repositorio del cliente <https://github.com/plandino/clienteUdrive>`_