#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
// include path to framework
#include <libximc/ximc.h>
#else
#include <ximc.h>
#endif

#ifdef _MSC_VER
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

void print_state (status_t* state)
{
	wprintf( L" rpm: %d", state->CurSpeed );
	wprintf( L" pos: %d", state->CurPosition );
	wprintf( L" upwr: %d", state->Upwr );
	wprintf( L" ipwr: %d", state->Ipwr );
	wprintf( L" flags: %x", state->Flags );
	if (state->Flags & STATE_ALARM)
		wprintf( L" ALARM" );
	if (state->Flags & STATE_ERRC)
		wprintf( L" ERRC" );
	if (state->Flags & STATE_ERRD)
		wprintf( L" ERRD" );
	wprintf( L"\n" );
}

// take note that arguments are given at the reverse order
uint32_t make_key (uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
	return ((uint32_t)b3 << 24) | ((uint32_t)b2 << 16) | ((uint32_t)b1 << 8) | b0;
}

void report_firmware_version (device_t device)
{
	result_t result;
	unsigned int firmware_ver[3];

	if ((result = get_firmware_version( device, firmware_ver+0, firmware_ver+1, firmware_ver+2 )) != result_ok)
		wprintf( L"error command %d\n", result );
	wprintf( L"firmware version: %d.%d.%d\n", firmware_ver[0], firmware_ver[1], firmware_ver[2] );
}

void report_has_firmware (const char* device_name)
{
	result_t result;
	uint8_t is_firmware = 0;

	if ((result = has_firmware( device_name, &is_firmware )) != result_ok) { wprintf( L"error command %d\n", result ); }

	if (is_firmware)
		wprintf( L"firmware is presented\n" );
	else
		wprintf( L"firmware is not presented\n" );
}

const wchar_t* loglevel_string (int loglevel)
{
	switch (loglevel)
	{
		case LOGLEVEL_ERROR: 	return L"ERROR";
		case LOGLEVEL_WARNING:	return L"WARN";
		case LOGLEVEL_INFO:		return L"INFO";
		case LOGLEVEL_DEBUG:	return L"DEBUG";
		default:				return L"UNKNOWN";
	}
}

char* widestr_to_str (const wchar_t* str)
{
	char *result;
	mbstate_t mbs;
	size_t len;
	memset(&mbs, 0, sizeof(mbs));
	len = wcsrtombs( NULL, &str, 0, &mbs );
	if (len == (size_t)(-1))
		return NULL;
	result = malloc(sizeof(char)*(len+1));
	if (result && wcsrtombs( result, &str, len+1, &mbs ) != len)
	{
		free(result);
		return NULL;
	}
	return result;
}

#define UNUSED(x) (void)x;

void XIMC_CALLCONV logging_callback(int loglevel, const wchar_t* message, void* user_data)
{
	fwprintf( stderr, L"%ls: %ls\n", loglevel_string( loglevel ), message );
}

typedef struct 
{
	int num;
	device_t id;
} thread_param_t;

void thread_fun (void* parg)
{
	thread_param_t* tp = (thread_param_t*)parg;
	status_t state;
	int x;
	wprintf( L"[%d] thread start\n", tp->num );

	for (x = 0; x < 10; ++x)
	{
		msec_sleep( 500 + 1000 * rand() / RAND_MAX );
		wprintf( L"[%d] %d start\n", tp->num, x );
		if (get_status( tp->id, &state ) != result_ok) wprintf( L"error command\n" );
		//print_state( &state );
		wprintf( L"[%d] %d end\n", tp->num, x );
	}
}

#define THREAD_COUNT 4

#ifndef _MSC_VER
void* thread_fun_posix (void* parg)
{
	thread_fun( parg );
	return 0;
}

void thread_test (device_t id)
{
	int i;
	pthread_t threads[THREAD_COUNT];
	thread_param_t params[THREAD_COUNT];
  pthread_attr_t pattr;

	wprintf( "thread test\n" );
  
  pthread_attr_init(&pattr);
  pthread_attr_setdetachstate(&pattr,PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < THREAD_COUNT; ++i)
	{
		params[i].num = i;
		params[i].id = id;
		if (pthread_create( &threads[i], &pattr, &thread_fun_posix, &params[i] ))
			abort();
	}

	for (int i = 0; i < THREAD_COUNT; ++i)
		pthread_join( threads[i], NULL );
}

