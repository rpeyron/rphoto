RPhoto is a small software aiming at the easy handling of digital camera's photos. (RPhoto is the next generation of IMPhoto). 
Its origin resides in the lack of a simple software capable of cropping photos with a constant ratio, to avoid white borders when printing.

Main features are :
  * Crop images with a constant width / height ratio (by example 4:3 for numeric photos)
  * Rotate / Flip photos
  * Lossless crop / rotate / flip operations.
  * Available under Linux and Windows.

See the user manual to have the extensive list of features.
A [FAQ](doc/FAQ.md) is also available on this github

Homepage : http://www.lprp.fr/soft/rphoto/rphoto_en.php3

# Install

See prebuilt binary packages in release tab for Windows and Debian.

PPA repository for Ubuntu : [ppa:rpeyron/ppa](https://launchpad.net/~rpeyron/+archive/ubuntu/ppa)

Debian repository :
```
# Add repository
sudo echo "deb [arch=amd64]  http://www.lprp.fr/debian stable main" > /etc/apt/sources.list.d/lprp.list
# Add apt key (remi+debian@via.ecp.fr)
sudo apt-key adv --keyserver keys.gnupg.net --recv-keys 090B93891134CECB
# Install
sudo apt-get install rphoto
```


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
sudo apt-get install pkg-config libexif-dev libwxgtk3.0-dev libjpeg8-dev zip libtool autoconf automake autopoint 
# Run dependancies
sudo apt-get install libwxgtk3.0-0v5 libexif12 libjpeg-progs jhead
```


## Windows build

Windows Build is done with Visual Studio Community Edition. 
Please use the latest version in build/ (older vc version are not maintained)

Starting with vc2019, dependancies may be managed with vcpkg : 
```
vcpkg integrate install
vcpkg --triplet x86-windows-static install wxwidgets 
vcpkg --triplet x86-windows-static install libexif
```


# Contributing

Contributions are welcome. You may use GitHub issues tracker for issues, or GitHub Pull Requests.

# Screenshot 

![Screenshot](https://www.lprp.fr/files/2017/10/rphoto_scr.jpg)

