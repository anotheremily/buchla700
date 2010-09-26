rm %1.1
rm %1.2
rm %1.3
cp68 -I c:\lib\include\ %1.c %1.i
c068 %1.i %1.1 %1.2 %1.3 -e
rm %1.i
