protocol "v1"
defaults with crc, answer, public

/**
	* \english
	* @name Control flags
	* Specify motor control settings by joystick or buttons.
	* May be combined with bitwise OR.
	* \endenglish
	* \russian
	* @name Флаги управления
	* Определяют параметры управления мотором с помощью джойстика или кнопок.
	* Могут быть объединены с помощью побитового ИЛИ.
	* \endrussian
	*/
flagset ControlFlags:
CONTROL_MODE_BITS				= 0x03	/**< \english Bits to control engine by joystick or buttons. \endenglish \russian Биты управления мотором с помощью джойстика или кнопок влево/вправо. \endrussian */
CONTROL_MODE_OFF				= 0x00	/**< \english Control is disabled. \endenglish \russian Управление отключено. \endrussian */
CONTROL_MODE_JOY				= 0x01	/**< \english Control by joystick. \endenglish \russian Управление с помощью джойстика. \endrussian */
CONTROL_MODE_LR					= 0x02	/**< \english Control by left/right buttons. \endenglish \russian Управление с помощью кнопок left/right. \endrussian */
CONTROL_BTN_LEFT_PUSHED_OPEN	= 0x04	/**< \english Pushed left button corresponds to open contact, if this flag is set. \endenglish \russian Левая кнопка нормально разомкнутая, если флаг установлен. \endrussian */
CONTROL_BTN_RIGHT_PUSHED_OPEN	= 0x08	/**< \english Pushed right button corresponds to open contact, if this flag is set. \endenglish \russian Правая кнопка нормально разомкнутая, если флаг установлен. \endrussian */

/** $XIR
	* \english
	* Read settings of motor control.
	* When choosing CTL_MODE = 1 switches motor control with the joystick.
	* In this mode, the joystick to the maximum engine tends
	* Move at MaxSpeed [i], where i = 0 if the previous use
	* This mode is not selected another i. Buttons switch the room rate i.
	* When CTL_MODE = 2 is switched on motor control using the
	* Left / right. When you click on the button motor starts to move in the appropriate direction at a speed MaxSpeed [0],
	* at the end of time Timeout [i] motor move at a speed MaxSpeed [i+1]. at
	* Transition from MaxSpeed [i] on MaxSpeed [i +1] to acceleration, as usual.
	* @param id an identifier of device
	* @param[out] control_settings structure contains settings motor control by joystick or buttons left/right.
	* \endenglish
	* \russian
	* Чтение настроек управления мотором.
	* При выборе CTL_MODE=1 включается управление мотором с помощью джойстика.
	* В этом режиме при отклонении джойстика на максимум двигатель стремится
	* двигаться со скоростью MaxSpeed [i], где i=0, если предыдущим использованием
	* этого режима не было выбрано другое i. Кнопки переключают номер скорости i.
	* При выборе CTL_MODE=2 включается управление мотором с помощью кнопок
	* left/right. При нажатии на кнопки двигатель начинает двигаться в соответствующую сторону со скоростью MaxSpeed [0], по истечении времени Timeout[i] мотор
	* двигается со скоростью MaxSpeed [i+1]. При
	* переходе от MaxSpeed [i] на MaxSpeed [i+1] действует ускорение, как обычно.
	* @param id идентификатор устройства
	* @param[out] control_settings структура, содержащая настройки управления мотором с помощью джойстика или кнопок влево/вправо.
	* \endrussian
	*/
/** $XIW
	* \english
	* Set settings of motor control.
	* When choosing CTL_MODE = 1 switches motor control with the joystick.
	* In this mode, the joystick to the maximum engine tends
	* Move at MaxSpeed [i], where i = 0 if the previous use
	* This mode is not selected another i. Buttons switch the room rate i.
	* When CTL_MODE = 2 is switched on motor control using the
	* Left / right. When you click on the button motor starts to move in the appropriate direction at a speed MaxSpeed [0],
	* at the end of time Timeout [i] motor move at a speed MaxSpeed [i+1]. at
	* Transition from MaxSpeed [i] on MaxSpeed [i +1] to acceleration, as usual.
	* @param id an identifier of device
	* @param[in] control_settings structure contains settings motor control by joystick or buttons left/right.
	* \endenglish
	* \russian
	* Запись настроек управления мотором.
	* При выборе CTL_MODE=1 включается управление мотором с помощью джойстика.
	* В этом режиме при отклонении джойстика на максимум двигатель стремится
	* двигаться со скоростью MaxSpeed [i], где i=0, если предыдущим использованием
	* этого режима не было выбрано другое i. Кнопки переключают номер скорости i.
	* При выборе CTL_MODE=2 включается управление мотором с помощью кнопок
	* left/right. При нажатии на кнопки двигатель начинает двигаться в соответствующую сторону со скоростью MaxSpeed [0], по истечении времени Timeout[i] мотор
	* двигается со скоростью MaxSpeed [i+1]. При
	* переходе от MaxSpeed [i] на MaxSpeed [i+1] действует ускорение, как обычно.
	* @param id идентификатор устройства
	* @param[in] control_settings структура, содержащая настройки управления мотором с помощью джойстика или кнопок влево/вправо.
	* \endrussian
	*/
