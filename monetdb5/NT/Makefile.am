
## This file is generated by autogen.py, do not edit
## Process this file with automake to produce Makefile.in
## autogen includes dependencies so automake doesn't need to generated them

AUTOMAKE_OPTIONS = no-dependencies 1.4 foreign

script_M5server.bat: M5server.bat
install-exec-local-M5server.bat: M5server.bat
	-mkdir -p $(DESTDIR)$(prefix)
	-$(RM) $(DESTDIR)$(prefix)/M5server.bat
	$(INSTALL) -m0755 $< $(DESTDIR)$(prefix)/M5server.bat

uninstall-local-M5server.bat: 
	$(RM) $(DESTDIR)$(prefix)/M5server.bat

AM_CPPFLAGS = -I$(srcdir)
BUILT_SOURCES = M5server.bat
MOSTLYCLEANFILES =
EXTRA_DIST = Makefile.ag Makefile.msc M5server.bat
bin_SCRIPTS = script_M5server.bat
install-exec-local-SCRIPTS: 
all-local-SCRIPTS: $(bin_SCRIPTS)
uninstall-local: uninstall-local-M5server.bat
install-exec-local: install-exec-local-M5server.bat

  include $(top_srcdir)/buildtools/conf/rules.mk
