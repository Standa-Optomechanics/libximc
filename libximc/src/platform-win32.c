#include "common.h"

#include <initguid.h>
// do not force to use DDK with MSVC or other
#ifdef _MSC_VER
#include <winioctl.h>
#else
#include <ddk/ntddser.h>
#endif
#include <setupapi.h>
#include <process.h>

#include "ximc.h"
#include "util.h"
#include "metadata.h"
#include "platform.h"

#include "protosup.h"
#include "wrapper.h"

/*
 * Serial port and pipe support
 */

result_t open_port_serial(device_metadata_t *metadata, const char* name)
{
	HANDLE hDevice;
	DCB dcb;
	COMMTIMEOUTS ctm;

	hDevice = CreateFileA( name,
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				0,
				0);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		log_system_error( L"can't open device %hs due to: ", name );
		return result_error;
	}

	if (!GetCommState( hDevice, &dcb ))
	{
		if (!CloseHandle( hDevice ))
			log_system_error( L"error closing port: " );
		log_system_error( L"can't get comm state due to: " );
		return result_error;
	}

	dcb.BaudRate = CBR_115200;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	//dcb.fTXContinueOnXoff;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = TRUE;
	dcb.ByteSize = 8;
	dcb.StopBits = 2;

	if (!SetCommState( hDevice, &dcb ))
	{
		if (!CloseHandle( hDevice ))
			log_system_error( L"error closing port: " );
		log_system_error( L"can't get comm state due to: " );
		return result_error;
	}

	ctm.ReadIntervalTimeout = MAXDWORD;
	ctm.ReadTotalTimeoutConstant = metadata->port_timeout;
	ctm.ReadTotalTimeoutMultiplier = MAXDWORD;
	ctm.WriteTotalTimeoutConstant = 0;
	ctm.WriteTotalTimeoutMultiplier = metadata->port_timeout;

	if (!SetCommTimeouts( hDevice, &ctm ))
	{
		if (!CloseHandle( hDevice ))
			log_system_error( L"error closing port: " );
		log_system_error( L"can't get comm state due to: " );
		return result_error;
	}

	/* save metadata */
	metadata->handle = hDevice;
	metadata->type = dtSerial;

	return result_ok;
}

int close_port_serial (device_metadata_t *metadata)
{
	if (CloseHandle( metadata->handle ) == -1)
	{
		log_system_error( L"error closing port: " );
		return result_serial_error;
	}
	return result_serial_ok;
}

int flush_port_serial (device_metadata_t *metadata)
{
	if (!PurgeComm( metadata->handle, PURGE_RXCLEAR|PURGE_TXCLEAR ))
		return result_serial_error;

	return result_serial_ok;
}

ssize_t read_port_serial (device_metadata_t *metadata, void *buf, size_t amount)
{
	DWORD read_bytes;
	if (TRUE == ReadFile( metadata->handle, buf, (DWORD)amount, &read_bytes, NULL ))
		return (ssize_t)read_bytes;
	else
		return 0;
}

ssize_t write_port_serial (device_metadata_t *metadata, const void *buf, size_t amount)
{
	DWORD writ_bytes;
	if (TRUE == WriteFile( metadata->handle, buf, (DWORD)amount, &writ_bytes, NULL ))
		return (ssize_t)writ_bytes;
	else
		return 0;
}



/*
 * Threading support
 */

/* Internal carry struct for thread function */
typedef struct fork_join_carry_t
{
	fork_join_thread_function_t function;
	void* arg;
} fork_join_carry_t;

/* win32 wrapper thread function */
unsigned int __stdcall check_thread_wrapper_win32 (void *arg)
{
	fork_join_carry_t* carry = (fork_join_carry_t*)arg;
	carry->function( carry->arg );
	return 0;
}

