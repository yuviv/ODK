#!/bin/bash

FILES=./*.jpg
for f in $FILES
do
  crop=`convert $f -virtual-pixel edge -fuzz 15% -trim -format '%wx%h%O' info:`
  convert $f -crop $crop +repage $f
done
