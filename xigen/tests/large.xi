protocol "v1"
defaults with crc, answer

command "move" writer "move"  (18)
with crc
fields:
  int32s position
  int16s uposition
  reserved 6
  
command "home" writer "home" (4)

command "right" writer "rigt" (4)

/** $XIW
Usual doxygen 1
*/
/** $XIR
Usual doxygen 2
*/
/** $XIS
Usual doxygen 3
*/
command "home_settings" reader "ghom" (32) writer "shom"  (32)
fields:
  int32u FastHome 
  int8u uFastHome
  int32u SlowHome /**< Usual doxygen 2 */
  int8u uSlowHome
  int32s HomeDelta
  int16s uHomeDelta
  reserved 10

command "home_settings" reader "ghom"  (33)writer "shom"  (33)
fields:
  int32u FastHome 
  int8u uFastHome
  int32u SlowHome /***< Usual doxygen 2 */
  int8u uSlowHome
  int32s HomeDelta
  int16s uHomeDelta
  int8u flag home_flags 
  reserved 10


/** $XIW
      * \english
      * Describes motor state
      * \endenglish
      * \russian
      * Состояние мотора
      * \endrussian
      */
command "write_key" writer "wwww" (10)
with crc, answer, inline
fields:			
  int32u key   /**< \english PWM is off, motor winding is cut off power. \endenglish \russian ШИМ выключен, мотор отключен от контроллера. \endrussian */

command "write_key" writer "rrrr"  (10)
with crc, answer, inline
fields:
  int32u key

//@{

/** $XIW
  foobar3
	fff
*/
command "disconnect" writer "xxxx" (4)
with crc, answer

/** vim: set ft=c: */
