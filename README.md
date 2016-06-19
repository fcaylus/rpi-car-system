RPI Car System [![Build Status](https://travis-ci.org/Tranqyll/rpi-car-system.svg?branch=master)](https://travis-ci.org/Tranqyll/rpi-car-system) [![Code Climate](https://codeclimate.com/github/Tranqyll/rpi-car-system/badges/gpa.svg)](https://codeclimate.com/github/Tranqyll/rpi-car-system) [![Issue Count](https://codeclimate.com/github/Tranqyll/rpi-car-system/badges/issue_count.svg)](https://codeclimate.com/github/Tranqyll/rpi-car-system)
==============

Full-featured headunit system. This project is in an early development stage and not ready for production.

Project components:
-------------------

- build-os : Scripts to build to complete OS
	- build-os/build-all.sh : Downloads all packages and create a system tarball (using buildroot)
	- build-os/flash-all.sh : Flash the tarball into a SD-Card
- launcher: Main GUI app. This app will be launched on startup.
- musicindex-generator: Creates and updates the media index.

Third-parties
-------------

RPI Car system uses some third-party libraries to work correctly:

* [Qt] - used for all GUIs
* [PugiXml] - reading xml files using XPath
* [Vlc-Qt] - all medias stuff
* [MiniFlatFlags] - used for icons in the "languages" menu
* [Material Icons] - all icons in the launcher

License
-------

Licensed under the [GNU GPL] license v3. See LICENSE file for more information.

[Qt]:http://www.qt.io/developers/
[PugiXml]:https://github.com/zeux/pugixml
[Vlc-Qt]:https://github.com/vlc-qt/vlc-qt
[MiniFlatFlags]:https://github.com/pixelstrolch/MiniFlatFlags
[Material Icons]:https://www.google.com/design/icons/
[GNU GPL]:http://www.gnu.org/licenses/gpl-3.0.en.html
