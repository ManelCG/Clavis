CLAVIS_VERSION = "1.1.0"

SDIR = src

IDIR = include
#CCCMD = x86_64-w64-mingw32-gcc
CCCMD = gcc
CFLAGS = -I$(IDIR) `pkg-config --cflags --libs gtk+-3.0` -lcrypto -Wall -Wno-deprecated-declarations

debug: CC = $(CCCMD) -DDEBUG_ALL -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)_DEBUG\"
debug: BDIR = debug

release: CC = $(CCCMD) -O2 -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)\"
release: BDIR = build

windows: CC = $(CCCMD) -O2 -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)\" -mwindows
windows: BDIR = build

install: CC = $(CCCMD) -O2	-DMAKE_INSTALL -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)\"
install: CLAVIS_DIR = /usr/lib/clavis
install: BDIR = $(CLAVIS_DIR)/bin

archlinux: CC = $(CCCMD) -O2 -DMAKE_INSTALL -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)\"

ODIR=.obj
LDIR=lib

LIBS = -lm -lpthread

_DEPS = clavis_popup.h clavis_normal.h clavis_constants.h gui_templates.h file_io.h folderstate.h algorithms.h clavis_passgen.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o clavis_popup.o clavis_normal.o gui_templates.o file_io.o folderstate.o algorithms.o clavis_passgen.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

release: $(OBJ)
	mkdir -p $(BDIR)
	mkdir -p $(ODIR)
	$(CC) -o $(BDIR)/clavis $^ $(CFLAGS) $(LIBS)

windows: $(OBJ)
	windres __windows__/my.rc -O coff $(ODIR)/my.res
	windres __windows__/appinfo.rc -O coff $(ODIR)/appinfo.res
	mkdir -p $(BDIR)
	mkdir -p $(ODIR)
	$(CC) -o $(BDIR)/clavis $^ $(CFLAGS) $(LIBS) $(ODIR)/my.res $(ODIR)/appinfo.res
	ldd build/clavis.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" build/

debug: $(OBJ)
	mkdir -p $(BDIR)
	mkdir -p $(ODIR)
	$(CC) -o $(BDIR)/clavis $^ $(CFLAGS) $(LIBS)

install: $(OBJ)
	mkdir -p $(CLAVIS_DIR)
	mkdir -p $(BDIR)
	mkdir -p $(ODIR)
	$(CC) -o $(BDIR)/clavis $^ $(CFLAGS) $(LIBS)
	ln -sf $(BDIR)/clavis /usr/bin/clavis
	cp assets/clavis.desktop /usr/share/applications/
	cp assets/app_icon/256.png /usr/share/pixmaps/clavis.png

archlinux: $(OBJ) $(OBJ_GUI)
	mkdir -p $(BDIR)/usr/lib/clavis
	mkdir -p $(BDIR)/usr/share/applications
	mkdir -p $(BDIR)/usr/share/pixmaps
	mkdir -p $(BDIR)/usr/bin/
	mkdir -p $(ODIR)
	$(CC) -o $(BDIR)/usr/bin/clavis $^ $(CFLAGS) $(LIBS)
	cp -r assets/ $(BDIR)/usr/lib/clavis/
	cp assets/clavis.desktop $(BDIR)/usr/share/applications/
	cp assets/app_icon/256.png $(BDIR)/usr/share/pixmaps/clavis.png

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

.PHONY: all
all: release clean
