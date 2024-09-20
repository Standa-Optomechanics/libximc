protocol "v1"
defaults with crc, answer

	/**
		* \english
		* Describes motor state
		* \endenglish
		* \russian
		* Состояние мотора
		* \endrussian
		*/
flagset DevState:
		DevStateOff 		= 0x00	/**< \english motor is turned off. \endenglish \russian мотор отключен от контроллера. \endrussian */
		DevStateStop 		= 0x01	/**< \english motor is stopped. \endenglish \russian мотор остановлен. \endrussian */
		DevStateMoving 	= 0x02	/**< \english motor is moving. \endenglish \russian мотор работает. \endrussian */
		DevStateTune 		= 0x03	/**< \english motor is tuning. \endenglish \russian мотор находится в состоянии точной доводки. \endrussian */
		DevStateHoming 	= 0x04	/**< \english motor is going to home position. \endenglish \russian мотор двигается в начальную позицию. \endrussian */
		DevStateCalibr 	= 0x0C 	/**< \english motor is calibrating. \endenglish \russian мотор находится в состоянии калибровки. \endrussian */
		DevStateAlarm 	= 0x0A	/**< \english engine in alarm state. \endenglish \russian контроллер находится в аварийном состоянии, мотор отключен от контроллера. \endrussian */


/**
		* \english
		* Describes PWM state.
		* Pulse-width modulation (PWM) module drive MOSFET full brige that control current through the motor
		* \endenglish
		* \russian
		* Состояние ШИМ.
		* Модуль ШИМ (широтно-импульсная модуляция) управляет силовыми MOSFET ключами, регулируя ток через мотор.
		* \endrussian
		*/
