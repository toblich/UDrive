FROM ubuntu:14.04

# RUN DEBIAN_FRONTEND=noninteractive add-apt-repository ppa:ubuntu-toolchain-r/test && apt-get update && apt-get install -y g++-4.9 libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev libgtest-dev cmake wget unzip && apt-get upgrade && wget https://github.com/toblich/UDrive/archive/develop.zip && unzip develop.zip && wget https://github.com/toblich/pruebasTravis/archive/docker.zip && unzip docker.zip && cp -r pruebasTravis-docker/docker UDrive-develop/ && cd UDrive-develop && chmod +x docker/* && ./docker/rocksdb.sh && ./docker/gtest.sh && cd && find . -type d
# && ./docker/UDrive.sh

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y dist-upgrade
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install python-software-properties
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install software-properties-common
RUN DEBIAN_FRONTEND=noninteractive add-apt-repository ppa:ubuntu-toolchain-r/test

RUN DEBIAN_FRONTEND=noninteractive apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y g++-4.9 libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev libgtest-dev cmake wget unzip
RUN DEBIAN_FRONTEND=noninteractive apt-get upgrade -y
RUN find . -type d
RUN chmod +x docker/rocksdb.sh && ./docker/rocksdb.sh 
RUN chmod +x docker/gtest.sh && ./docker/gtest.sh 
RUN find . -type d

CMD echo ------------------------------------------------------------------------------------------------------------------------------------------------------- && cd UDrive-develop && find . -type d  && chmod +x build.sh && ./build.sh -t -x
