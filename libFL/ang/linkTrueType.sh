#! /bin/sh -e

fp=/usr/share/data/fonts
ep=`echo -n $FL_FONT_PATH`
if [ -n "$ep" ]; then
  fp=$ep
fi

cd $fp
ln -s times.ttf Times-Roman
ln -s arial.ttf Helvetica
ln -s cour.ttf Utopia-Regular
