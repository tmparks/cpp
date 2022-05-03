FROM ubuntu

# Install development tools
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --yes \
    build-essential \
    clang  \
    clang-tidy \
    cmake \
    gcovr \
    git  \
    googletest \
    llvm \
    && rm --recursive --force /var/lib/apt/lists/*

# Install gmock and gtest
RUN BUILD=$(mktemp --directory) \
    && cmake -S /usr/src/googletest -B $BUILD \
    && make --directory=$BUILD install \
    && rm --recursive --force $BUILD

# Install Guidelines Support Library
RUN BUILD=$(mktemp --directory) \
    && git clone https://github.com/microsoft/GSL.git --depth 1 --single-branch $BUILD \
    && cmake -S $BUILD -B $BUILD/build \
    && make --directory=$BUILD/build install \
    && rm --recursive --force $BUILD
