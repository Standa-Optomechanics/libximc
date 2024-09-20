protocol "v1"
defaults with crc, answer, public

command "foo1_settings" universal "fo1" (10) 
fields:
	int32u speed			

/** $XIR
 * reader
 */
/** $XIW
 * writer
 */
/** $XIS
 * struct
 */
command "foobar" universal "fbr" (10)
with dualsync
fields:
  int32s position

command "bazbaz" reader "rbbz" (10)
with dualsync
fields:
  int32s velocity

command "quxqux_impl" reader "rqqx" (10)
with dualsync
fields:
  int32s velocity

command "foo2_settings" universal "fo2" (10) 
fields:
	int32u speed			