/** $XIS
	* \english
	* Control settings.
	* This structure contains control parameters.
	* When choosing CTL_MODE=1 switches motor control with the joystick.
	* In this mode, the joystick to the maximum engine tends
	* Move at MaxSpeed [i], where i=0 if the previous use
	* This mode is not selected another i. Buttons switch the room rate i.
	* When CTL_MODE=2 is switched on motor control using the
	* Left / right. When you click on the button motor starts to move in the appropriate direction at a speed MaxSpeed [0],
	* at the end of time Timeout[i] motor move at a speed MaxSpeed [i+1]. at
	* Transition from MaxSpeed [i] on MaxSpeed [i+1] to acceleration, as usual.
	* The figure above shows the sensitivity of the joystick feature on its position.
	* \endenglish
	* \russian
	* Настройки управления.
	* При выборе CTL_MODE=1 включается управление мотором с помощью джойстика.
	* В этом режиме при отклонении джойстика на максимум двигатель стремится
	* двигаться со скоростью MaxSpeed [i], где i=0, если предыдущим использованием
	* этого режима не было выбрано другое i. Кнопки переключают номер скорости i.
	* При выборе CTL_MODE=2 включается управление мотором с помощью кнопок
	* left/right. При нажатии на кнопки двигатель начинает двигаться в соответствующую сторону со скоростью MaxSpeed [0], по истечении времени Timeout[i] мотор
	* двигается со скоростью MaxSpeed [i+1]. При
	* переходе от MaxSpeed [i] на MaxSpeed [i+1] действует ускорение, как обычно.
	* \endrussian
	*/
command "control_settings" universal "ctl"(93)
fields:
	calb float MaxSpeed [10]			/**< \english Array of speeds using with joystick and button control. \endenglish \russian Массив скоростей, использующийся при управлении джойстиком или кнопками влево/вправо. \endrussian */
	normal int32u MaxSpeed [10]			/**< \english Array of speeds (full step) using with joystick and button control. Range: 0..1000000. \endenglish \russian Массив скоростей (в полных шагах), использующийся при управлении джойстиком или кнопками влево/вправо. Диапазон: 0..1000000. \endrussian */
	normal int8u uMaxSpeed [10]			/**< \english Array of speeds (1/256 microstep) using with joystick and button control. Range: 0..255. \endenglish \russian Массив скоростей (в 1/256 микрошагах), использующийся при управлении джойстиком или кнопками влево/вправо. Диапазон: 0..255. \endrussian */
	int16u Timeout [9]					/**< \english timeout[i] is time in ms, after that max_speed[i+1] is applying. It is using with buttons control only. Range: 0..65535. \endenglish \russian timeout[i] - время в мс, по истечении которого устанавливается скорость max_speed[i+1] (используется только при управлении кнопками). Диапазон: 0..65535. \endrussian */
	int16u MaxClickTime					/**< \english Maximum click time. Prior to the expiration of this time the first speed isn't enabled. \endenglish \russian Максимальное время клика. До истечения этого времени первая скорость не включается. \endrussian */
	int16u flag Flags of ControlFlags	/**< \english Flags. \endenglish \russian Флаги. \endrussian */
	calb float DeltaPosition			/**< \english Shift (delta) of position \endenglish \russian Смещение (дельта) позиции \endrussian */
	normal int32s DeltaPosition			/**< \english Shift (delta) of position \endenglish \russian Смещение (дельта) позиции \endrussian */
	normal int16s uDeltaPosition		/**< \english Fractional part of the shift in micro steps (-255 .. 255) is only used with stepper motor \endenglish \russian Дробная часть смещения в микрошагах (-255..255) используется только с шаговым двигателем \endrussian */
	reserved 9

/** vim: set ft=xi ts=4 sw=4: */
