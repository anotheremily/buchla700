cp68 -I c:\lib\include\ %1.c e:\tmp\cc.i
c068 e:\tmp\cc.i d:\tmp\cc.1 %1.2 e:\tmp\cc.3 -e
rm e:\tmp\cc.i
c168 d:\tmp\cc.1 %1.2 %1.s -l
rm d:\tmp\cc.1
rm %1.2
as68 -f e:\tmp\ -l -u -s c:\bin\ %1.s
rm %1.s
beep
