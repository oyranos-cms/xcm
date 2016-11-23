# Xcm

Tools which based on libXcm, a library for color management on X.
* xcmddc:  requests EDID from a monitor over the i2c bus.
* xcmedid:  is for parsing EDID data blocks. 
* xcmhextobin: convert hex encoded strings to binary data for xcmedid
* xcmevents: observes X11 color management events.


## Internet
* sources: [git clone git://github.com/oyranos-cms/xcm](https://github.com/oyranos-cms/xcm)
* www: [OpenICC](http://www.openicc.info)
* support: [email list](http://lists.freedesktop.org/mailman/listinfo/openicc)


## Dependencies:
* [Xorg](http://www.x.org)
* [libxcm-dev](https://github.com/oyranos-cms/libxcm)
* [liboyranos-dev](https://github.com/oyranos-cms/oyranos) - optionally


## Building
    $ make
    $ make install


