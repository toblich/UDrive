FROM ubuntu:14.04

RUN DEBIAN_FRONTEND=noninteractive apt-get update  && \
    DEBIAN_FRONTEND=noninteractive apt-get -y install python-software-properties && \
    DEBIAN_FRONTEND=noninteractive apt-get -y install software-properties-common && \
    DEBIAN_FRONTEND=noninteractive add-apt-repository ppa:ubuntu-toolchain-r/test && \
    DEBIAN_FRONTEND=noninteractive apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y g++ g++-4.9 libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev libgtest-dev cmake wget unzip

RUN ls && wget https://github.com/toblich/UDrive/archive/develop.zip && ls && unzip develop.zip && ls
RUN cd UDrive-develop && chmod +x docker/rocksdb.sh && sync && ./docker/rocksdb.sh 
RUN cd UDrive-develop && chmod +x docker/gtest.sh && sync && ./docker/gtest.sh 

CMD echo ------------------------------------------------------------------------------------------------------------------------------------------------------- && cd UDrive-develop && find . -type d  && chmod +x build.sh && sync && ./build.sh -t -x
