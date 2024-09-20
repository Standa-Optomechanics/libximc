#ifndef INC_PROTOSUP_H
#define INC_PROTOSUP_H

/*
 * I/O helpers
 */

// simple checked command (and its unsynced pair)
result_t command_checked (device_t id, const void* command, size_t command_len, byte* response, size_t response_len);
result_t command_checked_unsynced (device_t id, const void* command, size_t command_len, byte* response, size_t response_len);
// simple checked string command (and its unsynced pair)
result_t command_checked_str (device_t id, const char* command, byte* response, size_t response_len);
result_t command_checked_str_unsynced (device_t id, const char* command, byte* response, size_t response_len);

// simple command with echo response (and its unsynced pair)
result_t command_checked_echo (device_t id, const void* command, size_t command_len);
result_t command_checked_echo_unsynced (device_t id, const void* command, size_t command_len);
// simple string command with echo response
result_t command_checked_echo_str (device_t id, const char* command);
result_t command_checked_echo_str_unsynced (device_t id, const char* command);
// simple string command with echo response, and it's also locked
result_t command_checked_echo_str_locked (device_t id, const char* command);

result_t check_in_overrun(device_t id, size_t data_count, size_t buf_size, const byte* response);
result_t check_in_overrun_without_crc(device_t id, size_t data_count, size_t buf_size, const byte* response);
result_t check_out_overrun (size_t data_count, size_t buf_size);
result_t check_out_atleast_overrun (size_t data_count, size_t buf_size);

device_t open_device_impl (const char* name, int timeout);
result_t close_device_impl (device_t* id);

result_t normal_correction(device_t* id, float* newPosition);
result_t rewers_correction(device_t* id, float* newPosition);

void push_data(byte** where, const void* data, size_t size);
void push_crc (byte** where, const void* data, size_t size);
void push_crc_with_command (byte** where, const void* data, size_t size);
void push_str (byte** where, const char* str);

void push_uint64 (byte** where, uint64_t value);
void push_uint32 (byte** where, uint32_t value);
void push_uint16 (byte** where, uint16_t value);
void push_uint8 (byte** where, uint8_t value);
void push_int64 (byte** where, int64_t value);
void push_int32 (byte** where, int32_t value);
void push_int16 (byte** where, int16_t value);
void push_int8 (byte** where, int8_t value);
// push as 32-bit, 64-bit
void push_float (byte** where, float value);
void push_double (byte** where, double value);
void push_garbage(byte** where, size_t size);

uint64_t pop_uint64 (byte** where);
unsigned int pop_uint32 (byte** where);
unsigned int pop_uint16 (byte** where);
unsigned int pop_uint8 (byte** where);
int64_t pop_int64 (byte** where);
int pop_int32 (byte** where);
int pop_int16 (byte** where);
int pop_int8 (byte** where);
// pop as 32-bit, 64-bit
float pop_float (byte** where);
double pop_double (byte** where);
void pop_data(byte** where, void* data, size_t size);
void pop_str(byte** where, void* data, size_t size);
void pop_garbage(byte** where, size_t size);

int powi (int x, int n);

#define MAX_ENUM_MICROSTEP_MODE MICROSTEP_MODE_FRAC_256

#define CLEAR_TABLE \
do { \
	remov_table(&dm->table.X, &dm->table.dX); \
	dm->table.len_table = 0; \
} while (0)

#define CLEAR_TABLE_CLOSE_FILE \
do { \
	remov_table(&dm->table.X, &dm->table.dX); \
	dm->table.len_table = 0; \
	fclose(fp); \
} while (0)

#define XI_normal_to_calibrate(fvalue, value, mvalue, coeff) \
do { \
	if ((coeff)->MicrostepMode == 0 || (coeff)->MicrostepMode > MAX_ENUM_MICROSTEP_MODE) \
		return result_value_error; \
	(fvalue) = (float)(((coeff)->A) * ((float)(value)+((float)(mvalue)) / powi(2, (coeff)->MicrostepMode - 1))); \
} while (0)

#define XI_normal_to_calibrate_short(fvalue, value, coeff) \
	do { \
		(fvalue) = (float)(((coeff)->A) * ((float)(value))); \
	} while (0)

#define XI_calibrate_to_normal(fvalue, value, mvalue, coeff) \
do { \
	if ((coeff)->MicrostepMode == 0 || (coeff)->MicrostepMode > MAX_ENUM_MICROSTEP_MODE) \
		return result_value_error; \
	(value) = (int)((fvalue) / (coeff)->A); \
	(mvalue) = (int)(((fvalue) / ((coeff)->A) - (value)) * powi(2, (coeff)->MicrostepMode - 1)); \
} while (0)

#define XI_calibrate_to_normal_short(fvalue, value, coeff) \
do { \
	(value) = (int)((fvalue) / (coeff)->A); \
} while (0)

