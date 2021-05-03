FROM debian

RUN apt-get update && apt-get upgrade -y \
    && apt-get install \
        gcc \
        mingw-w64 \
        mingw-w64-tools \
        gcc-mingw-w64 \
        gcc-mingw-w64-i686 \
        wine64 \
        gdb -y

COPY src /tasks/


