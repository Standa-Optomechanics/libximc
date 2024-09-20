Eng.

========== ABOUT EXAMPLE ==========

This is an extended example for sophisticated programmers who want to write powerful python scripts. The example
contains most of libximc functions and thus can be thought as a demonstration of full libximc power. Also this example
can be used as a reference in case you want to implement something similar.

The example consists of:
* Extendtestpython.py					-	main file that provides UI. It uses: SelectionManager to choose a 8SMC5
											controller (and	connection type) and GeneralManager to control opened
											device.

* EXTIO_Manager/extio_manager.py		-	provides EXTIOManager that manages EXTIO settings: it lets you set
											appropriate flags. Used in GeneralManager class.

* Movement_Manager/movement_manager.py	-	provides MovementManager that manages movement: it lets you move left,
											move right, move to position, etc. Used in GeneralManager class.

* Settings_Manager/settings_manager.py	-	provides SettingsManager that manages general settings: speed, acceleration,
											deceleration, feedback type, microstep settings, etc. Used in GeneralManager
											class.

* support_routines.py					-	contains a decorator for handling wrong input.

Note: Python version 3.9 IS NOT SUPPORTED!

=============== RUN ===============
On Windows: 
	1. Run the example with the command: python Extendtestpython.py

On Linux/MacOS:
   To run the example, you can go two ways:
	1. Run ./Extendtestpython.sh
	2.
		* Install packages from the /ximc/deb archive folder: libximc7_x.x.x and libximc7-dev_x.x.x. Install strictly in the specified order.
		* Set LD_LIBRARY_PATH to let Python find libraries using RPATH. For that use:
			# specify the correct path for installed packages.
			export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:usr/lib
		* Run the example with the command: python Extendtestpython.py

========================================================================================================================

Rus.

======= ПАРА СЛОВ О ПРИМЕРЕ =======

Перед Вами расширенный пример для искушённых программистов, желающих писать продвинутые python скрипты. Пример содержит
бОльшую часть функционала библиотеки libximc, и поэтому может рассматриваться как демонстрация возможностей библиотеки.
Кроме того по образу и подобию представленного примера Вы можете написать свой собственный скрипт с аналогичным
функционалом.

Пример состоит из:
* Extendtestpython.py					-	главный файл, предоставляющий интерфейс. Extendtestpython.py использует
											SelectionManager для выбора 8SMC5 (и типа соединения) и GeneralManager для
											управления выбранным устройством.

* EXTIO_Manager/extio_manager.py		-	предоставляет EXTIOManager, который отвечает за настройку EXTIO: позволяет
											установить необходимые флаги. Используется в классе GeneralManager.

* Movement_Manager/movement_manager.py	-	предоставляет MovementManager, который отвечает за контроль движения:
											движение влево, движение вправо, движение в позицию и т.д. Используется в
											классе GeneralManager.

* Settings_Manager/settings_manager.py	-	предоставляет SettingsManager, который отвечает за общие настройки
											контроллера: настройку скорости, ускорения, типа обратной связи, режима
											микрошага и т.д. Используется в классе GeneralManager.

* support_routines.py					-	содержит декоратор для обработки неверного ввода.

Замечание: Python версии 3.9 НЕ ПОДДЕРЖИВАЕТСЯ!

========== КАК ЗАПУСТИТЬ ==========
Windows:
	Запустите пример командой: python Extendtestpython.py

Linux/MacOS:
	Для запуска примера можно пойти двумя путями:
	1. Запустить скрипт ./Extendtestpython.sh
	2.
		* Установите пакеты из папки /ximc/deb архива: libximc7_x.x.x и libximc7-dev_x.x.x. Устанавливать в указанном порядке! 
		* Установите LD_LIBRARY_PATH, чтобы Python мог находить библиотеки с помощью RPATH. Это можно сделать, например, с помощью:
			# Укажите правильный путь для установленных пакетов.
			export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:usr/lib
		* Запустите пример командой: python Extendtestpython.py

========================================================================================================================
