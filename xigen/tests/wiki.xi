protocol "v1"
defaults with crc, answer, public


/**
 	* \english
	* @name Border flags
	* Specify types of borders and motor behaviour on borders.
	* May be combined with bitwise OR.
	* \endenglish
	* \russian
	* @name Флаги границ
	* Типы границ и поведение позиционера на границах.
	* Могут быть объединены с помощью побитового ИЛИ.
	* \endrussian
	* @see get_edges_settings
	* @see set_edges_settings
	*/
flagset BorderFlags:
BORDER_IS_ENCODER	= 0x01	/**< \english Borders are fixed by predetermined encoder values, if set; borders position on limit switches, if not set. \endenglish \russian Если флаг установлен, границы определяются предустановленными точками на шкале позиции. Если флаг сброшен, границы определяются концевыми выключателями. \endrussian */
BORDER_STOP_LEFT	= 0x02	/**< \english Motor should stop on left border. \endenglish \russian Если флаг установлен, мотор останавливается при достижении левой границы. \endrussian */
BORDER_STOP_RIGHT	= 0x04	/**< \english Motor should stop on right border. \endenglish \russian Если флаг установлен, мотор останавливается при достижении правой границы. \endrussian */

/** 
 	* \english
	* @name Limit switches flags
	* Specify electrical behaviour of limit switches like order and pulled positions.
	* May be combined with bitwise OR.
	* \endenglish
	* \russian
	* @name Флаги концевых выключателей
	* Определяют направление и состояние границ. 
	* Могут быть объединены с помощью побитового ИЛИ.
	* \endrussian
	* @see get_edges_settings
	* @see set_edges_settings
	*/
flagset EnderFlags:
ENDER_SWAP		= 0x01	/**< \english First limit switch on the right side, if set; otherwise on the left side. \endenglish \russian Если флаг установлен, первый концевой выключатель находится справа; иначе - слева. \endrussian */
ENDER_LEFT_ACTIVE_LOW	= 0x02	/**< \english 1 - left limit switch is triggered by a low level on pin. \endenglish \russian 1 - левый концевик считается сработавшим по низкому уровню на контакте. \endrussian */
ENDER_RIGHT_ACTIVE_LOW	= 0x04	/**< \english 1 - right limit switch is triggered by a low level on pin. \endenglish \russian 1 - правый концевик считается сработавшим по низкому уровню на контакте. \endrussian */


/** $XIR
	* \english
	* Read border and limit switches settings.
	* @see set_edges_settings
	* @param id an identifier of device
	* @param[out] edges_settings edges settings, specify types of borders, motor behaviour and electrical behaviour of limit switches
	* \endenglish
	* \russian
	* Чтение настроек границ и концевых выключателей.
	* @see set_edges_settings
	* @param id идентификатор устройства
	* @param[out] edges_settings настройки, определяющие тип границ, поведение мотора при их достижении и параметры концевых выключателей
	* \endrussian
	*/
/** $XIW
	* \english
	* Set border and limit switches settings.
	* @see set_edges_settings
	* @param id an identifier of device
	* @param[in] edges_settings edges settings, specify types of borders, motor behaviour and electrical behaviour of limit switches
	* \endenglish
	* \russian
	* Запись настроек границ и концевых выключателей.
	* @see get_edges_settings
	* @param id идентификатор устройства
	* @param[in] edges_settings настройки, определяющие тип границ, поведение мотора при их достижении и параметры концевых выключателей
	* \endrussian
	*/
/** $XIS
	* \english
	* Edges settings.
	* This structure contains border and limit switches settings.
	* Please load new engine settings when you change positioner etc.
	* Please note that wrong engine settings lead to device malfunction, can lead to irreversible damage of board.
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
	int8u flag BorderFlags	of BorderFlags	/**< \english Border flags, specify types of borders and motor behaviour on borders. \endenglish \russian Флаги, определяющие тип границ и поведение мотора при их достижении. \endrussian */
	int8u flag EnderFlags of EnderFlags	/**< \english Ender flags, specify electrical behaviour of limit switches like order and pulled positions. \endenglish \russian Флаги, определяющие настройки концевых выключателей. \endrussian */
	int32s LeftBorder		/**< \english Left border position, used if BORDER_IS_ENCODER flag is set. Range: -2147483647..2147483647. \endenglish \russian Позиция левой границы, используется если установлен флаг BORDER_IS_ENCODER. Диапазон: -2147483647..2147483647. \endrussian */
	int16s uLeftBorder		/**< \english Left border position in 1/256 microsteps(used with stepper motor only). Range: -255..255. \endenglish \russian Позиция левой границы в 1/256 микрошагах( используется только с шаговым двигателем). Диапазон: -255..255. \endrussian */
	int32s RightBorder		/**< \english Right border position, used if BORDER_IS_ENCODER flag is set. Range: -2147483647..2147483647. \endenglish \russian Позиция правой границы, используется если установлен флаг BORDER_IS_ENCODER. Диапазон: -2147483647..2147483647. \endrussian */
	int16s uRightBorder		/**< \english Right border position in 1/256 microsteps. Range: -255..255(used with stepper motor only). \endenglish \russian Позиция правой границы в 1/256 микрошагах( используется только с шаговым двигателем). Диапазон: -255..255. \endrussian */
	reserved 6

/** vim: set ft=c ts=8 sw=8: */