flagset PwmState:
		PwmStateOff 		= 0x00		/**< \english PWM is off, motor winding is cut off power. \endenglish \russian ШИМ выключен, мотор отключен от контроллера. \endrussian */
		PwmStateBrak 		= 0x01		/**< \english braking mode, motor winding is closed. \endenglish \russian режим блокировки, выводы мотора закорочены. \endrussian */
		PwmStateRunfwd 	= 0x02		/**< \english motor running forward. \endenglish \russian мотор вращается вправо \endrussian */
		PwmStateRunbck 	= 0x03		/**< \english motor running backward. \endenglish \russian мотор вращается влево \endrussian */
		PwmStateInvbrak	= 0x04		/**< \english inverted braking mode. \endenglish \russian режим обратной блокировки, используется для калибровки в сервисных целях. \endrussian */


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
	MOVE_STATE_ANTIPLAY 		= 0x02	/**< \english Motor is playing compensation, if flag set. \endenglish \russian Выполняется компенсация люфта, если флаг установлен. \endrussian */
	
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
	POWER_STATE_ON 			= 0x01	/**< \english Step motor is powered on, if flag set. \endenglish \russian Обмотки мотора запитаны, если флаг установлен. \endrussian */
	POWER_STATE_FULL 		= 0x02	/**< \english By full current, if flag set. \endenglish \russian Обмотки запитаны полным током, если флаг установлен. \endrussian */
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
	STATE_RIGHT_EDGE				= 0x0001		/**< \english Engine stucks at the right edge \endenglish \russian Достижение правой границы \endrussian */
	STATE_LEFT_EDGE					= 0x0002		/**< \english Engine stucks at the left edge \endenglish \russian Достижение левой границы \endrussian */
	STATE_EXT_POWER					= 0x0004		/**< \english External power is used \endenglish \russian Используется внешнее питание \endrussian */
	STATE_ERRC							= 0x0008		/**< \english Command error encountered \endenglish \russian Недопустимая команда \endrussian */
	STATE_ERRD							= 0x0010		/**< \english Data error encountered \endenglish \russian Недопустимые данные \endrussian */
	STATE_POWER_DISABLE			= 0x0020		/**< \english FTDI reports "Power disable" \endenglish \russian Получено сообщение от FTDI об отключении USB-питания \endrussian */
	STATE_TTLIO_PINOUT			= 0x0040		/**< \english Synchronization In/Out works as Out, if flag set; otherwise works as In \endenglish \russian Если флаг установлен, вывод синхронизации работает как выход; если флаг сброшен, вывод работает как вход. \endrussian */
	STATE_TTLIO_LEVEL				= 0x0080		/**< \english State of synchronization In/Out pin \endenglish \russian Лог. уровень на выводе синхронизации \endrussian */
	STATE_BUTTON_RIGHT			= 0x0100		/**< \english Button "right" state (1 if pressed) \endenglish \russian Состояние кнопки "вправо" (1, если нажата). \endrussian */
	STATE_BUTTON_LEFT				= 0x0200		/**< \english Button "left" state (1 if pressed) \endenglish \russian Состояние кнопки "влево" (1, если нажата). \endrussian */
	STATE_CTP_ERROR					= 0x0400		/**< \english Control position error \endenglish \russian Контроль позиции нарушен \endrussian */
	STATE_EEPROM_CONNECTED	= 0x0800		/**< \english EEPROM with settings connected \endenglish \russian Подключена память EEPROM с настройками \endrussian */
	STATE_OVERHEAT					= 0x2000		/**< \english Temperature sensor show value that exceed maximum temperature rating less then 1 ms \endenglish \russian Температура превысила допустимую вличину. Время реакции меньше 1 мс. \endrussian */
	STATE_OVERLOAD_VOLTAGE	= 0x4000		/**< \english Voltage overload encountered. Voltage drop on motor exceed maximum rating less then 1 ms \endenglish \russian Перегрузка по напряжению. Время реакции меньше 1 мс. \endrussian */
	STATE_OVERLOAD_CURRENT	= 0x8000		/**< \english Current overlaod encountered. Motor current exceed maximum rating less then 1 ms \endenglish \russian Перегрузка по току. Время реакции меньше 1 мс. \endrussian */


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
	ENC_STATE_OK 			= 0x03

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
	WIND1_STATE_ABSENT 	= 0x00
	WIND1_STATE_UNKNOWN	= 0x01
	WIND1_STATE_MALFUNC	= 0x02
	WIND1_STATE_OK 			= 0x03
	WIND2_STATE_ABSENT 	= 0x00
	WIND2_STATE_UNKNOWN	= 0x10
	WIND2_STATE_MALFUNC	= 0x20
	WIND2_STATE_OK			= 0x30


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
	ADDSTATE_GPIO_DIR		= 0x01			/**< \english Direction of GPIO pin. Output if flag set; input otherwise \endenglish \russian Направление вывода GPIO. Если флаг установлен - выход, иначе вход \endrussian */
	ADDSTATE_GPIO_STATE	= 0x02			/**< \english State of GPIO pin \endenglish \russian Состояние вывода GPIO \endrussian */
	ADDSTATE_LEFT_BTN		= 0x04			/**< \english State of Left button pin \endenglish \russian Состояние вывода левой кнопки \endrussian */
	ADDSTATE_RIGHT_BTN	= 0x08			/**< \english State of Right button pin \endenglish \russian Состояние вывода правой кнопки \endrussian */
	ADDSTATE_BRAKE			= 0x10			/**< \english State of Brake pin \endenglish \russian Состояние вывода управления тормозом \endrussian */
	ADDSTATE_REV_SENSOR	= 0x20			/**< \english State of Revolution sensor pin \endenglish \russian Состояние вывода датчика оборотов \endrussian */
	ADDSTATE_HALL_A			= 0x40			/**< \english State of Hall_a pin \endenglish \russian Состояние вывода датчика холла(a) \endrussian */
	ADDSTATE_HALL_B			= 0x80			/**< \english State of Hall_b pin \endenglish \russian Состояние вывода датчика холла(b) \endrussian */
	ADDSTATE_HALL_C			= 0x100			/**< \english State of Hall_c pin \endenglish \russian Состояние вывода датчика холла(c) \endrussian */

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
	ENGINE_REVERSE				= 0x0001		/**< \english Reverse flag. It determines motor shaft rotation direction that corresponds to feedback counts increasing. If not set (default), motor shaft rotation direction under positive voltage corresponds to feedback counts increasing and vice versa. Change it if you see that positive directions on motor and feedback are opposite \endenglish \russian Флаг реверса. Связывает направление вращения мотора с направлением счета текущей позиции. При сброшенном флаге (по умолчанию) прикладываемое к мотору положительное напряжение увеличивает счетчик позиции. И наоборот, при установленном флаге счетчик позиции увеличивается, когда к мотору приложено отрицательное напряжение. Измените состояние флага, если положительное вращение мотора уменьшает счетчик позиции. \endrussian */
	ENGINE_FINISHING			= 0x0002		/**< \english Fine tuneup flag. If it set, engine makes special tuneup procedure and reach the predetermined end position accurately on low speed \endenglish \russian Флаг точной доводки. Если флаг установлен, мотор выполняет специальные действия для быстрого и точного достижения заданной позиции. \endrussian */
	ENGINE_MAX_SPEED			= 0x0004		/**< \english TODO Max speed flag. If it is set, engine uses maxumum speed achievable with the present engine settings as nominal speed \endenglish \russian Флаг максимальной скорости. Если флаг установлен, движение происходит на максимальной скорости. \endrussian */
	ENGINE_ANTIPLAY				= 0x0008		/**< \english Play compensation flag. If it set, engine makes backlash (play) compensation procedure and reach the predetermined position accurately on low speed \endenglish \russian Компенсация люфта. Если флаг установлен, позиционер будет подходить к заданной точке всегда с одной стороны. Например, при подходе слева никаких дополнительных действий не совершается, а при подходе справа позиционер проходит целевую позицию на заданное расстояния и возвращается к ней опять же справа. \endrussian */
	ENGINE_ACCEL_ON				= 0x0010		/**< \english Acceleration enable flag. If it set, motion begins with acceleration and ends with deceleration \endenglish \russian Ускорение. Если флаг установлен, движение происходит с ускорением. \endrussian */
	ENGINE_LIMIT_VOLT			= 0x0020		/**< \english Maxumum motor voltage limit enable flag \endenglish \russian Номинальное напряжение мотора. Если флаг установлен, напряжение на моторе ограничивается заданным номинальным значением. \endrussian */
	ENGINE_LIMIT_CURR			= 0x0040		/**< \english Maxumum motor current limit enable flag \endenglish \russian Номинальный ток мотора. Если флаг установлен, ток через мотор ограничивается заданным номинальным значением. \endrussian */
	ENGINE_LIMIT_RPM			= 0x0080		/**< \english TODO Maxumum motor speed limit enable flag \endenglish \russian Номинальная частота вращения мотора. Если флаг установлен, частота вращения ограничивается заданным номинальным значением. \endrussian */
	ENGINE_HOLD						= 0x0100		/**< \english Hold motor shaft when motor is stopped if this flag set \endenglish \russian Режим удержания. Ротор мотора удерживается в заданной позиции, если флаг установлен. \endrussian */
	ENGINE_DYNAMIC_HOLD		= 0x0200		/**< \english Holding motor shaft dynamically (power supply is applied to motor always), if set; when shaft position is different from specified, if not set \endenglish \russian Динамическое удержание. Если флаг установлен, воздействие мотора не прекращается при достижении заданной позиции, контроллер остается в состоянии Tune. Если флаг сброшен, при достижении заданной позиции контроллер переходит в состояние Stop,Brake. \endrussian */
	ENGINE_ONLY_FINISHING	= 0x0400		/**< \english Move in fine tuneup mode only if this flag set. Do not use this flag in normal operation mode. It used for PID tuning only \endenglish \russian Если флаг установлен, движение к точке начинается сразу в состоянии Tune. Это специальный режим, который используется при настройке ПИД контура. Не используйте его в обычной работе. \endrussian */

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
	MICROSTEP_MODE_FULL			= 0x01	/**< \english Full step mode. \endenglish \russian Полношаговый режим. \endrussian */
	MICROSTEP_MODE_FRAC_2		= 0x02	/**< \english 1/2 step mode. \endenglish \russian Деление шага 1/2. \endrussian */
	MICROSTEP_MODE_FRAC_4		= 0x03	/**< \english 1/4 step mode. \endenglish \russian Деление шага 1/4. \endrussian */
	MICROSTEP_MODE_FRAC_8		= 0x04	/**< \english 1/8 step mode. \endenglish \russian Деление шага 1/8. \endrussian */
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
	ENGINE_TYPE_DC				= 0x01	/**< \english DC motor. \endenglish \russian Мотор постоянного тока. \endrussian */
	ENGINE_TYPE_2DC				= 0x02	/**< \english 2 DC motors. \endenglish \russian Два мотора постоянного тока. \endrussian */
	ENGINE_TYPE_STEP			= 0x03	/**< \english Step motor. \endenglish \russian Шаговый мотор. \endrussian */
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
	DRIVER_TYPE_INTEGRATE			= 0x02	/**< \english Driver with integrate IC. \endenglish \russian Силовой драйвер с использованием ключей, интегрированных в микросхему \endrussian */
	DRIVER_TYPE_EXTERNAL			= 0x03	/**< \english External driver. \endenglish \russian Внешний силовой драйвер. \endrussian */
	

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
	POWER_OFF_ENABLED			= 0x02	/**< \english Power off enabled, if flag set. \endenglish \russian Выключение тока выполняется, если флаг установлен. \endrussian */
	POWER_OFF_MODE				= 0x0C	/**< \english TODO Power off enabled, if flag set. \endenglish \russian Выключение тока выполняется, если флаг установлен. \endrussian */
	

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
		* Describes feedback type
		* \endenglish
		* \russian
		* Тип обратной связи
		* \endrussian
		* @see set_feedback_settings 
		* @see get_feedback_settings 
		*/
