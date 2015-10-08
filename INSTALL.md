# Ejecución con Docker
Instalar docker ejecutando:
```
wget -qO- https://get.docker.com/ | sh
```

Luego, copiar el archivo ```Dockerfile``` a una carpeta vacía. Desde esa carpeta, ejecutar:
```
sudo docker build -t udrive .
sudo docker run udrive
```
Desde otra consola, ejecutar:
```
sudo docker ps | grep udrive
sudo docker inspect <containerID> | grep IPAddress
```
donde <containerID> es el ID del container obtenido en el primer comando. Con la IP obtenida en 
el último comando podrá interactuarse con el servidor (considerando que se lanza en el puerto 8080).

# Compilación y ejecución manual
El servidor en cuestión requiere tener los algunos programas y librerías instalados previamente.
Esta guía de instalación está pensada para Ubuntu 14.04, aunque debería ser funcionar también
con otras versiones de Ubuntu y ser análoga (con los manejadores de paquetes correspondientes) 
en otras versiones de Linux.

## Dependencias

### G++ 4.9
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install g++-4.9
sudo apt-get upgrade
```

### CMake
```
sudo apt-get update
sudo apt-get install cmake
```

### RocksDB
Ver [RocksDB](https://github.com/facebook/rocksdb) y su guía de instalación 
(requiere a su vez **libsnappy-dev**, **zlib1g-dev** y **libbz2-dev**).

Compilar con ```make static_lib && make install```

Alternativamente, ejecutar el script ```docker/rocksdb.sh``` habiendo instalado previamente
*g++-4.9*, *libsnappy-dev*, *zlib1g-dev* y *libbz2-dev*.


### GTest (para ejecutar los tests)
```
sudo apt-get install libgtest-dev
cd /usr/src/gtest
cmake CMakeLists.txt
make
cp *.a /usr/lib
```
Alternativamente, ejecutar el script ```docker/gtest.sh``` habiendo instalado previamente *CMake*

### LCOV (para analizar coverage)
```
sudo apt-get install lcov
```


### Compilación y Ejecución

Ejecutar ```build.sh -h``` para ver las distintas opciones de compilación. 
Habiendo instalado previamente todas las dependencias necesarias para la opción elegida,
ejecutar el script ```build.sh``` desde la raíz del proyecto
con las opciones en cuestión.

