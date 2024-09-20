protocol "v1"
defaults with crc, answer, public

command "readinlinearray" reader "rria" (38)
with inline
fields:
	int32u arr[8]

command "inlinevar" writer "winv" (10)
with inline
fields:
	int32u serial

command "faker" reader "fake" (18)
with inline
fields:
	int64u fielda
	int32u fieldb

command "barrr" universal "arr" (38)
fields:
	int32u myarr[8]

command "foobar" universal "foo" (30)
fields:
	calb float position
  normal int32s position
	normal int16s uposition
	int16s test
	int16s arr[8]

command "alpha" universal "alp" (72)
fields:
	calb float position[8]
  normal int32s position[8]
  normal int32s microposition[8]
	int16s test

command "beta" writer "wbet" (72)
with inline
fields:
	calb float position[8]
  normal int32s position[8]
  normal int32s microposition[8]
	int16s test


/** vim: set ft=c: */
