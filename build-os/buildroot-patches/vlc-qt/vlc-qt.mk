################################################################################
#
# Vlc-Qt
#
################################################################################

VLC_QT_VERSION = 1.0.0
VLC_QT_SITE = https://github.com/vlc-qt/vlc-qt/archive
VLC_QT_SOURCE = $(VLC_QT_VERSION).tar.gz
VLC_QT_INSTALL_STAGING = YES

$(eval $(cmake-package))
