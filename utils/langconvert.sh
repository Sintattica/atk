#!/bin/bash
### LangConverter V1.0 (final)
# This is the languageconverter script with Perl!
# It converts all languagefiles it can find (recursive to its location)
# to the new (array)(ATK5) format and renames them to ISO 639-1 compliant names
# It requires version 4 of sed and a recent version of perl
# IMPORTANT!!!: Back up your old language files as langconvert.sh will convert it and
#               just throw your old file away.
# Disclaimer: Using this script at your own risk!
###
LANGUAGEFILES="lang"
EXTENSION="lng"
BINDIR="/bin"

function getiso()
{
  case  "$1" in
"nederlands.$EXTENSION")
   ISO="nl";;
"english.$EXTENSION")
   ISO="en";;
"brazilian_portuguese.$EXTENSION")
   ISO="pt";;
"chinese.$EXTENSION")
   ISO="zh";;
"czech.$EXTENSION")
   ISO="cs";;
"danish.$EXTENSION")
   ISO="da";;
"deutsch.$EXTENSION")
   ISO="de";;
"finnish.$EXTENSION")
   ISO="fi";;
"french.$EXTENSION")
   ISO="fr";;
"hungarian.$EXTENSION")
   ISO="hu";;
"italian.$EXTENSION")
   ISO="it";;
"norwegian.$EXTENSION")
   ISO="no";;
"russian.$EXTENSION")
   ISO="ru";;
"spanish.$EXTENSION")
   ISO="es";;
esac
}

find | grep ".lng$" > $LANGUAGEFILES

if test -f $LANGUAGEFILES
then
 exec<$LANGUAGEFILES
 while read line
 do
  FILEPATH=${line%/*}
  FILE=${line##*/}
  echo $FILE > langconvertfilename
  PREFIX=$(perl -pe "s/(.*_)?.*/\1/;" langconvertfilename)
  if [ ! $PREFIX = "brazilian_" ] 
  then
    FILE=$(perl -pe "s/(.*)_//;" langconvertfilename)
  fi
  rm langconvertfilename
  getiso $FILE

  if [ ! $PREFIX ] || [ $PREFIX = "brazilian_" ]
  then
    $BINDIR/cat $line | $BINDIR/sed -e "2i \$$ISO=array(" -e 's/\$txt_/"/g' -e '/=>/!s/=/" => /g' -e 's/;/,/g' -e '$i );' > temprary
    /usr/bin/perl -pe 's/"((\s*[^"\s])*)\s*"/"\1"/g' temprary > $FILEPATH/$ISO.$EXTENSION
    echo converted $line to $FILEPATH/$ISO.$EXTENSION
  else
    cat $line > $FILEPATH/$PREFIX$ISO.$EXTENSION
    echo moved $line to $FILEPATH/$PREFIX$ISO.$EXTENSION
  fi

  $BINDIR/rm -f $line
 done
fi

$BINDIR/rm -f $LANGUAGEFILES
$BINDIR/rm -f temprary
