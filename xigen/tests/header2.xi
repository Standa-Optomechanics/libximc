protocol "v1"
defaults with crc, answer, public

command "reset" writer "rest" (4)
without answer, lock

command "updf" writer "updf" (4)
without public, lock

command "service_command_write_key_impl" writer "wkey" (18) reader "wkey" (15)
with inline
without public
fields:
	int32u key
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

command "service_command_goto_firmware_impl" writer "gofw" (4) reader "gofw" (15)
with inline
without public, crc
answer:
	int8u serviceanswer sresult
	reserved 8

command "service_command_has_firmware_impl" writer "hasf" (4) reader "hasf" (15)
with inline
without public, crc
answer:
	int8u serviceanswer sresult
	reserved 8

command "service_command_connect" writer "conn" (14) reader "conn" (15)
without public
with inline
fields:
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

command "service_command_disconnect" writer "disc" (14) reader "disc" (15)
without public
with inline
fields:
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

command "service_command_write_data" writer "wdat" (143) reader "wdat" (15)
without public
with inline
fields:
	int8u datalen
	int8u data[@datalen]
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

command "service_command_write_data1" writer "wdax" (142) reader "wdax" (4)
without public
with inline
fields:
	int8u data[128]
	reserved 8

command "service_command_new_section" writer "nsec" (22) reader "nsec" (15)
without public
with inline
fields:
	int8u data[8]
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

command "service_command_metacomm" writer "msec" (22) reader "msec" (15)
without public
with inline
fields:
	int8u data[8] metalen
	reserved 8
answer:
	int8u serviceresult sresult
	reserved 8

/** vim: set ft=c: */
