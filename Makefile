# farbfeldviewer - a simple viewer for farfeld files
# See LICENSE file for copyright and license details
include config.mk

oCFLAGS=${CLFAGS} -finstrument-functions -D_REENTRANT -pipe -march=core2 -fomit-frame-pointer -fpic -Wstrict-prototypes -Wall -Wextra -Wshadow -Wredundant-decls -Wunreachable-code -Wdisabled-optimization -fstack-protector-all
INCS=-I${INCLPREFIX}/gtk-3.0 -I${INCLPREFIX}/at-spi2-atk/2.0 -I${INCLPREFIX}/at-spi-2.0 -I${INCLPREFIX}/dbus-1.0 -I${INCL2PREFIX}/dbus-1.0/include -I${INCLPREFIX}/gtk-3.0 -I${INCLPREFIX}/gio-unix-2.0/ -I${INCLPREFIX}/cairo -I${INCLPREFIX}/pango-1.0 -I${INCLPREFIX}/harfbuzz -I${INCLPREFIX}/pango-1.0 -I${INCLPREFIX}/atk-1.0 -I${INCLPREFIX}/cairo -I${INCLPREFIX}/pixman-1 -I${INCLPREFIX}/freetype2 -I${INCLPREFIX}/libdrm -I${INCLPREFIX}/gdk-pixbuf-2.0 -I${INCLPREFIX}/libpng16 -I${INCLPREFIX}/glib-2.0 -I${INCL2PREFIX}/glib-2.0/include

LIBS=-lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lpthread -lpng

all:
	${CC} ${oCFLAGS} ${INCS} ${LIBS} -o ffviewer main.c ff2png.c
