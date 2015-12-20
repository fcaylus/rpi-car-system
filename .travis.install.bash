#!/bin/bash
set -ev

if [ $NO_CROSS_COMPILE_DEBUG -eq 1 ] || [ $NO_CROSS_COMPILE -eq 1 ]; then
    sudo add-apt-repository -y ppa:beineri/opt-qt551-trusty
    sudo apt-get update -q
    sudo apt-get install -yq build-essential libncurses5-dev rsync cpio python unzip bc wget qt55base qt55declarative qt55graphicaleffects qt55quickcontrols qt55tools qt55translations qt55xmlpatterns qt55svg
fi

