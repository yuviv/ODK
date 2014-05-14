#!/bin/bash

FILES=./*.jpg
for f in $FILES
do
  file=${f#./*}
  num=${file:0:1}
  out=$num'_'$RANDOM'.jpg'
  echo $out
  crop=`convert $f -virtual-pixel edge -fuzz 20% -trim -format '%wx%h%O' info:`
  convert $f -crop $crop +repage $out
done
