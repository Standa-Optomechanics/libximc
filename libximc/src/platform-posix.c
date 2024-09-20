#include "common.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>
#include <stddef.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_LOCKS
#include <semaphore.h>
#endif

#include "ximc.h"
#include "util.h"
#include "metadata.h"
#include "platform.h"

#include "protosup.h"
#ifdef HAVE_XIWRAPPER
#include "wrapper.h"
#endif

#ifdef STRERROR_R_CHAR_P
#error GNU variant of strerror_r is not supported
#endif

#ifdef __APPLE__
/* We need IOKit */
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>
#include <sys/sysctl.h>
/* and time too */
#include <mach/clock.h>
#include <mach/mach.h>
#endif


/*
 * Serial port support
 */

result_t open_port_serial (device_metadata_t *metadata, const char* name)
{
	int fd;
	struct termios options;

	fd = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd == -1)
	{
		log_system_error( L"unable to open port %s: ", name );
		return result_error;
	}

	/* Consult an advisory lock */

	if (flock( fd, LOCK_EX|LOCK_NB ) == -1 && errno == EWOULDBLOCK)
	{
		close( fd );
		log_error( L"unable to open locked port %s", name );
		return result_error;
	}

	if (flock( fd, LOCK_EX ) == -1)
	{
		log_system_error( L"can't lock file" );
		close( fd );
		log_error( L"unable to lock a port %s", name );
		return result_error;
	}


	/* Adjust settings */

	if (fcntl(fd, F_SETFL, 0) == -1)
	{
		close( fd );
		log_system_error( L"error setting port settings: " );
		return result_error;
	}

	if (tcgetattr(fd, &options) == -1)
	{
		close( fd );
		log_system_error( L"error getting port attrs: " );
		return result_error;
	}

	if (cfsetispeed( &options, B115200 ) == -1 ||
		cfsetospeed( &options, B115200 ) == -1)
	{
		close( fd );
		log_system_error( L"error setting port speed: " );
		return result_error;
	}

	// set port flags
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	options.c_cflag |= (CLOCAL | CREAD);
 	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag &= ~(PARENB | PARODD);
	options.c_cflag |= CSTOPB;
	options.c_cflag &= ~CRTSCTS;

	options.c_iflag &= ~(IXON | IXOFF | IXANY);
	options.c_iflag &= ~(INPCK | PARMRK | ISTRIP | IGNPAR);
	options.c_iflag &= ~(IGNBRK | BRKINT | INLCR | IGNCR | ICRNL | IMAXBEL);

	options.c_oflag &= ~OPOST;

	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = metadata->port_timeout/100;

	if (tcsetattr( fd, TCSAFLUSH, &options ) == -1)
	{
		close( fd );
		log_system_error( L"error setting port attrs: " );
		return result_error;
	}

	tcflush( fd, TCIOFLUSH );

	/* save metadata */
	metadata->handle = fd;
	metadata->type = dtSerial;

	return result_ok;
}

int close_port_serial (device_metadata_t *metadata)
{
	if (close( metadata->handle ) == -1)
	{
		log_system_error( L"error closing port: " );
		return result_serial_error;
	}
	return result_serial_ok;
}

int flush_port_serial (device_metadata_t *metadata)
{
	if (tcflush( metadata->handle, TCIOFLUSH ))
	{
		log_system_error( L"command flush port failed, reason: " );
		return result_serial_error;
	}
	return result_serial_ok;
}

ssize_t read_port_serial (device_metadata_t *metadata, void *buf, size_t amount)
{
	return read( metadata->handle, buf, amount );
}

ssize_t write_port_serial (device_metadata_t *metadata, const void *buf, size_t amount)
{
	return write( metadata->handle, buf, amount );
}


/*
 * Threading support
 */

/* Internal carry struct for thread function */
typedef struct fork_join_carry_t
{
	fork_join_thread_function_t function;
	void* arg;
	pthread_cond_t condition;
	pthread_mutex_t mutex;
} fork_join_carry_t;

/* posix wrapper thread function */
void* check_thread_wrapper_posix (void *arg)
{
	fork_join_carry_t* carry = (fork_join_carry_t*)arg;
	carry->function( carry->arg );
	return NULL;
}

