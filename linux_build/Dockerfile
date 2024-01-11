#
#  Author: Aleksander Rozhkov <aleksprog@hotmail.com>
#
# docker build --tag qt-linux-cmake .
# /usr/bin/linuxdeployqt ./usr/share/application/weprex.desktop -appimage -always-overwrite -verbose=2
# export ARCH=x86_64

ARG GCC_VERSION=10

ARG CMAKE_BUILD_TYPE="Release"

# The sample helps you build the application.
FROM ubuntu:20.04

# Select assembly tools.
RUN apt-get update \
 && apt-get upgrade -y \
 && DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-suggests --no-install-recommends install \
        wget \
        cmake \
        curl \
        gcovr \
        git \
        libc-dev \
        make \
        build-essential \
        libfontconfig1 \
        mesa-common-dev \
        libglu1-mesa-dev \
        qt5-default \
        libqt5network5 \
        libqt5quick5 \
        qtquickcontrols2-5-dev \
        libqt5serialport5-dev \
        libqt5gui5 \
        libqt5widgets5 \
        qtdeclarative5-dev \
        qtdeclarative5-dev-tools \
        qml-module-qtquick-controls \
        qml-module-qtquick-controls2 \
        qml-module-qtquick-dialogs \
        qml-module-qtquick-layouts \
        qml-module-qtquick2 \
        qml-module-qtquick-window2 \
        fuse \
        zlib1g-dev \
        curl  \
        unzip  \
        tar  \
        pkg-config \
        file \
        desktop-file-utils \
        gpg \
 && rm -rf /var/lib/apt/lists/*

# Install the required compiler version.
ARG GCC_VERSION
ENV GCC_VERSION=${GCC_VERSION}

RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-suggests --no-install-recommends install \
        gcc-${GCC_VERSION} \
        g++-${GCC_VERSION} \
        libstdc++-${GCC_VERSION}-dev \
        gdb \
 && rm -rf /var/lib/apt/lists/* \
 && update-alternatives --install \
                /usr/bin/gcc        gcc         /usr/bin/gcc-${GCC_VERSION} 100 \
        --slave /usr/bin/g++        g++         /usr/bin/g++-${GCC_VERSION} \
        --slave /usr/bin/gcov       gcov        /usr/bin/gcov-${GCC_VERSION}

# Install linuxdeployqt
RUN wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" -O /usr/bin/linuxdeployqt --no-check-certificate && \
    chmod a+x /usr/bin/linuxdeployqt

ENV ARCH=x86_64

WORKDIR /src