#else
unsigned int __stdcall thread_fun_win32 (void* parg)
{
	thread_fun( parg );
	return 0;
}

void thread_test (device_t id)
{
	int i;
	HANDLE threads[THREAD_COUNT];
	thread_param_t params[THREAD_COUNT];

	wprintf( L"thread test %d\n", id );
  
	for (i = 0; i < THREAD_COUNT; ++i)
	{
		params[i].num = i;
		params[i].id = id;
		threads[i] = (HANDLE)_beginthreadex( NULL, 0, thread_fun_win32, &params[i], 0, NULL );
		if (threads[i] == 0)
			abort();
	}

	if (WaitForMultipleObjects( THREAD_COUNT, threads, TRUE, INFINITE ) != WAIT_OBJECT_0)
		abort();
}

#endif

int main (int argc, char* argv[])
{
	device_t device;
	status_t state;
	result_t result;
	int names_count;
	char device_name[256];
	int i;
	unsigned int freq;
	uint32_t serial, skey;
	int device_specified = 0;

	device_enumeration_t devenum;

	analog_data_t ad;
	engine_settings_t engine_settings;
	chart_data_t chart_data;
	move_settings_t move_settings;
	device_information_t device_information;
	secure_settings_t crit;

	UNUSED(state);
	UNUSED(ad);
	UNUSED(serial);
	UNUSED(skey);
	UNUSED(freq);
	UNUSED(engine_settings);
	UNUSED(chart_data);
	UNUSED(move_settings);
	UNUSED(device_information);
	UNUSED(crit);
	UNUSED(result);

	/* Inherit system locale */
	setlocale(LC_ALL,"");
#ifdef _MSC_VER
	/* UTF-16 output on windows */
	_setmode( _fileno(stdout), _O_U16TEXT );
	_setmode( _fileno(stderr), _O_U16TEXT );
#endif

	set_logging_callback( logging_callback, NULL );

	wprintf( L"Hello! I'm a stupid test program!\n" );
	wprintf( L"I am %d bit\n", sizeof(int*)==4 ? 32 : 64 );
	wprintf( L"Give %d arguments\n", argc-1 );
	for (i = 1; i < argc; ++i) wprintf( L"  #%d: %hs\n", i, argv[i] );

	device_specified = argc == 2 && !strstr( argv[1], "-psn" );

	devenum = enumerate_devices( 0, NULL );
	if (!devenum)
	{
		wprintf( L"error opening\n" );
		return 1;
	}

	names_count = get_device_count( devenum );
	if (names_count == 0 && !device_specified)
	{
		wprintf( L"No devices found" );
		return 1;
	}

	if (device_specified)
		strcpy( device_name, argv[1] );
	else
		strcpy( device_name, get_device_name( devenum, 0 ) );

	for (i = 0; i < names_count; ++i)
	{
		wprintf( L"device name: %hs\n", get_device_name( devenum, i ) );
		if (get_enumerate_device_serial( devenum, i, &serial ) == result_ok)
			wprintf( L"device serial: 0x%X\n", serial );
		if (get_enumerate_device_information( devenum, i, &device_information ) == result_ok)
			wprintf( L"device info: %hs %hs %hs\n",
					device_information.Manufacturer,
					device_information.ManufacturerId,
					device_information.ProductDescription );
	}
	free_enumerate_devices( devenum );

	device = open_device( device_name );
	if (device == device_undefined)
	{
		wprintf( L"error opening\n" );
		return 1;
	}
	if (device == device_undefined)
	{
		wprintf( L"error opening\n" );
		return 1;
	}


//
#if 0
	msec_sleep( 3*1000 );

	report_firmware_version( device );
#endif

#if 0
	report_has_firmware( device_name );

	return 0;
#endif

#if 0

	device = open_device( device_name );
	if (device == device_undefined)
	{
		wprintf( L"error opening\n" );
		return 1;
	}

	wprintf( L"opened...\n" );

	report_firmware_version( device );

	if (get_status( device, &state ) != result_ok) wprintf( L"error command\n" );
	print_state( &state );

#endif
#if 0
	wprintf( L"opened...\n" );

	if (get_status( device, &state ) != result_ok) wprintf( L"error command\n" );
	print_state( &state );
#endif
#if 0

	wprintf( L"test device information\n" );
	if (get_device_information( device, &device_information ) != result_ok) wprintf( L"error command\n" );

	wprintf( L"info: manufacturer=%hs manufacturer_id=%hs product_description=%hs\n",
	    device_information.Manufacturer, device_information.ManufacturerId, device_information.ProductDescription );
#endif
#if 0
	if ((result = get_status( device, &state )) != result_ok)
		wprintf( L"error getting status %d\n", result );
	print_state( &state );
#endif

#if 0
	wprintf( L"call command_left...\n" );

	if ((result = command_left( device )) != result_ok)
		wprintf( L"error command left %d\n", result );

	wprintf( L"call command_left... done\n" );

	if ((result = get_status( device, &state )) != result_ok)
		wprintf( L"error getting status %d\n", result );
	print_state( &state );
#endif

#if 0
	wprintf( L"test move settings\n" );

	if (get_move_settings( device, &move_settings ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"move settings: rpm=%d \n", move_settings.speed);

#endif

#if 0
	for (i = 0; i < 20; ++i)
	{
	clock_t cl = clock();
	if (get_sync_settings( device, &sync_settings ) != result_ok) wprintf( L"error command\n" );
	if (set_sync_settings( device, &sync_settings ) != result_ok) wprintf( L"error command\n" );
	cl = clock() - cl;
	wprintf( L"msec: %.2f\n", cl*1000.0/CLOCKS_PER_SEC );
	msec_sleep( (int)0.5*1000 );
	}
#endif
#if 0
	if (get_sync_settings( device, &sync_settings ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sf=%x, si=%x, so=%x, sp=%d multishift=%d\n",
		sync_settings.setup_flags, sync_settings.syncin_flags, sync_settings.syncout_flags, sync_settings.sync_pin_mode,
		(sync_settings.syncin_flags & TTL_SYNCIN_MULTI_SHIFT)	);

	sync_settings.syncin_flags = TTL_SYNCIN_MULTI_SHIFT;
	if (set_sync_settings( device, &sync_settings ) != result_ok) wprintf( L"error command\n" );

	if (get_sync_settings( device, &sync_settings ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sf=%x, si=%x, so=%x, sp=%d multishift=%d\n",
		sync_settings.setup_flags, sync_settings.syncin_flags, sync_settings.syncout_flags, sync_settings.sync_pin_mode,
		(sync_settings.syncin_flags & TTL_SYNCIN_MULTI_SHIFT)	);

	sync_settings.syncin_flags = 0x0;
	if (set_sync_settings( device, &sync_settings ) != result_ok) wprintf( L"error command\n" );

	if (get_sync_settings( device, &sync_settings ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sf=%x, si=%x, so=%x, sp=%d multishift=%d\n",
		sync_settings.setup_flags, sync_settings.syncin_flags, sync_settings.syncout_flags, sync_settings.sync_pin_mode,
		(sync_settings.syncin_flags & TTL_SYNCIN_MULTI_SHIFT)	);
#endif

#if 0
	if ((result = get_analog_data( device, &ad )) != result_ok) wprintf( L"error command %d\n", result );
	wprintf( L"ad1 vout1=%d vout2=%d vext=%d vkey=%d veng=%d pot=%d vtkey=%d\n",
		ad.adc_vout1, ad.adc_vout2, ad.adc_vext, ad.adc_vkey, ad.adc_engcur, ad.adc_pot, ad.adc_tempkey );
	wprintf( L"ad2 vout1=%d vout2=%d vext=%d vkey=%d pwm=%d veng=%d engcur=%d vtkey=%d\n",
		ad.cdc_vout1, ad.cdc_vout2, ad.cdc_vext, ad.cdc_vkey, ad.cdc_pwmlevel, ad.cdc_veng, ad.cdc_engcur, ad.cdc_tempkey );
#endif


#if 0

	if (get_serial_number( device, &serial ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sn: get %x\n", serial );

	if (get_serial_number( device, &serial ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sn: get %x\n", serial );

	serial = 0x5555;
	if (set_serial_number( device, serial, skey ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sn: set %x\n", serial );

	if (get_serial_number( device, &serial ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sn: get %x\n", serial );
#endif

#if 0
	if (get_serial_number( device, &serial ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sn: get %x\n", serial );

	skey = make_key( 0, 0, 0, 0 );
	serial = 0x5555;
	if (set_serial_number( device, serial, skey ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sn: set %x\n", serial );

	if (get_serial_number( device, &serial ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sn: get %x\n", serial );

	wprintf( L"skey: %x\n", skey );

	wprintf( L"------------\n" );

	wprintf( L"set key...\n" );
	if (write_key( device_name, skey ) != result_ok) wprintf( L"error command\n" );

	msec_sleep( 5*1000 );

	wprintf( L"read sn...\n" );
	if (get_serial_number( device, &serial ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"sn: %x\n", serial );
#endif

#if 0
	for (i = 0; i < 5; ++i)
	{
		msec_sleep( 1000 );
		wprintf( L"resetting device\n" );
		if (command_reset( device ) != result_ok) wprintf( L"error command\n" );

		msec_sleep( 200 );

		if (command_connect( device ) != result_ok) wprintf( L"error command\n" );
		wprintf( L"connected\n" );
		if (command_disconnect( device ) != result_ok) wprintf( L"error command\n" );
		wprintf( L"disconnected\n" );
	}

#endif
#if 0
	if (get_status( device, &state ) != result_ok) wprintf( L"error command\n" );
	print_state( &state );

	if (get_secure_settings( device, &crit ) != result_ok) wprintf( L"error command\n" );

	crit.critical_t = 320;
	if (set_secure_settings( device, &crit ) != result_ok) wprintf( L"error command\n" );

	wprintf( L"crit curr=%d, volt=%d, temp=%d\n", crit.critical_upwr, crit.critical_ipwr, crit.critical_t);

	for (i = 0; i < 10; ++i)
	{
		if (get_status( device, &state ) != result_ok) wprintf( L"error command\n" );
		wprintf( L"cur temp=%d\n", state.curt );
		msec_sleep( 1000 );
	}

	wprintf( L"trashed\n" );

	if (get_status( device, &state ) != result_ok) wprintf( L"error command\n" );
	print_state( &state );

/*
	if (state.Sts == DevStateAlarm)
	{
		wprintf( L"in alarm. press when ready to init\n" );

		getchar();

		wprintf( L"send INIT\n" );

		if (init_device( device ) != result_ok) wprintf( L"error command\n" );

		wprintf( L"sent INIT\n" );

		msec_sleep( 5000 );

		if (get_status( device, &state ) != result_ok) wprintf( L"error command\n" );
		print_state( &state );

	}
	*/

	/*
	if (get_secure_settings( device,
				&critical_curr, &critical_voltage, &critical_temp ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"crit curr=%d, volt=%d, temp=%d\n",
		critical_curr, critical_voltage, critical_temp );

	msec_sleep( 3000 );

	if (get_status( device, &state ) != result_ok) wprintf( L"error command\n" );
	print_state( &state );

	msec_sleep( 3000 );

	if (get_status( device, &state ) != result_ok) wprintf( L"error command\n" );
	print_state( &state );

	if (get_chart_data( device, &chart_data ) != result_ok) wprintf( L"error command\n" );
	wprintf( L"cur temp=%d\n",
		chart_data.current_temp );*/
#endif

#if 1
	thread_test( device );
#endif

	if (close_device( &device ) != result_ok) wprintf( L"error closing\n" );

	wprintf( L"Done\n" );
	return 0;
}

