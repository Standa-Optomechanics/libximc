protocol "v1"
defaults with crc, answer, public

command "foobar" universal "fbr" (10)
with dualsync
fields:
  int32s position

/** vim: set ft=c: */
