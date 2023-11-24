FROM ubuntu:22.10 as base

RUN apt-get update && apt-get install -y cmake g++ libasan6 libcurl4-openssl-dev
WORKDIR cbdp
ENV CBDP_PORT 4242

FROM base as coordinator

COPY cmake-build-debug/coordinator .
CMD exec ./coordinator https://db.in.tum.de/teaching/ws2223/clouddataprocessing/data/filelist.csv "$CBDP_PORT"

FROM base as worker

ENV CBDP_COORDINATOR coordinator
COPY cmake-build-debug/worker .
CMD echo "worker $CBDP_COORDINATOR $CBDP_PORT" && exec ./worker "$CBDP_COORDINATOR" "$CBDP_PORT"

