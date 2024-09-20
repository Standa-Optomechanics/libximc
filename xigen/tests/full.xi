protocol "v1"
defaults with crc, answer, public

/**
	* \english
	* @name TODO
	* Describes motor state
	* \endenglish
	* \russian
	* @name TODO
	* Состояние мотора
	* \endrussian
	*/
flagset DevState:
DEV_STATE_OFF		= 0x00	/**< \english motor is turned off. \endenglish \russian мотор отключен от контроллера. \endrussian */
DEV_STATE_STOP		= 0x01	/**< \english motor is stopped. \endenglish \russian мотор остановлен. \endrussian */
DEV_STATE_MOVING	= 0x02	/**< \english motor is moving. \endenglish \russian мотор работает. \endrussian */
DEV_STATE_TUNE		= 0x03	/**< \english motor is tuning. \endenglish \russian мотор находится в состоянии точной доводки. \endrussian */
DEV_STATE_HOMING	= 0x04	/**< \english motor is going to home position. \endenglish \russian мотор двигается в начальную позицию. \endrussian */
DEV_STATE_CALIBR	= 0x0C	/**< \english motor is calibrating. \endenglish \russian мотор находится в состоянии калибровки. \endrussian */
DEV_STATE_ALARM		= 0x0A	/**< \english engine in alarm state. \endenglish \russian контроллер находится в аварийном состоянии, мотор отключен от контроллера. \endrussian */



/**
	* \english
	* @name TODO
	* Describes PWM state.
	* Pulse-width modulation (PWM) module drive MOSFET full brige that control current through the motor
	* \endenglish
	* \russian
	* @name TODO
	* Состояние ШИМ.
	* Модуль ШИМ (широтно-импульсная модуляция) управляет силовыми MOSFET ключами, регулируя ток через мотор.
	* \endrussian
	*/
flagset PwmState:
PWM_STATE_OFF		= 0x00	/**< \english PWM is off, motor winding is cut off power. \endenglish \russian ШИМ выключен, мотор отключен от контроллера. \endrussian */
PWM_STATE_BRAK		= 0x01	/**< \english braking mode, motor winding is closed. \endenglish \russian режим блокировки, выводы мотора закорочены. \endrussian */
PWM_STATE_RUNFWD	= 0x02	/**< \english motor running forward. \endenglish \russian мотор вращается вправо \endrussian */
PWM_STATE_RUNBCK	= 0x03	/**< \english motor running backward. \endenglish \russian мотор вращается влево \endrussian */
PWM_STATE_INVBRAK	= 0x04	/**< \english inverted braking mode. \endenglish \russian режим обратной блокировки, используется для калибровки в сервисных целях. \endrussian */


/** \english
	@name Flags of move state
	* Specify move states
	* Flags returned be query of device status
	\endenglish
	\russian
	@name Флаги состояния движения 
	* Возвращаются командой get_status
	\endrussian
	* @see state_t::move_state
	* @see get_status
	*/
flagset MoveStatus:
MOVE_STATE_TARGET_SPEED	= 0x01	/**< \english Target speed is reached, if flag set. \endenglish \russian Если флаг установлен, заданная скорость достигнута. \endrussian */
MOVE_STATE_ANTIPLAY	= 0x02	/**< \english Motor is playing compensation, if flag set. \endenglish \russian Выполняется компенсация люфта, если флаг установлен. \endrussian */

/** \english
	@name Flags of power state of stepper motor
	* Specify power states
	* Flags returned be query of device status
	\endenglish
	\russian
	@name Флаги состояния питания шагового мотора
	* Возвращаются командой get_status
	\endrussian
	* @see state_t::power_state
	* @see get_status
	*/
flagset PowerStatus:
POWER_STATE_ON		= 0x01	/**< \english Step motor is powered on, if flag set. \endenglish \russian Обмотки мотора запитаны, если флаг установлен. \endrussian */
POWER_STATE_FULL	= 0x02	/**< \english By full current, if flag set. \endenglish \russian Обмотки запитаны полным током, если флаг установлен. \endrussian */
POWER_STATE_REDUCT	= 0x04	/**< \english Or reducted current, if this flag set. \endenglish \russian бмотки запитаны уменьшенным током, если этот флаг установлен. \endrussian */

/** \english
	@name Status flags
	* Controller flags returned by device query.
	* Contains boolean part of controller status.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги состояния
	* Содержат бинарные значения состояния контроллера. Могут быть объединены с помощью логического ИЛИ.
	\endrussian
	* @see state_t::flags
	* @see get_status
	*/
flagset StatusFlags:
STATE_RIGHT_EDGE	= 0x0001	/**< \english Engine stucks at the right edge \endenglish \russian Достижение правой границы \endrussian */
STATE_LEFT_EDGE		= 0x0002	/**< \english Engine stucks at the left edge \endenglish \russian Достижение левой границы \endrussian */
STATE_EXT_POWER		= 0x0004	/**< \english External power is used \endenglish \russian Используется внешнее питание \endrussian */
STATE_ERRC		= 0x0008	/**< \english Command error encountered \endenglish \russian Недопустимая команда \endrussian */
STATE_ERRD		= 0x0010	/**< \english Data error encountered \endenglish \russian Недопустимые данные \endrussian */
STATE_POWER_DISABLE	= 0x0020	/**< \english FTDI reports "Power disable" \endenglish \russian Получено сообщение от FTDI об отключении USB-питания \endrussian */
STATE_TTLIO_PINOUT	= 0x0040	/**< \english Synchronization In/Out works as Out, if flag set; otherwise works as In \endenglish \russian Если флаг установлен, вывод синхронизации работает как выход; если флаг сброшен, вывод работает как вход. \endrussian */
STATE_TTLIO_LEVEL	= 0x0080	/**< \english State of synchronization In/Out pin \endenglish \russian Лог. уровень на выводе синхронизации \endrussian */
STATE_BUTTON_RIGHT	= 0x0100	/**< \english Button "right" state (1 if pressed) \endenglish \russian Состояние кнопки "вправо" (1, если нажата). \endrussian */
STATE_BUTTON_LEFT	= 0x0200	/**< \english Button "left" state (1 if pressed) \endenglish \russian Состояние кнопки "влево" (1, если нажата). \endrussian */
STATE_CTP_ERROR		= 0x0400	/**< \english Control position error \endenglish \russian Контроль позиции нарушен \endrussian */
STATE_EEPROM_CONNECTED	= 0x0800	/**< \english EEPROM with settings connected \endenglish \russian Подключена память EEPROM с настройками \endrussian */
STATE_OVERHEAT		= 0x2000	/**< \english Temperature sensor show value that exceed maximum temperature rating less then 1 ms \endenglish \russian Температура превысила допустимую вличину. Время реакции меньше 1 мс. \endrussian */
STATE_OVERLOAD_VOLTAGE	= 0x4000	/**< \english Voltage overload encountered. Voltage drop on motor exceed maximum rating less then 1 ms \endenglish \russian Перегрузка по напряжению. Время реакции меньше 1 мс. \endrussian */
STATE_OVERLOAD_CURRENT	= 0x8000	/**< \english Current overlaod encountered. Motor current exceed maximum rating less then 1 ms \endenglish \russian Перегрузка по току. Время реакции меньше 1 мс. \endrussian */


/** \english
	@name TODO
	* Controller flags returned by device query.
	* Contains boolean part of controller status.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name TODO
	* Содержат бинарные значения состояния контроллера. Могут быть объединены с помощью логического ИЛИ.
	\endrussian
	* @see state_t::encsts
	* @see get_status
	*/
flagset EncodeStatus:
ENC_STATE_ABSENT	= 0x00
ENC_STATE_UNKNOWN	= 0x01
ENC_STATE_MALFUNC	= 0x02
ENC_STATE_OK		= 0x03

/** \english
	@name TODO
	* Controller flags returned by device query.
	* Contains boolean part of controller status.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name TODO
	* Содержат бинарные значения состояния контроллера. Могут быть объединены с помощью логического ИЛИ.
	\endrussian
	* @see state_t::windsts
	* @see get_status
	*/
flagset WindStatus:
WIND1_STATE_ABSENT	= 0x00
WIND1_STATE_UNKNOWN	= 0x01
WIND1_STATE_MALFUNC	= 0x02
WIND1_STATE_OK		= 0x03
WIND2_STATE_ABSENT	= 0x00
WIND2_STATE_UNKNOWN	= 0x10
WIND2_STATE_MALFUNC	= 0x20
WIND2_STATE_OK		= 0x30


/** \english
	@name TODO
	* Controller flags returned by device query.
	* Contains boolean part of controller status.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name TODO
	* Содержат бинарные значения состояния контроллера. Могут быть объединены с помощью логического ИЛИ.
	\endrussian
	* @see state_t::mvcmdsts
	* @see get_status
	*/
flagset MvcmdStatus:
MVCMD_STATE	= 0x01
MVCMD_NAME	= 0xFE

/** \english
	@name Additional status flags
	* Controller flags returned by device query.
	* Contains boolean part of controller status.
	\endenglish
	\russian
	@name Дополнительные флаги состояния
	* Содержат бинарные значения состояния дополнительного разъема контроллера.
	\endrussian
	* @see additional_state_t::flags
	* @see get_additional_status
	*/
flagset AdditionalStatus:
ADDSTATE_GPIO_DIR	= 0x01		/**< \english Direction of GPIO pin. Output if flag set; input otherwise \endenglish \russian Направление вывода GPIO. Если флаг установлен - выход, иначе вход \endrussian */
ADDSTATE_GPIO_STATE	= 0x02		/**< \english State of GPIO pin \endenglish \russian Состояние вывода GPIO \endrussian */
ADDSTATE_LEFT_BTN	= 0x04		/**< \english State of Left button pin \endenglish \russian Состояние вывода левой кнопки \endrussian */
ADDSTATE_RIGHT_BTN	= 0x08		/**< \english State of Right button pin \endenglish \russian Состояние вывода правой кнопки \endrussian */
ADDSTATE_BRAKE		= 0x10		/**< \english State of Brake pin \endenglish \russian Состояние вывода управления тормозом \endrussian */
ADDSTATE_REV_SENSOR	= 0x20		/**< \english State of Revolution sensor pin \endenglish \russian Состояние вывода датчика оборотов \endrussian */
ADDSTATE_HALL_A		= 0x40		/**< \english State of Hall_a pin \endenglish \russian Состояние вывода датчика холла(a) \endrussian */
ADDSTATE_HALL_B		= 0x80		/**< \english State of Hall_b pin \endenglish \russian Состояние вывода датчика холла(b) \endrussian */
ADDSTATE_HALL_C		= 0x100		/**< \english State of Hall_c pin \endenglish \russian Состояние вывода датчика холла(c) \endrussian */

/** \english
	@name Flags of engine settings
	* Specify motor shaft movement algorithm and list of limitations.
	* Flags returned by query of engine settings. May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги параметров мотора
	* Определяют настройки движения и работу ограничителей.
	* Возращаются командой get_engine_settings. Могут быть объединены с помощью логического ИЛИ.
	\endrussian
	* @see engine_settings_t::flags
	* @see set_engine_settings
	* @see get_engine_settings
	*/
