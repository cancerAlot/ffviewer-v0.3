# farbfeldviewer - a simple viewer for farfeld files
# See LICENSE file for copyright and license details

CFLAGS=-Os -finstrument-functions -D_REENTRANT -pipe -fomit-frame-pointer -fpic -Wstrict-prototypes -Wall -Wextra -Wshadow -Wredundant-decls -Wunreachable-code -Wdisabled-optimization -fstack-protector-all

INCS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0 libpng`

all:
	${CC} ${CFLAGS} ${INCS} ${LIBS} -o ffviewer main.c ff2png.c
