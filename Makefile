all: 
	@if [ "`uname -s`" = "Solaris" ] ; then \
		make -f Makefile.sunos ; \
	else \
		make -f Makefile.linux ; \
	fi

clean: 
	@if [ "`uname -s`" = "Solaris" ] ; then \
		make clean -f Makefile.sunos ; \
	else \
		make clean -f Makefile.linux ; \
	fi