flagset EngineFlags:
ENGINE_REVERSE		= 0x0001	/**< \english Reverse flag. It determines motor shaft rotation direction that corresponds to feedback counts increasing. If not set (default), motor shaft rotation direction under positive voltage corresponds to feedback counts increasing and vice versa. Change it if you see that positive directions on motor and feedback are opposite \endenglish \russian Флаг реверса. Связывает направление вращения мотора с направлением счета текущей позиции. При сброшенном флаге (по умолчанию) прикладываемое к мотору положительное напряжение увеличивает счетчик позиции. И наоборот, при установленном флаге счетчик позиции увеличивается, когда к мотору приложено отрицательное напряжение. Измените состояние флага, если положительное вращение мотора уменьшает счетчик позиции. \endrussian */
ENGINE_FINISHING	= 0x0002	/**< \english Fine tuneup flag. If it set, engine makes special tuneup procedure and reach the predetermined end position accurately on low speed \endenglish \russian Флаг точной доводки. Если флаг установлен, мотор выполняет специальные действия для быстрого и точного достижения заданной позиции. \endrussian */
ENGINE_MAX_SPEED	= 0x0004	/**< \english TODO Max speed flag. If it is set, engine uses maxumum speed achievable with the present engine settings as nominal speed \endenglish \russian Флаг максимальной скорости. Если флаг установлен, движение происходит на максимальной скорости. \endrussian */
ENGINE_ANTIPLAY		= 0x0008	/**< \english Play compensation flag. If it set, engine makes backlash (play) compensation procedure and reach the predetermined position accurately on low speed \endenglish \russian Компенсация люфта. Если флаг установлен, позиционер будет подходить к заданной точке всегда с одной стороны. Например, при подходе слева никаких дополнительных действий не совершается, а при подходе справа позиционер проходит целевую позицию на заданное расстояния и возвращается к ней опять же справа. \endrussian */
ENGINE_ACCEL_ON		= 0x0010	/**< \english Acceleration enable flag. If it set, motion begins with acceleration and ends with deceleration \endenglish \russian Ускорение. Если флаг установлен, движение происходит с ускорением. \endrussian */
ENGINE_LIMIT_VOLT	= 0x0020	/**< \english Maxumum motor voltage limit enable flag \endenglish \russian Номинальное напряжение мотора. Если флаг установлен, напряжение на моторе ограничивается заданным номинальным значением. \endrussian */
ENGINE_LIMIT_CURR	= 0x0040	/**< \english Maxumum motor current limit enable flag \endenglish \russian Номинальный ток мотора. Если флаг установлен, ток через мотор ограничивается заданным номинальным значением. \endrussian */
ENGINE_LIMIT_RPM	= 0x0080	/**< \english TODO Maxumum motor speed limit enable flag \endenglish \russian Номинальная частота вращения мотора. Если флаг установлен, частота вращения ограничивается заданным номинальным значением. \endrussian */
ENGINE_HOLD		= 0x0100	/**< \english Hold motor shaft when motor is stopped if this flag set \endenglish \russian Режим удержания. Ротор мотора удерживается в заданной позиции, если флаг установлен. \endrussian */
ENGINE_DYNAMIC_HOLD	= 0x0200	/**< \english Holding motor shaft dynamically (power supply is applied to motor always), if set; when shaft position is different from specified, if not set \endenglish \russian Динамическое удержание. Если флаг установлен, воздействие мотора не прекращается при достижении заданной позиции, контроллер остается в состоянии Tune. Если флаг сброшен, при достижении заданной позиции контроллер переходит в состояние Stop,Brake. \endrussian */
ENGINE_ONLY_FINISHING	= 0x0400	/**< \english Move in fine tuneup mode only if this flag set. Do not use this flag in normal operation mode. It used for PID tuning only \endenglish \russian Если флаг установлен, движение к точке начинается сразу в состоянии Tune. Это специальный режим, который используется при настройке ПИД контура. Не используйте его в обычной работе. \endrussian */

/** \english
	@name Flags of microstep mode
	* Specify settings of microstep mode. Using with step motors.
	* Flags returned by query of engine settings. May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги параметров микрошагового режима
	* Определяют деление шага в микрошаговом режиме. Используются с шаговыми моторами.
	* Возращаются командой get_engine_settings. Могут быть объединены с помощью логического ИЛИ.
	\endrussian
	* @see engine_settings_t::flags
	* @see set_engine_settings
	* @see get_engine_settings
	*/
flagset MicrostepMode:
MICROSTEP_MODE_FULL	= 0x01	/**< \english Full step mode. \endenglish \russian Полношаговый режим. \endrussian */
MICROSTEP_MODE_FRAC_2	= 0x02	/**< \english 1/2 step mode. \endenglish \russian Деление шага 1/2. \endrussian */
MICROSTEP_MODE_FRAC_4	= 0x03	/**< \english 1/4 step mode. \endenglish \russian Деление шага 1/4. \endrussian */
MICROSTEP_MODE_FRAC_8	= 0x04	/**< \english 1/8 step mode. \endenglish \russian Деление шага 1/8. \endrussian */
MICROSTEP_MODE_FRAC_16	= 0x05	/**< \english 1/16 step mode. \endenglish \russian Деление шага 1/16. \endrussian */
MICROSTEP_MODE_FRAC_32	= 0x06	/**< \english 1/32 step mode. \endenglish \russian Деление шага 1/32. \endrussian */
MICROSTEP_MODE_FRAC_64	= 0x07	/**< \english 1/64 step mode. \endenglish \russian Деление шага 1/64. \endrussian */
MICROSTEP_MODE_FRAC_128	= 0x08	/**< \english 1/128 step mode. \endenglish \russian Деление шага 1/128. \endrussian */
MICROSTEP_MODE_FRAC_256	= 0x09	/**< \english 1/256 step mode. \endenglish \russian Деление шага 1/256. \endrussian */

/** \english
	@name Flags of engine type
	* Specify motor type.
	* Flags returned by query of engine settings.
	\endenglish
	\russian
	@name Флаги, определяющие тип мотора
	* Определяют тип мотора.
	* Возращаются командой get_entype_settings.
	\endrussian
	* @see engine_settings_t::flags
	* @see set_entype_settings
	* @see get_entype_settings
	*/
flagset EngineType:
ENGINE_TYPE_DC		= 0x01	/**< \english DC motor. \endenglish \russian Мотор постоянного тока. \endrussian */
ENGINE_TYPE_2DC		= 0x02	/**< \english 2 DC motors. \endenglish \russian Два мотора постоянного тока. \endrussian */
ENGINE_TYPE_STEP	= 0x03	/**< \english Step motor. \endenglish \russian Шаговый мотор. \endrussian */
ENGINE_TYPE_STEP_ENC	= 0x04	/**< \english Step motor with encoder. \endenglish \russian Шаговый мотор с энкодером. \endrussian */
ENGINE_TYPE_BRUSHLESS	= 0x05	/**< \english Brushless motor. \endenglish \russian Безщеточный мотор. \endrussian */
ENGINE_TYPE_EXTERNAL	= 0x06	/**< \english External motor driver. \endenglish \russian Управление внешним драйвером мотора. \endrussian */

/** \english
	@name Flags of driver type
	* Specify driver type.
	* Flags returned by query of engine settings.
	\endenglish
	\russian
	@name Флаги, определяющие тип силового драйвера
	* Определяют тип силового драйвера.
	* Возращаются командой get_entype_settings.
	\endrussian
	* @see engine_settings_t::flags
	* @see set_entype_settings
	* @see get_entype_settings
	*/
flagset DriverType:
DRIVER_TYPE_DISCRETE_FET	= 0x01	/**< \english Driver with discrete FET keys. Default option. \endenglish \russian Силовой драйвер на дискретных мосфет-ключах. Используется по умолчанию. \endrussian */
DRIVER_TYPE_INTEGRATE		= 0x02	/**< \english Driver with integrate IC. \endenglish \russian Силовой драйвер с использованием ключей, интегрированных в микросхему \endrussian */
DRIVER_TYPE_EXTERNAL		= 0x03	/**< \english External driver. \endenglish \russian Внешний силовой драйвер. \endrussian */


/** \english
	@name Flags of power settings of stepper motor
	* Specify power settings
	* Flags returned be query of power settings
	\endenglish
	\russian
	@name Флаги параметров питания шагового мотора
	* Возвращаются командой get_power_settings
	\endrussian
	* @see power_settings_t::power_flags
	* @see get_power_settings
	* @see set_power_settings
	*/
flagset PowerFlags:
POWER_REDUCT_ENABLED	= 0x01	/**< \english Current reduction enabled, if flag set. \endenglish \russian Уменьшение тока выполняется, если флаг установлен. \endrussian */
POWER_OFF_ENABLED	= 0x02	/**< \english Power off enabled, if flag set. \endenglish \russian Выключение тока выполняется, если флаг установлен. \endrussian */
POWER_OFF_MODE		= 0x0C	/**< \english TODO Power off enabled, if flag set. \endenglish \russian Выключение тока выполняется, если флаг установлен. \endrussian */


/** \english
	@name TODO
	* Specify power settings
	* Flags returned be query of power settings
	\endenglish
	\russian
	@name TODO
	* Возвращаются командой get_power_settings
	\endrussian
	* @see power_settings_t::power_flags
	* @see get_power_settings
	* @see set_power_settings
	*/
flagset SecureFlags:
ALARM_ON_DRIVER_OVERHEATING	= 0x01	/**< \english TODO Current reduction enabled, if flag set. \endenglish \russian Уменьшение тока выполняется, если флаг установлен. \endrussian */

/** 
	* \english
	* @name TODO
	* Describes feedback type
	* \endenglish
	* \russian
	* @name TODO
	* Тип обратной связи
	* \endrussian
	* @see set_feedback_settings 
	* @see get_feedback_settings 
	*/
flagset FeedbackType:
FEEDBACK_POTENTIOMETER	= 0x00	/**< \english feedback by potentiometer. \endenglish \russian обратная связь с помощью потенциометра. \endrussian */
FEEDBACK_ENCODER	= 0x01	/**< \english feedback by encoder. \endenglish \russian обратная связь с помощью энкодера. \endrussian */
FEEDBACK_ENCODERDIFF	= 0x02	/**< \english feedback by encoder with differential input. \endenglish \russian обратная связь с помощью энкодера с дифференциальным входом. \endrussian */
FEEDBACK_ENCODERHALL	= 0x03	/**< \english feedback by Hall detector. \endenglish \russian обратная связь с помощью датчика Холла. \endrussian */
FEEDBACK_EMF		= 0x04	/**< \english feedback by EMF. \endenglish \russian обратная связь по ЭДС. \endrussian */
FEEDBACK_NONE		= 0x05	/**< \english feedback is absent. \endenglish \russian обратная связь отсутствует. \endrussian */

/** 
	* \english
	* @name TODO
	* Describes feedback flags
	* \endenglish
	* \russian
	* @name TODO
	* Флаги обратной связи
	* \endrussian
	* @see set_feedback_settings 
	* @see get_feedback_settings 
	*/
flagset FeedbackFlags:
FEEDBACK_ENC_REVERSE	= 0x01	/**< \english Reverse count of encoder. Using with motor type ENGINE_TYPE_STEP_ENC only. \endenglish \russian Обратный счет у энкодера. Применяется только с моторами типа ENGINE_TYPE_STEP_ENC. \endrussian */


/** \english
	@name Flags of synchronization In/Out setup
	\endenglish
	\russian
	@name Флаги настроек синхронизации входа/выхода
	\endrussian
	* @see sync_settings_t::setup_flags
	* @see get_sync_settings
	* @see set_sync_settings
	*/
flagset TTLSetupFlags:
TTL_PIN_GPIO		= 0x00	/**< \english Synchronization In/Out works as Out, if flag set; otherwise works as In \endenglish \russian Если флаг установлен, вывод синхронизации работает как выход; если флаг сброшен, вывод синхронизации работает как вход. \endrussian */
TTL_PIN_SYNC_IN		= 0x04	/**< \english Synchronization In/Out works as Out, if flag set; otherwise works as In \endenglish \russian Если флаг установлен, вывод синхронизации работает как выход; если флаг сброшен, вывод синхронизации работает как вход. \endrussian */
TTL_PIN_SYNC_OUT	= 0x08	/**< \english Synchronization In/Out works as Out, if flag set; otherwise works as In \endenglish \russian Если флаг установлен, вывод синхронизации работает как выход; если флаг сброшен, вывод синхронизации работает как вход. \endrussian */
TTL_SETUP_DIROUT	= 0x01	/**< \english Synchronization In/Out works as Out, if flag set; otherwise works as In \endenglish \russian Если флаг установлен, вывод синхронизации работает как выход; если флаг сброшен, вывод синхронизации работает как вход. \endrussian */
TTL_SETUP_OUTSET	= 0x02	/**< \english Value on Synchronization Out, used if direction is out only \endenglish \russian Флаг определяет логический уровень на выводе синхронизации, установленном как выход. \endrussian */
TTL_SETUP_FRONT		= 0x10	/**< \english Synchronization In mode. Response on front, if set. Response on cut, if not set \endenglish \russian Настройка синхронизации в режиме входа. Если флаг установлен, срабатывание происходит по фронту импульса. Если флаг сброшен, срабатывание происходит по спаду импульса. \endrussian */

/** \english
	@name Flags for synchronization input setup
			TODO проверить флаги по документации
	\endenglish
	\russian
	@name Флаги настроек синхронизации входа
	\endrussian
	* @see sync_settings_t::syncin_flags
	* @see get_sync_settings
	* @see set_sync_settings
	*/
