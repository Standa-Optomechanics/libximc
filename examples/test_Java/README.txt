Eng.

===== RUN =====
On Windows:
	1. To run the example for win32 or win64, you need to install java and register the JDK variable. 
	2. Create in the environment variables JDK_HOME and JDK32_HOME with the value C:\Program Files\Java\jdk-9.0.4
	3. Add a value to the Path variable C:\Program Files\Java\jdk-9.0.4\bin

Example: JDK is installed in C:\Program Files\Java\jdk-9.0.4 => My Computer -> Properties -> Additional System Parameters -> Environment Variables -> System variables, there we create the variables JDK_HOME and JDK32_HOME with the value C:\Program Files\Java\jdk-9.0.4. Adding a value to the Path variable C:\Program Files\Java\jdk-9.0.4\bin
 
	4. In the example folder, at the command line, type:
		$ java -classpath libjximc.jar -classpath test_Java.jar ru.ximc.TestJava
	5. For other platforms, use the compiled folder. You will need to copy the dependencies into it:
		$ xiwrapper.dll, libximc.lib, libximc.dll, libximc.def, libjximc.jar, libjximc.dll, bindy.lib и bindy.dll

===== For modify =====
1. Unpack jar:
	$ jar xvf test_Java.jar ru META-INF
2. Modyfy sources
3. Build example:
	$ javac -classpath libjximc.jar -Xlint ru/ximc/TestJava.java
	$ jar cmf META-INF\MANIFEST.MF test_Java.jar ru
 
When assembling the example, it is necessary to pay attention to the fact that only one path to the Java developer's system is registered in the system paths, otherwise a situation may arise when compilation will be performed by one version of the package, and assembly or execution by another, which may lead to errors.


Rus.

===== ЗАПУСК =====
Windows:
	1. Для запуска примера для win32 или win64 необходимо установить java и прописать переменную JDK. 
	2. Создать в переменных средах JDK_HOME и JDK32_HOME со значением C:\Program Files\Java\jdk-9.0.4
	3. Добавит в переменную Path значение C:\Program Files\Java\jdk-9.0.4\bin

Пример: JDK установлен в C:\Program Files\Java\jdk-9.0.4 => Мой компьютер -> Свойства -> Дополнительные параметры системы -> Переменные среды -> Системные переменные, там мы создаем переменные JDK_HOME и JDK32_HOME со значением C:\Program Files\Java\jdk-9.0.4. Добавляем в переменную Path значение C:\Program Files\Java\jdk-9.0.4\bin
 
	4. Перейти в папку с примером набрать в командной строке: 
		$ java -classpath libjximc.jar -classpath test_Java.jar ru.ximc.TestJava
	5. Для других платформ используйте папку compiled. В нее понадобится скопировать зависимости: 
		xiwrapper.dll, libximc.lib, libximc.dll, libximc.def, libjximc.jar, libjximc.dll, bindy.lib и bindy.dll 
 
===== Для модификации примера =====
1. Распакуйте jar командой:
	$ jar xvf test_Java.jar ru META-INF
2. Модифицируйте исходные коды
3. Соберите пример:
	$ javac -classpath libjximc.jar -Xlint ru/ximc/TestJava.java
	$ jar cmf META-INF/MANIFEST.MF test_Java.jar ru
 
При сборке примера необходимо обратить внимание на то, что в системных путях прописан только один путь к системе разработчика Java, иначе может возникнуть ситуация, когда компиляция будет выполняться одной версией пакета, а сборка или выполнение в другой, что может привести к ошибкам.