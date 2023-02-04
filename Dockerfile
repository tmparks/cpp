FROM ubuntu

# Install development tools
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --yes \
    build-essential \
    clang  \
    clang-tidy \
    cmake \
    gcovr \
    gdb \
    git  \
    googletest \
    llvm \
    nano \
    && rm --recursive --force /var/lib/apt/lists/*

# Install gmock and gtest
RUN CMAKE_BUILD_TYPE=Release \
    BUILD=$(mktemp --directory) \
    && cmake -S /usr/src/googletest -B $BUILD \
    && make --directory=$BUILD install \
    && rm --recursive --force $BUILD

# Install Guidelines Support Library
RUN CMAKE_BUILD_TYPE=Release \
    SOURCE=$(mktemp --directory) \
    BUILD=$(mktemp --directory) \
    && git clone https://github.com/microsoft/GSL.git --depth 1 --single-branch $SOURCE \
    && cmake -S $SOURCE -B $BUILD \
    && make --directory=$BUILD install \
    && rm --recursive --force $SOURCE $BUILD
