#ifndef INC_XIMC_H
#define INC_XIMC_H

/** @file ximc.h
	* \english
	*		@brief Header file for libximc library
	* \endenglish
	* \russian
	*		@brief Заголовочный файл для библиотеки libximc
	* \endrussian
	*/

/** @def XIMC_API
	* \english
	* 		@brief Library import macro.
	* 		Macros allows to automatically import function from shared library.
	* 		It automatically expands to dllimport on msvc when including header file.
	* \endenglish
	* \russian
	*		@brief Макрос импорта библиотеки.
	*		Макросы позволяют автоматически импортировать функцию из общей библиотеки.
	*		Он автоматически расширяется до dllimport на msvc при включении файла заголовка.
	* \endrussian
	*/
#if defined(_WIN32) || defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT) || defined(MATLAB_IMPORT)
	#define XIMC_API __stdcall
#else
	#ifdef LIBXIMC_EXPORTS
	#define XIMC_API __attribute__((visibility("default")))
	#else
	#define XIMC_API
	#endif
#endif

/** @def XIMC_CALLCONV
	* \english
	*		@brief Library calling convention macros.
	* \endenglish
	* \russian
	*		@brief Библиотека вызывающая условные макросы.
	* \endrussian
	*/
#if defined(_WIN32) || defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT) || defined(MATLAB_IMPORT)
	#define XIMC_CALLCONV __stdcall
#else
	#define XIMC_CALLCONV
#endif

/** @def XIMC_RETTYPE
	* \english
	* 		@brief Thread return type.
	* \endenglish
	* \russian
	*		@brief Возвращаеемый тип потока.
	* \endrussian
	*/
#if defined(_WIN32) || defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT) || defined(MATLAB_IMPORT)
#define XIMC_RETTYPE unsigned int
#else
#define XIMC_RETTYPE void*
#endif


#if !defined(XIMC_NO_STDINT)

#if ( (defined(_MSC_VER) && (_MSC_VER < 1600)) || defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT)) && !defined(MATLAB_IMPORT)
// msvc types burden
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

/* labview doesn't speak C99 */
#if defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT)
typedef unsigned __int64 ulong_t;
typedef __int64 long_t;
#else
typedef unsigned long long ulong_t;
typedef long long long_t;
#endif

#endif

#include <time.h>

