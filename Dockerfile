FROM ubuntu:latest

RUN sed -i 's@archive.ubuntu.com@ftp.jaist.ac.jp/pub/Linux@g' /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    make \
    cmake \
    clang \
    gcc \
    git \
    lld \
    nasm \
    gdb \
    tree && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
