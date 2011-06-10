c168 %1.1 %1.2 %1.s -ld
rm %1.1
rm %1.2
as68 -l -u -s c:\bin\ %1.s
rm %1.s
wait.prg