/* posix wrapper thread function with mutex */
void* check_thread_wrapper_posix_with_mutex(void *arg)
{
	fork_join_carry_t* carry = (fork_join_carry_t*)arg;
	carry->function(carry->arg);
	pthread_mutex_lock(&carry->mutex);
	pthread_cond_signal(&carry->condition);
	pthread_mutex_unlock(&carry->mutex);
	return NULL;
}

/* posix implementation of fork/join */
result_t fork_join (fork_join_thread_function_t function, int count, void* args, size_t arg_element_size)
{
	result_t result = result_ok;
	pthread_t* tids;
	pthread_attr_t thread_attr;
	int i;
	fork_join_carry_t* carry;

	pthread_attr_init( &thread_attr );
	pthread_attr_setdetachstate( &thread_attr, PTHREAD_CREATE_JOINABLE );

	tids = (pthread_t*)malloc( count*sizeof(pthread_t) );
	carry = (fork_join_carry_t*)malloc( count*sizeof(fork_join_carry_t) );

	/* launch and join */
	for (i = 0; i < count; ++i)
	{
		carry[i].function = function;
		carry[i].arg = (byte*)args + i*arg_element_size;
		if (pthread_create( &tids[i], &thread_attr, &check_thread_wrapper_posix, &carry[i] ))
		{
			result = result_error;
			log_system_error( L"Failed to create a pthread due to: " );
		}
	}
	for (i = 0; i < count; ++i)
	{
		if (pthread_join( tids[i], NULL ))
		{
			result = result_error;
			log_system_error( L"Failed to join a pthread due to: " );
		}
	}

	free( tids );
	free( carry );

	return result;
}

void single_thread_launcher(XIMC_RETTYPE(XIMC_CALLCONV *func)(void*), void *arg)
{
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	pthread_t tid;

	if (pthread_create(&tid , &thread_attr, func, arg) != 0) {
		log_system_error(L"Failed to create a pthread due to: ");
	}
}

/* posix implementation of fork/join with timeout */
// TODO: fix net_enum abstraction leak
void fork_join_with_timeout(fork_join_thread_function_t function, int count, void* args, size_t arg_element_size, int timeout_ms, net_enum_t* net_enum)
{
	pthread_t* tids;
	pthread_attr_t thread_attr;
	int i;
	fork_join_carry_t* carry;

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

	tids = (pthread_t*)malloc(count*sizeof(pthread_t));
	carry = (fork_join_carry_t*)malloc(count*sizeof(fork_join_carry_t));

	/* launch and join */
	for (i = 0; i < count; ++i)
	{
		pthread_cond_init(&carry[i].condition, NULL);
		pthread_mutex_init(&carry[i].mutex, NULL);
		carry[i].function = function;
		carry[i].arg = (byte*)args + i*arg_element_size;
		if (pthread_create(&tids[i], &thread_attr, &check_thread_wrapper_posix_with_mutex, &carry[i]))
		{
			log_system_error(L"Failed to create a pthread due to: ");
		}
	}

	/* Wait for timeout and get data from those who returned */
	struct timespec abstime;
	#ifdef __APPLE__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	abstime.tv_sec = mts.tv_sec;
	abstime.tv_nsec = mts.tv_nsec;
	#else
	clock_gettime(CLOCK_REALTIME, &abstime);
	#endif

	time_t sec = (time_t)(timeout_ms / 1000);
	long nsec = (timeout_ms - 1000 * sec) * 1000 * 1000;
	abstime.tv_sec += sec;
	abstime.tv_nsec += nsec;
	long billion = 1000 * 1000 * 1000;
	if (abstime.tv_nsec > billion) {
		abstime.tv_sec += 1;
		abstime.tv_nsec -= billion;
	}

	for (i = 0; i < count; ++i)
	{
		pthread_mutex_lock(&carry[i].mutex);
		int result;
		do {
			result = pthread_cond_timedwait(&carry[i].condition, &carry[i].mutex, &abstime);
		} while (result != ETIMEDOUT && result != 0);
		pthread_mutex_unlock(&carry[i].mutex);
	}

	// Lock each personal mutex and copy thread data
	for (i = 0; i < count; ++i)
	{
		pthread_mutex_lock(&carry[i].mutex);
		netthread_state_t state = *(netthread_state_t*)(carry[i].arg);
		if (state.status) {
			net_enum->pbufs[i] = state.pbuf;
			net_enum->device_count[i] = state.devices_found;
		}
		pthread_mutex_unlock(&carry[i].mutex);
	}

	// Unlock the external mutex
	mutex_unlock(net_enum->mutex);
	// After this we can't touch our parameter pointers (net_enum), since they might be already invalid

	/* Wait for infinity to clean up after remaining threads */
	for (i = 0; i < count; ++i)
	{
		if (pthread_join(tids[i], NULL))
		{
			log_system_error(L"Failed to join a pthread due to: ");
		}
	}

	for (i = 0; i < count; ++i) {
		pthread_cond_destroy(&carry[i].condition);
		pthread_mutex_destroy(&carry[i].mutex);
	}
	
	free(tids);
	free(carry);
}

