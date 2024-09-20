protocol "v1"
defaults with crc, answer, public

/**
	* \english
	* @name State
	* Describes motor state
	* \endenglish
	* \russian
	* @name Состояние
	* Состояние мотора
	* \endrussian
	*/
flagset DevStateFoobar:
	DEV_STATE_FOOBAR_OFF 		= 0x00	/**< \english motor is turned off. \endenglish \russian мотор отключен от контроллера. \endrussian */
	DEV_STATE_FOOBAR_STOP 		= 0x01	/**< \english motor is stopped. \endenglish \russian мотор остановлен. \endrussian */
	DEV_STATE_FOOBAR_MOVING 	= 0x02	/**< \english motor is moving. \endenglish \russian мотор работает. \endrussian */

command "foobar" reader "gfbr" (13)
with inline
fields:
  int8u flag flags 
  reserved 6

command "baz" reader "gfbr" (13)
with inline
fields:
  int8u flag flags
  reserved 6

/** vim: set ft=c: */
