rm f:\*
rm f:\*.*
rm f:\tmp\*
rm f:\tmp\*.*
copy c:\bin\ar68.prg f:\
f:\ar68 r f:\sl0 msl.o msm.o asgvce.o chgsef.o m7menu.o procpfl.o
f:\ar68 r f:\sl0 wsdsp.o wdfield.o wdselbx.o wscalc.o instdsp.o idfield.o
f:\ar68 r f:\sl0 idselbx.o initi.o libdsp.o ldfield.o ldselbx.o scope.o
rm c:\tests\sl0
copy f:\sl0 c:\tests\
rm f:\sl0
f:\ar68 r f:\sl1 localkb.o execkey.o scordsp.o scfield.o scselbx.o
f:\ar68 r f:\sl1 asgdsp.o adfield.o adselbx.o tundsp.o tdfield.o tdselbx.o
f:\ar68 r f:\sl1 etstrn.o vtyper.o curset.o sendval.o wheel.o enterit.o
rm c:\tests\sl1
copy f:\sl1 c:\tests\
rm f:\sl1
f:\ar68 r f:\sl2 lcdlbls.o dcopy.o etttab.o ettval.o ettpch.o etwavs.o etwvce.o 
f:\ar68 r f:\sl2 etwhar.o etwhrv.o etwoff.o etwpnt.o etwslt.o showcfg.o etmcfn.o
f:\ar68 r f:\sl2 ptdisp.o ptfield.o ptselbx.o stmproc.o smscrl.o ctcpos.o ptdkey.o
rm c:\tests\sl2
copy f:\sl2 c:\tests\
rm f:\sl2
f:\ar68 r f:\sl3 eticnf.o etidin.o etipnt.o etivce.o etimlt.o etires.o etitim.o
f:\ar68 r f:\sl3 etival.o etiosc.o etiact.o etiwsn.o etsnbt.o dformat.o
rm c:\tests\sl3
copy f:\sl3 c:\tests\
rm f:\sl3
f:\ar68 r f:\sl4 etdyn.o etinst.o etioas.o etloc.o etrel.o etres1.o
f:\ar68 r f:\sl4 etscor.o ettrns.o etvel.o etatab.o etaopt.o etavgr.o
rm c:\tests\sl4
copy f:\sl4 c:\tests\
rm f:\sl4
f:\ar68 r f:\sl5 etaccn.o etagpt.o etagch.o etains.o etadyn.o etatun.o
f:\ar68 r f:\sl5 etaprg.o etaint.o etarat.o etadep.o scinit.o rscript.o
f:\ar68 r f:\sl5 setv2gi.o execins.o barbadj.o
rm c:\tests\sl5
copy f:\sl5 c:\tests\
rm f:\sl5
f:\ar68 r f:\sl6 sqscan.o frfind.o cminit.o puteq.o librw.o setgc.o
f:\ar68 r f:\sl6 im700.o swinit.o itcpos.o ttcpos.o stcpos.o infield.o
f:\ar68 r f:\sl6 select.o fpuint.o seccpy.o chksec.o nedacc.o fcnote.o
rm c:\tests\sl6
copy f:\sl6 c:\tests\
rm f:\sl6
f:\ar68 r f:\sl7 sctrak.o scgoto.o scadv.o uslice.o ucslice.o seexec.o
f:\ar68 r f:\sl7 sreset.o scread.o scwrite.o tofpu.o addfpu.o dec2fr.o
f:\ar68 r f:\sl7 delnote.o delpnts.o pix2mid.o kbobj.o gcurpos.o
f:\ar68 r f:\sl7 sedisp.o sedump.o timeint.o dbentr.o
rm c:\tests\sl7
copy f:\sl7 c:\tests\
rm f:\sl7
rm c:\tests\midas.o
copy c:\ram\midas.o c:\tests\
dir c:\tests\sl?
dir c:\tests\midas.o
