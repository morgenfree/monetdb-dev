
## This file is generated by autogen.py, do not edit
## Process this file with automake to produce Makefile.in
## autogen includes dependencies so automake doesn't need to generated them

AUTOMAKE_OPTIONS = no-dependencies 1.4 foreign

lib_gsl_la_CFLAGS=-DLIBGSL $(AM_CFLAGS)
gsldir = $(libdir)/monetdb5
lib_gsl_la_LIBADD = $(gsl_LIBS)
$(do)install-gslLTLIBRARIES : 
lib_gsl_la-gsl.lo: gsl.c ../../../../monetdb5/mal/../../gdk/gdk.h ../../../../monetdb5/mal/mal.h ../../../../monetdb5/mal/mal_instruction.h ../../../../monetdb5/mal/mal_type.h ../../../../monetdb5/mal/mal_stack.h ../../../../monetdb5/mal/mal_properties.h ../../../../monetdb5/mal/mal_namespace.h ../../../../monetdb5/mal/mal_errors.h ../../../../monetdb5/mal/mal_exception.h
	$(LIBTOOL) --tag=CC --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lib_gsl_la_CFLAGS) $(CFLAGS) $(gsl_CFLAGS) -c -o lib_gsl_la-gsl.lo `test -f 'gsl.c' || echo '$(srcdir)/'`gsl.c
nodist_lib_gsl_la_SOURCES =
dist_lib_gsl_la_SOURCES = gsl.c
lib_gsl_la_LDFLAGS = -module -avoid-version
gsl.o gsl.lo: gsl.c ../../../../monetdb5/mal/../../gdk/gdk.h ../../../../monetdb5/mal/mal.h ../../../../monetdb5/mal/mal_instruction.h ../../../../monetdb5/mal/mal_type.h ../../../../monetdb5/mal/mal_stack.h ../../../../monetdb5/mal/mal_properties.h ../../../../monetdb5/mal/mal_namespace.h ../../../../monetdb5/mal/mal_errors.h ../../../../monetdb5/mal/mal_exception.h
install-exec-local-46_gsl.sql: 46_gsl.sql
	-mkdir -p $(DESTDIR)$(libdir)/monetdb5/createdb
	-$(RM) $(DESTDIR)$(libdir)/monetdb5/createdb/46_gsl.sql
	$(INSTALL_DATA) $< $(DESTDIR)$(libdir)/monetdb5/createdb/46_gsl.sql

uninstall-local-46_gsl.sql: 
	$(RM) $(DESTDIR)$(libdir)/monetdb5/createdb/46_gsl.sql

AM_CPPFLAGS = -I$(srcdir) -I../../../../monetdb5/mal -I$(srcdir)/../../../../monetdb5/mal -I../../../../common/options -I$(srcdir)/../../../../common/options -I../../../../common/stream -I$(srcdir)/../../../../common/stream -I../../../../common/utils -I$(srcdir)/../../../../common/utils -I../../../../gdk -I$(srcdir)/../../../../gdk
install-exec-local-73_gsl.mal: 73_gsl.mal
	-mkdir -p $(DESTDIR)$(libdir)/monetdb5/autoload
	-$(RM) $(DESTDIR)$(libdir)/monetdb5/autoload/73_gsl.mal
	$(INSTALL_DATA) $< $(DESTDIR)$(libdir)/monetdb5/autoload/73_gsl.mal

uninstall-local-73_gsl.mal: 
	$(RM) $(DESTDIR)$(libdir)/monetdb5/autoload/73_gsl.mal

install-exec-local-gsl.mal: gsl.mal
	-mkdir -p $(DESTDIR)$(libdir)/monetdb5
	-$(RM) $(DESTDIR)$(libdir)/monetdb5/gsl.mal
	$(INSTALL_DATA) $< $(DESTDIR)$(libdir)/monetdb5/gsl.mal

uninstall-local-gsl.mal: 
	$(RM) $(DESTDIR)$(libdir)/monetdb5/gsl.mal

dist-hook:
	mkdir -p $(distdir)/Tests
	cp -pR $(srcdir)/Tests/* $(distdir)/Tests
BUILT_SOURCES =
MOSTLYCLEANFILES =
EXTRA_DIST = Makefile.ag Makefile.msc 46_gsl.sql 73_gsl.mal gsl.c gsl.mal
gsl_LTLIBRARIES = lib_gsl.la
uninstall-local: uninstall-local-46_gsl.sql uninstall-local-73_gsl.mal uninstall-local-gsl.mal
install-exec-local: install-exec-local-46_gsl.sql install-exec-local-73_gsl.mal install-exec-local-gsl.mal

  include $(top_srcdir)/buildtools/conf/rules.mk
