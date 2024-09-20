#ifndef INC_UTIL_H
#define INC_UTIL_H

// time to wait to open port for normal work
#define DEFAULT_TIMEOUT_TIME 5000

// time to wait to open port when enumerating devices
// TODO temporarily set to default time
#define ENUMERATE_TIMEOUT_TIME 5000

// system timeout for port functions
#define PORT_TIMEOUT_TIME 500

// time to wait before retry attempts
#define WAIT_BEFORE_RETRY_TIME 200

// time for controller to reboot and enter the loader, in msec
#define RESET_TIME 100

// xism board port redetection time
#define XISM_PORT_DETECT_TIME 60000

// amount of zeroes to send in case of an error
#define SYNC_ZERO_COUNT 64

// syncronization attempts count
#define SYNC_RETRY_COUNT 4

// sleep time in milliseconds in wait loops
#define SLEEP_WAIT_TIME 1

// timeout after close, maybe should fix kernel race error, in msec
#define ENUMERATE_CLOSE_TIMEOUT 100

// trace like hell
//#define DEBUG_TRACE

uint16_t get_crc(const byte* pbuf, size_t n);

void log_generic (int loglevel, const wchar_t* format, ...);
void log_debug (const wchar_t* format, ...);
void log_info (const wchar_t* format, ...);
void log_warning (const wchar_t* format, ...);
void log_error (const wchar_t* format, ...);
void log_system_error (const wchar_t* format, ...);

size_t ximc_min (size_t a, size_t b);
size_t ximc_max (size_t a, size_t b);

int powi(int x, int y);

void dump_bytes (const byte* data, size_t amount);

int is_numeric (const char* s);
int is_hex (const char* s);

wchar_t* str_to_widestr (const char* str);
char* widestr_to_str (const wchar_t* str);

void wcsrstrip (wchar_t* str, wchar_t* delim);

int parse_uri(const char *name,
		char *scheme, size_t scheme_len,
		char *host, size_t host_len,
		char *path, size_t path_len,
		char *paramname, size_t paramname_len,
		char *paramvalue, size_t paramvalue_len);

char *uri_encode(const char *str);
char *uri_decode(const char *str);
char *uri_copy(const char *str);

#endif

