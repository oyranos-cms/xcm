
AM_CFLAGS = -I${top_srcdir} ${libXcm_CFLAGS} ${libX11_CFLAGS} ${libXfixes_CFLAGS} ${libXmu_CFLAGS} $(liboyranos_CFLAGS)
AM_CFLAGS   += -Wall

AM_CFLAGS += -I$(top_srcdir)

bin_PROGRAMS = xcmedid

xcmedid_SOURCES = xcmedid.c
xcmedid_LDADD  = ${libXcmEDID_LIBS} -lm
if HAVE_LINUX
bin_PROGRAMS += xcmddc
xcmddc_SOURCES = xcmddc.c
xcmddc_LDADD  = ${libXcmDDC_LIBS} -lm
endif
if HAVE_X11
bin_PROGRAMS += xcm xcmevents
xcm_SOURCES = xcm.c
xcm_LDADD  = ${libXcmX11_LIBS} ${libX11_LIBS} ${libXfixes_LIBS} \
	${libXmu_LIBS} $(liboyranos_LIBS) -lm
xcmevents_SOURCES = xcmevents.c
xcmevents_LDADD  = ${libXcmX11_LIBS} ${libX11_LIBS} ${libXfixes_LIBS} \
	${libXmu_LIBS} $(liboyranos_LIBS) -lm
endif

