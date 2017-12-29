# FAQ

## Are you sure cropping JPEG in RPhoto is lossless ?

Yes RPhoto is able to do lossless cropping and rotating. It uses for that the command line tool jpegtran. Please check that it is available in your path (if you used the ppa, that should be the case with the package dependancies). Please note that compression blocks are really small (8x8), and thus, the rounding to the next 8x8 block is pratically unnoticeable for large images.

It is also capable of cropping with losses. Please note the light bulb button next the Fit on page button. It is green when lossless cropping is used, and turn red for operations with losses. It can turn red if you click on it, or use some operation not compatible with lossless (ie free rotation, flip and resize), and with non JPEG file formats. RPhoto will try a first run of jpegtran with option -perfect and display a message (if activated in options) if it fails. 

Note that as stated in the jpegtran manpage :
1. the rounding to blocksize does only apply for left upper corner (not for right down corner, I guess this is handled by image size)  
2. the  rounding is silent, even if -perfect option is set. 
