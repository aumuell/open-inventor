#! /bin/sh -e

dps=/usr/lib/DPS/outline/base
type1=/usr/lib/X11/fonts/Type1

if [ ! -d $dps ]; then
  echo -n "$dps does not exist; create? "
  read reply
  if [ $reply = "y" ]; then
    mkdir -p $dps
  else
    exit
  fi
fi

for i in $type1/*.pfa; do
  out=$dps`egrep ^/FontName $i | cut -d' ' -f2`
  if [ ! -e $out ]; then
    ln -s $i $out
  fi
done
