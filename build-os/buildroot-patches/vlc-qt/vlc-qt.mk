################################################################################
#
# Vlc-Qt
#
################################################################################

VLC_QT_VERSION = 1.1.0
VLC_QT_SITE = https://github.com/vlc-qt/vlc-qt/archive
VLC_QT_SOURCE = $(VLC_QT_VERSION).tar.gz
VLC_QT_LICENSE = LGPLv3
VLC_QT_LICENSE_FILES = LICENSE.md
VLC_QT_INSTALL_STAGING = YES

VLC_QT_DEPENDENCIES = \
	qt5base \
	libvlc

$(eval $(cmake-package))
