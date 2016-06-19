################################################################################
#
# libvlc
#
################################################################################

LIBVLC_VERSION = 2.2.3
LIBVLC_SITE = http://get.videolan.org/vlc/$(LIBVLC_VERSION)
LIBVLC_SOURCE = vlc-$(LIBVLC_VERSION).tar.xz
LIBVLC_LICENSE = GPLv2+ LGPLv2.1+
LIBVLC_LICENSE_FILES = COPYING COPYING.LIB
LIBVLC_DEPENDENCIES = host-pkgconf
LIBVLC_AUTORECONF = YES
LIBVLC_INSTALL_STAGING = YES

# VLC defines two autoconf functions which are also defined by our own pkg.m4
# from pkgconf. Unfortunately, they are defined in a different way: VLC adds
# --enable- options, but pkg.m4 adds --with- options. To make sure we use
# VLC's definition, rename these two functions.
define LIBVLC_OVERRIDE_PKG_M4
	$(SED) 's/PKG_WITH_MODULES/VLC_PKG_WITH_MODULES/g' \
		-e 's/PKG_HAVE_WITH_MODULES/VLC_PKG_HAVE_WITH_MODULES/g' \
		$(@D)/configure.ac $(@D)/m4/with_pkg.m4
endef
LIBVLC_POST_PATCH_HOOKS += LIBVLC_OVERRIDE_PKG_M4

LIBVLC_CONF_OPTS += \
	--disable-rpath \
	--disable-nls \
	--disable-winstore-app \
	--enable-dbus \
	--disable-mmx \
	--disable-sse \
	--disable-neon \
	--disable-altivec \
	--disable-lua \
	--disable-httpd \
	--disable-vlm \
	--disable-addonmanagermodules \
	--disable-live555 \
	--disable-dc1394 \
	--disable-dv1394 \
	--disable-linsys \
	--disable-dvdread \
	--disable-dvdnav \
	--disable-bluray \
	--disable-opencv \
	--disable-smbclient \
	--disable-sftp \
	--disable-v4l2 \
	--disable-decklink \
	--disable-gnomevfs \
	--disable-vcdx \
	--disable-vcd \
	--disable-libcddb \
	--disable-vnc \
	--disable-freerdp \
	--disable-realrtsp \
	--disable-macosx-eyetv \
	--disable-macosx-qtkit \
	--disable-macosx-avfoundation \
	--disable-asdcp \
	--disable-dvdpsi \
	--disable-gme \
	--disable-sid \
	--enable-ogg \
	--enable-mux_ogg \
	--disable-shout \
	--enable-mkv \
	--enable-mod \
	--disable-mpc \
	--disable-wma-fixed \
	--disable-shine \
	--disable-crystalhd \
	--enable-mad \
	--disable-gst-decode \
	--enable-avcodec \
	--disable-libva \
	--disable-dxva2 \
	--enable-avformat \
	--enable-swscale \
	--disable-postproc \
	--enable-faad \
	--enable-vpx \
	--enable-twolame \
	--enable-fdkaac \
	--disable-quicktime \
	--disable-a52 \
	--disable-dca \
	--enable-flac \
	--enable-libmpeg2 \
	--enable-vorbis \
	--disable-tremor \
	--disable-speex \
	--disable-opus \
	--disable-theora \
	--disable-schroedinger \
	--enable-png \
	--enable-jpeg \
	--disable-x262 \
	--disable-x265 \
	--disable-x26410b \
	--enable-x264 \
	--disable-mfx \
	--disable-fluidsynth \
	--disable-vbi \
	--enable-telx \
	--enable-libass \
	--disable-kate \
	--disable-tiger \
	--disable-gles1 \
	--disable-gles2 \
	--without-x \
	--disable-xcb \
	--disable-xvideo \
	--disable-vdpau \
	--disable-sdl \
	--disable-sdl-image \
	--enable-freetype \
	--enable-fribidi \
	--enable-fontconfig \
	--enable-svg \
	--enable-svgdec \
	--disable-directfb \
	--disable-aa \
	--disable-caca \
	--disable-kva \
	--disable-mmal-vout \
	--enable-alsa \
	--disable-pulse \
	--disable-oss \
	--disable-wasapi \
	--disable-audioqueue \
	--enable-jack \
	--disable-opensles \
	--enable-samplerate \
	--disable-kai \
	--disable-chromaprint \
	--disable-qt \
	--disable-skins2 \
	--disable-libtar \
	--disable-macosx \
	--disable-minimal-macosx \
	--disable-ncurses \
	--disable-lirc \
	--disable-goom \
	--disable-projectm \
	--disable-vsxu \
	--disable-atmo \
	--disable-glspectrum \
	--disable-bonjour \
	--disable-udev \
	--disable-mtp \
	--disable-upnp \
	--enable-libxml2 \
	--disable-libgcrypt \
	--disable-gnutls \
	--enable-taglib \
	--disable-update-check \
	--disable-growl \
	--disable-notify \
	--disable-vlc \
	--disable-macosx-vlc-app

LIBVLC_DEPENDENCIES += \
	dbus \
	libogg \
	libmatroska \
	libmodplug \
	libmad \
	ffmpeg \
	faad2 \
	libvpx \
	twolame \
	fdk-aac \
	flac \
	libmpeg2 \
	libvorbis \
	libpng \
	libjpeg \
	x264 \
	libass \
	freetype \
	libfribidi \
	fontconfig \
	libsvg \
	librsvg \
	alsa-lib \
	jack2 \
	libsamplerate \
	libxml2 \
	taglib

$(eval $(autotools-package))