void fork_join_2_threads(fork_join_thread_function_t function1, void* args1, int condition1, fork_join_thread_function_t function2, void* args2, int condition2)
{
    
    pthread_t tids[2];
    pthread_attr_t thread_attr;
    int i, count_launched;
    fork_join_carry_t* carry;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    count_launched = 0;
    carry = (fork_join_carry_t*)malloc(2*sizeof(fork_join_carry_t));
    if (condition1)
    {
        carry[0].function = function1;
        carry[0].arg = (byte*)args1;
        if (pthread_create(&tids[0], &thread_attr, &check_thread_wrapper_posix, &carry[0]))
        {
            log_system_error(L"Failed to create a pthread due to: ");
        }
        else
            count_launched++;
    }
    if (condition2)
    {
        carry[count_launched].function = function2;
        carry[count_launched].arg = (byte*)args2;
        if (pthread_create(&tids[count_launched], &thread_attr, &check_thread_wrapper_posix, &carry[count_launched]))
        {
            log_system_error(L"Failed to create a pthread due to: ");
        }
        else
            count_launched++;

    }

    for (i = 0; i < count_launched; ++i)
    {
        if (pthread_join(tids[i], NULL))
        {
            log_system_error(L"Failed to join a pthread due to: ");
        }
    }
    free(carry);
}


unsigned long long get_thread_id()
{
	return (unsigned long long)(uintptr_t)pthread_self();
}

/*
 * Device enumeration support
 */

int like_com_device_by_prefix (const char* prefix, const char* name)
{
	return strlen( name ) > strlen( prefix ) && !memcmp( prefix, name, strlen( prefix ) );
}

#ifdef __APPLE__
// OS X
// Sometimes name can be 'tty.usbmodem*' or 'tty.usbserial-*'
// but we prefer to use generic mask 'tty.*'
// path must be slash-terminated
bool is_device_name_ok (char* directory, char* name, int flags)
{
	XIMC_UNUSED(directory);
	XIMC_UNUSED(flags);
	return
		!strcmp( directory, "/dev" ) &&
			like_com_device_by_prefix( "tty.", name )
		;
}
#else
// generic unix
bool is_device_name_ok (char* directory, char* name, int flags)
{
	return
		(!strcmp( directory, "/dev" ) && (flags & ENUMERATE_ALL_COM) && (
				like_com_device_by_prefix( "ttyUSB", name ) ||
				like_com_device_by_prefix( "ttyACM", name )))
		||
        ((!strcmp(directory, "/dev/ximc") || !strcmp(directory, "/dev/mdrive")) &&
		 		is_hex( name ));
}
#endif

#ifdef __APPLE__

bool is_osx_elcapitan_or_later()
{
	char str[256];
	size_t size = sizeof(str);
	int major;
	int ret = sysctlbyname("kern.osrelease", str, &size, NULL, 0);
	if (ret == 0)
	{
		if (sscanf(str, "%d.", &major) == 1)
		{
			return major >= 15;
		}
	}
	return false;
}

