protocol "v1"
defaults with crc, answer, public


/** $XIR
	* \english
	* Read settings of joystick.
	* If joystick position is outside DeadZone limits from the central position a movement with speed,
	* defined by the joystick DeadZone edge to 100% deviation, begins. Joystick positions inside DeadZone limits 
	* correspond to zero speed (soft stop of motion) and positions beyond Low and High limits correspond MaxSpeed [i] 
	* or -MaxSpeed [i] (see command SCTL), where i = 0 by default and can be changed with left/right buttons (see command SCTL).
	* This is illustrated on the following picture.
	* !/attachments/download/3092/ExpJoystick.png!
	* The relationship between the deviation and the rate is exponential,
	* allowing no switching speed combine high mobility and accuracy. The following picture illustrates this: 
	* !/attachments/download/3092/ExpJoystick.png!
	* The nonlinearity parameter is adjustable. Setting it to zero makes deviation/speed relation linear.
	* @param id an identifier of device
	* @param[out] joystick_settings structure contains joystick settings
	* \endenglish
	* \russian
	* Чтение настроек джойстика.
	* \endrussian
	*/
/** $XIS
	* \english
	* Joystick settings.
	* \endenglish
	* \russian
	* Настройки джойстика.
	* \endrussian
	*/
command "foo_settings" reader "gfoo"(22)
fields:
	int16u JoyLowEnd	/**< \english Joystick lower end position. \endenglish \russian Значение в шагах джойстика, соответствующее нижней границе диапазона отклонения устройства. Должно лежать в пределах [0,10000]. \endrussian */	
	int16u JoyCenter	/**< \english Joystick center position. \endenglish \russian Значение в шагах джойстика, соответствующее неотклонённому устройству. Должно лежать в пределах [0,10000]. \endrussian */	
	int16u JoyHighEnd	/**< \english Joystick higher end position. \endenglish \russian Значение в шагах джойстика, соответствующее верхней границе диапазона отклонения устройства. Должно лежать в пределах [0,10000]. \endrussian */	
	int8u ExpFactor	/**< \english Exponential nonlinearity factor. \endenglish \russian Фактор экспоненциальной нелинейности отклика джойстика. \endrussian */	
	int8u DeadZone		/**< \english Joystick dead zone. \endenglish \russian Отклонение от среднего положения, которое не вызывает начала движения (в десятых долях процента). Максимальное мёртвое отклонение +-25.5%, что составляет половину рабочего диапазона джойстика. \endrussian */	
	int8u flag JoyFlags	/**< \english Joystick control flags. \endenglish \russian Флаги управления джойстиком. \endrussian */
	reserved 7