flagset TTLSyncInFlags:
TTL_SYNCIN_ONMOVE	= 0x01	/**< \english Infinite moving in TTL_SYNCIN_DIRRIGHT direction, if set; moving to given position or given shift, if not set \endenglish \russian Бесконечное вращение в направлении TTL_SYNCIN_DIRRIGHT, если флаг установлен; иначе - смещение на заданную позицию. \endrussian */
TTL_SYNCIN_MULTI_SHIFT	= 0x04	/**< \english Response on every incoming pulse, if set; one time response, if not set \endenglish \russian Если флаг установлен, происходит многократное срабатывание по команде SHIFT; если сброшен - однократное срабатывание по команде MOVE. \endrussian */
TTL_SYNCIN_DIRRIGHT	= 0x08	/**< \english Movement direction to the right, if set. Used when TTL_SYNCIN_ONMOVE set \endenglish \russian Направление вращения при синхронизации. Если флаг установлен, вращение вправо. Если флаг сброшен - влево. Используется, когда установлен флаг TTL_SYNCIN_ONMOVE. \endrussian */
TTL_SYNCIN_FRONT	= 0x10	/**< \english Rising front is active, if set; falling front is active if not set. \endenglish \russian Передний фронт активный, если флаг установлен; иначе - задний. \endrussian */
TTL_SYNCIN_ENABLED	= 0x20	/**< \english Synchronization in mode is enabled, if set. \endenglish \russian Синхронизация входа включена, если флаг установлен. \endrussian */

/** \english
	@name Flags of synchronization output
	\endenglish
	\russian
	@name Флаги настроек синхронизации выхода
	\endrussian
	* @see sync_settings_t::syncout_flags
	* @see get_sync_settings
	* @see set_sync_settings
	*/
flagset TTLSyncOutFlags:
TTL_SYNCOUT_ONSTART	= 0x01	/**< \english Generate synchronization pulse when moving started \endenglish \russian Генерация синхронизирующего импульса при начале движения. \endrussian */
TTL_SYNCOUT_ONSTOP	= 0x02	/**< \english Generate synchronization pulse when moving stopped \endenglish \russian Генерация синхронизирующего импульса при остановке. \endrussian */
TTL_SYNCOUT_ONPERIOD	= 0x04	/**< \english Generate synchronization pulse every impulse_period encoder pulses \endenglish \russian Генерация синхронизирующего импульса при каждом импульсе энкодера. \endrussian */
TTL_SYNCOUT_FRONT	= 0x08	/**< \english High level is active, if set. \endenglish \russian Высокий лог.уровень является активным, если флаг установлен. \endrussian */
TTL_SYNCOUT_ENABLED	= 0x10	/**< \english Synchronization out mode is enabled, if set. \endenglish \russian Синхронизация выхода включена, если флаг установлен. \endrussian */

/** \english
	@name TODO
	\endenglish
	\russian
	@name TODO
	\endrussian
	* @see extio_settings_t::extio_setup_flags
	* @see get_extio_settings
	* @see set_extio_settings
	*/
flagset ExtioSetupFlags:
EXTIO_SETUP_OUTPUT	= 0x01
EXTIO_SETUP_INVERT	= 0x02

/** \english
	@name TODO
	\endenglish
	\russian
	@name TODO
	\endrussian
	* @see extio_settings_t::extio_mode_flags
	* @see get_extio_settings
	* @see set_extio_settings
	*/
flagset ExtioModeFlags:
EXTIO_SETUP_MODE_IN_STOP		= 0x00
EXTIO_SETUP_MODE_IN_POWER_OFF		= 0x01
EXTIO_SETUP_MODE_IN_MOVR		= 0x02
EXTIO_SETUP_MODE_OUT_OFF		= 0x00
EXTIO_SETUP_MODE_OUT_ON			= 0x10
EXTIO_SETUP_MODE_OUT_MOVING		= 0x20
EXTIO_SETUP_MODE_OUT_ALARM		= 0x30
EXTIO_SETUP_MODE_OUT_MEMORY_WRITE	= 0x40
EXTIO_SETUP_MODE_OUT_MOTOR_ON		= 0x50
EXTIO_SETUP_MODE_OUT_MOTOR_FOUND	= 0x60
	

/** \english
	@name Border flags
	* Specify types of borders and motor behaviour on borders.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги границ
	* Типы границ и поведение позиционера на границах.
	* Могут быть объединены с помощью побитового ИЛИ.
	\endrussian
	* @see get_edges_settings
	* @see set_edges_settings
	*/
flagset BorderFlags:
BORDER_IS_ENCODER	= 0x01	/**< \english Borders are fixed by predetermined encoder values, if set; borders position on limit switches, if not set \endenglish \russian Если флаг установлен, границы определяются предустановленными точками на шкале позиции. Если флаг сброшен, границы определяются концевыми выключателями. \endrussian */
BORDER_STOP_LEFT	= 0x02	/**< \english Motor should stop on left border \endenglish \russian Если флаг установлен, мотор останавливается при достижении левой границы. \endrussian */
BORDER_STOP_RIGHT	= 0x04	/**< \english Motor should stop on right border \endenglish \russian Если флаг установлен, мотор останавливается при достижении правой границы. \endrussian */

/** \english
	@name Limit switches flags
	* Specify electrical behaviour of limit switches like order and pulled positions.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги концевых выключателей
	* Определяют направление и состояние границ. 
	* Могут быть объединены с помощью побитового ИЛИ.
	\endrussian
	* @see get_edges_settings
	* @see set_edges_settings
	*/
flagset EnderFlags:
ENDER_DIRECT	= 0x01	/**< \english First limit switch on the right side, if set; otherwise on the left side \endenglish \russian Если флаг установлен, первый концевой выключатель находится справа; иначе - слева. \endrussian */
ENDER_ON1	= 0x02	/**< \english First limit switch pulled position is closed, if set; otherwise pulled position is opened \endenglish \russian Если флаг установлен, первый концевой выключатель является нормально замкнутым; иначе - нормально разомкнутым. \endrussian */
ENDER_ON2	= 0x04	/**< \english Second limit switch pulled position is closed, if set; otherwise pulled position is opened \endenglish \russian Если флаг установлен, второй концевой выключатель является нормально замкнутым; иначе - нормально разомкнутым. \endrussian */

/** \english
	@name Brake settings flags
	* Specify behaviour of brake.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги настроек тормоза
	* Определяют поведение тормоза
	* Могут быть объединены с помощью побитового ИЛИ.
	\endrussian
	* @see get_brake_settings
	* @see set_brake_settings
	*/
flagset BrakeFlags:
BRAKE_ENABLED		= 0x01	/**< \english Brake control is enabled, if flag set. \endenglish \russian Управление тормозом включено, если флаг установлен. \endrussian */
BRAKE_ENG_PWROF		= 0x02	/**< \english Brake turns off power of step motor, if flag set. \endenglish \russian Тормоз отключает питание шагового мотора, если флаг установлен. \endrussian */
	
/** \english
	@name Control flags
	* Specify motor control settings by joystick or buttons.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги управления
	* Определяют параметры управления мотором с помощью джойстика или кнопок
	* Могут быть объединены с помощью побитового ИЛИ.
	\endrussian
	* @see get_control_settings
	* @see set_control_settings
	*/
flagset ControlFlags:
CONTROL_MODE_OFF		= 0x00	/**< \english Control is disabled. \endenglish \russian Управление отключено. \endrussian */
CONTROL_MODE_JOY		= 0x01	/**< \english Control by joystick. \endenglish \russian Управление с помощью джойстика. \endrussian */
CONTROL_MODE_LR			= 0x02	/**< \english Control by buttons left/right. \endenglish \russian Управление с помощью кнопок left/right. \endrussian */
CONTROL_BTN_LEFT_PULLED_OPEN	= 0x04	/**< \english Left button is electrical pulled open, if flag set. \endenglish \russian Левая кнопка нормально разомкнутая, если флаг установлен. \endrussian */
CONTROL_BTN_RIGHT_PULLED_OPEN	= 0x08	/**< \english Right button is electrical pulled open, if flag set. \endenglish \russian Правая кнопка нормально разомкнутая, если флаг установлен. \endrussian */
	

/** \english
	@name Position control flags
	* Specify settings of position control
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги контроля позиции
	* Определяют настройки контроля позиции
	* Могут быть объединены с помощью побитового ИЛИ.
	\endrussian
	* @see get_ctp_settings
	* @see set_ctp_settings
	*/
flagset CtpFlags:
CTP_ENABLED		= 0x01	/**< \english Position control is enabled, if flag set. \endenglish \russian Контроль позиции включен, если флаг установлен. \endrussian */
CTP_BASE		= 0x02	/**< \english Base is revolution sensor, if flag set; else - encoder. \endenglish \russian Опорой является датчик оборотов, если флаг установлен; иначе - энкодер. \endrussian */
CTP_STOP_ON_ERROR	= 0x04	/**< \english Stop on mismatch, if flag set. \endenglish \russian Остановиться при расхождении позиции, если флаг установлен. \endrussian */

/** \english
	@name Home settings flags
	* Specify behaviour for home command.
	* May be combined with bitwise OR
	\endenglish
	\russian
	@name Флаги настроек команды home
	* Определяют поведение для команды home
	* Могут быть объединены с помощью побитового ИЛИ.
	\endrussian
	* @see get_home_settings
	* @see set_home_settings
	* @see command_home
	*/
flagset HomeFlags:
HOME_DIR_FAST		= 0x01	/**< \english Flag defines direction of 1st motion after execute home command. Direction is right, if set; otherwise left. \endenglish \russian Определяет направление первоначального движения мотора после поступления команды HOME. Если флаг установлен - вправо; иначе - влево. \endrussian */
HOME_STOP_FAST		= 0x02	/**< \english Fast motion stops by synchronization input, if set; otherwise by limit switch. \endenglish \russian Если флаг установлен, то первоначальное движение завершается по сигналу со входа синхронизации; иначе - по сигналу с концевого выключателя. \endrussian */
HOME_DIR_SLOW		= 0x04	/**< \english Flag defines direction of 2nd motion. Direction is right, if set; otherwise left. \endenglish \russian Определяет направление второго движения мотора. Если флаг установлен - вправо; иначе - влево. \endrussian */
HOME_REV_EN		= 0x08	/**< \english Revolution sensor is used, if set; otherwise this action passed. \endenglish \russian Если флаг установлен, используется Revolution sensor; иначе - этап пропускается. \endrussian */


/** \english
	@name Engine control
	* Main functions for controlling engine movement
	\endenglish
	\russian
	@name Управление мотором
	* Основные функции для управления движением мотора.
	\endrussian
	*/

//@{

/** $XIW
	* \english
	* Move engine to position \a pos
	* @param pos position to move. Range: -2147483647..2147483647.
	* @param upos part of the position to move, microsteps. Range: -255..255.
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Движение в заданную позицию \a pos.
	* @param pos заданная позиция. Диапазон: -2147483647..2147483647.
	* @param upos часть позиции в микрошагах. Диапазон: -255..255.
	* @param id идентификатор устройства
	* \endrussian
	*/
command "command_move" writer "move" (18)
with inline
fields:
	int32s position
	int16s uposition
	reserved 6


/** $XIW
	* \english
	* Move engine to the distance \a offset relatively to initial position
	* @param offset shift from initial position. Range: -2147483647..2147483647.
	* @param uoffset part of the offset shift, microsteps. Range: -255..255.
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Смещение на заданную позицию относительно текущей, или заданной последней командой \a command_move
	* @param offset смещение. Диапазон: -2147483647..2147483647.
	* @param uoffset часть смещения в микрошагах. Диапазон: -255..255.
	* @param id идентификатор устройства
	* \endrussian
	*/
command "command_movr" writer "movr" (18)
with inline
fields:
	int32s position
	int16s uposition
	reserved 6

/** $XIW
	* \english
	* Set encoder position counter to \a pos.
	* @param pos this position will write in encoder counter. Range: -2147483647..2147483647.
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Установка позиции энкодера равной \a pos.
	* @param pos заданная позиция энкодера. Диапазон: -2147483647..2147483647.
	* @param id идентификатор устройства
	* \endrussian
	*/
command "encoder" writer "senc" (18)
with inline
fields:
	int32s position
	reserved 8

/** $XIW
	* \english
	* Start continous moving to the left
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Движение влево
	* @param id идентификатор устройства
	* \endrussian
	*/
command "command_left" writer "left" (4)

/** $XIW
	* \english
	* Start continous moving to the right
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Движение вправо
	* @param id идентификатор устройства
	* \endrussian
	*/
command "command_right" writer "rigt" (4)

/** $XIW
	* \english
	* Stop engine
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Остановка
	* @param id идентификатор устройства
	* \endrussian
	*/
command "command_stop" writer "stop" (4)