result_t enumerate_iokit_vid_pid(enumerate_devices_directory_callback_t callback, void* arg, int flags, long usbVendor, long usbProduct)
{
    CFMutableDictionaryRef matchingDict;
    CFNumberRef numberRef;
    kern_return_t kr;
    io_iterator_t ioIterator;
    io_service_t usbDevice;
    CFStringRef refPath;
    char cpath[PATH_MAX];
    const char *usbServiceStr = is_osx_elcapitan_or_later() ? "IOUSBHostDevice" : kIOUSBDeviceClassName;

    XIMC_UNUSED(flags);

    // IOUSBDevice and its subclasses
    log_debug(L"Searching IOKit with service %s", usbServiceStr);
    matchingDict = IOServiceMatching(usbServiceStr);
    if (matchingDict == NULL)
    {
        log_debug(L"IOKit: Failed IOServiceMatching call");
        return result_error;
    }

    // Create a CFNumber for the idVendor, idProduct and set the value in the dictionary
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbVendor);
    CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorID), numberRef);
    CFRelease(numberRef);

    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbProduct);
    CFDictionarySetValue(matchingDict, CFSTR(kUSBProductID), numberRef);
    CFRelease(numberRef);

    if ((kr = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, &ioIterator)) == KERN_SUCCESS)
    {
        log_debug(L"IOKit: IOServiceGetMatchingServices succeeded");
        while ((usbDevice = IOIteratorNext(ioIterator)) != 0)
        {
            log_debug(L"IOKit: iterator succeeded");
            if ((refPath = IORegistryEntrySearchCFProperty(usbDevice, kIOServicePlane,
                CFSTR("IODialinDevice"), kCFAllocatorDefault, kIORegistryIterateRecursively)) != NULL)
            {
                CFStringGetCString(refPath, cpath, sizeof(cpath), kCFStringEncodingASCII);
                cpath[sizeof(cpath)-1] = 0;
                log_debug(L"Look to device %hs", cpath);
                /* device name is okay by default */
                callback(cpath, arg);
                CFRelease(refPath);
            }
            IOObjectRelease(usbDevice);
        }
    }
    return result_ok;
}

/* directory must not end with slash */
result_t enumerate_iokit(enumerate_devices_directory_callback_t callback, void* arg, int flags)
{
    const long usbVendor = 0x1CBE;
    const long usbProduct = 0x0007;

    result_t result;
    if ((result = enumerate_iokit_vid_pid(callback, arg, flags, usbVendor, usbProduct)) != result_ok)
        return result;

    const long usbVendor_mdrive = 0x1CBC;
    const long usbProduct_mdrive = 0x0031;

    return enumerate_iokit_vid_pid(callback, arg, flags, usbVendor_mdrive, usbProduct_mdrive);
}


#endif

bool is_same_device (const char* name1, const char* name2)
{
	char realname1[PATH_MAX], realname2[PATH_MAX];
	if (!realpath( name1, realname1 ))
	{
		log_system_error( L"Cannot resolve real path of device %hs", name1 );
		return false;
	}
	if (!realpath( name2, realname2 ))
	{
		log_system_error( L"Cannot resolve real path of device %hs", name2 );
		return false;
	}
	return !strcmp( realname1, realname2 );
}

/* directory must not end with slash */
result_t enumerate_specific_directory (char* directory, enumerate_devices_directory_callback_t callback, void* arg, int flags)
{
	result_t result = result_ok;
	DIR* dev_dir;
	struct dirent* de;
	struct dirent* de_result;
	size_t de_size = 0;
	struct stat stat_buf;
	char full_path[PATH_MAX];

	/* do not fail if directory does not exist */
	if (stat( directory, &stat_buf ) == -1 && (errno == ENOENT || errno == ENOTDIR))
		return result_ok;

	if (!directory[0] || directory[strlen( directory )-1] == '/')
		return result_error;

	log_debug( L"Enumerating specific directory %hs", directory );

	/* Open a dir */
	dev_dir = opendir( directory );

	if (!dev_dir)
	{
		log_system_error( L"Can't open %hs dir due to: ", directory );
		return result_error;
	}

	de_size = offsetof(struct dirent, d_name) + pathconf(directory, _PC_NAME_MAX) + 1;
    de = (struct dirent*)malloc( de_size );

	for (;;)
	{
		if (readdir_r( dev_dir, de, &de_result) != 0)
		{
			log_system_error( L"Can't readdir %hs dir due to: ", directory );
			break;
		}

		/* No more entries */
		if (de_result == NULL)
			break;

		portable_snprintf( full_path, sizeof(full_path), "%s/%s", directory, de->d_name );
		full_path[sizeof(full_path)-1] = 0;

		/* Get statistic of file (resolved path for symlink) */
		if (stat( full_path, &stat_buf ) == -1)
		{
			if (errno == ENOENT || errno == ENOTDIR)
			{
				log_warning( L"File does not exist: %hs", full_path );
				continue;
			}
			else
			{
				log_system_error( L"Error accessing file: %hs", full_path );
				return result_error;
			}
		}
		/* Skip nondevice file */
		if (!S_ISCHR(stat_buf.st_mode))
			continue;

		log_debug( L"Look to device %hs", de->d_name );
		// check only name of the device
		if (is_device_name_ok( directory, de->d_name, flags ))
		{
			callback( full_path, arg );
		}
		else
			log_debug( L"Skip port %hs/%hs", directory, de->d_name );
	}

	free( de );
	closedir( dev_dir );

	return result;
}