/* win32 implementation of fork/join */
result_t fork_join (fork_join_thread_function_t function, int count, void* args, size_t arg_element_size)
{
	result_t result = result_ok;
	HANDLE* tids;
	int i;
	fork_join_carry_t* carry;

	tids = (HANDLE*)malloc( count*sizeof(HANDLE) );
	carry = (fork_join_carry_t*)malloc( count*sizeof(fork_join_carry_t) );

	/* launch and join */
	for (i = 0; i < count; ++i)
	{
		carry[i].function = function;
		carry[i].arg = (byte*)args + i*arg_element_size;
		tids[i] = (HANDLE)_beginthreadex( NULL, 0, check_thread_wrapper_win32, &carry[i], 0, NULL );
		if (tids[i] == 0)
		{
			result = result_error;
			log_system_error( L"Failed to create a crt thread due to: " );
		}
	}
	/* Join threads or say there are no devices at all */
	switch (WaitForMultipleObjects( count, tids, TRUE, INFINITE ))
	{
		case WAIT_OBJECT_0:
			break;
		case WAIT_FAILED:
			log_system_error( L"Failed to wait for threads termination due to: " );
			result = result_error;
			break;
		default:
			log_error( L"Failed to wait for threads termination due to strange reason" );
			result = result_error;
	}
	for (i = 0; i < count; ++i)
		CloseHandle( tids[i] );

	free( tids );
	free( carry );

	return result;
}

void single_thread_launcher(XIMC_RETTYPE(XIMC_CALLCONV *func)(void*), void *arg)
{
	if (_beginthreadex(NULL, 0, func, arg, 0, NULL) == 0)
		log_system_error(L"Failed to create a crt thread due to: ");
}

/* win32 implementation of fork/join with timeout */
void fork_join_with_timeout(fork_join_thread_function_t function, int count, void* args, size_t arg_element_size, int timeout_ms, net_enum_t* net_enum)
{
	HANDLE* tids;
	int i;
	fork_join_carry_t* carry;
	mutex_t **mutexes;

	tids = (HANDLE*)malloc(count*sizeof(HANDLE));
	carry = (fork_join_carry_t*)malloc(count*sizeof(fork_join_carry_t));
	mutexes = (mutex_t**)malloc(count*sizeof(mutex_t*));

	/* launch and join */
	for (i = 0; i < count; ++i)
	{
		carry[i].function = function;
		carry[i].arg = (byte*)args + i*arg_element_size;
		mutexes[i] = mutex_init(i); // the nonce is unused on windows only
		tids[i] = (HANDLE)_beginthreadex(NULL, 0, check_thread_wrapper_win32, &carry[i], 0, NULL);
		if (tids[i] == 0)
		{
			log_system_error(L"Failed to create a crt thread due to: ");
		}
	}
	/* Wait for timeout and get data from those who returned */
	switch (WaitForMultipleObjects(count, tids, TRUE, timeout_ms))
	{
	case WAIT_TIMEOUT:
		log_info(L"Timed out waiting for all network enumeration threads to complete");
		break;
	case WAIT_OBJECT_0:
		break;
	case WAIT_FAILED:
		log_system_error(L"Failed to wait for threads termination due to: ");
		break;
	default:
		log_error(L"Failed to wait for threads termination due to strange reason");
	}

	// Lock each personal mutex and copy thread data
	for (i = 0; i < count; ++i)
	{
		mutex_lock(mutexes[i]);
		netthread_state_t state = *(netthread_state_t*)(carry[i].arg);
		if (state.status) {
			net_enum->pbufs[i] = state.pbuf;
			net_enum->device_count[i] = state.devices_found;
		}
		mutex_unlock(mutexes[i]);
	}

	// Unlock the external mutex
	mutex_unlock(net_enum->mutex);
	// After this we can't touch our parameter pointers (net_enum), since they might be already invalid

	/* Wait for infinity to clean up after remaining threads */
	switch (WaitForMultipleObjects(count, tids, TRUE, INFINITE))
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_FAILED:
		log_system_error(L"Failed to wait for threads termination due to: ");
		break;
	default:
		log_error(L"Failed to wait for threads termination due to strange reason");
	}

	for (i = 0; i < count; ++i) {
		CloseHandle(tids[i]);
		mutex_close(mutexes[i]);
	}

	free(tids);
	free(carry);
	free(mutexes);
}

