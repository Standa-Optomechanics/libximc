protocol "v1"
defaults with crc, answer

command "right" writer "movr" (4)

command "move" writer "movr" (18)
with inline
fields:
  int32s position
  int16s uposition
  reserved 6
  
command "stage_information" writer "spwm" (18) reader "gpwm" (18)
fields:
  int32s position
  int16s uposition
  reserved 6
  
command "pwm_freq" universal "pwm" (10) 
with inline
fields:
  int32s freq

command "pwm_freq_xtend" universal "pwm" (44) 
fields:
  int32u freq
  int16s uposition
  int16s uposition[10]
  int16s flag myflags of fooflags
	float rational
  reserved 6
  
  
command "weird" universal "wrd" (136) 
fields:
  int16u len = 120
  int8u varlen[120]
  reserved 8

command "device_information" universal "eti" (36) 
fields:
				char manufacturer[10]
				char manufacturer_id[10]
				char desc[10]

/** vim: set ft=c: */