#if defined(__cplusplus)
extern "C"
{
#endif


	/**
		\english
		* Type describes device identifier
		\endenglish
		\russian
		* Тип идентификатора устройства
		\endrussian
		*/
	typedef int device_t;

	/**
		\english
		* Type specifies result of any operation
		\endenglish
		\russian
		* Тип, определяющий результат выполнения команды.
		\endrussian
		*/
	typedef int result_t;

	/**
		\english
		* Type describes device enumeration structure
		\endenglish
		\russian
		* Тип, определяющий структуру данных о всех контроллерах, обнаруженных при опросе устройств.
		\endrussian
		*/
	#if defined(_WIN64) || defined(__LP64__) || defined(LABVIEW64_IMPORT)
	typedef uint64_t device_enumeration_t;
	#else
	typedef uint32_t device_enumeration_t;
	#endif
	//typedef device_enumeration_t* pdevice_enumeration_t;

	/**
		\english
		* Handle specified undefined device
		\endenglish
		\russian
		* Макрос, означающий неопределенное устройство
		\endrussian
		*/
#define device_undefined -1

	/** \english
		@name Result statuses
		\endenglish
		\russian
		@name Результаты выполнения команд
		\endrussian
		*/
	//@{

	/**
		\english
		* success
		\endenglish
		\russian
		* выполнено успешно
		\endrussian
		*/
#define result_ok 0

	/**
		\english
		* generic error
		\endenglish
		\russian
		* общая ошибка
		\endrussian
		*/
#define result_error -1

	/**
		\english
		* function is not implemented
		\endenglish
		\russian
		* функция не определена
		\endrussian
		*/
#define result_not_implemented -2

	/**
		\english
		* value error
		\endenglish
		\russian
		* ошибка записи значения
		\endrussian
		*/
#define result_value_error -3

	/**
		\english
		* device is lost
		\endenglish
		\russian
		* устройство не подключено
		\endrussian
		*/
#define result_nodevice -4

	//@}

	/** \english
		@name Logging level
		\endenglish
		\russian
		@name Уровень логирования
		\endrussian
		*/
	//@{

	/**
		\english
		* Logging level - error
		\endenglish
		\russian
		* Уровень логирования - ошибка
		\endrussian
		*/
#define LOGLEVEL_ERROR 		0x01
	/**
		\english
		* Logging level - warning
		\endenglish
		\russian
		* Уровень логирования - предупреждение
		\endrussian
		*/
#define LOGLEVEL_WARNING 	0x02
	/**
		\english
		* Logging level - info
		\endenglish
		\russian
		* Уровень логирования - информация
		\endrussian
		*/
#define LOGLEVEL_INFO		0x03
	/**
		\english
		* Logging level - debug
		\endenglish
		\russian
		* Уровень логирования - отладка
		\endrussian
		*/
#define LOGLEVEL_DEBUG		0x04
	//@}


    /**
		\english
		* Calibration structure
		*
		* Where to find all values for calculations?
		* - XILab (don’t forget to load the profile for your positioner. The profile should match the full name of your positioner, e.g., 8MT173-25-MEn1.cfg):
		*   - In XILab settings, go to the "User units" tab. Divide the second number by the first one — this is your A coefficient.
		*   - In the "DC motor" / "BLDC motor" / "Stepper motor" tab (depending on your motor type), find the "Encoder counts per turn" field and use it in the formula for calculating coefficient B.
		* - Profile file (open with any text editor; make sure it matches the full name of your positioner, e.g., 8MT173-25-MEn1.cfg):
		*   - Find Step_multiplier= and Unit_multiplier=. Divide the second by the first — this is your A coefficient.
		*   - Find Encoder_CPT= and use this value in the B coefficient formula instead of ENCODER_COUNTS_PER_TURN.

		* How to calculate Speed, Accel, Decel, and AntiplaySpeed in user units when using a stepper motor with encoder or DC/BLDC motor?
		* 1. Load the correct profile in XILab for your positioner (e.g., 8MT173-25-MEn1.cfg).
		* 2. Enable Feedback encoder mode if not already enabled.
		* 3. Enter speed in the "Working speed" field in user units.
		* 4. In the "User units" tab, disable the "User units" flag to see RPM.
		* 5. Multiply the RPM value from "Working speed" by coefficient B.
		*    Example: 480 * 0.0000009375 = 0.00045. This value for the 8MT173-25-MEn1 in Encoder mode equals 2 mm/s.
		*
		*    Acceleration, deceleration, and antiplay speed are calculated the same way.
		\endenglish

		\russian
		* Структура калибровок
		*
		* Где найти все значения для расчета? 
		* - XILab (не забудьте загрузить профиль для вашего позиционера. Профиль должен соответствовать полному названию вашего позиционера. Например: 8MT173-25-MEn1.cfg): 
		*   - В настройках XILab перейдите во вкладку user units. Разделите второе число на первое — это и будет коэффициент A
		*   - В настройках XILab, перейдите во вкладку DC motor/BLDC motor/Stepper motor (зависит от используемого типа двигателя). Подставьте значение из поля Encoder counts per turn в формулу подсчета B коэффициента
		* - Профиль (откройте профиль любым текстовым редактором. Профиль должен соответствовать полному названию вашего позиционера. Например: 8MT173-25-MEn1.cfg): 
		*   - Найдите в файле профиля поля Step_multiplier= и Unit_multiplier=. Разделите второе число на первое — это и будет коэффициент A
		*   - Найдите в файле профиля поле Encoder_CPT=. Подставьте значение из этого поля в формулу подсчета B коэффициента вместо ENCODER_COUNTS_PER_TURN

		* Как посчитать Speed, Accel, Decel и AntiplaySpeed в пользовательских единицах при использовании шагового двигателя с энкодером или DC/BLDC двигателей? 
		* 1. Используя XILab, загрузите профиль для вашего позиционера. Профиль должен соответствовать полному названию вашего позиционера. Например: 8MT173-25-MEn1.cfg
		* 2. Включите режим Feedback encoder, если он не был включен ранее
		* 3. Вводите скорость в пользовательских единицах в поле Working speed
		* 4. Во вкладке User units выключите флаг User units. Это позволит видеть значение в поле Working speed в RPM
		* 5. Умножьте значение из поля Working speed (в RPM) на коэффициент B. Например: 480 * 0.0000009375 = 0.00045. Значение 0.00045 для позиционера 8MT173-25-MEn1 в режиме Encoder будет равно скорости 2 мм/сек
		*
		* Ускорение, замедление и скорость в режиме антилюфта считаются аналогично
		\endrussian	*/
	typedef struct calibration_t
	{
		double A; /**<
		 * \english
		 * Conversion coefficient equal to the number of millimeters (or other user units) per one step.
		 * Must be non-zero and positive.
		 * Used for position and movement conversion.
		 * - For stepper motor without encoder, only one coefficient A is used. Conversion formula: [user_unit/steps].
		 *   Example: 800 steps = 1 mm, then A = 1/800 = 0.00125
		 * - When using a stepper motor with encoder or DC/BLDC motors, the position is set in counts,
		 *   but speed, acceleration/deceleration, and antiplay speed are set in RPM,
		 *   so two coefficients are required:
		 *   - A. Conversion for position: [user_unit/counts]. Example: 16000 counts = 1 mm, then A = 1/16000 = 0.0000625
		 *   - B. Conversion for speed, acceleration/deceleration, and antiplay speed: [60/ENCODER_COUNTS_PER_TURN * A].
		 *     Example: 60 / 4000 * 0.0000625 = 0.0000009375
		 * \endenglish
		 * \russian
		 * Коэффициент преобразования, равный количеству миллиметров (или других пользовательских единиц) на один шаг.
		 * Должен быть отличным от нуля и положительным.
		 * Используется для пересчёта положений и перемещений.
		 * - Для шагового двигателя без энкодера используется только один коэффициент A. Формула пересчёта: [user_unit/steps].
		 *   Пример: 800 шагов = 1 мм, тогда A = 1/800 = 0.00125
		 * - При использовании шагового двигателя с энкодером или двигателей DC/BLDC позиция задаётся в counts,
		 *   но скорость, ускорение/замедление и скорость в режиме антилюфта задаются в RPM,
		 *   поэтому требуются два коэффициента:
		 *   - A. Формула пересчёта для позиции: [user_unit/counts]. Пример: 16000 counts = 1 мм, тогда A = 1/16000 = 0.0000625
		 *   - B. Формула пересчёта для скорости, ускорения/замедления и скорости в режиме антилюфта: [60/ENCODER_COUNTS_PER_TURN * A].
		 *     Пример: 60 / 4000 * 0.0000625 = 0.0000009375
		 * \endrussian
		 */
		unsigned int MicrostepMode;			/**< \english Controller setting which is determine a step division mode \endenglish \russian Настройка контроллера, определяющая режим пошагового деления. \endrussian */
	} calibration_t;

	/**
		\english
		* Device network information structure.
		\endenglish
		\russian
		* Структура данных с информацией о сетевом устройстве.
		\endrussian	 */
	typedef struct device_network_information_t
	{
		uint32_t ipv4; 		/**< \english IPv4 address, passed in network byte order (big-endian byte order) \endenglish \russian IPv4-адрес, передаваемый в сетевом байтовом порядке (big-endian byte order) \endrussian */
		char nodename[16]; 		/**< \english name of the Bindy node which hosts the device \endenglish \russian имя узла Bindy, на котором размещено устройство \endrussian */
		
		uint32_t axis_state; 		/**< \english flags representing device state \endenglish \russian флаги, представляющие состояние устройства \endrussian */
		char locker_username[16]; 		/**< \english name of the user who locked the device (if any) \endenglish \russian имя пользователя, заблокировавшего устройство (если таковое имеется) \endrussian */
		char locker_nodename[16]; 		/**< \english Bindy node name, which was used to lock the device (if any) \endenglish \russian имя узла Bindy, которое использовалось для блокировки устройства (если таковое имеется) \endrussian */
		time_t locked_time; 		/**< \english time the lock was acquired at (UTC, microseconds since the epoch) \endenglish \russian время, в которое была получена блокировка (UTC, микросекунды с момента начала эпохи) \endrussian */
	} device_network_information_t;


/* @@GENERATED_CODE@@ */

/* hand-crafted functions begin */

	/**
		* \english
		* Reboot to firmware
		* @param id an identifier of device
		* @param[out] ret RESULT_OK, if reboot to firmware is possible. Reboot is done after reply to this command. RESULT_NO_FIRMWARE, if firmware is not found. RESULT_ALREADY_IN_FIRMWARE, if this command was sent when controller is already in firmware.
		* \endenglish
		* \russian
		* Перезагрузка в прошивку в контроллере
		* @param id идентификатор устройства
		* @param[out] ret RESULT_OK, если переход из загрузчика в прошивку возможен. После ответа на эту команду выполняется переход. RESULT_NO_FIRMWARE, если прошивка не найдена. RESULT_ALREADY_IN_FIRMWARE, если эта команда была вызвана из прошивки.
		* \endrussian
		*/
	result_t XIMC_API goto_firmware(device_t id, uint8_t* ret);

	/**
		* \english
		* Check for firmware on device
		* @param uri a uri of device
		* @param[out] ret non-zero if firmware existed
		* \endenglish
		* \russian
		* Проверка наличия прошивки в контроллере
		* @param uri уникальный идентификатор ресурса устройства
		* @param[out] ret ноль, если прошивка присутствует
		* \endrussian
		*/
	result_t XIMC_API has_firmware(const char* uri, uint8_t* ret);

	/**
		* \english
		* Update firmware. Manufacturer only.
		* Service command
		* @param uri a uri of device
		* @param data firmware byte stream
		* @param data_size size of byte stream
		* \endenglish
		* \russian
		* Обновление прошивки. Команда только для производителя.
		* @param uri идентификатор устройства
		* @param data указатель на массив байтов прошивки
		* @param data_size размер массива в байтах
		* \endrussian
		*/
	result_t XIMC_API command_update_firmware(const char* uri, const uint8_t* data, uint32_t data_size);

/**
	* \english
	* Write controller key.
	* Can be used by manufacturer only
	* @param uri a uri of device
	* @param[in] key protection key. Range: 0..4294967295
	* \endenglish
	* \russian
	* Запись ключа защиты
	* Функция используется только производителем.
	* @param uri идентификатор устройства
	* @param[in] key ключ защиты. Диапазон: 0..4294967295
	* \endrussian
	*/
	result_t XIMC_API write_key (const char* uri, uint8_t* key);

/**
	* \english
	* Reset controller.
	* Can be used by manufacturer only
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Перезагрузка контроллера.
	* Функция используется только производителем.
	* @param id идентификатор устройства
	* \endrussian
	*/
	result_t XIMC_API command_reset(device_t id);

/**
	* \english
	* Clear controller FRAM.
	* Can be used by manufacturer only
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Очистка FRAM памяти контроллера.
	* Функция используется только производителем.
	* @param id идентификатор устройства
	* \endrussian
	*/
	result_t XIMC_API command_clear_fram(device_t id);

	//@}

	// ------------------------------------

	/**
		\english
		@name Boards and drivers control
		* Functions for searching and opening/closing devices
		\endenglish
		\russian
		@name Управление устройством
		* Функции поиска и открытия/закрытия устройств
		\endrussian
		*/
	//@{

	/**
		* \english
		* Open a device with OS \a uri and return identifier of the device which can be used in calls.
		* @param[in] uri - a device URI.
		* Device URI has a form of "xi-com:port" or "xi-net://host/serial" or "xi-emu:///abs_path_to_file". For POSIX systems one can ommit root-slash in abs_path_to_file; for example, "xi-emu:///home/user/virt_controller.bin". 
		* In case of USB-COM port, the "port" is the OS device URI. For example, "xi-com:\\\\\.\\COM3" in Windows (note that double-backslash will be transformed to single-backslash) or "xi-com:///dev/ttyACM0" in Linux/Mac.
		* In case of network device, the "host" is an IPv4 address or fully qualified domain URI (FQDN), "serial" is the device serial number in hexadecimal system.
		* For example, "xi-net://192.168.0.1/00001234" or "xi-net://hostname.com/89ABCDEF".
		* In case of TCP protocol, use "xi-tcp://<ip/host>:<port>.
		* For example, "xi-tcp://192.168.0.1:1818".
		* In case of virtual device, the "abs_file_to_file" is the full path to the virtual device's file. If it doesn't exist, then it is created and initialized with default values.
		* For example, "xi-emu:///C:/dir/file.bin" in Windows or "xi-emu:///home/user/file.bin" in Linux/Mac.
		* \endenglish
		* \russian
		* Открывает устройство по имени \a uri и возвращает идентификатор, который будет использоваться для обращения к устройству.
		* @param[in] uri - уникальный идентификатор устройства.
		* URI устройства имеет вид "xi-com:port" или "xi-net://host/serial" или "xi-emu:///abs_path_to_file". На POSIX системах допускается пропуск "рутовского" слэша; например, "xi-emu:///home/user/virt_controller.bin".
		* Для USB-COM устройства "port" это URI устройства в ОС.
		* Например, "xi-com:\\\\\.\\COM3" в Windows (с учётом экранирования двойные обратные слэши преобразуются в одинарные) или "xi-com:///dev/ttyACM0" в Linux/Mac.
		* Для сетевого устройства "host" это IPv4 адрес или полностью определённое имя домена, "serial" это серийный номер устройства в шестнадцатеричной системе.
		* Например, "xi-net://192.168.0.1/00001234" или "xi-net://hostname.com/89ABCDEF".
		* Для работы по TCP протоколу используйте "xi-tcp://<ip/host>:<port>.
		* Например, "xi-tcp://192.168.0.1:1818".
		* Для виртуального устройства "abs_file_to_file" это путь к файлу с сохраненным состоянием устройства. Если файл не существует, он будет создан и инициализирован значениями по умолчанию.
		* Например, "xi-emu:///C:/dir/file.bin" в Windows или "xi-emu:///home/user/file.bin" в Linux/Mac.
		* \endrussian
		*/
	device_t XIMC_API open_device (const char* uri);

	/**
		* \english
		* Close specified device
		* @param id an identifier of device
		* \note
		* The id parameter in this function is a C pointer, unlike most library functions that use this parameter
		* \endenglish
		* \russian
		* Закрывает устройство
		* @param id - идентификатор устройства
		* \note
		* Параметр id в данной функции является Си указателем, в отличие от большинства функций библиотеки использующих данный параметр
		* \endrussian
		*/
	result_t XIMC_API close_device (device_t* id);

	/**
	* \english
	* Command of loading a correction table from a text file.
	* The correction table is used for position correction in case of mechanical inaccuracies.
	* It works for some parameters in _calb commands.
	* @param id an identifier the device
	* @param[in] namefile - the file name must be either a full path or a relative path. If the file name is set to NULL,
	* the correction table will be cleared. File format: two tab-separated columns. Column headers are strings.
	* Data is real, the dot is a delimiter. The first column is a coordinate. The second one is the deviation
	* caused by a mechanical error. The maximum length of a table is 100 rows. Coordinate column must be sorted in
	* ascending order.
	* @see command_move
	* @see get_position_calb
	* @see get_position_calb_t
	* @see get_status_calb
	* @see status_calb_t
	* @see get_edges_settings_calb
	* @see set_edges_settings_calb
	* @see edges_settings_calb_t
	*
	* \endenglish
	* \russian
	* Команда загрузки корректирующей таблицы из текстового файла.
	* Таблица используется для коррекции положения в случае механических неточностей.
	* Работает для некоторых параметров в _calb командах.
	* @param id - идентификатор устройства
	* @param[in] namefile - путь до файла должен быть полным или относительным.
	* Если параметр равен NULL, таблица коррекции будет очищена.
	* Формат файла: два столбца, разделенных табуляцией.
	* Заголовки столбцов строковые.
	* Данные действительные, разделитель точка.
	* Первый столбец - координата. Второй - отклонение, вызванное ошибкой механики.
	* Максимальная длина таблицы 100 строк. Координаты должны быть отсортированы по возрастанию.
	* @see command_move
	* @see command_movr
	* @see get_position_calb
	* @see get_position_calb_t
	* @see get_status_calb
	* @see status_calb_t
	* @see get_edges_settings_calb
	* @see set_edges_settings_calb
	* @see edges_settings_calb_t
	* \endrussian
	*/
	result_t XIMC_API set_correction_table(device_t id, const char* namefile);

	/**
		* \english
		* Check if a device with OS uri \a uri is XIMC device.
		* Be carefuly with this call because it sends some data to the device.
		* @param[in] uri - a device uri
		* \endenglish
		* \russian
		* Проверяет, является ли устройство с уникальным идентификатором \a uri XIMC-совместимым.
		* Будьте осторожны с вызовом этой функции для неизвестных устройств, т.к. она отправляет данные.
		* @param[in] uri - уникальный идентификатор устройства
		* \endrussian
		*/
	result_t XIMC_API probe_device (const char* uri);

	/**
		* \english
		* Search and list of available devices. By default, it creates a list of devices connected to this computer and presented as COM ports.
		* Additionally, you can enable the search for network devices. Devices found in the local network will be included in the same list.<br>
		* To obtain information from the collected list, use the corresponding functions with the <code>get_enumerate_</code> prefix and
		* the received <code>device_enumeration</code> identifier.<br>
		* After finishing working with the list of found devices, you should free up memory using the <code>free_enumerate_devices()</code> function.
		* @param[in] enumerate_flags a set of flags that specify search modes. Flags can be used together via bitwise "OR":
		* <ul>
		* <li><code>ENUMERATE_NETWORK</code> - enables searching for network devices. If the flag is set, network devices will be added to the general list.
		* If the flag is not set, the list will only include devices connected to this computer.</li>
		* <li><code>ENUMERATE_ALL_COM</code> - when enabled, queries all COM port devices in the system. When disabled, queries only devices whose names
		* match the XIMC device mask ("XIMC Motor Controller" in Windows, <code>/dev/ximc/</code> and <code>/dev/ttyACM/</code> on Linux/Mac).</li>
		* <li><code>ENUMERATE_PROBE</code> - enables checking of devices and collecting additional information (serial number, version, model, name...).
		* If this flag is set, only devices that are guaranteed to be open will be added to the list, but devices connected via RS232 converters may not be included.
		* If the flag is not set, the list will include more devices (in particular, devices explicitly listed in <code>hints</code> will be included),
		* but availability and compatibility with this library is not guaranteed.</li>
		* </ul>
		* @param[in] hints additional information to improve the search efficiency. It makes sense to use in case of complex network configuration,
		* when automatic search may not find everything. Format - string <code>"key1=value1\nkey2=value2"</code>. Unknown keys are ignored.
		* One key can have several values, which are listed separated by commas: <code>key=value1,value2,value3</code>. Valid keys:
		* <ul>
		* <li><code>addr</code> - list of URLs of network controllers or servers with connected controllers. The field is used together with the <code>ENUMERATE_NETWORK</code> flag.
		* Protocols and address formats:
		* <ul>
		* <li><code>xi-tcp://&lt;ip-address&gt; - network controllers and controllers connected via Ethernet-RS232 converters. If the <code>ENUMERATE_PROBE</code>
		* flag is not set, all listed devices will be included in the list.</li>
		* <li>xi-net://&lt;ip-address&gt; - network multi-axis systems, xi-net servers. The request for information about the availability of controllers at these
		* addresses will be made regardless of the results of the automatic network search procedure.</li>
		* </ul>
		* </li>
		* <li><code>adapter_addr</code> - list of IP addresses of local network adapters through which the search should be performed.
		* If the key is missing or no adapters are specified, the search is performed on all adapters.</li>
		* </ul>
		* \return <code>device_enumeration</code> - device list identifier. Used to obtain information about devices using functions with <code>get_enumerate_</code> prefixes.
		* \par Examples of use:
		* <pre>
		* <code>
		* // Search for local devices without checking
		* device_enumeration_t device_enumeration = enumerate_devices(0, "");
		* // Search for local devices with verification
		* device_enumeration_t device_enumeration = enumerate_devices(ENUMERATE_PROBE, "");
		* // Fully automatic search for local and network devices
		* device_enumeration_t device_enumeration = enumerate_devices(ENUMERATE_NETWORK, "");
		* // Search for local and network devices
		* // using the local computer adapter with the address 192.168.0.100
		* // and explicit requests to the network controller with the address 192.168.0.11
		* // and the xi-net server with the address 192.168.0.10
		* device_enumeration_t device_enumeration = enumerate_devices(
		* ENUMERATE_NETWORK,
		* "addr=192.168.0.10,xi-tcp://192.168.0.11\nadapter_addr=192.168.0.100" 
		* ); 
		* </code>
		* </pre>
		* \endenglish
		* \russian
		* Поиск и составление списка доступных устройств. По умолчанию формирует список устройств, подключенных к данному компьютеру и представленных в виде COM-портов. 
		* Дополнительно можно включить поиск сетевых устройств. Найденные в локальной сети устройства попадут в тот же список.<br>
		* Для получения информации из собранного списка воспользуйтесь соответствующими функциями с префиксом <code>get_enumerate_</code> и
		* полученным идентификатором <code>device_enumeration</code>.<br>
		* После завершения работы со списком найденных устройств следует освободить память с помощью функции <code>free_enumerate_devices()</code>.
		* @param[in] enumerate_flags набор флагов, задающих режимы поиска. Флаги могут применяться совместно через побитовое "ИЛИ".
		* <ul>
		* <li><code>ENUMERATE_NETWORK</code> - включает поиск сетевых устройств. Если флаг установлен, сетевые устройства будут добавлены в общий список.
		* Если флаг не установлен, в списке будут только устройства, подключенные к данному компьютеру.</li>
		* <li><code>ENUMERATE_ALL_COM</code> - при включенной опции опрашивает все устройства типа COM-порт в системе. При отключенной опции опрашивает только устройства,
		* имена которых соответствуют маске устройств XIMC ("XIMC Motor Controller" в Windows, <code>/dev/ximc/</code> и <code>/dev/ttyACM/</code> на Linux/Mac).</li>
		* <li><code>ENUMERATE_PROBE</code> - включает проверку устройств и сбор дополнительной информации (серийный номер, версию, модель, имя...).
		* Если данный флаг установлен, в список будут добавлены только устройства, которые гарантированно можно открыть, но могут не попадать устройства,
		* подключенные через RS232-преобразователи. Если флаг не установлен, то в списке будет больше устройств (в частности, попадут устройства,
		* явным образом перечисленные в <code>hints</code>), но доступность и совместимость с данной библиотекой не гарантируется.</li>
		* </ul>
		* @param[in] hints дополнительная информация для повышения эффективности поиска. Имеет смысл использовать в случае сложной сетевой конфигурации,
		* когда автоматический поиск может находить не всё. Формат - строка <code>"ключ1=значение1\nключ2=значение2"</code>. Неизвестные ключи игнорируются.
		* Один ключ может иметь несколько значений, которые перечисляются через запятую: <code>ключ=значение1,значение2,значение3</code>. Допустимые ключи:
		* <ul>
		* <li><code>addr</code> - список URLов сетевых контроллеров или серверов с подключенными контроллерами. Поле применяется совместно с флагом <code>ENUMERATE_NETWORK</code>.
		* Протоколы и форматы адресов:
		* <ul>
		* <li><code>xi-tcp://&lt;ip-адрес&gt;</code> - сетевые контроллеры и контроллеры, подключенные через Ethernet-RS232 преобразователи.
		* Если флаг <code>ENUMERATE_PROBE</code> не установлен, все перечисленные устройства попадут в список.</li>
		* <li><code>xi-net://&lt;ip-адрес&gt;</code> - сетевые многоосевые системы, xi-net сервера. Запрос информации о наличии контроллеров по этим адресам будет сделан
		* независимо от результатов автоматической процедуры сетевого поиска.</li>
		* </ul>
		* </li>
		* <li><code>adapter_addr</code></li> - список IP-адресов локальных сетевых адаптеров, через который должен осуществляться поиск. Если ключ отсутствует или ни одного адаптера не указано,
		* то поиск производится на всех адаптерах.</li>
		* </ul>
		* \return <code>device_enumeration</code> - идентификатор списка устройств. Используется для получения информации об устройствах с помощью функций с префиксами <code>get_enumerate_</code>.<br>
		* \par Примеры использования:
		* <pre>
		* <code>
		* // Поиск локальных устройств без проверки
		* device_enumeration_t device_enumeration = enumerate_devices(0, ""); 
		* // Поиск локальных устройств c проверкой
		* device_enumeration_t device_enumeration = enumerate_devices(ENUMERATE_PROBE, "");
		* // Полностью автоматический поиск локальных и сетевых устройств
		* device_enumeration_t device_enumeration = enumerate_devices(ENUMERATE_NETWORK, "");
		* // Поиск локальных и сетевых устройств
		* // с использованием адаптера локального компьютера с адресом 192.168.0.100
		* // и явным обращениям к сетевому контроллеру с адресом 192.168.0.11
		* // и xi-net серверу с адресом  192.168.0.10
		* device_enumeration_t device_enumeration = enumerate_devices(
		* ENUMERATE_NETWORK,
		* "addr=192.168.0.10,xi-tcp://192.168.0.11\nadapter_addr=192.168.0.100"
		* );
		* </code>
		* </pre>
		* \endrussian
	 */
	device_enumeration_t XIMC_API enumerate_devices(int enumerate_flags, const char *hints);

	/**
		* \english
		* Free memory returned by \a enumerate_devices.
		* @param[in] device_enumeration opaque pointer to an enumeration device data
		* \endenglish
		* \russian
		* Освобождает память, выделенную \a enumerate_devices.
		* @param[in] device_enumeration закрытый указатель на данные о перечисленных устойствах
		* \endrussian
	 */
	result_t XIMC_API free_enumerate_devices(device_enumeration_t device_enumeration);

	/**
		* \english
		* Get device count.
		* @param[in] device_enumeration opaque pointer to an enumeration device data
		* \endenglish
		* \russian
		* Возвращает количество подключенных устройств.
		* @param[in] device_enumeration закрытый указатель на данные о перечисленных устойствах
		* \endrussian
	 */
	int XIMC_API get_device_count(device_enumeration_t device_enumeration);

	/**
		* \english
		* Nevermind
		* \endenglish
		* \russian
		* Не обращайте на меня внимание
		* \endrussian
	*/
	typedef char* pchar;

	/**
		* \english
		* Get device name from the device enumeration.
		* Returns \a device_index device name.
		* @param[in] device_enumeration opaque pointer to an enumeration device data
		* @param[in] device_index device index
		* \endenglish
		* \russian
		* Возвращает имя подключенного устройства из перечисления устройств.
		* Возвращает имя устройства с номером \a device_index.
		* @param[in] device_enumeration закрытый указатель на данные о перечисленных устойствах
		* @param[in] device_index номер устройства
		* \endrussian
	 */
	pchar XIMC_API get_device_name(device_enumeration_t device_enumeration, int device_index);


	/**
		* \english
		* Get device serial number from the device enumeration.
		* Returns \a device_index device serial number.
		* @param[in] device_enumeration opaque pointer to an enumeration device data
		* @param[in] device_index device index
		* @param[out] serial device serial number
		* \endenglish
		* \russian
		* Возвращает серийный номер подключенного устройства из перечисления устройств.
		* Возвращает серийный номер устройства с номером \a device_index.
		* @param[in] device_enumeration закрытый указатель на данные о перечисленных устойствах
		* @param[in] device_index номер устройства
		* @param[in] serial серийный номер устройства
		* \endrussian
	 */
	result_t XIMC_API get_enumerate_device_serial(device_enumeration_t device_enumeration, int device_index, uint32_t* serial);

	/**
		* \english
		* Get device information from the device enumeration.
		* Returns \a device_index device information.
		* @param[in] device_enumeration opaque pointer to an enumeration device data
		* @param[in] device_index device index
		* @param[out] device_information device information data
		* \endenglish
		* \russian
		* Возвращает информацию о подключенном устройстве из перечисления устройств.
		* Возвращает информацию о устройстве с номером \a device_index.
		* @param[in] device_enumeration закрытый указатель на данные о перечисленных устойствах
		* @param[in] device_index номер устройства
		* @param[out] device_information информация об устройстве
		* \endrussian
	 */
	result_t XIMC_API get_enumerate_device_information(device_enumeration_t device_enumeration, int device_index, device_information_t* device_information);

	/**
		* \english
		* Get controller name from the device enumeration.
		* Returns \a device_index device controller name.
		* @param[in] device_enumeration opaque pointer to an enumeration device data
		* @param[in] device_index device index
		* @param[out] controller_name controller name
		* \endenglish
		* \russian
		* Возвращает имя подключенного устройства из перечисления устройств.
		* Возвращает имя устройства с номером \a device_index.
		* @param[in] device_enumeration закрытый указатель на данные о перечисленных устойствах
		* @param[in] device_index номер устройства
		* @param[out] controller name имя устройства
		* \endrussian
	 */
	result_t XIMC_API get_enumerate_device_controller_name(device_enumeration_t device_enumeration, int device_index, controller_name_t* controller_name);

	/**
		* \english
		* Get stage name from the device enumeration.
		* Returns \a device_index device stage name.
		* @param[in] device_enumeration opaque pointer to an enumeration device data
		* @param[in] device_index device index
		* @param[out] stage_name stage name
		* \endenglish
		* \russian
		* Возвращает имя подвижки для подключенного устройства из перечисления устройств.
		* Возвращает имя подвижки устройства с номером \a device_index.
		* @param[in] device_enumeration закрытый указатель на данные о перечисленных устойствах
		* @param[in] device_index номер устройства
		* @param[out] stage name имя подвижки
		* \endrussian
	 */
	result_t XIMC_API get_enumerate_device_stage_name(device_enumeration_t device_enumeration, int device_index, stage_name_t* stage_name);

	/**
		* \english
		* Get device network information from the device enumeration.
		* Returns \a device_index device network information.
		* @param[in] device_enumeration opaque pointer to an enumeration device data
		* @param[in] device_index device index
		* @param[out] device_network_information device network information data
		* \endenglish
		* \russian
		* Возвращает сетевую информацию о подключенном устройстве из перечисления устройств.
		* Возвращает сетевую информацию о устройстве с номером \a device_index.
		* @param[in] device_enumeration закрытый указатель на данные о перечисленных устойствах
		* @param[in] device_index номер устройства
		* @param[out] device_network_information сетевая информация об устройстве
		* \endrussian
	 */
	result_t XIMC_API get_enumerate_device_network_information(device_enumeration_t device_enumeration, int device_index, device_network_information_t* device_network_information);

	/** \english
		* Resets the error of incorrect data transmission.
		* \endenglish
		* \russian
		* Сбрасывает ошибку неправильной передачи данных.
		* \endrussian
	*/
	result_t XIMC_API reset_locks ();

	/** \english
		* Sleeps for a specified amount of time
		* @param msec time in milliseconds
		* \endenglish
		* \russian
		* Приостанавливает работу на указанное время
		* @param msec время в миллисекундах
		* \endrussian
		*/
	void XIMC_API msec_sleep (unsigned int msec);

	/** \english
		* Returns a library version
		* @param version a buffer to hold a version string, 32 bytes is enough
		* \endenglish
		* \russian
		* Возвращает версию библиотеки
		* @param version буфер для строки с версией, 32 байт достаточно
		* \endrussian
		*/
	void XIMC_API ximc_version (char* version);

#if !defined(MATLAB_IMPORT) && !defined(LABVIEW64_IMPORT) && !defined(LABVIEW32_IMPORT)

	/** \english
		* Logging callback prototype
		* @param loglevel a loglevel
		* @param message a message
		* \endenglish
		* \russian
		* Прототип функции обратного вызова для логирования
		* @param loglevel уровень логирования
		* @param message сообщение
		* \endrussian
		*/
	typedef void (XIMC_CALLCONV *logging_callback_t)(int loglevel, const wchar_t* message, void* user_data);

	/** \english
		* Simple callback for logging to stderr in wide chars
		* @param loglevel a loglevel
		* @param message a message
		* \endenglish
		* \russian
		* Простая функция логирования на stderr в широких символах
		* @param loglevel уровень логирования
		* @param message сообщение
		* \endrussian
		*/
	void XIMC_API logging_callback_stderr_wide(int loglevel, const wchar_t* message, void* user_data);

	/** \english
		* Simple callback for logging to stderr in narrow (single byte) chars
		* @param loglevel a loglevel
		* @param message a message
		* \endenglish
		* \russian
		* Простая функция логирования на stderr в узких (однобайтных) символах
		* @param loglevel уровень логирования
		* @param message сообщение
		* \endrussian
		*/
	void XIMC_API logging_callback_stderr_narrow(int loglevel, const wchar_t* message, void* user_data);

	/**
		* \english
		* Sets a logging callback.
		* Call resets a callback to default (stderr, syslog) if NULL passed.
		* @param logging_callback a callback for log messages
		* \endenglish
		* \russian
		* Устанавливает функцию обратного вызова для логирования.
		* Вызов назначает стандартный логгер (stderr, syslog), если передан NULL
		* @param logging_callback указатель на функцию обратного вызова
		* \endrussian
		*/
	void XIMC_API set_logging_callback(logging_callback_t logging_callback, void* user_data);

#endif

/**
	* \english
	* Return device state.
	* @param id an identifier of device
	* @param[out] status structure with snapshot of controller status
	* \endenglish
	* \russian
	* Возвращает информацию о текущем состоянии устройства.
	* @param id идентификатор устройства
	* @param[out] status структура с информацией о текущем состоянии устройства
	* \endrussian
	*/
/**
	* \english
	* Device state.
	* Useful structure that contains current controller status, including speed, position and boolean flags.
	* \endenglish
	* \russian
	* Состояние устройства.
	* Эта структура содержит основные параметры текущего состояния контроллера, такие как скорость, позиция и флаги состояния.
	* \endrussian
	* @see get_status
	*/
	result_t XIMC_API get_status (device_t id, status_t* status);

/**
	* \english
	* Return device state.
	* @param id an identifier of device
	* @param[out] status structure with snapshot of controller status
	* @param calibration user unit settings
	* \endenglish
	* \russian
	* Возвращает информацию о текущем состоянии устройства.
	* @param id идентификатор устройства
	* @param[out] status структура с информацией о текущем состоянии устройства
	* @param calibration настройки пользовательских единиц
	* \endrussian
	*/
/**
	* \english
	* Calibrated device state.
	* Useful structure that contains current controller status, including speed, position and boolean flags.
	* \endenglish
	* \russian
	* Состояние устройства в калиброванных единицах.
	* Эта структура содержит основные параметры текущего состояния контроллера, такие как скорость, позиция и флаги состояния, размерные величины выводятся в калиброванных единицах.
	* \endrussian
	* @see get_status
	*/
	result_t XIMC_API get_status_calb (device_t id, status_calb_t* status, const calibration_t* calibration);

/**
	* \english
	* Return device information.
	* All fields must point to allocated string buffers with at least 10 bytes.
	* Works with both raw or initialized device.
	* @param id an identifier of device
	* @param[out] device_information device information
	* \endenglish
	* \russian
	* Возвращает информацию об устройстве.
	* Все входные параметры должны быть указателями на выделенные области памяти длиной не менее 10 байт.
	* Команда доступна как из инициализированного состояния, так и из исходного.
	* @param id идентификатор устройства.
	* @param[out] device_information информация об устройстве
	* \endrussian
	*/
/**
	* \english
	* Device information.
	* \endenglish
	* \russian
	* Информация об устройстве.
	* \endrussian
	* @see get_device_information
	*/
	result_t XIMC_API get_device_information (device_t id, device_information_t* device_information);

/**
	* \english
	* Wait for stop
	* @param id an identifier of device
	* @param refresh_interval_ms Status refresh interval. The function waits this number of milliseconds between get_status requests to the controller.
	* Recommended value of this parameter is 10 ms. Use values of less than 3 ms only when necessary - small refresh interval values do not significantly
	* increase response time of the function, but they create substantially more traffic in controller-computer data channel.
	* @param[out] ret RESULT_OK if controller has stopped and result of the first get_status command which returned anything other than RESULT_OK otherwise.
	* \endenglish
	* \russian
	* Ожидание остановки контроллера
	* @param id идентификатор устройства
	* @param refresh_interval_ms Интервал обновления. Функция ждет столько миллисекунд между отправками контроллеру запроса get_status для проверки статуса остановки.
	* Рекомендуемое значение интервала обновления - 10 мс. Используйте значения меньше 3 мс только если это необходимо - малые значения интервала обновления
	* незначительно ускоряют обнаружение остановки, но создают существенно больший поток данных в канале связи контроллер-компьютер.
	* @param[out] ret RESULT_OK, если контроллер остановился, в противном случае первый результат выполнения команды get_status со статусом отличным от RESULT_OK.
	* \endrussian
	*/
	result_t XIMC_API command_wait_for_stop(device_t id, uint32_t refresh_interval_ms);
	
	/**
	* \english
	* Make home command, wait until it is finished and make zero command. This is a convinient way to calibrate zero position.
	* @param id an identifier of device
	* @param[out] ret RESULT_OK if controller has finished home & zero correctly or result of first controller query that returned anything other than RESULT_OK.
	* \endenglish
	* \russian
	* Запустить процедуру поиска домашней позиции, подождать её завершения и обнулить позицию в конце. Это удобный путь для калибровки нулевой позиции.
	* @param id идентификатор устройства
	* @param[out] ret RESULT_OK, если контроллер завершил выполнение home и zero корректно или результат первого запроса к контроллеру со статусом отличным от RESULT_OK.
	* \endrussian
	*/
	result_t XIMC_API command_homezero(device_t id);
	//@}

#if defined(__cplusplus)
};
#endif

#endif

// vim: ts=4 shiftwidth=4

