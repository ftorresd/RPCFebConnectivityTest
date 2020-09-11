FROM ubuntu:20.04

LABEL maintainer.name="Felipe Silva"
LABEL maintainer.email="felipe.silva@cern.ch"

ENV LANG=C.UTF-8

WORKDIR /opt

RUN apt update -y && \
    apt upgrade -y

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata

RUN apt install -y wget cmake g++ && \
    apt install -y freetype2-demos && \
    wget https://root.cern/download/root_v6.22.00.Linux-ubuntu19-x86_64-gcc9.2.tar.gz && \
    tar -xzf root_v6.22.00.Linux-ubuntu19-x86_64-gcc9.2.tar.gz && \
    rm -rf root_v6.22.00.Linux-ubuntu19-x86_64-gcc9.2.tar.gz && \
    echo /opt/root/lib >> /etc/ld.so.conf && \
    ldconfig

ENV ROOTSYS /opt/root
ENV PATH $ROOTSYS/bin:$PATH
ENV PYTHONPATH $ROOTSYS/lib:$PYTHONPATH
ENV CLING_STANDARD_PCH none

CMD ["bash"]