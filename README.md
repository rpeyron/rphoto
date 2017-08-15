# rphoto

RPhoto is a small software aiming at the easy handling of digital camera's photos. (RPhoto is the next generation of IMPhoto). 
Its origin resides in the lack of a simple software capable of cropping photos with a constant ratio, to avoid white borders when printing.

Main features are :
  * Crop images with a constant width / height ratio (by example 4:3 for numeric photos)
  * Rotate / Flip photos
  * Lossless crop / rotate / flip operations.
  * Available under Linux and Windows.

See the user manual to have the extensive list of features.

Homepage : http://www.lprp.fr/soft/rphoto/rphoto_en.php3

# Install

See prebuilt binary packages in release tab for Windows and Debian.

PPA repository for Ubuntu : [ppa:rpeyron/ppa](https://launchpad.net/~rpeyron/+archive/ubuntu/ppa)

Other OSes should build the package (see below).

# Build instructions

## Dependancies

RPhoto depends on :
- wxWidgets  (https://www.wxwidgets.org/)
- libexif (http://libexif.sourceforge.net/)
- libjpeg (included)
- wxVillaLib (included and patched)

## Linux build

RPhoto is a standard autotools project :

```
./configure
make
```

You may install dependancies on debian systems by :
```
# Build dependancies
sudo apt-get install pkg-config libexif-dev libwxgtk2.8-dev libjpeg8-dev zip libtool autoconf automake autopoint 
# Run dependancies
sudo apt-get install libwxgtk2.8-0 libexif12 libjpeg-progs jhead
```


## Windows build

Windows Build is done with Visual Studio Community Edition. 
Please use the latest version in build/ (older vc version are not maintained)


# Contributing

Contributions are welcome. You may use GitHub issues tracker for issues, or GitHub Pull Requests.

# Screenshot 

![Screenshot](http://www.lprp.fr/soft/rphoto/rphoto_scr.jpg)