/** $XIW
	* \english
	* Reset current position to zero
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Установка текущей позиции в 0
	* @param id идентификатор устройства
	* \endrussian
	*/
command "command_zero" writer "zero" (4)

/** $XIW
	* \english
	* Position calibrating
		Move in direction, defined by HOME_DIR_FAST (right, if set; otherwise - left) with speed home_settings_t::fast_home.
		If HOME_STOP_FAST is set, motor stops by synchronization input activity; otherwise - by limit switch.
		Shift in reverse direction to home_settings_t::home_delta with speed home_settings_t::slow_home.
		Move in direction, defined by HOME_DIR_SLOW (right, if set; otherwise - left) with speed home_settings_t::slow_home.
		If HOME_STOP_SLOW is set, motor stops by limit switch; otherwise - by synchronization input activity.
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Калибровка позиции. 
		Вращает мотор в сторону, заданную HOME_DIR_FAST (если установлен - вправо; иначе - влево) со скоростью home_settings_t::fast_home.
		Если HOME_STOP_FAST установлен, мотор останавливается при поступлении сигнала со входа синхронизации; иначе - при достижении концевого выключателя.
		Вращает мотор в обратную сторону на расстояние home_settings_t::home_delta со скоростью home_settings_t::slow_home. 
		Вращает мотор в сторону, заданную HOME_DIR_SLOW (если установлен - вправо; иначе - влево) со скоростью home_settings_t::slow_home. 
		Если HOME_STOP_SLOW установлен, мотор останавливается при достижении концевого выключателя; иначе - при поступлении сигнала со входа синхронизации.
	* @param id идентификатор устройства
	* \endrussian
	@see home_settings_t
	@see get_home_settings
	@see set_home_settings
	*/
command "command_home" writer "home" (4)

/** $XIW
	* \english
	* Offset by engine_settings_t::antiplay and then move back.
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Смещение из текущей позиции на engine_settings_t::antiplay и возвращение в прежнюю позицию.
	* @param id идентификатор устройства
	* \endrussian
	*/
command "command_loft" writer "loft" (4)



/** $XIR
	* \english
	* Return device information. 
	* TODO All fields must point to allocated string buffers with at least 10 bytes
	* Works with both raw or initialized device.
	* @param id an identifier of device
	* @param[out] device_information device information
	* \endenglish
	* \russian
	* Возвращает информацию об устройстве. 
	* Команда доступна как из инициализированного состояния, так и из исходного.
	* @param id идентификатор устройства.
	* @param[out] device_information информация об устройстве
	* \endrussian
	*/
/** $XIS
 * Some docs about struct
 */
command "device_information_impl" reader "geti" (36)
without lock, public
fields:
	char manufacturer[4]
	char manufacturer_id[2]
	char product_description[8]
	reserved 16

/**  $XIR
	* \english
	* Device state.
	* Useful structure that contains current controller status, including speed, position and boolean flags
	* \endenglish
	* \russian
	* Состояние устройства. 
	* Эта структура содержит основные параметры текущего состоянии контроллера такие как скорость, позиция и флаги состояния.
	* \endrussian
	* @see get_status
	*/
/** $XIS
	* \english
	* Return device state.
	* Useful function that fill structure with snapshot of controller status, including speed, position and boolean flags
	* @param id an identifier of device
	* @param[out] state structure with snapshot of controller status
	* \endenglish
	* \russian
	* Возвращает информацию о текущем состоянии устройства.
	* @param id идентификатор устройства
	* @param[out] state структура с информацией о текущем состоянии устройства
	* \endrussian
	*/
command "status_impl" reader "gets" (50)
without lock, public
fields:
	int8u flag device_state			/**< \english Device state \endenglish \russian Состояние устройства \endrussian */
	int8u flag pwm_state 			/**< \english PWM state, PWM module drive MOSFET full brige that control current through the motor \endenglish \russian Состояние ШИМ \endrussian */
	int8u flag move_state			/**< \english Move state \endenglish \russian Состояние движения \endrussian */
	int8u flag power_state			/**< \english Power state \endenglish \russian Состояние питания мотора \endrussian */
	int32s current_position			/**< \english Motor shaft position by encoder \endenglish \russian Текущая позиция по показаниям энкодера \endrussian */
	int32s step_position			/**< \english Step motor shaft position in full steps \endenglish \russian Текущая позиция шагового мотора выраженная в полных шагах \endrussian */
	int16s ustep				/**< \english Step motor shaft position in 1/256 microsteps \endenglish \russian Текущая позиция шагового мотора выраженная в 1/256 микрошагах \endrussian */
	int32s current_speed			/**< \english Motor shaft speed \endenglish \russian Текущая скорость \endrussian */
	int16s ucurrent_speed			/**< \english Part of motor shaft speed in 1/256 microsteps \endenglish \russian Дробная часть текущей скорости, выраженной в 1/256 микрошагах. \endrussian */
	int16s current_engine_voltage		/**< \english Current average engine voltage \endenglish \russian Текущее усредненное напряжение на моторе \endrussian */
	int16s ipwr				/**< \english TODO Engine current \endenglish \russian Ток через мотор \endrussian */
	int16s upwr				/**< \english TODO Power supply voltage \endenglish \russian Напряжение питания \endrussian */
	int16u flag state_flags			/**< \english Set of flags specify motor shaft movement algorithm and list of limitations \endenglish \russian Флаги состояния \endrussian */
	int16s iusb				/** TODO */
	int16s uusb				/** TODO */
	int8u encsts				/** TODO */
	int8u wndsts				/** TODO */
	int8u mvcmdsts				/** TODO */
	reserved 9

/** $XIR
	* \english
	* Return state of pins of additional connector.
	* This function fills structure with snapshot of joystick and potentiometer codes, boolean flags
	* @param id an identifier of device
	* @param[out] additional_state structure with snapshot of additional connector pins
	* \endenglish
	* \russian
	* Возвращает информацию о кодах джойстика и потенциометра, состоянии других выводов дополнительного разъема
	* @param id идентификатор устройства
	* @param[out] additional_state структура с информацией о текущем состоянии выводов дополнительного разъема
	* \endrussian
	*/
/**  $XIS
	* \english
	* Additional onboard connector pins state.
	* This structure contains values and flags of onboard connector pins.
	* \endenglish
	* \russian
	* Состояние выводов дополнительного разъема.
	* Эта структура содержит состояние выводов дополнительного разъема.
	* \endrussian
	* @see get_additional_status
	*/
command "additional_status" reader "geta" (22)
fields:
	int16u potadc		/**< \english ADC value of potentiometer. \endenglish \russian Оцифрованное значение напряжения на входе потенциометра. \endrussian */
	int16u joyadc		/**< \english ADC value of joystick. \endenglish \russian Оцифрованное значение напряжения на входе джойстика. \endrussian */
	int16u flag flags	/**< \english Flags. \endenglish \russian Флаги. \endrussian */
	reserved 10


//@}


/** \english
	@name Engine setup
	* Functions for adjusting engine read/write almost all controller settings
	\endenglish
	\russian
	@name Настройки привода
	* Функции для чтения/записи большинства настроек контроллера
	\endrussian
	*/

//@{ 


/** $XIR
	* \english
	* Read DC motor information
	* @param id an identifier of device
	* @param[out] dc_information structure contains information about dc motor
	* \endenglish
	* \russian
	* Чтение информации о DC моторе
	* @param id идентификатор устройства
	* @param[out] dc_information структура, содержащая информацию о dc моторе
	* \endrussian
	*/
/** $XIW
	* \english
	* Set DC motor information
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] dc_information structure contains information about dc motor
	* \endenglish
	* \russian
	* Запись информации о DC моторе
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* @param[in] dc_information структура, содержащая информацию о dc моторе
	* \endrussian
	*/
/** $XIS
	* \english
	* DC motor information 
	* \endenglish
	* \russian
	* Информация о DC моторе
	* \endrussian
	* @see set_stage_settings
	* @see get_stage_settings
	*/
command "dc_information" universal "dci" (246)
fields:
	char manufacturer[16]		/**< \english Manufacturer. Max string length: 16 chars. \endenglish \russian Производитель. Максимальная длина строки: 16 символов. \endrussian */	
	char series[32]			/**< \english Series. Max string length: 32 chars. \endenglish \russian Серия. Максимальная длина строки: 32 символа. \endrussian */	
	char article_number[32]		/**< \english Article number. Max string length: 32 chars. \endenglish \russian Номер модели. Максимальная длина строки: 32 символа. \endrussian */	
	char description[128]		/**< \english Description. Max string length: 128 chars. \endenglish \russian Описание. Максимальная длина строки: 128 символов. \endrussian */	
	reserved 32

/** $XIR
	* \english
	* Read DC motor information
	* @param id an identifier of device
	* @param[out] dc_information structure contains information about dc motor
	* \endenglish
	* \russian
	* Чтение информации о DC моторе
	* @param id идентификатор устройства
	* @param[out] dc_information структура, содержащая информацию о dc моторе
	* \endrussian
	*/
/** $XIW
	* \english
	* Set DC motor information
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] dc_information structure contains information about dc motor
	* \endenglish
	* \russian
	* Запись информации о DC моторе
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* @param[in] dc_information структура, содержащая информацию о dc моторе
	* \endrussian
	*/
/** $XIS
	* \english
	* DC motor information 
	* \endenglish
	* \russian
	* Информация о DC моторе
	* \endrussian
	* @see set_stage_settings
	* @see get_stage_settings
	*/
command "dc_settings" universal "dcs" (78)
fields:
	float nominal_voltage		/**< \english Nominal voltage. Data type: float. \endenglish \russian Номинальное напряжение. Тип данных: float. \endrussian */	
	float no_load_speed		/**< \english No load speed. Data type: float. \endenglish \russian Скорость в холостом режиме. Тип данных: float. \endrussian */	
	float no_load_current		/**< \english No load current. Data type: float. \endenglish \russian Ток потребления в холостом режиме. Тип данных: float. \endrussian */	
	float stall_torque		/**< \english Stall torque. Data type: float. \endenglish \russian Крутящий момент в застопоренном режиме. Тип данных: float. \endrussian */	
	float max_continuous_current	/**< \english Max continuous current. Data type: float. \endenglish \russian Максимальный постоянный ток. Тип данных: float. \endrussian */	
	float terminal_resistance	/**< \english Terminal resistance. Data type: float. \endenglish \russian Сопротивление обмотки. Тип данных: float. \endrussian */	
	float terminal_inductance	/**< \english Terminal inductance. Data type: float. \endenglish \russian Индуктивность обмотки. Тип данных: float. \endrussian */	
	float torque_constant		/**< \english Torque constant. Data type: float. \endenglish \russian Константа крутящего момента. Тип данных: float. \endrussian */	
	float speed_constant		/**< \english Speed constant. Data type: float. \endenglish \russian Константа скорости. Тип данных: float. \endrussian */	
	float speed_torque_gradient	/**< \english Speed torque gradient. Data type: float. \endenglish \russian Speed torque gradient. Тип данных: float. \endrussian */	
	float mechanical_time_constant	/**< \english Mechanical time constant. Data type: float. \endenglish \russian Mechanical time constant. Тип данных: float. \endrussian */	
	float rotor_inertia		/**< \english Rotor inertia. Data type: float. \endenglish \russian Инерция ротора. Тип данных: float. \endrussian */	
	reserved 24

/** $XIR
	* \english
	* Read step motor information
	* @param id an identifier of device
	* @param[out] step_information structure contains information about step motor
	* \endenglish
	* \russian
	* Чтение информации о шаговом моторе
	* @param id идентификатор устройства
	* @param[out] step_information структура, содержащая информацию о шаговом моторе
	* \endrussian
	*/
/** $XIW
	* \english
	* Set step motor information
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] step_information structure contains information about step motor
	* \endenglish
	* \russian
	* Запись информации о шаговом моторе
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* @param[in] step_information структура, содержащая информацию о шаговом моторе
	* \endrussian
	*/
/** $XIS
	* \english
	* Stepper motor information 
	* \endenglish
	* \russian
	* Информация о шаговом моторе
	* \endrussian
	* @see set_stage_settings
	* @see get_stage_settings
	*/
command "step_information" universal "smi" (246)
fields:
	char manufacturer[16]		/**< \english Manufacturer. Max string length: 16 chars. \endenglish \russian Производитель. Максимальная длина строки: 16 символов. \endrussian */	
	char series[32]			/**< \english Series. Max string length: 32 chars. \endenglish \russian Серия. Максимальная длина строки: 32 символа. \endrussian */	
	char article_number[32]		/**< \english Article number. Max string length: 32 chars. \endenglish \russian Номер модели. Максимальная длина строки: 32 символа. \endrussian */	
	char description[128]		/**< \english Description. Max string length: 128 chars. \endenglish \russian Описание. Максимальная длина строки: 128 символов. \endrussian */	
	reserved 32

