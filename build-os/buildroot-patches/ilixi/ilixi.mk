################################################################################
#
# ilixi
#
################################################################################

ILIXI_VERSION = 1.0.0
ILIXI_SITE = http://ilixi.org/releases/
ILIXI_SOURCE = ilixi-$(ILIXI_VERSION).tar.gz
ILIXI_LICENSE = GPLv3+
ILIXI_LICENSE_FILES = COPYING
ILIXI_INSTALL_STAGING = YES
ILIXI_AUTORECONF = YES

ILIXI_CONF_OPTS = \
	--enable-shared \
	--disable-static \
	--disable-nls \
	--disable-doxygen-doc

ILIXI_DEPENDENCIES = directfb libxml2 libsigc

$(eval $(autotools-package))
