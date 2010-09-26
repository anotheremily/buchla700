as68 -s c:\bin\ -l wait.s
link68 wait.68k=wait.o
relmod wait
rm wait.o
rm wait.68k