result_t enumerate_devices_directory (enumerate_devices_directory_callback_t callback, void* arg, int flags)
{
	result_t result;
    
	#ifdef __APPLE__
	if (!(flags & ENUMERATE_ALL_COM))
	{
		/* use IOKit if only specific USB devices are needed */
		return enumerate_iokit( callback, arg, flags );
	}
	#endif

	/* enumerate /dev/ximc/ or /dev/mdrive first */
    /* if directory does not exist it is no error */

    if ((result = enumerate_specific_directory("/dev/ximc", callback, arg, flags)) != result_ok)
        return result;

    if ((result = enumerate_specific_directory("/dev/mdrive", callback, arg, flags)) != result_ok)
		return result;

 	/* enumerate all other devices in /dev/ because there are symlinks to them */
	if ((result = enumerate_specific_directory( "/dev", callback, arg, flags )) != result_ok)
		return result;

	return result_ok;
}

/*
 * Error handling
 */

int is_error_nodevice(unsigned int errcode)
{
	return errcode == ENXIO || errcode == EIO;
}

void set_error_nodevice ()
{
	errno = ENXIO;
}

unsigned int get_system_error_code ()
{
	return errno;
}

char* strerror_r_ensure (int code)
{
    size_t buflen = 256;
    char* buf = (char*)malloc( buflen );

#ifdef STRERROR_R_CHAR_P
    char* ret;
    ret = strerror_r( code, buf, buflen );
    if (ret != buf)
        free( buf );
    return ret;
#else
    int ret;
    do
    {
        ret = strerror_r( code, buf, buflen );
        if (ret == ERANGE)
        {
            buflen = (size_t)(buflen * 1.4);
            free( buf );
            buf = (char*)malloc( buflen );
        }
    } while (!(ret == 0 || ret == EINVAL));

    return buf;
#endif
}

wchar_t* get_system_error_str (int code)
{
	char* str;
	wchar_t* result;
	str = strerror_r_ensure( code );
	if (!str)
		str = portable_strdup( "strerror_r failed, unknown error" );
	result = str_to_widestr( str );
	free( str );
	return result;
}

void free_system_error_str (wchar_t* str)
{
	if (str)
		free( str );
}

/*
 * Misc
 */

/* Dummy, windows-only */
int fix_usbser_sys(const char* device_name)
{
	XIMC_UNUSED(device_name);
	return 0;
}

void XIMC_API msec_sleep(unsigned int msec)
{
	// POSIX 1.b
	struct timespec ts;
	ts.tv_sec = (time_t)(msec / 1E3);
	ts.tv_nsec = (long)(msec*1E6 - ts.tv_sec*1E9);
	if (nanosleep( &ts, NULL ) != 0)
		log_system_error( L"nanosleep failed" );
}

void get_wallclock_us(uint64_t* us)
{
	struct timeval now;
	gettimeofday(&now, 0);
	if (us)
	{
		*us = now.tv_sec * 1000000 + now.tv_usec;
	}
}

void get_wallclock(time_t* sec, int* msec)
{
	struct timeval now;
	gettimeofday( &now, 0 );
	if (sec && msec)
	{
		*sec = now.tv_sec;
		*msec = now.tv_usec / 1000;
	}
}

