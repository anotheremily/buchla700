rm f:\tmp\*
rm f:\tmp\*.*
f:\cp68 -I c:\lib\include\ %1.c f:\tmp\cc.i
f:\c068 f:\tmp\cc.i f:\tmp\cc.1 f:\tmp\cc.2 f:\tmp\cc.3 -e
rm f:\tmp\cc.i
f:\c168 f:\tmp\cc.1 f:\tmp\cc.2 %1.s -l
rm f:\tmp\cc.1
rm f:\tmp\cc.2
f:\as68 -f f:\tmp\ -l -u -s c:\bin\ %1.s
rm %1.s
beep