/** $XIR
	* \english
	* Read step motor information
	* @param id an identifier of device
	* @param[out] step_information structure contains information about step motor
	* \endenglish
	* \russian
	* Чтение информации о шаговом моторе
	* @param id идентификатор устройства
	* @param[out] step_information структура, содержащая информацию о шаговом моторе
	* \endrussian
	*/
/** $XIW
	* \english
	* Set step motor information
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] step_information structure contains information about step motor
	* \endenglish
	* \russian
	* Запись информации о шаговом моторе
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* @param[in] step_information структура, содержащая информацию о шаговом моторе
	* \endrussian
	*/
/** $XIS
	* \english
	* Stepper motor information 
	* \endenglish
	* \russian
	* Информация о шаговом моторе
	* \endrussian
	* @see set_stage_settings
	* @see get_stage_settings
	*/
command "step_settings" universal "sms" (58)
fields:
	float nominal_voltage		/**< \english Nominal voltage. Data type: float. \endenglish \russian Номинальное напряжение. Тип данных: float. \endrussian */	
	float current_per_winding	/**< \english Current per winding. Data type: float. \endenglish \russian Ток в обмотке. Тип данных: float. \endrussian */	
	float holding_torque		/**< \english Holding toraue. Data type: float. \endenglish \russian Крутящий момент удержания. Тип данных: float. \endrussian */	
	float resistance_per_winding	/**< \english Resistance per winding. Data type: float. \endenglish \russian Сопротивление обмоток. Тип данных: float. \endrussian */	
	float inductance_per_winding	/**< \english Inductance per winding. Data type: float. \endenglish \russian Индуктивность обмоток. Тип данных: float. \endrussian */	
	float rotor_inertia		/**< \english Rotor inertia. Data type: float. \endenglish \russian Инерция ротора. Тип данных: float. \endrussian */	
	float max_speed			/**< \english Max speed. Data type: float. \endenglish \russian Максимальная скорость. Тип данных: float. \endrussian */	
	reserved 24


/** $XIR
	* \english
	* Read encoder information
	* @param id an identifier of device
	* @param[out] encoder_information structure contains information about encoder
	* \endenglish
	* \russian
	* Чтение информации об энкодере
	* @param id идентификатор устройства
	* @param[out] encoder_information структура, содержащая информацию об энкодере
	* \endrussian
	*/
/** $XIW
	* \english
	* Set encoder information
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] encoder_information structure contains information about encoder
	* \endenglish
	* \russian
	* Запись информации об энкодере
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* @param[in] encoder_information структура, содержащая информацию об энкодере
	* \endrussian
	*/
/**  $XIS
	* \english
	* Encoder information
	* \endenglish
	* \russian
	* Информация об энкодере
	* \endrussian
	* @see set_stage_settings
	* @see get_stage_settings
	*/
command "encoder_information" universal "eni" (246)
fields:
	char manufacturer[16]		/**< \english Manufacturer. Max string length: 16 chars. \endenglish \russian Производитель. Максимальная длина строки: 16 символов. \endrussian */	
	char series[32]			/**< \english Series. Max string length: 32 chars. \endenglish \russian Серия. Максимальная длина строки: 32 символа. \endrussian */	
	char article_number[32]		/**< \english Article number. Max string length: 32 chars. \endenglish \russian Номер модели. Максимальная длина строки: 32 символа. \endrussian */	
	char description[128]		/**< \english Description. Max string length: 128 chars. \endenglish \russian Описание. Максимальная длина строки: 128 символов. \endrussian */	
	reserved 32



/** $XIR
	* \english
	* Read encoder information
	* @param id an identifier of device
	* @param[out] encoder_information structure contains information about encoder
	* \endenglish
	* \russian
	* Чтение информации об энкодере
	* @param id идентификатор устройства
	* @param[out] encoder_information структура, содержащая информацию об энкодере
	* \endrussian
	*/
/** $XIW
	* \english
	* Set encoder information
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] encoder_information structure contains information about encoder
	* \endenglish
	* \russian
	* Запись информации об энкодере
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* @param[in] encoder_information структура, содержащая информацию об энкодере
	* \endrussian
	*/
/** $XIS
	* \english
	* Encoder information
	* \endenglish
	* \russian
	* Информация об энкодере
	* \endrussian
	* @see set_stage_settings
	* @see get_stage_settings
	*/
command "encoder_settings" universal "ens" (114)
fields:
	float max_operating_frequency		/**< \english Max operation frequency. Data type: float. \endenglish \russian Максимальная частота. Тип данных: float. \endrussian */	
	float max_speed				/**< \english Max speed. Data type: float. \endenglish \russian Максимальная скорость. Тип данных: float. \endrussian */	
	float supply_voltage_1			/**< \english Minimum supply voltage. Data type: float. \endenglish \russian Минимальное напряжение питания. Тип данных: float. \endrussian */	
	float supply_voltage_2			/**< \english Maximum supply voltage. Data type: float. \endenglish \russian Максимальное напряжение питания. Тип данных: float. \endrussian */	
	float max_current_consumption		/**< \english Max current consumption. Data type: float. \endenglish \russian Максимальное потребление тока. Тип данных: float. \endrussian */	
	char output_type[64]			/**< \english Output type. Max string length: 64 chars. \endenglish \russian Тип выхода. Максимальная длина строки: 64 символа. \endrussian */	
	reserved 24


/** $XIR
	* \english
	* Read gear information
	* @param id an identifier of device
	* @param[out] gear_information structure contains information about step gearhead
	* \endenglish
	* \russian
	* Чтение информации о редукторе
	* @param id идентификатор устройства
	* @param[out] gear_information структура, содержащая информацию о редукторе
	* \endrussian
	*/
/** $XIW
	* \english
	* Set gear information
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] gear_information structure contains information about step gearhead
	* \endenglish
	* \russian
	* Запись информации о редукторе
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* @param[in] gear_information структура, содержащая информацию о редукторе
	* \endrussian
	*/
/**  $XIS
	* \english
	* Gear information
	* \endenglish
	* \russian
	* Информация о редукторе
	* \endrussian
	* @see set_stage_settings
	* @see get_stage_settings
	*/
command "gear_information" universal "gri" (246)
fields:
	char manufacturer[16]		/**< \english Manufacturer. Max string length: 16 chars. \endenglish \russian Производитель. Максимальная длина строки: 16 символов. \endrussian */	
	char series[32]			/**< \english Series. Max string length: 32 chars. \endenglish \russian Серия. Максимальная длина строки: 32 символа. \endrussian */	
	char article_number[32]		/**< \english Article number. Max string length: 32 chars. \endenglish \russian Номер модели. Максимальная длина строки: 32 символа. \endrussian */	
	char description[128]		/**< \english Description. Max string length: 128 chars. \endenglish \russian Описание. Максимальная длина строки: 128 символов. \endrussian */	
	reserved 32



/** $XIR
	* \english
	* Read gear information
	* @param id an identifier of device
	* @param[out] gear_information structure contains information about step gearhead
	* \endenglish
	* \russian
	* Чтение информации о редукторе
	* @param id идентификатор устройства
	* @param[out] gear_information структура, содержащая информацию о редукторе
	* \endrussian
	*/
/** $XIW
	* \english
	* Set gear information
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] gear_information structure contains information about step gearhead
	* \endenglish
	* \russian
	* Запись информации о редукторе
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* @param[in] gear_information структура, содержащая информацию о редукторе
	* \endrussian
	*/
/** $XIS
	* \english
	* Gear information
	* \endenglish
	* \russian
	* Информация о редукторе
	* \endrussian
	* @see set_stage_settings
	* @see get_stage_settings
	*/
command "gear_settings" universal "grs" (46)
fields:
	int32u reduction_1		/**< \english Reduction coefficient 1. Data type: unsigned int. \endenglish \russian Коэффициент редуктора 1. Тип данных: unsigned int. \endrussian */	
	int32u reduction_2		/**< \english Reduction coefficient 2. Data type: unsigned int. \endenglish \russian Коэффициент редуктора 2. Тип данных: unsigned int. \endrussian */	
	float max_continuous_torque	/**< \english Max continuous torque. Data type: float. \endenglish \russian Максимальный крутящий момент. Тип данных: float. \endrussian */	
	float max_input_speed		/**< \english Max input speed. Data type: float. \endenglish \russian Максимальная скорость. Тип данных: float. \endrussian */	
	reserved 24




/** $XIR
	* \english
	* Read engine settings.
	* This function fill structure with set of useful motor settings stored in controller's memory.
	* These settings specify motor shaft movement algorithm, list of limitations and rated characteristics
	* @see set_engine_settings
	* @param id an identifier of device
	* @param[out] engine_settings engine settings
	* \endenglish
	* \russian
	* Чтение настроек мотора.
	* @see set_engine_settings
	* @param id идентификатор устройства
	* @param[out] engine_settings структура с настройками мотора
	* \endrussian
	*/
/** $XIW
	* \english
	* Set engine settings.
	* This function send structure with set of engine settings to controller's memory.
	* These settings specify motor shaft movement algorithm, list of limitations and rated characteristics.
	* Use it when you change motor, encoder, positioner etc.
	* Please note that wrong engine settings lead to device malfunction, can lead to irreversible damage of board
	* @see get_engine_settings
	* @param id an identifier of device
	* @param[in] engine_settings engine settings
	* \endenglish
	* \russian
	* Запись настроек мотора.
	* @see get_engine_settings
	* @param id идентификатор устройства
	* @param[in] engine_settings структура с настройками мотора
	* \endrussian
	*/
/** $XIS
	* \english
	* Engine settings.
	* This structure contains useful motor settings.
	* These settings specify motor shaft movement algorithm, list of limitations and rated characteristics.
	* All boards are supplied with standart set of engine setting on controller's flash memory.
	* Please load new engine settings when you change motor, encoder, positioner etc.
	* Please note that wrong engine settings lead to device malfunction, can lead to irreversible damage of board
	* \endenglish
	* \russian
	* Настройки мотора.
	* Эта структура содержит настройки мотора. 
	* Настройки определяют номинальные значения напряжения, тока, скорости мотора, характер движения и тип мотора.
	* Пожалуйста, загружайте новые настройки когда вы меняете мотор, энкодер или позиционер.
	* Помните, что неправильные настройки мотора могут повредить оборудование.
	* \endrussian
	* @see set_engine_settings
	* @see get_engine_settings
	*/
command "engine_settings" universal "eng" (34)
fields:
	int16u nom_voltage	/**< \english Rated voltage. Controller will keep the voltage drop on motor below this value if ENGINE_LIMIT_VOLT flag is set. Range: 1..65535 \endenglish \russian Номинальное напряжение мотора. Контроллер будет сохранять напряжение на моторе не выше номинального, если установлен флаг ENGINE_LIMIT_VOLT. Диапазон: 1..65535 \endrussian */
	int16u nom_input	/**< \english Rated current. Controller will keep current consumed by motor below this value if ENGINE_LIMIT_CURR flag is set. Range: 1..65535 \endenglish \russian Номинальный ток через мотор. Контроллер будет сохранять ток через мотор не выше номинального, если установлен флаг ENGINE_LIMIT_CURR. Диапазон: 1..65535 \endrussian */
	int32u nom_rpm		/**< \english Rated RPM. Controller will keep motor shaft RPM below this value if ENGINE_LIMIT_RPM flag is set. Range: 1..1000000. \endenglish \russian Номинальная скорость мотора. Контроллер будет сохранять скорость мотора не выше номинальной, если установлен флаг ENGINE_LIMIT_RPM. Диапазон: 1..1000000. \endrussian */
	int8u urpm		/**< \english Rated RPM in 1/256 microsteps. Range: 0..255 \endenglish \russian Номинальная скорость мотора в 1/256 микрошагах. Диапазон: 0..255. \endrussian */
	int16u flag flags	/**< \english Set of flags specify motor shaft movement algorithm and list of limitations \endenglish \russian Флаги, управляющие работой мотора. \endrussian */
	int16s antiplay		/**< \english Number of pulses for backlash (play) compensation procedure. Used if ENGINE_ANTIPLAY flag is set. Range: 1..65535 \endenglish \russian Количество импульсов энкодера, на которое будет проходить позиционер заданную позицию для подхода к ней с одной и той же стороны. Используется, если установлен флаг ENGINE_ANTIPLAY. Диапазон: 1..65535 \endrussian */
	int8u microstep_mode	/**< \english Settings of microstep mode \endenglish \russian Настройки микрошагового режима. \endrussian */
	int16u steps_per_rev	/**< \english Number of full steps per revolution. Range: 1..65535. \endenglish \russian Количество полных шагов на оборот. Диапазон: 1..65535. \endrussian */
	reserved 12