void uri_path_to_absolute(const char *uri_path, char *abs_path, size_t len)
{
	*abs_path = 0;
	// Add leading '/' to POSIX path if not presented. #85091#note-28
	if (uri_path[0] != '/')
		strncat(abs_path, "/", len);
	strncat(abs_path, uri_path, len);
	abs_path[len-1] = 0;
}

/* Returns non-zero on success */
int set_default_bindy_key()
{
#ifdef HAVE_XIWRAPPER
	char path[PATH_MAX] = { 0 };
#if defined(__APPLE__) && !defined(HAVE_CONFIG_H)
	/* get keyfile from resource */
	CFStringRef keyfilePath;
	CFURLRef keyfileURL;
	CFBundleRef bundleRef = CFBundleGetBundleWithIdentifier(CFSTR("ru.ximc.libximc"));
	if (bundleRef)
	{
		keyfileURL = CFBundleCopyResourceURL(bundleRef, CFSTR("keyfile.sqlite"), NULL, NULL);
		if (keyfileURL)
		{
			keyfilePath = CFURLCopyFileSystemPath(keyfileURL, kCFURLPOSIXPathStyle);
			if (keyfilePath)
			{
				if (!CFStringGetCString(keyfilePath, path, sizeof(path), kCFStringEncodingASCII))
				{
					CFRelease(keyfileURL);
					return 0;
				}
			}
			else
			{
				CFRelease(keyfileURL);
				return 0;
			}
			CFRelease(keyfileURL);
		}
		else
			return 0;
	}
	else
		return 0;
#else
	#ifndef DATADIR
	#error Please check DATADIR
	#endif
	strcpy(path, DATADIR "/libximc/keyfile.sqlite");
#endif
	log_debug( L"Setting standard resource key %hs", path);
	return strlen(path) && bindy_setkey(path);
#else
	return 0;
#endif
}

/*
 * Lock support
 */

#ifdef HAVE_LOCKS

struct mutex_t
{
	sem_t* impl;
};

mutex_t* mutex_init(unsigned int nonce)
{
	char name[_POSIX_PATH_MAX];
	int counter = 0;
	mutex_t* mutex = malloc( sizeof(mutex_t) );
	if (!mutex)
	{
		log_system_error( L"can't create semaphore" );
		return NULL;
	}
	/* thanks APUE for this idea */
	do
	{
		if (counter)
			log_error( L"cannot use semaphore %s, increasing suffix", name );
  		portable_snprintf( name, sizeof(name), "/sem-ximc-%ld.%x.%d", (long)getpid(), nonce, counter++ );
		mutex->impl = sem_open( name, O_CREAT|O_EXCL, 0777, 1 );
	}
	while (mutex->impl == SEM_FAILED && errno == EEXIST);
	if (mutex->impl == SEM_FAILED)
	{
		free( mutex );
		log_system_error( L"can't create semaphore" );
		return NULL;
	}
	/* unlink early */
	if (sem_unlink( name ))
	{
		free( mutex );
		log_system_error( L"can't unlink semaphore" );
		return NULL;
	}
	return mutex;
}

void mutex_close(mutex_t* mutex)
{
	if (mutex)
	{
		if (mutex->impl != SEM_FAILED)
		{
			if (sem_close( mutex->impl ) != 0)
				log_system_error( L"can't close semaphore due to " );
		}
		free( mutex );
	}
}

void mutex_lock(mutex_t* mutex)
{
	if (!mutex || mutex->impl == SEM_FAILED)
	{
		log_error( L"no semaphore specified" );
		return;
	}
	if (sem_wait( mutex->impl ) == -1)
		log_system_error( L"can't wait on semaphore %p due to ", mutex->impl );
}

void mutex_unlock(mutex_t* mutex)
{
	if (!mutex || mutex->impl == SEM_FAILED)
	{
		log_error( L"no semaphore specified" );
		return;
	}
	if (sem_post( mutex->impl ) == -1)
		log_system_error( L"can't post on semaphore %p due to ", mutex->impl );
}

#endif

// vim: syntax=c tabstop=4 shiftwidth=4
