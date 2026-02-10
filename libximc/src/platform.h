#ifndef INC_PLATFORM_H
#define INC_PLATFORM_H

/*
 * Serial port support
 */

// Return code on success
#define result_serial_ok 0

// Return code on generic error
#define result_serial_error -1

// Return code on device lost
#define result_serial_nodevice -2

// Return code on timeout
#define result_serial_timeout -3


/*
 * Platform-specific serial port routines
 */
result_t open_port_serial(device_metadata_t* metadata, const char* name);
int close_port_serial(device_metadata_t* metadata);
int flush_port_serial(device_metadata_t* metadata);
ssize_t read_port_serial(device_metadata_t* metadata, void* buf, size_t amount);
ssize_t write_port_serial(device_metadata_t* metadata, const void* buf, size_t amount);


/*
 * Default ports values for TCP, UDP
 */

#define XIMC_UDP_PORT 1818
#define XIMC_TCP_PORT 1820


/*
 * Platform-specific UDP routines
 */

/**
 * Opens UDP-socket as a client.
 * Assumes host contains as follows: just an IP-address:port.
 */
result_t open_udp(device_metadata_t* metadata, const char* ip4_port);

/**
 * Closes previuosly opened UDP socket and removes all metadata associated.
 */
result_t close_udp(device_metadata_t* metadata);

/**
 * Writes to UDP socket.
 */
ssize_t write_udp(device_metadata_t* metadata, const byte* command, size_t command_len);

/**
 * Reads from  UDP socket.
 */
ssize_t read_udp(device_metadata_t* metadata, void* buf, size_t amount);


/*
 * Platform-specific TCP routines
 */

/**
 * Opens TCP-socket as a client.
 * Assumes host contains as follows: just an IP-address:port.
 */
result_t open_tcp(device_metadata_t* metadata, const char* ip4_port);

/**
 * Closes previuosly opened TCP socket and removes all metadata associated.
 */
result_t close_tcp(device_metadata_t* metadata);

/**
 * Writes to TCP socket.
 */
ssize_t write_tcp(device_metadata_t* metadata, const byte* command, size_t command_len);

/**
 * Reads from  TCP socket.
 */
ssize_t read_tcp(device_metadata_t* metadata, void* buf, size_t amount);


/*
* Mutex
*/

typedef struct mutex_t mutex_t;
void mutex_close(mutex_t* mutex);
mutex_t* mutex_init(unsigned int nonce);
void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);


/*
 * Threading support
 */

unsigned long long get_thread_id();

/* Callback for user actions */
typedef void (*fork_join_thread_function_t)(void* arg);

/* Platform-specific fork/join function */
result_t fork_join(fork_join_thread_function_t function, int count, void* args, size_t arg_element_size);

/* Platform-specific fork/join function with timeout */
result_t fork_join_with_timeout(fork_join_thread_function_t function, int count, void* args, size_t arg_element_size, int timeout_ms, mutex_t* ext_mutex);

/* Platform-specific fork/join function for 2 fucntions */
void fork_join_2_threads(fork_join_thread_function_t function_1, void* args_1, fork_join_thread_function_t function_2, void* args_2);

/* Platform-specific thread launcher */
void single_thread_launcher(XIMC_RETTYPE(XIMC_CALLCONV *func)(void*), void* arg);


/*
 * Device enumeration support
 */

/* Callback for user actions */
typedef void (*enumerate_devices_directory_callback_t)(char* name, void* arg);

/**
 * The function searches for local devices.
 * @param[in] callback Callback function that saves found devices to the 'dev_enum' structure.
 * @param[out] arg Pointer to 'dev_enum' structure.
 * @param[in] flags 
 */
result_t discover_local_devices(enumerate_devices_directory_callback_t callback, void* arg, int flags);

bool is_same_device(const char* name_1, const char* name_2);


/*
 * Error handling
 */

void free_system_error_str(wchar_t* str);
unsigned int get_system_error_code();
wchar_t* get_system_error_str(int code);

/* Specific nodevice errors */
int is_error_nodevice(unsigned int errcode);
void set_error_nodevice();


/*
 * Misc
 */
int fix_usbser_sys(const char* device_name);

void XIMC_API msec_sleep(unsigned int msec);

void get_wallclock(time_t* sec, int* msec);
void get_wallclock_us(uint64_t* us);

/* Converts path to absolute (add leading slash on posix) */
void uri_path_to_absolute(const char* uri_path, char* abs_path, size_t len);

#endif // !INC_PLATFORM_H