/** $XIR
	* \english
	* TODO Read engine voltage, current and temperature maxumum ratings.
	* If one of these litims exceed engine will be stopped and de-energized, corresponding flag in state_t structuse will be set
	* @see state_t::flags
	* @param id an identifier of device
	* @param[out] critical_curr maxumum current rating, if it will be exceed STATE_OVERLOAD_CURRENT flag in state_t structuse will be set
	* @param[out] critical_voltage maxumum voltage rating, if it will be exceed STATE_OVERLOAD_VOLTAGE flag in state_t structure will be set
	* @param[out] critical_temp maxumum temperature rating, if it will be exceed STATE_OVERHEAT flag in state_t structuse will be set
	* \endenglish
	* \russian
	* TODO Чтение критических значений напряжения, тока и температуры.
	* Если одно из этих значений превышено, контроллер отключает все силовые выходы и устанавливает соответствующий флаг в структуре state_t. 
	* Время реакции меньше 1 мс.
	* @see state_t::flags
	* @param id идентификатор устройства
	* @param[out] critical_curr критический ток, при превышении устанавливается флан STATE_OVERLOAD_CURRENT структуры state_t
	* @param[out] critical_voltage критическое напряжение, при превышении устанавливается флаг STATE_OVERLOAD_VOLTAGE структуры state_t
	* @param[out] critical_temp критическая температура, при превышении устанавливается флаг STATE_OVERHEAT структуры state_t
	* \endrussian
	*/
/** $XIW
	* \english
	* TODO Set engine voltage, current and temperature maxumum ratings.
	* If one of these litims exceed engine will be stopped and de-energized, corresponding flag in state_t structuse will be set.
	* Use this function when you change motor, encoder, positioner etc.
	* Please note that wrong maxumum ratings can lead to irreversible damage of motor
	* @see state_t::flags
	* @param id an identifier of device
	* \endenglish
	* \russian
	* TODO Запись критических значений напряжения, тока и температуры.
	* Если одно из этих значений превышено, контроллер отключает все силовые выходы и устанавливает соответствующий флаг в структуре state_t. 
	* Время реакции меньше 1 мс.
	* @param id идентификатор устройства
	* \endrussian
	*/
/** $XIS
	* \english
	* TODO
	* This structure contains raw analog data from ADC embedded on board.
	* These data used for device testing and deep recalibraton by manufacturer only
	* \endenglish
	* \russian
	* TODO
	* Эта структура содержит необработанные данные с АЦП и нормированные значения.
	* Эти данные используются в сервисных целях для тестирования и калибровки устройства.
	* \endrussian
	* @see get_secure_settings
	*/
command "secure_settings" universal "sec" (20)
fields:
	int16u critical_ipwr
	int16u critical_upwr
	int16u critical_t
	int16u critical_iusb
	int16u critical_uusb
	int16u minimum_uusb
	int8u flag flags
	reserved 1

/** $XIR
	* \english
	* Read border and limit switches settings
	* @see set_edges_settings
	* @param id an identifier of device
	* @param[out] edges_settings edges settings, specify types of borders, motor behaviour and electrical behaviour of limit switches
	* \endenglish
	* \russian
	* Чтение настроек границ и концевых выключателей
	* @see set_edges_settings
	* @param id идентификатор устройства
	* @param[out] edges_settings настройки, определяющие тип границ, поведение мотора при их достижении и параметры концевых выключателей
	* \endrussian
	*/
/** $XIW
	* \english
	* Set border and limit switches settings
	* @see set_edges_settings
	* @param id an identifier of device
	* @param[in] edges_settings edges settings, specify types of borders, motor behaviour and electrical behaviour of limit switches
	* \endenglish
	* \russian
	* Запись настроек границ и концевых выключателей
	* @see get_edges_settings
	* @param id идентификатор устройства
	* @param[in] edges_settings настройки, определяющие тип границ, поведение мотора при их достижении и параметры концевых выключателей
	* \endrussian
	*/
/**  $XIS
	* \english
	* Edges settings.
	* This structure contains border and limit switches settings
	* Please load new engine settings when you change positioner etc.
	* Please note that wrong engine settings lead to device malfunction, can lead to irreversible damage of board
	* \endenglish
	* \russian
	* Настройки границ.
	* Эта структура содержит настройки границ и концевых выключателей. 
	* Пожалуйста, загружайте новые настройки когда вы меняете позиционер.
	* Помните, что неправильные настройки мотора могут повредить оборудование.
	* \endrussian
	* @see set_edges_settings
	* @see get_edges_settings
	*/
command "edges_settings" universal "eds" (26)
fields:
	int8u flag border_flags		/**< \english Border flags, specify types of borders and motor behaviour on borders. \endenglish \russian Флаги, определяющие тип границ и поведение мотора при их достижении. \endrussian */
	int8u flag ender_flags		/**< \english Ender flags, specify electrical behaviour of limit switches like order and pulled positions. \endenglish \russian Флаги, определяющие настройки концевых выключателей. \endrussian */
	int32s left_border		/**< \english Left border position, used if BORDER_IS_ENCODER flag is set. Range: -2147483647..2147483647. \endenglish \russian Позиция левой границы, используется если установлен флаг BORDER_IS_ENCODER. Диапазон: -2147483647..2147483647. \endrussian */
	int16s uleft_border		/**< \english Left border position in 1/256 microsteps. Range: -255..255. \endenglish \russian Позиция левой границы в 1/256 микрошагах. Диапазон: -255..255. \endrussian */
	int32s right_border		/**< \english Right border position, used if BORDER_IS_ENCODER flag is set. Range: -2147483647..2147483647. \endenglish \russian Позиция правой границы, используется если установлен флаг BORDER_IS_ENCODER. Диапазон: -2147483647..2147483647. \endrussian */
	int16s uright_border		/**< \english Right border position in 1/256 microsteps. Range: -255..255. \endenglish \russian Позиция правой границы в 1/256 микрошагах. Диапазон: -255..255. \endrussian */
	reserved 6

/** $XIR
	* \english
	* Read PID settings.
	* This function fill structure with set of motor PID settings stored in controller's memory.
	* These settings specify behaviour of PID routines for position, speed, voltage and current.
	* These factors are slightly different for different positioners
	* All boards are supplied with standart set of PID setting on controller's flash memory
	* @see set_pid_settings
	* @param id an identifier of device
	* @param[out] pid_settings pid settings
	* \endenglish
	* \russian
	* Чтение настроек ПИД контуров.
	* Эти настройки определяют поведение контуров позиции, скорости, напряжения и тока.
	* Настройки различны для разных позиционеров.
	* @see set_pid_settings
	* @param id идентификатор устройства
	* @param[out] pid_settings настройки ПИД
	* \endrussian
	*/
/** $XIW
	* \english
	* Set PID settings.
	* This function send structure with set of PID factors to controller's memory.
	* These settings specify behaviour of PID routines for position, speed, voltage and current.
	* These factors are slightly different for different positioners.
	* All boards are supplied with standart set of PID setting on controller's flash memory.
	* Please use it for loading new PID settings when you change positioner.
	* Please note that wrong PID settings lead to device malfunction
	* @see get_pid_settings
	* @param id an identifier of device
	* @param[in] pid_settings pid settings
	* \endenglish
	* \russian
	* Запись настроек ПИД контуров.
	* Эти настройки определяют поведение контуров позиции, скорости, напряжения и тока.
	* Настройки различны для разных позиционеров.
	* @see get_pid_settings
	* @param id идентификатор устройства
	* @param[in] pid_settings настройки ПИД
	* \endrussian
	*/
/**  $XIS
	* \english
	* PID settings.
	* This structure contains factors for PID routines.
	* Range: 0..65535.
	* It specify behaviour of PID routines for position, speed, voltage and current.
	* These factors are slightly different for different positioners.
	* All boards are supplied with standart set of PID setting on controller's flash memory.
	* Please load new PID settings when you change positioner.
	* Please note that wrong PID settings lead to device malfunction
	* \endenglish
	* \russian
	* Настройки ПИД.
	* Эта структура содержит коэффициенты для ПИД регуляторов.
	* Диапазон: 0..65535.
	* Они определяют работу ПИД контуров напряжения, тока, скорости и позиции.
	* Эти коэффициенты хранятся во flash памяти памяти контроллера.
	* Пожалуйста, загружайте новые настройки, когда вы меняете мотор или позиционер.
	* Помните, что неправильные настройки ПИД контуров могут повредить оборудование.
	* \endrussian
	
	* @see set_pid_settings
	* @see get_pid_settings
	*/
command "pid_settings" universal "syn" (48)
fields:
	int16u kpi		/**< \english Proportional gain for current PID routine \endenglish \russian Пропорциональный коэффициент ПИД контура по току \endrussian */
	int16u kii		/**< \english Integral gain for current PID routine \endenglish \russian Интегральный коэффициент ПИД контура по току \endrussian */
	int16u kdi		/**< \english Differential gain for current PID routine \endenglish \russian Дифференциальный коэффициент ПИД контура по току \endrussian */
	int16u kpu		/**< \english Proportional gain for voltage PID routine \endenglish \russian Пропорциональный коэффициент ПИД контура по напряжению \endrussian */
	int16u kiu		/**< \english Integral gain for voltage PID routine \endenglish \russian Интегральный коэффициент ПИД контура по напряжению \endrussian */
	int16u kdu		/**< \english Differential gain for voltage PID routine \endenglish \russian Диференциальный коэффициент ПИД контура по напряжению \endrussian */
	int16u kprpm		/**< \english Proportional gain for speed PID routine \endenglish \russian Пропорциональный коэффициент ПИД контура по скорости \endrussian */
	int16u kirpm		/**< \english Integral gain for speed PID routine \endenglish \russian Интегральный коэффициент ПИД контура по скорости \endrussian */
	int16u kdrpm		/**< \english Differential gain for speed PID routine \endenglish \russian Дифференциальный коэффициент ПИД контура по скорости \endrussian */
	int16u kppos		/**< \english Proportional gain for position PID routine \endenglish \russian Пропорциональный коэффициент ПИД контура по позиции \endrussian */
	int16u kipos		/**< \english Integral gain for position PID routine \endenglish \russian Интегральный коэффициент ПИД контура по позиции \endrussian */
	int16u kdpos		/**< \english Differential gain for position PID routine \endenglish \russian Дифференциальный коэффициент ПИД контура по позиции \endrussian */
	int16u kpe1		/**< \english Proportional gain for ext1 PID routine \endenglish \russian Пропорциональный коэффициент ПИД доп1 \endrussian */
	int16u kie1		/**< \english Integral gain for ext1 PID routine \endenglish \russian Интегральный коэффициент ПИД доп1 \endrussian */
	int16u kde1		/**< \english Differential gain for ext1 PID routine \endenglish \russian Дифференциальный коэффициент ПИД доп1 \endrussian */
	int16u kpe2		/**< \english Proportional gain for ext2 PID routine \endenglish \russian Пропорциональный коэффициент ПИД доп2 \endrussian */
	int16u kie2		/**< \english Integral gain for ext2 PID routine \endenglish \russian Интегральный коэффициент ПИД доп2 \endrussian */
	int16u kde2		/**< \english Differential gain for ext2 PID routine \endenglish \russian Дифференциальный коэффициент ПИД доп2 \endrussian */
	int16u kpe3		/**< \english Proportional gain for ext3 PID routine \endenglish \russian Пропорциональный коэффициент ПИД доп3 \endrussian */
	int16u kie3		/**< \english Integral gain for ext3 PID routine \endenglish \russian Интегральный коэффициент ПИД доп3 \endrussian */
	int16u kde3		/**< \english Differential gain for ext3 PID routine \endenglish \russian Дифференциальный коэффициент ПИД доп3 \endrussian */

/** $XIR
	* \english
	* Read synchronization settings.
	* This function fill structure with set synchronization settings, modes, periods and flags, that specify behaviour of synchronization In/Out.
	* All boards are supplied with standart set of these settings
	* @see set_sync_settings
	* @param id an identifier of device
	* @param[out] sync_settings synchronization settings
	* \endenglish
	* \russian
	* Чтение настроек синхронизации.
	* Эта функция заполняет структуру с настройками синхронизации, определяющими поведение вывода синхронизации.
	* @see set_sync_settings
	* @param id идентификатор устройства
	* @param[out] sync_settings настройки синхронизации
	* \endrussian
	*/
