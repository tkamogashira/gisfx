INCLUDES = \
	-I$(top_srcdir)/libnkf \
	-I$(top_srcdir)/libarc

I_LIBS = $(top_srcdir)/libarc/libarc.a $(top_srcdir)/libnkf/libnkf.a

bin_PROGRAMS = GISfx test drawxml arccat

arccat_SOURCES = arccat.c

arccat_LDADD = $(I_LIBS)

drawxml_SOURCES = drawxml.cpp XGL.cpp XGL.h

drawxml_LDADD = $(I_LIBS)

test_SOURCES = test.cpp GISFile.cpp  SalFile.cpp \
Data.h  GISFile.h  SalFile.h TextureSDL_ttf.cpp TextureSDL_ttf.h \
XGL.cpp XGL.h

test_LDADD = $(I_LIBS)

GISfx_SOURCES = GISFile.cpp  GISfx.cpp  SalFile.cpp  TextureImage.cpp \
Data.h  GISFile.h  SalFile.h  TextureImage.h \
TextureSDL_ttf.cpp TextureSDL_ttf.h XGL.cpp XGL.h \
Mesh50M.cpp Mesh50M.h Viewer.cpp Viewer.h \
SLMset.cpp SLMset.h DisplayList.cpp DisplayList.h

GISfx_LDADD = $(I_LIBS)

