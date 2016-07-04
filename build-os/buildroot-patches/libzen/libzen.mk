################################################################################
#
# libzen
#
################################################################################

LIBZEN_VERSION = 0.4.33
LIBZEN_SITE = https://github.com/MediaArea/ZenLib/archive
LIBZEN_SOURCE = v$(LIBZEN_VERSION).tar.gz
LIBZEN_LICENSE = ZLib
LIBZEN_LICENSE_FILES = License.txt
LIBZEN_DEPENDENCIES = host-pkgconf
LIBZEN_AUTORECONF = YES
LIBZEN_INSTALL_STAGING = YES

LIBZEN_SUBDIR = Project/GNU/Library

LIBZEN_CONF_OPTS += \
	--disable-static \
	--enable-shared

$(eval $(autotools-package))