/** $XIW
	* \english
	* Set synchronization settings.
	* This function send structure with set of synchronization settings, that specify behaviour of synchronization In/Out, to controller's memory.
	* All boards are supplied with standart set of these settings
	* @see get_sync_settings
	* @param id an identifier of device
	* @param[in] sync_settings synchronization settings
	* \endenglish
	* \russian
	* Запись настроек синхронизации.
	* Эта функция записывает структуру с настройками синхронизации, определяющими поведение вывода синхронизации, в память контроллера.
	* @see get_sync_settings
	* @param id идентификатор устройства
	* @param[in] sync_settings настройки синхронизации
	* \endrussian
	*/
/** $XIS
	* \english
	* TODO XXX sync_pin_mode
	* Synchronization settings.
	* This structure contains all synchronization settings, modes, periods and flags.
	* It specify behaviour of synchronization In/Out.
	* All boards are supplied with standart set of these settings
	* \endenglish
	* \russian
	* Настройки синхронизации.
	* Эта структура содержит все настройки, определяющие поведение входа\выхода синхронизации.
	* \endrussian
	* @see get_sync_settings
	* @see set_sync_settings
	*/
command "sync_settings" universal "syn" (23)
fields:
	int8u flag setup_flags		/**< \english Generic setup flags \endenglish \russian Основные флаги синхронизации \endrussian */
	int8u flag syncin_flags		/**< \english Input synchronization flags \endenglish \russian Флаги синхронизации входа \endrussian */
	int8u flag syncout_flags	/**< \english Output synchronization flags \endenglish \russian Флаги синхронизации выхода \endrussian */
	int16u impulse_time		/**< \english Output synchronization pulse duration or input synchronization pulse dead time. Range: 0..65535 \endenglish \russian Длительность импульса синхронизации для выхода или задержка защиты от дребезга для входа. Диапазон: 0..65535 \endrussian */
	int16u impulse_period		/**< \english This value specify number of encoder pulses between two output synchronization pulses when TTL_SYNCOUT_ONPERIOD is set. Range: 0..65535 \endenglish \russian Период генерации импульсов, используется при установленном флаге TTL_SYNCOUT_ONPERIOD. Диапазон: 0..65535 \endrussian */
	reserved 10

/** $XIR
 * TODO reader
 */
/** $XIW
 * TODO writer
 */
/** $XIS
 * TODO struct
 */
command "extio_settings" universal "hom" (18)
fields:
	int8u flag setup_flags	
	int8u flag mode_flags		
	reserved 10

/** $XIR
	* \english
	* Read home settings.
	* This function fill structure with settings of calibrating position
	* @see home_settings_t
	* @param id an identifier of device
	* @param[out] home_settings calibrating position settings
	* \endenglish
	* \russian
	* Чтение настроек калибровки позиции.
	* Эта функция заполняет структуру настроек, использующихся для калибровки позиции.
	* @see home_settings_t
	* @param id идентификатор устройства
	* @param[out] home_settings настройки калибровки позиции
	* \endrussian
	*/
/** $XIW
	* \english
	* Set home settings.
	* This function send structure with calibrating position settings to controller's memory.
	* @see home_settings_t
	* @param id an identifier of device
	* @param[in] home_settings calibrating position settings
	* \endenglish
	* \russian
	* Запись настроек калибровки позиции.
	* Эта функция записывает структуру настроек, использующихся для калибровки позиции, в память контроллера.
	* @see home_settings_t
	* @param id идентификатор устройства
	* @param[out] home_settings настройки калибровки позиции
	* \endrussian
	*/
/** $XIS
	* \english
	* Position calibration settings.
	* This structure contains settings used in position calibrating.
	* It specify behaviour of calibrating position.
	* \endenglish
	* \russian
	* Настройки калибровки позиции.
	* Эта структура содержит настройки, использующиеся при калибровке позиции.
	* \endrussian
	* @see get_home_settings
	* @see set_home_settings
	* @see command_home
	*/
command "home_settings" universal "hom" (33)
fields:
	int32u fast_home	/**< \english Speed used for fast motion. Range: 0..1000000. \endenglish \russian Скорость быстрого движения. Диапазон: 0..1000000 \endrussian */
	int8u ufast_home	/**< \english Part of the speed for fast motion, microsteps. Range: 0..255. \endenglish \russian Дробная часть скорости быстрого движения в микрошагах. Диапазон: 0..255. \endrussian */
	int32u slow_home	/**< \english Speed used for slow motion. Range: 0..1000000. \endenglish \russian Скорость медленного движения. Диапазон: 0..1000000. \endrussian */
	int8u uslow_home	/**< \english Part of the speed for slow motion, microsteps. Range: 0..255. \endenglish \russian Дробная часть скорости медленного движения в микрошагах. Диапазон: 0..255. \endrussian */
	int32s delta		/**< \english Distance from break point. Range: -2147483647..2147483647. \endenglish \russian Расстояние отхода от точки останова. Диапазон: -2147483647..2147483647. \endrussian */
	int16s udelta		/**< \english Part of the delta distance, microsteps. Range: -255..255. \endenglish \russian Дробная часть асстояния отхода от точки останова в микрошагах. Диапазон: -255..255. \endrussian */
	int8u flag flags	/**< \english Set of flags specify direction and stopping conditions. \endenglish \russian Набор флагов, определяющие такие параметры, как направление и условия останова. \endrussian */
	reserved 10

/** $XIW
	* \english
	* Read all settings from controller's flash memory to controller's RAM, replacing previous data in controller's RAM
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Чтение всех настроек контроллера из flash памяти в оперативную, заменяя текущие настройки.
	* @param id идентификатор устройства
	* \endrussian
	*/
command "read_settings" writer "read" (4)

/** $XIW
	* \english
	* Save all settings from controller's RAM to controller's flash memory, replacing previous data in controller's flash memory
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Запись всех текущих настроек во flash память контроллера.
	* @param id идентификатор устройства
	* \endrussian
	*/
command "save_settings" writer "save" (4)

/** $XIW
	* \english
	* Read all settings from stage's EEPROM memory to controller's RAM, replacing previous data in controller's RAM
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Чтение всех настроек из EEPROM памяти позиционера в оперативную память контроллера
	* @param id идентификатор устройства
	* \endrussian
	*/
command "eeread_settings" writer "eerd" (4)

/** $XIW
	* \english
	* Save all settings from controller's RAM to stage's EEPROM memory, replacing previous data in EEPROM memory
	* Can be used by manufacturer only
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Запись всех текущих настроек контроллера в EEPROM память позиционера
	* Функция должна использоваться только производителем
	* @param id идентификатор устройства
	* \endrussian
	*/
command "eesave_settings" writer "eesv" (4)


/** $XIR
	* \english
	* Read byte from UART buffer
	* @param id an identifier of device
	* @param[out] data byte of data from UART
	* \endenglish
	* \russian
	* Чтение байта из UART
	* @param id идентификатор устройства
	* @param[out] data считанный байт данных из UART
	* \endrussian
*/
command "read_uart" reader "rdut" (20)
with inline
fields:
	int8u data
	reserved 7
	int8u flag flags
	reserved 5

/** $XIW
	* \english
	* Write byte to UART buffer
	* @param id an identifier of device
	* @param[in] data byte of data
	* \endenglish
	* \russian
	* Запись байта в UART
	* @param id идентификатор устройства
	* @param[in] data байт данных для записи в UART
	* TODO why flags?
	* \endrussian
*/
command "write_uart" writer "wrut" (20)
with inline
fields:
	int8u data
	reserved 13

/** $XIR
	* \english
	* Read device serial number
	* @param id an identifier of device
	* @param[out] serial serial number
	* \endenglish
	* \russian
	* Чтение серийного номера контроллера
	* @param id идентификатор устройства
	* @param[out] serial serial number
	* \endrussian
	*/
command "serial_number" reader "gser" (10)
with inline
fields:
	int32u serial

/** $XIR
	* \english
	* TODO Read controller's firmware version
	* @param id an identifier of device
	* @param[out] major major version
	* @param[out] minor minor version
	* @param[out] release release version
	* \endenglish
	* \russian
	* TODO Чтение номера версии прошивки контроллера
	* @param id идентификатор устройства
	* @param[out] major номер основной версии
	* @param[out] minor номер дополнительной версии
	* @param[out] release номер релиза
	* \endrussian
	*/
command "bootloader_version" reader "gblv" (10)
with inline
fields:
	int8u major
	int8u minor
	int16u release

/** $XIR
	* \english
	* Read controller's firmware version
	* @param id an identifier of device
	* @param[out] major major version
	* @param[out] minor minor version
	* @param[out] release release version
	* \endenglish
	* \russian
	* Чтение номера версии прошивки контроллера
	* @param id идентификатор устройства
	* @param[out] major номер основной версии
	* @param[out] minor номер дополнительной версии
	* @param[out] release номер релиза
	* \endrussian
	*/
command "firmware_version" reader "gfwv" (10)
with inline
fields:
	int8u major
	int8u minor
	int16u release

/** $XIR
	* \english
	* Read analog data structure that contains raw analog data from ADC embedded on board.
	* This function used for device testing and deep recalibraton by manufacturer only
	* @param id an identifier of device
	* @param[out] ad analog data coefficients
	* \endenglish
	* \russian
	* Чтение аналоговых данных, содержащих данные с АЦП и нормированные значения величин.
	* Эта функция используется для тестирования и калибровки устройства.
	* @param id идентификатор устройства
	* @param[out] ad аналоговые данные
	* \endrussian
	*/
/** $XIS
	* \english
	* Analog data.
	* This structure contains raw analog data from ADC embedded on board.
	* These data used for device testing and deep recalibraton by manufacturer only
	* \endenglish
	* \russian
	* Аналоговые данные.
	* Эта структура содержит необработанные данные с АЦП и нормированные значения.
	* Эти данные используются в сервисных целях для тестирования и калибровки устройства.
	* \endrussian
	* @see get_analog_data
	*/
command "analog_data" reader "rdan" (54)
fields:
	int16u adc_va1			/**< \english "Voltage on pin 1 winding A" raw data from ADC \endenglish \russian "Выходное напряжение на 1 выводе обмотки А" необработанные данные с АЦП \endrussian */
	int16u adc_va2			/**< \english "Voltage on pin 2 winding A" raw data from ADC \endenglish \russian "Выходное напряжение на 2 выводе обмотки А" необработанные данные с АЦП \endrussian */
	int16u adc_vb1			/**< \english "Voltage on pin 1 winding B" raw data from ADC \endenglish \russian "Выходное напряжение на 1 выводе обмотки B" необработанные данные с АЦП \endrussian */
	int16u adc_vb2			/**< \english "Voltage on pin 2 winding B" raw data from ADC \endenglish \russian "Выходное напряжение на 2 выводе обмотки B" необработанные данные с АЦП \endrussian */
	int16u adc_vext			/**< \english "External voltage" raw data from ADC \endenglish \russian "Напряжение питания" необработанные данные с АЦП \endrussian */
	int16u adc_vsup			/**< \english "Voltage on the top of MOSFET full brige" raw data from ADC \endenglish \russian "Напряжение питания ключей Н-моста" необработанные данные с АЦП \endrussian */
	int16u adc_acur			/**< \english "Winding A current" raw data from ADC \endenglish \russian "Ток через обмотку А" необработанные данные с АЦП \endrussian */
	int16u adc_bcur			/**< \english "Winding B current" raw data from ADC \endenglish \russian "Ток через обмотку B" необработанные данные с АЦП \endrussian */
	int16u adc_fullcur		/**< \english "Full current" raw data from ADC \endenglish \russian "Полный ток" необработанные данные с АЦП \endrussian */
	int16u adc_tempkey		/**< \english Voltage from temperature sensor, raw data from ADC \endenglish \russian Напряжение с датчика температуры, необработанные данные с АЦП \endrussian */
	int16u adc_pot			/**< \english Voltage on "Potentiometer", raw data from ADC \endenglish \russian "Потенциометр" необработанные данные с АЦП \endrussian */
	int16s cdc_va1			/**< \english "Voltage on pin 1 winding A" calibrated data \endenglish \russian "Выходное напряжение на 1 выводе обмотки А" откалиброванные данные \endrussian */
	int16s cdc_va2			/**< \english "Voltage on pin 2 winding A" calibrated data \endenglish \russian "Выходное напряжение на 2 выводе обмотки А" откалиброванные данные \endrussian */
	int16s cdc_vb1			/**< \english "Voltage on pin 1 winding B" calibrated data \endenglish \russian "Выходное напряжение на 1 выводе обмотки B" откалиброванные данные \endrussian */
	int16s cdc_vb2			/**< \english "Voltage on pin 2 winding B" calibrated data \endenglish \russian "Выходное напряжение на 2 выводе обмотки B" откалиброванные данные \endrussian */
	int16s cdc_vext			/**< \english "External voltage" calibrated data \endenglish \russian "Напряжение питания" откалиброванные данные \endrussian */
	int16s cdc_vsup			/**< \english "Voltage on the top of MOSFET full brige" calibrated data \endenglish \russian "Напряжение питания ключей Н-моста" откалиброванные данные \endrussian */
	int16s cdc_acur			/**< \english "Winding A current" calibrated data \endenglish \russian "Ток через обмотку А" откалиброванные данные \endrussian */
	int16s cdc_bcur			/**< \english "Winding B current" calibrated data \endenglish \russian "Ток через обмотку B" откалиброванные данные \endrussian */
	int16s cdc_fullcur		/**< \english "Full current" calibrated data \endenglish \russian "Полный ток" откалиброванные данные \endrussian */
	int16s cdc_tempkey		/**< \english Temperature, calibrated data \endenglish \russian Температура, откалиброванные данные \endrussian */
	int16s cdc_dutycycle		/**< \english Duty cycle of PWM \endenglish \russian Коэффициент заполнения ШИМ \endrussian */
	reserved 4

