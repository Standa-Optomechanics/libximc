#ifndef INC_PLATFORM_H
#define INC_PLATFORM_H

/*
 * Serial port support
 */

// return code on success
#define result_serial_ok 0

// return code on generic error
#define result_serial_error -1

// return code on device lost
#define result_serial_nodevice -2

// return code on timeout
#define result_serial_timeout -3

/*
 * Platform-specific serial port routines
 */
result_t open_port_serial (device_metadata_t *metadata, const char* name);

int close_port_serial (device_metadata_t *metadata);
int flush_port_serial (device_metadata_t *metadata);
ssize_t read_port_serial (device_metadata_t *metadata, void *buf, size_t amount);
ssize_t write_port_serial (device_metadata_t *metadata, const void *buf, size_t amount);

/*
* Default ports values for tcp-, udp-
*/

#define XIMC_UDP_PORT 1818
#define XIMC_TCP_PORT 1820


/*
* Platform-specific udp routines
*/
/**
* Opens UDP-socket as a client
* Assumes host contains as follows: just an IP-address:port
*/
result_t open_udp(device_metadata_t *metadata, const char* ip4_port);

/**
Closes previuosly opened UDP socket and removes all metadata associated
*/
result_t close_udp(device_metadata_t *metadata);

/**
* Writes to UDP socket

*/
ssize_t write_udp(device_metadata_t *metadata, const byte* command, size_t command_len);

/**
* Reads from  UDP socket

*/

ssize_t read_udp(device_metadata_t *metadata, void *buf, size_t amount);


/*
* Platform-specific tcp routines
*/
/**
* Opens TCP-socket as a client
* Assumes host contains as follows: just an IP-address:port
*/
result_t open_tcp(device_metadata_t *metadata, const char* ip4_port);

/**
Closes previuosly opened TCP socket and removes all metadata associated
*/
result_t close_tcp(device_metadata_t *metadata);

/**
* Writes to TCP socket

*/
ssize_t write_tcp(device_metadata_t *metadata, const byte* command, size_t command_len);

/**
* Reads from  TCP socket

*/
ssize_t read_tcp(device_metadata_t *metadata, void *buf, size_t amount);


/*
 * Threading support
 */

/* Callback for user actions */
typedef void (*fork_join_thread_function_t)(void *arg);

/* Platform-specific fork/join function */
result_t fork_join (fork_join_thread_function_t function, int count, void* args, size_t arg_element_size);

void single_thread_wrapper_function(void *arg);

unsigned long long get_thread_id();

/*
 * Device enumeration support
 */

/* Callback for user actions */
typedef void (*enumerate_devices_directory_callback_t) (char* name, void* arg);

/* Platform-specific enumerator */
result_t enumerate_devices_directory (enumerate_devices_directory_callback_t callback, void* arg, int flags);

bool is_same_device (const char* name1, const char* name2);

/*
 * Error handling
 */
unsigned int get_system_error_code ();

wchar_t* get_system_error_str (int code);

void free_system_error_str (wchar_t* str);

/* specific nodevice errors */
int is_error_nodevice(unsigned int errcode);
void set_error_nodevice ();

/*
 * Misc
 */
int fix_usbser_sys(const char* device_name);

void XIMC_API msec_sleep(unsigned int msec);

void get_wallclock(time_t* sec, int* msec);
void get_wallclock_us(uint64_t* us);

/* Converts path to absolute (add leading slash on posix) */
void uri_path_to_absolute(const char *uri_path, char *abs_path, size_t len);

/*
 * Mutex
 */
typedef struct mutex_t mutex_t;

mutex_t* mutex_init(unsigned int nonce);
void mutex_close(mutex_t* mutex);
void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);

typedef struct net_enum_t
{
	mutex_t * mutex;	// mutex
	int server_count;	// server count
	int* device_count;	// array of number of found devices on each server
	char** addrs;		// array of strings with address of each server
	uint8_t*** pbufs;	// array of pointers to buffer for each server
	char* adapter_addr; // network adapter address (optional)
} net_enum_t;

typedef struct netthread_state_t
{
	const char* addr;
	const char* adapter_addr;
	uint8_t** pbuf;
	int status;
	unsigned int devices_found;
} netthread_state_t;

/* Platform-specific thread launcher */
void single_thread_launcher(XIMC_RETTYPE(XIMC_CALLCONV *func)(void*), void *arg);

/* Platform-specific fork/join function with timeout*/
void fork_join_with_timeout(fork_join_thread_function_t function, int count, void* args, size_t arg_element_size, int timeout_ms, net_enum_t *net_enum);

/* Platform-specific fork/join function for 2 fucntions launched on a specified condition*/
void fork_join_2_threads(fork_join_thread_function_t function1, void* args1, int condition1, fork_join_thread_function_t function2, void* args2, int condition2);

#endif
