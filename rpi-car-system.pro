!unix|macx {
    error(You are not in a linux system !)
}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    launcher \
    musicindex-generator #\
    #updatemanager

