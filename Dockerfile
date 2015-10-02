FROM ubuntu:14.04

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y dist-upgrade
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install python-software-properties
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install software-properties-common
RUN DEBIAN_FRONTEND=noninteractive add-apt-repository ppa:ubuntu-toolchain-r/test

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y g++ g++-4.9 libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev libgtest-dev cmake wget unzip
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y

RUN ls && wget https://github.com/toblich/pruebas_travis/archive/docker.zip && ls && unzip docker.zip && ls
RUN cd pruebas_travis-docker && ls 
RUN cd pruebas_travis-docker && chmod +x docker/rocksdb.sh && sync && ./docker/rocksdb.sh 
RUN cd pruebas_travis-docker && chmod +x docker/gtest.sh && sync && ./docker/gtest.sh 
RUN cd pruebas_travis-docker && find . -type d

CMD echo ------------------------------------------------------------------------------------------------------------------------------------------------------- && cd pruebas_travis-docker && find . -type d  && chmod +x build.sh && sync && ./build.sh -t -x