void fork_join_2_threads(fork_join_thread_function_t function1, void* args1, int condition1, fork_join_thread_function_t function2, void* args2, int condition2)
{
    HANDLE handles[2];
    int i, count_launched;
    fork_join_carry_t* carry;
    
    count_launched = 0;
    carry = (fork_join_carry_t*)malloc(2*sizeof(fork_join_carry_t));
    if (condition1)
    {
        carry[0].function = function1;
        carry[0].arg = (byte*)args1;
        handles[0] = (HANDLE)_beginthreadex(NULL, 0, check_thread_wrapper_win32, &carry[0], 0, NULL);
        if (handles[0] == 0)
        {
            log_system_error(L"Failed to create a luanch_sheck_threads thread due to: ");
        }
        else
            count_launched++;

    }
    if (condition2)
    {
        carry[count_launched].function = function2;
        carry[count_launched].arg = (byte*)args2;
        handles[count_launched] = (HANDLE)_beginthreadex(NULL, 0, check_thread_wrapper_win32, &carry[count_launched], 0, NULL);
        if (handles[count_launched] == 0)
        {
             log_system_error(L"Failed to create a single_wrapper thread due to: ");
        }
        else
            count_launched++;
    }

    /* Join threads or say there are no devices at all */
    if (count_launched)
    {

        switch (WaitForMultipleObjects(count_launched, handles, TRUE, INFINITE))
        {
        case WAIT_OBJECT_0:
            break;
        case WAIT_FAILED:
            log_system_error(L"Failed to wait for threads termination due to: ");
            break;
        default:
            log_error(L"Failed to wait for threads termination due to strange reason");
        }
    }

    for (i = 0; i < count_launched; ++i)
        CloseHandle(handles[i]);
    free(carry);
}


unsigned long long get_thread_id()
{
	return (unsigned long long)GetCurrentThreadId();
}

/*
 * Device enumeration support
 */

bool is_device_name_COM(const char* name)
{
	const char *prefix;
	// COM3
	prefix = "COM";
	if (!portable_strncasecmp( name, prefix, strlen( prefix ) ) &&
			is_numeric( name + strlen(prefix) ))
		return true;
	// \\.\COM10
	prefix = "\\\\.\\COM";
	if (!portable_strncasecmp( name, prefix, strlen( prefix ) ) &&
			is_numeric( name + strlen(prefix) ))
		return true;
	return false;
}

bool is_device_name_ok (char* name, char* description, int flags)
{
	const char* ximcDeviceSignature = "XIMC Motor Controller";
    const char*mdriveDeviceSignature = "mDrive Motor Controller";
	if (is_device_name_COM( name ))
	{
		return (flags & ENUMERATE_ALL_COM)
			? true
            : (strstr(description, ximcDeviceSignature) != NULL || strstr(description, mdriveDeviceSignature) != NULL);
	}
	else
		return false;
}

bool is_same_device (const char* name1, const char* name2)
{
	return !strcmp( name1, name2 );
}

result_t enumerate_devices_directory (enumerate_devices_directory_callback_t callback, void* arg, int flags)
{
	int nIndex;
	SP_DEVINFO_DATA devInfo;
	char pszValue[256];
	char friendlyName[512];
	char fullName[256];
	DWORD dwSize;
	DWORD dwType;
	GUID guid = GUID_DEVINTERFACE_COMPORT;
	HKEY hDeviceKey;
	HDEVINFO hDevInfoSet;

	/* Open a dir */
	hDevInfoSet = SetupDiGetClassDevs( &guid, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES );

	if (hDevInfoSet == INVALID_HANDLE_VALUE)
	{
		log_system_error( L"Can't open class due to: " );
		return result_error;
	}

	for (nIndex = 0;; ++nIndex)
	{
		//Enumerate the current device
		devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
		if (!SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo))
			break;

		//Get the registry key which stores the ports settings
		hDeviceKey = SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
		if (hDeviceKey != INVALID_HANDLE_VALUE)
		{
			dwSize = sizeof(pszValue);
			dwType = 0;
			if (RegQueryValueExA(hDeviceKey, "PortName", NULL, &dwType, (LPBYTE)pszValue, &dwSize) == ERROR_SUCCESS &&
				dwType == REG_SZ)
			{
				if (!SetupDiGetDeviceRegistryPropertyA(hDevInfoSet, &devInfo, SPDRP_FRIENDLYNAME, NULL,
							(BYTE*)friendlyName, sizeof(friendlyName), NULL))
					log_warning( L"Cannot get friendly name for port %hs", pszValue );
				friendlyName[sizeof(friendlyName)-1] = 0;
				log_debug( L"Friendly name: %hs", friendlyName );

				if (is_device_name_ok( pszValue, friendlyName, flags ))
				{
					portable_snprintf( fullName, sizeof(fullName), "\\\\.\\%s", pszValue );
					fullName[sizeof(fullName)-1] = 0;
					callback( fullName, arg );
				}
				else
					log_debug( L"Skip port %hs", pszValue );
			}
			RegCloseKey(hDeviceKey);
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfoSet);

	return result_ok;
}

