
# Hardware version always equals to the major version
# Only changes on hardware modifications
HARDWARE_VERSION = 0
MINOR_VERSION = 0
PATCH_VERSION = 1
APPLICATION_VERSION = $${HARDWARE_VERSION}.$${MINOR_VERSION}.$${PATCH_VERSION}

# Add defines to compiler
DEFINES += \
    "APPLICATION_VERSION=\\\"$${APPLICATION_VERSION}\\\"" \
    "HARDWARE_VERSION=$${HARDWARE_VERSION}" \
    "MINOR_VERSION=$${MINOR_VERSION}" \
    "PATCH_VERSION=$${PATCH_VERSION}"
