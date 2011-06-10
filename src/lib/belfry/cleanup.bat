copy c:\bin\find.prg f:\
f:\find c:\ -name '*.map' -print  >clean.bat
f:\find c:\ -name '*.srt' -print >>clean.bat
f:\find c:\ -name '*.tmp' -print >>clean.bat
f:\find c:\ -name '*.prn' -print >>clean.bat
f:\find d:\ -name '*.map' -print >>clean.bat
f:\find d:\ -name '*.srt' -print >>clean.bat
f:\find d:\ -name '*.tmp' -print >>clean.bat
f:\find d:\ -name '*.prn' -print >>clean.bat
f:\find e:\ -name '*.map' -print >>clean.bat
f:\find e:\ -name '*.srt' -print >>clean.bat
f:\find e:\ -name '*.tmp' -print >>clean.bat
f:\find e:\ -name '*.prn' -print >>clean.bat
me clean.bat
