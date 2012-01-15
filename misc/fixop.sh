#!/bin/sh

fixop() {
  for f in *.c; do
    [[ -f $f ]] || continue
    cp "$f" "$f~"
  done

  perl -pi -w -e 's/(\s)EQ(\s)/$1\=\=$2/g;' *.c
  perl -pi -w -e 's/(\s)NE(\s)/$1\!\=$2/g;' *.c
  perl -pi -w -e 's/(\s)GE(\s)/$1\>\=$2/g;' *.c
  perl -pi -w -e 's/(\s)LE(\s)/$1\<\=$2/g;' *.c
  perl -pi -w -e 's/(\s)AND(\s)/$1\&\&$2/g;' *.c
  perl -pi -w -e 's/(\s)OR(\s)/$1\|\|$2/g;' *.c
  perl -pi -w -e 's/(\s)GT(\s)/$1\>$2/g;' *.c
  perl -pi -w -e 's/(\s)LT(\s)/$1\<$2/g;' *.c 
  perl -pi -w -e 's/(\s)NOT(\s)/$1\!$2/g;' *.c
  perl -pi -w -e 's/(\()NOT(\s)/$1\!$2/g;' *.c
}

while true; do
    read -p "Change all operator macros (EQ,NE,GE...) to real operators (==,!=,>=) in *.c in current directory? y/n : " yn
    case $yn in
        [Yy]* ) fixop; echo "Done."; exit;;
        [Nn]* ) return;;
        * ) echo "Please answer y or n.";;
    esac
done
