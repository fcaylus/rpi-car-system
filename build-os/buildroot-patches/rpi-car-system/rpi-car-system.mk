################################################################################
#
# rpi-car-system
#
################################################################################

RPI_CAR_SYSTEM_SOURCE = rpi-car-system-sources.tar.gz
RPI_CAR_SYSTEM_SITE = ${TOPDIR}/..
RPI_CAR_SYSTEM_SITE_METHOD = file

RPI_CAR_SYSTEM_DEPENDENCIES = \
	vlc-qt \
	qt5base \
	qt5declarative \
	eudev \
	libmediainfo \
	libarchive \
	ncurses

define RPI_CAR_SYSTEM_CONFIGURE_CMDS
	(cd $(@D); $(TARGET_MAKE_ENV) $(QT5_QMAKE) CONFIG+=READY_FOR_CARSYSTEM "HOST_BIN_DIR = $(HOST_DIR)/usr/bin")
endef

define RPI_CAR_SYSTEM_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef

define RPI_CAR_SYSTEM_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) install INSTALL_ROOT=$(TARGET_DIR)
endef

$(eval $(generic-package))
