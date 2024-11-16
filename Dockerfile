FROM ubuntu:rolling

# Install development tools
RUN export DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
    && apt-get install --yes \
        build-essential \
        clang  \
        clang-format \
        clang-tidy \
        cmake \
        gcovr \
        gdb \
        git  \
        googletest \
        libboost-dev \
        libboost-timer-dev \
        libeigen3-dev \
        libgomp1 \
        libomp-dev \
        llvm \
        nano \
    && rm --recursive --force /var/lib/apt/lists/*

# Install gmock and gtest
RUN export CMAKE_BUILD_TYPE=Release \
        BUILD=$(mktemp --directory) \
    && cmake -S /usr/src/googletest -B $BUILD \
    && make --directory=$BUILD install \
    && rm --recursive --force $BUILD

# Install Guidelines Support Library
RUN export CMAKE_BUILD_TYPE=Release \
        SOURCE=$(mktemp --directory) \
        BUILD=$(mktemp --directory) \
    && git clone https://github.com/microsoft/GSL.git --depth 1 --branch v4.1.0 $SOURCE \
    && cmake -S $SOURCE -B $BUILD \
    && make --directory=$BUILD install \
    && rm --recursive --force $SOURCE $BUILD
