AC_INIT([tracker], [0.1.0], [foo@bar.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_FILES([Makefile src/Makefile])
# Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADER(bcm2835.h, , [AC_MSG_ERROR([Couldn't find bcm2835.h...try downloading the source from http://www.airspayce.com/mikem/bcm2835/] )])
AC_CHECK_HEADER(gps.h, , [AC_MSG_ERROR([Couldn't find gps.h...try downloading the source from http://catb.org/gpsd/] )])

AC_OUTPUT