flagset FeedbackType:
	FEEDBACK_POTENTIOMETER	= 0x00	/**< \english feedback by potentiometer. \endenglish \russian обратная связь с помощью потенциометра. \endrussian */
	FEEDBACK_ENCODER				=	0x01	/**< \english feedback by encoder. \endenglish \russian обратная связь с помощью энкодера. \endrussian */
	FEEDBACK_ENCODERDIFF		= 0x02	/**< \english feedback by encoder with differential input. \endenglish \russian обратная связь с помощью энкодера с дифференциальным входом. \endrussian */
	FEEDBACK_ENCODERHALL		= 0x03	/**< \english feedback by Hall detector. \endenglish \russian обратная связь с помощью датчика Холла. \endrussian */
	FEEDBACK_EMF						= 0x04	/**< \english feedback by EMF. \endenglish \russian обратная связь по ЭДС. \endrussian */
	FEEDBACK_NONE						= 0x05	/**< \english feedback is absent. \endenglish \russian обратная связь отсутствует. \endrussian */
	
	/** 
		* \english
		* Describes feedback flags
		* \endenglish
		* \russian
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
	TTL_PIN_GPIO			= 0x00	/**< \english Synchronization In/Out works as Out, if flag set; otherwise works as In \endenglish \russian Если флаг установлен, вывод синхронизации работает как выход; если флаг сброшен, вывод синхронизации работает как вход. \endrussian */
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
	TTL_SYNCIN_ONMOVE				= 0x01	/**< \english Infinite moving in TTL_SYNCIN_DIRRIGHT direction, if set; moving to given position or given shift, if not set \endenglish \russian Бесконечное вращение в направлении TTL_SYNCIN_DIRRIGHT, если флаг установлен; иначе - смещение на заданную позицию. \endrussian */
	TTL_SYNCIN_MULTI_SHIFT	= 0x04	/**< \english Response on every incoming pulse, if set; one time response, if not set \endenglish \russian Если флаг установлен, происходит многократное срабатывание по команде SHIFT; если сброшен - однократное срабатывание по команде MOVE. \endrussian */
	TTL_SYNCIN_DIRRIGHT			= 0x08	/**< \english Movement direction to the right, if set. Used when TTL_SYNCIN_ONMOVE set \endenglish \russian Направление вращения при синхронизации. Если флаг установлен, вращение вправо. Если флаг сброшен - влево. Используется, когда установлен флаг TTL_SYNCIN_ONMOVE. \endrussian */
	TTL_SYNCIN_FRONT				= 0x10	/**< \english Rising front is active, if set; falling front is active if not set. \endenglish \russian Передний фронт активный, если флаг установлен; иначе - задний. \endrussian */
	TTL_SYNCIN_ENABLED			= 0x20	/**< \english Synchronization in mode is enabled, if set. \endenglish \russian Синхронизация входа включена, если флаг установлен. \endrussian */
	
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
	TTL_SYNCOUT_ONSTART		= 0x01	/**< \english Generate synchronization pulse when moving started \endenglish \russian Генерация синхронизирующего импульса при начале движения. \endrussian */
	TTL_SYNCOUT_ONSTOP		= 0x02	/**< \english Generate synchronization pulse when moving stopped \endenglish \russian Генерация синхронизирующего импульса при остановке. \endrussian */
	TTL_SYNCOUT_ONPERIOD	= 0x04	/**< \english Generate synchronization pulse every impulse_period encoder pulses \endenglish \russian Генерация синхронизирующего импульса при каждом импульсе энкодера. \endrussian */
	TTL_SYNCOUT_FRONT			= 0x08	/**< \english High level is active, if set. \endenglish \russian Высокий лог.уровень является активным, если флаг установлен. \endrussian */
	TTL_SYNCOUT_ENABLED		= 0x10	/**< \english Synchronization out mode is enabled, if set. \endenglish \russian Синхронизация выхода включена, если флаг установлен. \endrussian */

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
	EXTIO_SETUP_MODE_IN_STOP					= 0x00
	EXTIO_SETUP_MODE_IN_POWER_OFF			= 0x01
	EXTIO_SETUP_MODE_IN_MOVR					= 0x02
	EXTIO_SETUP_MODE_OUT_OFF					= 0x00
	EXTIO_SETUP_MODE_OUT_ON						= 0x10
	EXTIO_SETUP_MODE_OUT_MOVING				= 0x20
	EXTIO_SETUP_MODE_OUT_ALARM				= 0x30
	EXTIO_SETUP_MODE_OUT_MEMORY_WRITE	= 0x40
	EXTIO_SETUP_MODE_OUT_MOTOR_ON			= 0x50
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
	ENDER_ON1			= 0x02	/**< \english First limit switch pulled position is closed, if set; otherwise pulled position is opened \endenglish \russian Если флаг установлен, первый концевой выключатель является нормально замкнутым; иначе - нормально разомкнутым. \endrussian */
	ENDER_ON2			= 0x04	/**< \english Second limit switch pulled position is closed, if set; otherwise pulled position is opened \endenglish \russian Если флаг установлен, второй концевой выключатель является нормально замкнутым; иначе - нормально разомкнутым. \endrussian */
	
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
	BRAKE_ENABLED		= 0x01		/**< \english Brake control is enabled, if flag set. \endenglish \russian Управление тормозом включено, если флаг установлен. \endrussian */
	BRAKE_ENG_PWROF	= 0x02		/**< \english Brake turns off power of step motor, if flag set. \endenglish \russian Тормоз отключает питание шагового мотора, если флаг установлен. \endrussian */
		
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
	CONTROL_MODE_OFF							= 0x00		/**< \english Control is disabled. \endenglish \russian Управление отключено. \endrussian */
	CONTROL_MODE_JOY							= 0x01		/**< \english Control by joystick. \endenglish \russian Управление с помощью джойстика. \endrussian */
	CONTROL_MODE_LR								= 0x02		/**< \english Control by buttons left/right. \endenglish \russian Управление с помощью кнопок left/right. \endrussian */
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
	CTP_ENABLED				= 0x01	/**< \english Position control is enabled, if flag set. \endenglish \russian Контроль позиции включен, если флаг установлен. \endrussian */
	CTP_BASE					= 0x02	/**< \english Base is revolution sensor, if flag set; else - encoder. \endenglish \russian Опорой является датчик оборотов, если флаг установлен; иначе - энкодер. \endrussian */
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
	HOME_STOP_FAST	= 0x02	/**< \english Fast motion stops by synchronization input, if set; otherwise by limit switch. \endenglish \russian Если флаг установлен, то первоначальное движение завершается по сигналу со входа синхронизации; иначе - по сигналу с концевого выключателя. \endrussian */
	HOME_DIR_SLOW		= 0x04	/**< \english Flag defines direction of 2nd motion. Direction is right, if set; otherwise left. \endenglish \russian Определяет направление второго движения мотора. Если флаг установлен - вправо; иначе - влево. \endrussian */
	HOME_REV_EN			= 0x08	/**< \english Revolution sensor is used, if set; otherwise this action passed. \endenglish \russian Если флаг установлен, используется Revolution sensor; иначе - этап пропускается. \endrussian */

/** $XIW
 * foo */
command "movr" writer "movr" (4)

/** vim: set ft=c: */
