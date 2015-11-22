RPI-CAR-SYSTEM
==============

This project aims to create a full-featured headunit.

Project components:
-------------------


- build-os : Scripts to build to complete os
	- build-os/build-all.sh : Bash script that downloads all needed packages and create a system tarball
	- build-os/flash-all.sh : Bash script that flash the tarballs into a SD-Card (for the rpi boot system)
- launcher: Main GUI app. This app will be launched on startup.
- musicindex-generator: simple app that will create the media index. This task cannot be executed in the launcher app since VlcInstance is not thread safe.
- updatemanager: manage updates. If this app or passwordmanager need to be updated, a physical intervention is required