/** $XIR
	* \english
	* Return device electrical parameters, useful for charts.
	* Useful function that fill structure with snapshot of controller voltages and currents.
	* @see chart_data_t
	* @param id an identifier of device
	* @param[out] chart_data structure with snapshot of controller parameters.
	* \endenglish
	* \russian
	* Возвращает информацию с электрическими параметрами, удобную для построения графиков.
	* @see chart_data_t
	* @param id идентификатор устройства
	* @param[out] chart_data structure with snapshot of controller parameters.
	* \endrussian
	*/
/** $XIS
	* \english
	* Additional device state.
	* This structure contains additional values such as winding's voltages, currents and temperature.
	* \endenglish
	* \russian
	* Дополнительное состояние устройства.
	* Эта структура содержит основные дополнительные параметры текущего состоянии контроллера, такие напряжения и токи обмоток и температуру.
	* \endrussian
	* @see get_chart_data
	*/
command "chart_data" reader "getc" (38)
fields:
	int16s winding_voltage_a		/**< \english Winding A voltage \endenglish \russian Напряжение на обмотке А \endrussian */
	int16s winding_voltage_b		/**< \english Winding B voltage \endenglish \russian Напряжение на обмотке B \endrussian */
	int16s winding_voltage_c		/**< \english Winding C voltage \endenglish \russian Напряжение на обмотке C \endrussian */
	int16s winding_current_a		/**< \english Winding A current \endenglish \russian Ток через обмотке А \endrussian */
	int16s winding_current_b		/**< \english Winding B current \endenglish \russian Ток через обмотке B \endrussian */
	int16s winding_current_c		/**< \english Winding C current \endenglish \russian Ток через обмотке C \endrussian */
	int16s current_temp			/**< \english Temperature of MOSFET keys \endenglish \russian Температура MOSFET ключей \endrussian */
	reserved 18

//@} 


/** \english
	@name Service functions
	* Functions are intended for deep controller setup.
	* There is no necessity to use it during normal operation.
	* These functions have to be used by skilled engineers only.
	* If you want to use it please consult with the manufacturer.
	* Wrong usage lead to device malfunction, can lead to irreversible damage of board 
	\endenglish
	\russian
	@name Сервисные функции
	* Эти функции необходимы для начального конфигурирования, тестирования и обновления устройства.
	* Не применяйте их при обычной работе с контроллером.
	* При необходимости, проконсультируйтесь пожалуйста с производителем.
	* Неправильное использование этих функций может привести к неработоспособности устройства.
	\endrussian
	*/

//@{ 

/** $XIR
	* \english
	* Read calibration coefficients.
	* This function fill structure with set of calibration data stored in controller's memory.
	* There is no necessity to use it during normal operation.
	* These data change after special device recalibraton only
	* @see set_calibration_coeffs
	* @param id an identifier of device
	* @param[out] cs calibration coefficients
	* \endenglish
	* \russian
	* Чтение калибровочных коэффициентов.
	* Эта функция заполняет структуру калибровочных коэффициентов, которые хранятся в памяти контроллера.
	* Для обычной работы с устройством эта функция не требуется.
	* Калибровочные коэффициенты применяются для калибровки контроллера.
	* @see set_calibration_coeffs
	* @param id идентификатор устройства
	* @param[out] cs калибровочные коэффициенты
	* \endrussian
	*/
/** $XIW
	* \english
	* Set calibration coefficients.
	* This function send structure with set of calibration data to controller's memory.
	* Note that all boards are supplied with full set of right calibration data.
	* There is no necessity to change it during normal operation.
	* These data have to be changed after special hardware device recalibraton only.
	* If you want to recalibrate board please consult with the manufacturer.
	* Wrong calibration settings lead to device malfunction, can lead to irreversible damage of board 
	* @see get_calibration_coeffs
	* @param id an identifier of device
	* @param[in] cs calibration coefficients
	* \endenglish
	* \russian
	* Запись калибровочных коэффициентов.
	* Эта функция записывает калибровочные коэффициенты во flash контроллера.
	* Все устройства при изготовлении проходят процедуру калибровки и при обычной работе их не требуется изменять.
	* Если вы хотите произвести калибровку устройства, проконсультируйтесь с производителем.
	* Установка неправильных коэффициентов может стать причиной перегрева и выхода контроллера из строя.
	* @see get_calibration_coeffs
	* @param id идентификатор устройства
	* @param[in] cs калибровочные коэффициенты
	* \endrussian
	*/
/** $XIS
	* \english
	* Calibration settings.
	* This structure contains calibration settings for all analog data from ADC embedded on board.
	* Range: -32767..32767.
	* All boards are supplied with full set of calibration data on controller's flash memory.
	* There is no necessity to change it during normal operation.
	* These data have to be changed after special device recalibraton only.
	* If you want to recalibrate board please consult with the manufacturer.
	* Wrong calibration settings lead to device malfunction, can lead to irreversible damage of board
	* \endenglish
	* \russian
	* Калибровочные коэффициенты.
	* Эта структура содержит коэффициенты, которые используются для преобразования данных с АЦП в используемые единицы измерения.
	* Диапазон: -32767..32767.
	* В контроллере эти коэффициенты хранятся в энергонезависимой flash памяти.
	* Все устройства при изготовлении проходят процедуру калибровки и при обычной работе их не требуется изменять.
	* Если вы хотите произвести калибровку устройства, проконсультируйтесь с производителем.
	* Неправильные коэффициенты могут быть причиной перегрева и выхода контроллера из строя.
	* \endrussian
	* @see get_calibration_coeffs
	* @see set_calibration_coeffs
	*/
command "calibration_settings" universal "cal" (86)
fields:
	int16s amp_vout1	/**< \english Scale factor for "Voltage out1" \endenglish \russian Коэффициент усиления для "Выходного напряжения 1" \endrussian */
	int16s amp_vout2	/**< \english Scale factor for "Voltage out2" \endenglish \russian Коэффициент усиления для "Выходного напряжения 2" \endrussian */
	int16s amp_vext		/**< \english Scale factor for "Supply voltage" \endenglish \russian Коэффициент усиления для "Напряжения внешнего питания" \endrussian */
	int16s amp_vkey		/**< \english Scale factor for raw voltage on the top of MOSFET full brige \endenglish \russian Коэффициент уисления для "Напряжения питания ключей Н-моста" \endrussian */
	int16s amp_engcur	/**< \english Scale factor for "Engine current" \endenglish \russian Коэффициент усиления для "Тока через мотор" \endrussian */
	int16s amp_tempkey	/**< \english Scale factor for raw data from temperature sensor \endenglish \russian Коэффициент усиления для "Температуры MOSFET ключей" \endrussian */
	reserved 28
	int16s off_vout1	/**< \english Offset factor for "Voltage out1" \endenglish \russian Коэффициент смещения для "Выходного напряжения 1" \endrussian */
	int16s off_vout2	/**< \english Offset factor for "Voltage out2" \endenglish \russian Коэффициент смещения для "Выходного напряжения 2" \endrussian */
	int16s off_vext		/**< \english Offset factor for "Supply voltage" \endenglish \russian Коэффициент смещения для "Напряжения внешнего питания" \endrussian */
	int16s off_vkey		/**< \english Offset factor for raw voltage on the top of MOSFET full brige \endenglish \russian Коэффициент смещения для "Напряжения питания ключей Н-моста" \endrussian */
	int16s off_engcur	/**< \english Offset factor for "Engine current" \endenglish \russian Коэффициент смещения для "Тока через мотор" \endrussian */
	int16s off_tempkey	/**< \english Offset factor for raw data from temperature sensor \endenglish \russian Коэффициент смещения для "Температуры MOSFET ключей" \endrussian */
	reserved 28

/** $XIW
	* \english
	* TODO special
	* Write device serial number to controller's flash memory.
	* Can be used by manufacturer only
	* @param id an identifier of device
	* @param[in] serial serial number. Range: 0..4294967295
	* @param[in] key protection key. Range: 0..4294967295
	* \endenglish
	* \russian
	* Запись серийного номера во flash память контроллера.
	* Функция используется только производителем.
	* @param id идентификатор устройства
	* @param[in] serial серийный номер. Диапазон: 0..4294967295
	* @param[in] key ключ защиты. Диапазон: 0..4294967295
	* \endrussian
	*/
command "serial_number" writer "sser" (14)
with inline
fields:
	int32u serial
	int32u key


/** $XIW
	* \english
	* Calibrate controller.
	* Can be used by manufacturer only
	* @param id an identifier of device
	* \endenglish
	* \russian
	* Перевод контроллера в состояние калибровки.
	* Функция используется только производителем.
	* @param id идентификатор устройства
	* \endrussian
	*/
command "calibrate" writer "calb" (4)

/** $XIW
	* \english
	* Write controller key.
	* Can be used by manufacturer only
	* @param name a name of device
	* @param[in] key protection key. Range: 0..4294967295
	* \endenglish
	* \russian
	* Запись ключа защиты
	* Функция используется только производителем.
	* @param name имя устройства
	* @param[in] key ключ защиты. Диапазон: 0..4294967295
	* \endrussian
	*/
command "service_command_write_key_impl" writer "wkey" (18) reader "wkey" (15)
with inline
without public
fields:
	int32u key
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8


command "reset_impl" writer "rest" (4)
without answer, public

command "service_command_updf" writer "updf" (4)
without public

	/** $XIW
		* \english
		* TODO Check for firmware on device
		* @param id an identifier of device
		* @param[out] ret non-zero if firmware existed
		* \endenglish
		* \russian
		* TODO Проверка наличия прошивки в контроллере
		* @param id идентификатор устройства
		* @param[out] ret не ноль, если прошивка присутствует
		* \endrussian
		*/
command "service_command_goto_firmware_impl" writer "gofw" (4) reader "gofw" (15)
with inline
without public, crc
answer:
	int8u serviceanswer sresult
	reserved 8

/** $XIW
	* \english
	* Check for firmware on device
	* @param name a name of device
	* @param[out] ret non-zero if firmware existed
	* \endenglish
	* \russian
	* Проверка наличия прошивки в контроллере
	* @param name имя устройства
	* @param[out] ret не ноль, если прошивка присутствует
	* \endrussian
	*/
command "service_command_has_firmware_impl" writer "hasf" (4) reader "hasf" (15)
with inline
without public, crc
answer:
	int8u serviceanswer sresult
	reserved 8


command "service_command_connect" writer "conn" (14) reader "conn" (15)
without public
with inline
fields:
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

command "service_command_disconnect" writer "disc" (14) reader "disc" (15)
without public
with inline
fields:
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

command "service_command_write_data" writer "wdat" (143) reader "wdat" (15)
without public
with inline
fields:
	int8u datalen
	int8u data[@datalen]
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

command "service_command_new_section" writer "nsec" (22) reader "nsec" (15)
without public
with inline
fields:
	int8u data[8]
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8





/** vim: set ft=c ts=8 sw=8: */
