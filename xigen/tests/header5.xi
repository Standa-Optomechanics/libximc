protocol "v1"
defaults with crc, answer, public

command "foobar" universal "foo" (40)
fields:
	calb float position
  normal int32s position
	normal int16s uposition
	int16s test
	char strfoo[6]
	int16s arr[8]
	char strbar[4]

command "bazqux" universal "qux" (30)
with inline
fields:
	calb float position
  normal int32s position
	normal int16s uposition
	int16s test
	int16s arr[8]

command "repeated" universal "rpt" (24)
with inline
fields:
	calb float position
  normal int32s position
	normal int16s uposition
	int16s test
	calb float speed
  normal int32s speed
	normal int16s uspeed
	float another

command "foobar2" universal "foo" (28)
with inline
fields:
	calb float position
  normal int32s position
	int16s test
	int16s arr[8]

command "foobar3" universal "foo" (36)
with inline
fields:
	calb float position
  normal int32s position
	calb float speed
  normal int32s speed
  normal int32s microspeed
	int16s test
	int16s arr[8]

command "arr1" universal "arr" (72)
with inline
fields:
	calb float position[8]
  normal int32s position[8]
  normal int32s microposition[8]
	int16s test

command "arr2" universal "arr" (72)
fields:
	calb float position[8]
  normal int32s position[8]
  normal int32s microposition[8]
	int16s test

command "arr3" universal "arr" (48)
fields:
	calb float position[8]
  normal int32s position[8]
  normal int8s microposition[8]
	int16s test

command "byted" universal "byt" (64)
fields:
	calb float position[8]
  normal int32s position[8]
  normal int8s microposition[8]
  byte key[16]
	int16s test

