################################################################################
#
# libmediainfo
#
################################################################################

LIBMEDIAINFO_VERSION = 0.7.86
LIBMEDIAINFO_SITE = https://github.com/MediaArea/MediaInfoLib/archive
LIBMEDIAINFO_SOURCE = v$(LIBMEDIAINFO_VERSION).tar.gz
LIBMEDIAINFO_LICENSE = BSD-2-Clause
LIBMEDIAINFO_LICENSE_FILES = License.html
LIBMEDIAINFO_DEPENDENCIES = host-pkgconf
LIBMEDIAINFO_AUTORECONF = YES
LIBMEDIAINFO_INSTALL_STAGING = YES

LIBMEDIAINFO_SUBDIR = Project/GNU/Library

LIBMEDIAINFO_CONF_OPTS += \
	--disable-static \
	--enable-shared

LIBMEDIAINFO_DEPENDENCIES += libzen zlib

$(eval $(autotools-package))

