rm f:\tmp\*
rm f:\tmp\*.*
f:\cp68 -I c:\lib\include\ %1.c f:\tmp\cc.i
f:\c068 f:\tmp\cc.i f:\tmp\cc.1 f:\tmp\%1.2 f:\tmp\cc.3 -e -T
rm f:\tmp\cc.i
f:\c168 f:\tmp\cc.1 f:\tmp\%1.2 %1.s -Ta
rm f:\tmp\cc.1
rm f:\tmp\%1.2
