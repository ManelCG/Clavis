CLAVIS_VERSION = "1.3.0"

SDIR = src

IDIR = include
LOCALENAME = clavis
CCCMD = gcc
CFLAGS = -I$(IDIR) `pkg-config --cflags --libs gtk+-3.0` -lcrypto -Wall -Wno-deprecated-declarations -DCLAVIS_LOCALE_=\"$(LOCALENAME)\"

debug: CC = $(CCCMD) -DDEBUG_ALL -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)_DEBUG\"
debug: BDIR = build

release: CC = $(CCCMD) -O2 -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)\"
release: BDIR = build

windows: CC = $(CCCMD) -O2 -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)\" -mwindows
windows: BDIR = build
windows_GTKENV: BDIR = build

install: CC = $(CCCMD) -O2	-DMAKE_INSTALL -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)\"
install: CLAVIS_DIR = /usr/lib/clavis
install: BDIR = $(CLAVIS_DIR)/bin

archlinux: CC = $(CCCMD) -O2 -DMAKE_INSTALL -DCLAVIS_VERSION=\"$(CLAVIS_VERSION)\"

locale: LOCALEDIR = locale

ODIR=.obj/linux
WODIR=.obj/win
DODIR=.obj/debug
LDIR=lib

LIBS = -lm -lpthread

_DEPS = clavis_popup.h clavis_normal.h clavis_constants.h gui_templates.h file_io.h folderstate.h algorithms.h clavis_passgen.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o clavis_popup.o clavis_normal.o gui_templates.o file_io.o folderstate.o algorithms.o clavis_passgen.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
WOBJ = $(patsubst %,$(WODIR)/%,$(_OBJ))
DOBJ = $(patsubst %,$(DODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(WODIR)/%.o: $(SDIR)/%.c $(DEPS)
	mkdir -p $(WODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(DODIR)/%.o: $(SDIR)/%.c $(DEPS)
	mkdir -p $(DODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

release: $(OBJ)
	mkdir -p $(BDIR)
	mkdir -p $(ODIR)
	$(CC) -o $(BDIR)/clavis $^ $(CFLAGS) $(LIBS)

windows: $(WOBJ)
	mkdir -p $(WODIR)
	windres __windows__/my.rc -O coff $(WODIR)/my.res
	windres __windows__/appinfo.rc -O coff $(WODIR)/appinfo.res
	mkdir -p $(BDIR)
	$(CC) -o $(BDIR)/clavis $^ $(CFLAGS) $(LIBS) $(WODIR)/my.res $(WODIR)/appinfo.res

windows_GTKENV: windows
	ldd $(BDIR)/clavis.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" $(BDIR)/
	cp -ru __windows__/windows_assets/* $(BDIR)/
	cp -ru assets $(BDIR)/
	cp -ru locale/ $(BDIR)/
	cp LICENSE $(BDIR)/


debug: $(DOBJ)
	mkdir -p $(BDIR)
	mkdir -p $(DODIR)
	$(CC) -o $(BDIR)/clavis_DEBUG $^ $(CFLAGS) $(LIBS)

install: $(OBJ)
	mkdir -p $(CLAVIS_DIR)
	mkdir -p $(BDIR)
	mkdir -p $(ODIR)
	mkdir -p /usr/lib/clavis
	$(CC) -o $(BDIR)/clavis $^ $(CFLAGS) $(LIBS)
	ln -sf $(BDIR)/clavis /usr/bin/clavis
	cp -ru assets/ /usr/lib/clavis
	cp -ru locale/ /usr/lib/clavis
	cp LICENSE /usr/lib/clavis
	cp assets/clavis.desktop /usr/share/applications/
	cp assets/app_icon/256.png /usr/share/pixmaps/clavis.png

archlinux: $(OBJ) $(OBJ_GUI)
	mkdir -p $(BDIR)/usr/lib/clavis
	mkdir -p $(BDIR)/usr/share/applications
	mkdir -p $(BDIR)/usr/share/pixmaps
	mkdir -p $(BDIR)/usr/bin/
	mkdir -p $(ODIR)
	$(CC) -o $(BDIR)/usr/bin/clavis $^ $(CFLAGS) $(LIBS)
	cp -ru assets/ $(BDIR)/usr/lib/clavis/
	cp -ru locale/ $(BDIR)/usr/lib/clavis/
	cp LICENSE $(BDIR)/usr/lib/clavis/
	cp assets/clavis.desktop $(BDIR)/usr/share/applications/
	cp assets/app_icon/256.png $(BDIR)/usr/share/pixmaps/clavis.png

locale: $(LOCALEDIR)/es/LC_MESSAGES/$(LOCALENAME).mo $(LOCALEDIR)/ru/LC_MESSAGES/$(LOCALENAME).mo $(LOCALEDIR)/ca/LC_MESSAGES/$(LOCALENAME).mo

$(LOCALEDIR)/%/LC_MESSAGES/$(LOCALENAME).mo: $(LOCALEDIR)/%/$(LOCALENAME).po
	msgfmt --output-file=$(LOCALEDIR)/$*/LC_MESSAGES/$(LOCALENAME).mo $(LOCALEDIR)/$*/$(LOCALENAME).po
$(LOCALEDIR)/%/$(LOCALENAME).po: $(LOCALEDIR)/$(LOCALENAME).pot
	msgmerge --update $(LOCALEDIR)/$*/$(LOCALENAME).po $(LOCALEDIR)/$(LOCALENAME).pot
	mkdir -p $(LOCALEDIR)/$*/LC_MESSAGES
$(LOCALEDIR)/$(LOCALENAME).pot: $(SDIR)/*
	xgettext --keyword=_ --language=C --from-code=UTF-8 --add-comments --sort-output -o $(LOCALEDIR)/$(LOCALENAME).pot $(SDIR)/*.c

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(WODIR)/*.o $(DODIR)/*.o *~ core $(INCDIR)/*~

.PHONY: all
all: release clean