/*
 * Error handling
 */

int is_error_nodevice(unsigned int errcode)
{
	return
		errcode == ERROR_GEN_FAILURE || errcode == ERROR_OPERATION_ABORTED ||
		errcode == ERROR_FILE_NOT_FOUND || errcode == ERROR_DEVICE_NOT_CONNECTED;
}

void set_error_nodevice ()
{
	SetLastError(ERROR_DEVICE_NOT_CONNECTED);
}

unsigned int get_system_error_code ()
{
	return GetLastError();
}

wchar_t* get_system_error_str (int code)
{
	wchar_t* result;
	if (FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
			code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &result, 0, NULL ) == 0)
		return NULL;
	wcsrstrip( result, L"\r\n" );
	return result;
}

void free_system_error_str (wchar_t* str)
{
	if (str)
		LocalFree( str );
}

/*
 * Misc
 */

/* See bug #883, #1904 */
int fix_usbser_sys(const char* device_name)
{
	int devIndex;
	SP_DEVINFO_DATA devInfo;
	SP_DEVINFO_LIST_DETAIL_DATA_W devInfoListDetail;
	SP_PROPCHANGE_PARAMS pcp;
	HDEVINFO devs;
	wchar_t devID[512];
	BYTE friendlyName[512];
	GUID guid = {0x4d36e978, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
	const wchar_t* devid_pattern = L"USB\\VID_1CBE&PID_0007&MI_";
	/* Should be like "(COM1)" */
	wchar_t name_pattern[64];

	while (device_name && memcmp(device_name, "COM", 3))
		++device_name;
	swprintf( name_pattern, sizeof(name_pattern)/sizeof(wchar_t)-1, L"(%hs)", device_name );
	log_debug( L"fix_usbser_sys: mod device name is %hs, name pattern is #%ls", device_name, name_pattern );

	pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
	pcp.StateChange = DICS_PROPCHANGE;
	pcp.HwProfile = 0;

	/* GUID of device class "Ports (COM & USB)" */
	devs = SetupDiGetClassDevsExW( &guid, NULL, NULL, DIGCF_PRESENT, NULL, NULL, NULL );
	if (devs == INVALID_HANDLE_VALUE)
	{
		log_system_error( L"Cannot open device class: ");
		return 1;
	}
	devInfoListDetail.cbSize = sizeof(devInfoListDetail);
	if(!SetupDiGetDeviceInfoListDetailW( devs, &devInfoListDetail ))
	{
		log_system_error( L"Cannot get device info list: ");
		SetupDiDestroyDeviceInfoList( devs );
		return 1;
	}
	devInfo.cbSize = sizeof(devInfo);
	for(devIndex=0; SetupDiEnumDeviceInfo( devs, devIndex, &devInfo ); devIndex++)
	{
		log_debug( L"fix_usbser_sys: look at device #%d", devIndex );
		if (SetupDiGetDeviceInstanceIdW( devs, &devInfo, devID, sizeof(devID)/sizeof(wchar_t)-1, NULL ) &&
				wcsstr(devID, devid_pattern))
		{
			log_debug( L"fix_usbser_sys: device id is #%ls", devID );
			// reset only device with "(COMnn)" name, where nn is the desired port
			memset(friendlyName, 0, sizeof(friendlyName));
			if (SetupDiGetDeviceRegistryPropertyW(devs, &devInfo, SPDRP_FRIENDLYNAME, NULL,
						friendlyName, sizeof(friendlyName), NULL))
			{
				log_debug( L"fix_usbser_sys: friendly name is #%ls", friendlyName );
				if (wcsstr((wchar_t*)friendlyName, name_pattern))
				{
					if (!SetupDiSetClassInstallParamsW( devs, &devInfo, &pcp.ClassInstallHeader, sizeof(pcp) ))
					{
						log_system_error( L"Cannot get class install params: " );
						SetupDiDestroyDeviceInfoList( devs );
						return 1;
					}
					if (!SetupDiCallClassInstaller( DIF_PROPERTYCHANGE, devs, &devInfo ))
					{
						log_system_error( L"Cannot get call a class installer: " );
						SetupDiDestroyDeviceInfoList( devs );
						return 1;
					}
					log_debug( L"fix_usbser_sys: success" );
				}
			}
		}
	}
	SetupDiDestroyDeviceInfoList(devs);

	return 0;
}

void XIMC_API msec_sleep(unsigned int msec)
{
	Sleep( msec );
}

void get_wallclock_us(uint64_t* us)
{
	const time_t DELTA_EPOCH_IN_MICROSECS = (time_t)11644473600000000;
	FILETIME ft;
	time_t tmpres = 0;
	if (us != NULL)
	{
		memset( &ft, 0, sizeof(ft) );

		GetSystemTimeAsFileTime( &ft );

		tmpres = ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres /= 10;  /*convert into microseconds*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		*us = (time_t)tmpres;
	}
}

void get_wallclock(time_t* sec, int* msec)
{
	uint64_t us;
	get_wallclock_us(&us);
	*sec = (time_t)(us / 1000000);
	*msec = (us % 1000000) / 1000; // use milliseconds
}

void uri_path_to_absolute(const char *uri_path, char *abs_path, size_t len)
{
	strncpy(abs_path, uri_path, len);
	abs_path[len-1] = 0;
}

/* Returns non-zero on success */
int set_default_bindy_key()
{
#ifdef HAVE_XIWRAPPER
	/* relative to the current directory of the process which called libximc.dll */
	return bindy_setkey("keyfile.sqlite");
#else
	return 0;
#endif
}

/*
 * Lock support
 */

/* We must have conditional here because vcproj can't select
 * files by macro */

#ifdef HAVE_LOCKS

struct mutex_t
{
	HANDLE impl;
};

mutex_t* mutex_init(unsigned int nonce)
{
	mutex_t* mutex = malloc( sizeof(mutex_t) );
	XIMC_UNUSED(nonce);
	if (!mutex)
	{
		log_system_error( L"can't create semaphore" );
		return NULL;
	}
	mutex->impl = CreateSemaphore( NULL, 1, 1, NULL );
	if (!mutex->impl)
	{
		free( mutex );
		log_system_error( L"can't create semaphore" );
		return NULL;
	}
	log_debug( L"semaphore %ld inited", mutex->impl );
	return mutex;
}

void mutex_close(mutex_t* mutex)
{
	if (mutex)
	{
		if (mutex->impl)
		{
			CloseHandle( mutex->impl );
		}
		log_debug( L"semaphore %ld closed", mutex->impl );
		free( mutex );
	}
}

void mutex_lock(mutex_t* mutex)
{
	if (!mutex || !mutex->impl)
	{
		log_error( L"no semaphore specified" );
		return;
	}
	switch (WaitForSingleObject( mutex->impl, INFINITE ))
	{
		case WAIT_OBJECT_0:
			// ok
			break;
		default:
			log_system_error( L"can't wait on semaphore %ld due to ", mutex->impl );
	}
}

void mutex_unlock(mutex_t* mutex)
{
	if (!mutex || !mutex->impl)
	{
		log_error( L"no semaphore specified" );
		return;
	}
	if (!ReleaseSemaphore( mutex->impl, 1, NULL ))
		log_system_error( L"can't post on semaphore %ld due to ", mutex->impl );
}

#endif

// vim: syntax=c tabstop=4 shiftwidth=4
