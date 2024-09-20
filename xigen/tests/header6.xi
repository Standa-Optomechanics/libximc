protocol "v1"
defaults with crc, answer, public

/** $XIS
Usual doxygen struct
*/
/** $XIW
Usual doxygen write
*/
/** $XIR
Usual doxygen read
*/
command "testdoc1" universal "doc" (10) 
fields:
	int32s foo

/** $XIW
Usual doxygen write
*/
/** $XIS
Usual doxygen struct
*/
/** $XIR
Usual doxygen read
*/
command "testdoc2" universal "doc" (10) 
fields:
	int32s foo

/** $XIR
Usual doxygen read
*/
/** $XIS
Usual doxygen struct
*/
command "testdoc3" reader "tdoc" (10) 
fields:
	int32s foo

/** $XIS
Usual doxygen struct
*/
/** $XIW
Usual doxygen write
*/
/** $XIR
Usual doxygen read
*/
/** $XISC
Usual doxygen struct calb
*/
/** $XIRC
Usual doxygen read calb
*/
/** $XIWC
Usual doxygen write calb
*/
command "testdoc4" universal "doc" (12) 
fields:
	calb float foo
	normal int32s foo
	normal int16s ufoo


