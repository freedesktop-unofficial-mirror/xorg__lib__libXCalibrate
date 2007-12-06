#
# Copyright © 2003 Philip Blundell
#
# Permission to use, copy, modify, distribute, and sell this software and its
# documentation for any purpose is hereby granted without fee, provided that
# the above copyright notice appear in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation, and that the name of Philip Blundell not be used in
# advertising or publicity pertaining to distribution of the software without
# specific, written prior permission.  Philip Blundell makes no
# representations about the suitability of this software for any purpose.  It
# is provided "as is" without express or implied warranty.
#
# PHILIP BLUNDELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
# EVENT SHALL PHILIP BLUNDELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
# DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#

AM_CFLAGS = $(XCALIBRATE_CFLAGS) $(X_CFLAGS)

lib_LTLIBRARIES = libXcalibrate.la

libXcalibrate_la_SOURCES = 	\
	xcalibrate.c

libXcalibrate_la_LIBADD = @XCALIBRATE_LIBS@

libXcalibrateincludedir = $(includedir)/X11/extensions
libXcalibrateinclude_HEADERS = xcalibrate.h 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = xcalibrate.pc

EXTRA_DIST = xcalibrate.pc.in

EXTRA_DIST += ChangeLog
MAINTAINERCLEANFILES = ChangeLog

.PHONY: ChangeLog

ChangeLog:
	(GIT_DIR=$(top_srcdir)/.git git-log > .changelog.tmp && mv .changelog.tmp ChangeLog; rm -f .changelog.tmp) || (touch ChangeLog; echo 'git directory not found: installing possibly empty changelog.' >&2)

dist-hook: ChangeLog
