FROM ubuntu:22.04

ENV ARCH=aarch64
ENV CC=clang
ENV LD=ld.lld

RUN apt-get update && apt-get install -y \
    bc \
    bison \
    flex \
    libssl-dev \
    make \
    cmake \
    clang \
    clangd \
    lld \
    llvm-dev \
    device-tree-compiler \
    file \
    rsync \
    cpio \
    wget \
    curl \
    git \
    qemu \
    gcc-aarch64-linux-gnu \
    binutils-aarch64-linux-gnu \
    libnewlib-arm-none-eabi \
    gcc-arm-none-eabi \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
