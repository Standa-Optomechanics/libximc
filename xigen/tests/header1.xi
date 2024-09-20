protocol "v1"
defaults with crc, answer, public

command "command_move" writer "move" (18)
with inline
fields:
  int32s position
  int16s uposition
  reserved 6

command "foobar" universal "fbr" (18)
fields:
  int32s position
  int16s uposition
  reserved 6

command "get_foobarbaz" reader "gfbb" (18)
fields:
  int32s position
  int16s uposition
  reserved 6

command "macguffin" reader "gmcg" (26)
fields:
  int32u field1
  int32s field2
  int16u field3
  int16s field4
  int8u field5
  int8s field6
	int16u flag additional_flags		
	float foobar

command "left" writer "left" (4)

command "leftX" writer "left" (4)
without lock

command "serial_number" writer "sser" (14)
with inline
fields:
	int32u serial
	int32u key

command "serial_number" reader "gser" (10)
with inline
fields:
	int32u serial

command "stringified" reader "geti" (36)
fields:
	char manufacturer[4]
	char manufacturer_id[2]
	char product_description[8]
	reserved 16

command "stringifiedX" reader "geti" (10)
without lock
fields:
	int32u foobar

command "reset" writer "rest" (4)
without answer, lock

command "non_public_struct_private1" reader "get1" (10)
without public
fields:
	int32u foo
	
command "non_public_struct_private2" reader "get2" (10)
without public, publicstruct
fields:
	int32u foo		
	
command "non_public_struct_public" reader "get3" (10)
without public
with publicstruct
fields:
	int32u foo

command "inlinearray" writer "winl" (40)
with inline
fields:
	int32u arr[8] metalen
	int16u extra

/** $XIW
	this should test inline parameter 'unsigned char' type 
*/
command "inlinearray2" writer "winm" (14)
with inline
fields:
	int8u arr[8] metalen

/** $XIW
 */
/** $XIS
 * This must be changed later to produce an array of uint8_t at libximc header generation. Now it is just an unsigned int. The best way is to mark it with a special type aka 'bytes foo[8]'
*/
command "truearray" writer "trua" (14)
fields:
	int8u foo[8]

/** vim: set ft=c: */
