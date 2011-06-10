move *.o c:\tests\
cd \tests
rm f:\*.*
rm midas.abs
rm midas.sym
newver
link68 [tem[f:],ab,tex[10000],s,com[ram.ldr]]
dir midas.abs
sizes midas.abs