#define XI_normal_to_calibrate_corr(fvalue, value, mvalue, coeff, table) \
  do { \
	if ((coeff)->MicrostepMode == 0 || (coeff)->MicrostepMode > MAX_ENUM_MICROSTEP_MODE) \
		return result_value_error;\
	(fvalue) = (float)(((coeff)->A) * ((float)(value)+((float)(mvalue)) / powi(2, (coeff)->MicrostepMode - 1))); \
	if (rewers_correction(&(table), &(fvalue)) == 0) \
		return result_value_error; \
  } while (0)

#define XI_normal_to_calibrate_corr_short(fvalue, value, coeff, table) \
  do { \
  (fvalue) = (float) (((coeff)->A) * ((float)(value) )); \
  if (rewers_correction(&(table), &(fvalue)) == 0) \
		return result_value_error; \
  } while (0)

#define XI_calibrate_to_normal_corr(fvalue, value, mvalue, coeff, table) \
  do { \
	if ((coeff)->MicrostepMode == 0 || (coeff)->MicrostepMode > MAX_ENUM_MICROSTEP_MODE) \
		return result_value_error; \
	float fvalue1;	\
	fvalue1 = (fvalue); \
	if (normal_correction(&(table), &(fvalue1)) == 0) \
		return result_value_error; \
	(value) = (int)((fvalue1) / (coeff)->A); \
	(mvalue) = (int)(((fvalue1) / ((coeff)->A) - (value)) * powi(2, (coeff)->MicrostepMode - 1)); \
  } while (0)

#define XI_calibrate_to_normal_corr_short(fvalue, value, coeff, table) \
  do { \
  float fvalue1;	\
  fvalue1 = (fvalue); \
  if (normal_correction(&(table), &(fvalue1)) == 0) \
	return result_value_error; \


#define XI_calibrate_to_normal_Dcorr(fvalue, value, mvalue, coeff, table) \
do {\
	if ((coeff)->MicrostepMode == 0 || (coeff)->MicrostepMode > MAX_ENUM_MICROSTEP_MODE) \
		return result_value_error; \
	float fvalue1, fvalue2;	\
	get_position_calb_t the_get_position_calb;\
	get_position_calb(table, &(the_get_position_calb), coeff); \
	fvalue2 = (the_get_position_calb).Position; \
	fvalue1 = (fvalue)+(fvalue2); \
	if (normal_correction(&(table), &(fvalue1)) == 0) \
		return result_value_error; \
	if (normal_correction(&(table), &(fvalue2)) == 0) \
		return result_value_error; \
	(value) = (int)((fvalue1 - fvalue2) / (coeff)->A); \
	(mvalue) = (int)(((fvalue1 - fvalue2) / ((coeff)->A) - (value)) * powi(2, (coeff)->MicrostepMode - 1)); \
} while (0)
/*
#define XI_normal_to_calibrate_corr(fvalue, value, mvalue, coeff, table) \
do {\
if ((coeff)->MicrostepMode == 0 || (coeff)->MicrostepMode > MAX_ENUM_MICROSTEP_MODE) \
	return result_value_error; \
	(fvalue) = (float)(((coeff)->A) * ((float)(value)+((float)(mvalue)) / powi(2, (coeff)->MicrostepMode - 1))); \
} while (0)

#define XI_normal_to_calibrate_corr_short(fvalue, value, coeff, table) \
do {	\
	(fvalue) = (float)(((coeff)->A) * ((float)(value))); \
} while (0)

#define XI_calibrate_to_normal_corr(fvalue, value, mvalue, coeff, table) \
do { \
if ((coeff)->MicrostepMode == 0 || (coeff)->MicrostepMode > MAX_ENUM_MICROSTEP_MODE) \
	return result_value_error; \
	(value) = (int)((fvalue) / (coeff)->A); \
	(mvalue) = (int)(((fvalue) / ((coeff)->A) - (value)) * powi(2, (coeff)->MicrostepMode - 1)); \
} while (0)

#define XI_calibrate_to_normal_corr_short(fvalue, value, coeff, table) \
do { \
	(value) = (int)((fvalue) / (coeff)->A); \
} while (0)
*/

/*
 * Fine-grained locks
 */
void lock(device_t id);
void unlock(device_t id);
result_t unlocker (device_t id, result_t res);

/*
 * Global lock
 */
void lock_global();
void unlock_global ();
result_t unlocker_global (result_t res);

typedef struct device_enumeration_opaque_t
{
	int allocated_count;
	int count;
	int flags;
	char** names;
	char** raw_names;
	uint32_t* serials;
	device_information_t* infos;
	controller_name_t* controller_names;
	stage_name_t* stage_names;
	device_network_information_t* dev_net_infos;
} device_enumeration_opaque_t;

uint32_t conn_id_by_device_id(device_t id);
uint32_t serial_by_device_id(device_t id);

/*
 * Global metadata lock
 */
void lock_metadata();
void unlock_metadata();

/*
 * File log
 */
void filelog_text(const char* direction, device_type_t type,
		uint32_t serial, const char* line);
void filelog_data(const char* direction, device_type_t type,
		uint32_t serial, const char* ptr, size_t length);


#endif
