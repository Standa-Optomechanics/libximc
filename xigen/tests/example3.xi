protocol "v1"
defaults with crc, answer

command "move" writer "move" (4)

service command "move" writer "move" (4)

command "rest" writer "rest" (4) reader "rest" (0)
with crc without answer

command "rest2" writer "rest" (4)
with crc without answer

command "rest3" writer "rest" (4)
without answer

command "rest4" writer "rest" (4) reader "rest" (0)
without answer

command "rest4" writer "rest" (4) reader "rest" (4)

command "nsec" writer "wsec" (22) reader "rsec" (15)
fields:
				int8u data[8]
				reserved 8
answer:
				int8u res
				reserved 8

  
  
/** vim: set ft=c: */
