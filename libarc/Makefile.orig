CC = gcc
CFLAGS = -O2 -I. -Wall

PREFIX = /usr/local
AR = ar -cr
RANLIB = ranlib
INSTALL = install -c
RM = rm -f

OBJS = inflate.o url.o url_file.o url_inflate.o net.o url_buff.o url_ftp.o \
       url_mem.o url_dir.o url_http.o url_pipe.o arc.o arc_lzh.o \
       arc_tar.o arc_zip.o deflate.o unlzh.o memb.o strtab.o mblock.o \
       explode.o url_uudecode.o url_b64decode.o url_cache.o \
       url_hqxdecode.o url_qsdecode.o url_news.o url_newsgroup.o url_cgiesc.o \
       arc_mime.o

# readdir_win.o (for Windows)

LIB=libarc.a


all:: $(LIB)

$(LIB): $(OBJS)
	$(AR) $@ $(OBJS)
	$(RANLIB) $@

arccat: arccat.o $(LIB)
	$(CC) -o $@ arccat.o $(LIB)

clean::
	$(RM) $(LIB) *.o arccat *~ libarc/*~

install::
	test -d $(PREFIX)/lib || mkdir -p $(PREFIX)/lib
	$(INSTALL) -m 444 $(LIB) $(PREFIX)/lib
	$(RANLIB) $(PREFIX)/lib/$(LIB)
	test -d $(PREFIX)/include/libarc || mkdir -p $(PREFIX)/include/libarc
	$(INSTALL) -m 444 libarc/url.h $(PREFIX)/include/libarc
	$(INSTALL) -m 444 libarc/arc.h $(PREFIX)/include/libarc
	$(INSTALL) -m 444 libarc/mblock.h $(PREFIX)/include/libarc
	$(INSTALL) -m 444 libarc/memb.h $(PREFIX)/include/libarc
