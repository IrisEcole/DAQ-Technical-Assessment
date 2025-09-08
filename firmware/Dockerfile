FROM ubuntu:22.04 AS dev
ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /app

# Install dependencies, addded libxml2, cmake, can untils, from trial and error trying to run cmake locally
RUN apt-get update && \
    apt-get install -y python3-pip wget autoconf libtool git make g++ valgrind cmake can-utils libxml2-dev && \
    rm -rf /var/lib/apt/lists/*

# Clone and build dbcppp following the read me from git
RUN git clone --recurse-submodules https://github.com/xR3b0rn/dbcppp.git /tmp/dbcppp && \
    cd /tmp/dbcppp && \
    mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    rm -rf /tmp/dbcppp

#Could do a docker ignore instead    
COPY solution/ ./solution/
COPY dbc-files/ ./dbc-files/
COPY dump.log ./dump.log
COPY tests/ ./tests/

# Create a clean build directory and build
RUN mkdir -p /app/build
WORKDIR /app/build
RUN cmake ../solution -DCMAKE_PREFIX_PATH=/usr/local && make -j

ENTRYPOINT ["/app/build/answer"]
