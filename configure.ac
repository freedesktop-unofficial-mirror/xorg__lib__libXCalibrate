AC_PREREQ([2.57])
AC_INIT(libXCalibrate, 0.1.0, [pb@nexus.co.uk])
AM_INIT_AUTOMAKE([foreign dist-bzip2])

# Check for progs
AC_PROG_CC
AC_PROG_LIBTOOL

# Check for X
PKG_CHECK_MODULES(XCALIBRATE, x11 xext xextproto xcalibrateproto)
		  
AC_OUTPUT([Makefile
           xcalibrate.pc])
