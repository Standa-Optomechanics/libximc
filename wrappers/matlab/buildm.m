dir
[notfound, warnings] = loadlibrary('libximc.dll', 'ximcm.h', 'mfilename', 'ximcm.m', 'includepath', '..\..\ximc\win64', 'addheader', 'ximc.h')
if ~isempty(notfound)
  exit(1)
end
unloadlibrary('libximc')
exit
