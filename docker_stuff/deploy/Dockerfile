FROM ftorresd/rpcfebconnectivitytest_buildenv

LABEL maintainer.name="Felipe Silva"
LABEL maintainer.email="felipe.silva@cern.ch"

ENV LANG=C.UTF-8

WORKDIR /opt

ARG TAG_NAME=1.0.2

RUN apt update -y && \
    apt upgrade -y && \
    apt install -y unzip && \
    wget https://github.com/ftorresd/RPCFebConnectivityTest/archive/v${TAG_NAME}.zip && \
    unzip v${TAG_NAME}.zip && \
    rm -rf v${TAG_NAME}.zip && \
    mv  RPCFebConnectivityTest-${TAG_NAME} RPCFebConnectivityTest && \
    cd RPCFebConnectivityTest  && \
    cmake . && \
    make

ENV PATH /opt/RPCFebConnectivityTest/bin:$PATH

WORKDIR /data

CMD ["/opt/RPCFebConnectivityTest/bin/connectivitytest"]