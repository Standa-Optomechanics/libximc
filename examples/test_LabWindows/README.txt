Eng.

What are the examples?

The testcli example is a console application for the LabWindows CVI environment using the libximc library.
The testgui example is an application with a User Interface Editor for the LabWindows CVI environment using the libximc library.
The work of the examples was tested on the LabWindows CVI 2017 version.

===== RUN =====
On Windows:
	1. Open the project ..\examples\test_LabWindows\testcli\testcli.prj or ..\examples\test_LabWindows\testgui\simple.prj in LabWindows CVI.
	2. By default, the examples have a 64-bit build installed, and the 64-bit libximc library is connected to the example.
	3. To build, run Build->Rebuild.
	4. If you need to build a 32-bit version of the application, select one of the 32-bit builds in the Build->Configuration menu.
	Remove the 64-bit version of the libximc.lib library from the project and connect the 32-bit one.
	5. The libximc library with dependencies is located in the ximc/win32 and ximc/win64 folders.
	6. To run the compiled example, you need to copy bindy.dll, libximc.dll and xiwrapper.dll from the folder of the corresponding ximc/win** bit depth to the folder with the executable file.


Rus.

Что представляют собой примеры.

testcli - пример представляет собой консольное приложение для среды LabWindows CVI использующее библиотеку libximc.
testgui - пример представляет собой приложение с User Interface Editor для среды LabWindows CVI использующее библиотеку libximc.
Работа примеров проверялась на версии LabWindows CVI 2017.

===== ЗАПУСК =====
Windows:
	1. Открыть проект ..\examples\test_LabWindows\testcli\testcli.prj или ..\examples\test_LabWindows\testgui\simple.prj в LabWindows CVI.
	2. По умолчанию в примерах установлена 64-битная сборка, и к примеру подключена 64-битная библиотека libximc.
	3. Для сборки необходимо выполнить Build->Rebuild.
	4. Если необходимо собрать 32-битную версию приложения в меню Build->Configuration выбрать одну из 32-битных сборок. 
	Удалить из проекта 64-битную версию библиотеки libximc.lib и подключить 32-битную.
	5. Библиотека libximc с зависимостями расположена в папках ximc/win32 и ximc/win64.
	6. Для запуска скомпилированного примера необходимо скопировать bindy.dll, libximc.dll и xiwrapper.dll из папки соответствующей разрядности ximc/win** в папку с выполняемым файлом.
