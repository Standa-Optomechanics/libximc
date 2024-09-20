Eng.

===== RUN =====
1. Before launch:
	On Windows: 
		Before the start nothing needs to be done. Change current directory in the MATLAB to the examples/matlab.
		The library with dependencies is located in the ximc/win** folders. For the example to work, you need the following files: bindy.dll, libximc.dll, xiwrapper.dll.
	On Linux: 
		Install libximc*deb and libximc-dev*dev of target architecture. Then copy ximc/macosx/wrappers/ximcm.h to the directory examples/test_MATLAB. Install gcc compatible with Matlab.
	On macOS: 
		Copy ximc/macosx/libximc.framework, ximc/macosx/wrappers/ximcm.h, ximc/ximc.h to the directory examples/test_MATLAB. Install XCode compatible with Matlab. 
2. Then launch in MATLAB prompt.


Rus.

===== ЗАПУСК =====
1. Перед запуском:
	Windows: 
		Перед запуском ничего не нужно делать. Изменить текущий каталог в среде MATLAB в examples/matlab.
		Библиотека с зависимостями находится в папках ximc/win**. Для работы примера неоходимы следующие файлы: bindy.dll, libximc.dll, xiwrapper.dll.
	Linux: 
		Установите libximc*deb и libximc-dev*dev целевой архитектуры. Затем скопируйте ximc/macosx/wrappers/ximcm.h в каталог examples/test_MATLAB. 
		Установите gcc, совместимый с Matlab.
	macOS: 
		Скопируйте ximc/macosx/libximc.framework, ximc/macosx/wrappers/ximcm.h, ximc/ximc.h в каталог examples/test_MATLAB. Установите XCode, совместимый с Matlab.
2. Затем запустите программу в командной строке MATLAB.
