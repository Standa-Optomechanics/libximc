#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
// include path to framework
#include <libximc/ximc.h>
#else
#include <ximc.h>
#endif

#ifndef _WIN32
#include <syslog.h>
#else
#include <io.h>
#include <fcntl.h>
#endif

void print_state (status_t* state)
{
	wprintf( L" rpm: %d", state->CurSpeed );
	wprintf( L" pos: %d", state->CurPosition );
	wprintf( L" upwr: %d", state->Upwr );
	wprintf( L" ipwr: %d", state->Ipwr );
	wprintf( L" flags: %x", state->Flags );
	wprintf( L" mvsts: %x", state->MvCmdSts );
	if (state->Flags & STATE_ALARM)
		wprintf( L" ALARM" );
	if (state->Flags & STATE_ERRC)
		wprintf( L" ERRC" );
	if (state->Flags & STATE_ERRD)
		wprintf( L" ERRD" );
	wprintf( L"\n" );
}

void print_state_calb(status_calb_t* state)
{
	wprintf(L" rpm: %f", state->CurSpeed);
	wprintf(L" pos: %f", state->CurPosition);
	wprintf(L" upwr: %d", state->Upwr);
	wprintf(L" ipwr: %d", state->Ipwr);
	wprintf(L" flags: %x", state->Flags);
	wprintf(L" mvsts: %x", state->MvCmdSts);
	if (state->Flags & STATE_ALARM)
		wprintf(L" ALARM");
	if (state->Flags & STATE_ERRC)
		wprintf(L" ERRC");
	if (state->Flags & STATE_ERRD)
		wprintf(L" ERRD");
	wprintf(L"\n");
}

void print_edges(edges_settings_t* edges)
{
	wprintf(L" Lpos: %d", edges->LeftBorder);
	wprintf(L" uLpos: %d", edges->uLeftBorder);
	wprintf(L" Rpos: %d", edges->RightBorder);
	wprintf(L" uRpos: %d", edges->uRightBorder);
	wprintf(L"\n");
}

void print_edges_calb(edges_settings_calb_t* edges)
{
	wprintf(L" Lpos: %f", edges->LeftBorder);
	wprintf(L" Rpos: %f", edges->RightBorder);
	wprintf(L"\n");
}

void print_position_calb(get_position_calb_t* pos_c)
{
	wprintf(L" pos: %f", pos_c->Position);

	wprintf(L"\n");
}

const wchar_t* error_string (result_t result)
{
	switch (result)
	{
		case result_error:				return L"error";
		case result_not_implemented:	return L"not implemented";
		case result_nodevice:			return L"no device";
		default:						return L"success";
	}
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

#ifndef _WIN32
int loglevel_to_sysloglevel (int loglevel)
{
	switch (loglevel)
	{
		case LOGLEVEL_ERROR: 	return LOG_ERR;
		case LOGLEVEL_WARNING:	return LOG_WARNING;
		case LOGLEVEL_INFO:		return LOG_INFO;
		case LOGLEVEL_DEBUG:	return LOG_DEBUG;
		default:				return LOG_INFO;
	}
}
#endif

static char* widestr_to_str (const wchar_t* str)
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

void XIMC_CALLCONV my_logging_callback(int loglevel, const wchar_t* message, void* user_data)
{
	wchar_t wbuf[2048];
	char *abuf;
	(void)user_data;
	int used_loglevel = user_data ? *((int*)user_data) : LOGLEVEL_DEBUG;
	if (loglevel > used_loglevel)
		return;

	/* Print to console unicode chars */
	swprintf( wbuf, sizeof(wbuf)/sizeof(wbuf[0])-1, L"XIMC %ls: %ls", loglevel_string( loglevel ), message );
	fwprintf( stderr, L"%ls\n", wbuf );

#ifdef _WIN32
	(void)abuf;
#else
	/* Print to syslog ANSI chars */
	abuf = widestr_to_str( wbuf );
	if (abuf)
	{
		syslog( loglevel_to_sysloglevel( loglevel ), "%s", abuf );
		free( abuf );
	}
#endif
}

int main (int argc, char* argv[])
{
	device_t device;
	engine_settings_t engine_settings;
	engine_settings_calb_t engine_settings_calb;
	calibration_t calibration;
	// correction_t correction;
	device_information_t di;
	status_t state;
	status_calb_t state1;
	result_t result;
	// set_position_calb_t set_pos;
	get_position_calb_t get_pos;
	edges_settings_calb_t edges_settings_calb;
	edges_settings_t edges_settings;
	motor_settings_t motor_settings;
	emf_settings_t emf_settings;
	engine_advansed_setup_t eas_settings;


	int names_count;
	char device_name[256];
	int i;
	int device_specified = 0;
	const int probe_flags = ENUMERATE_PROBE | ENUMERATE_NETWORK;
	const char* enumerate_hints = "addr=";
	// const char* enumerate_hints =  "addr=192.168.1.1,172.16.2.3";// this hint will use broadcast enumeration, if ENUMERATE_NETWORK flag is enabled
	char ximc_version_str[32];

	device_enumeration_t devenum;

	/* Inherit system locale */
	setlocale(LC_ALL,"");
#ifdef _MSC_VER
	/* UTF-16 output on windows */
	_setmode( _fileno(stdout), _O_U16TEXT );
	_setmode( _fileno(stderr), _O_U16TEXT );
#endif

	int used_loglevel = getenv("XIMC_testapp_C_VERBOSE") ? LOGLEVEL_DEBUG : LOGLEVEL_WARNING;
	set_logging_callback(my_logging_callback, &used_loglevel);
	ximc_version( ximc_version_str );

	wprintf( L"Hello! I'm a stupid test program!\n" );
	wprintf( L"libximc version %hs\n", ximc_version_str );
	wprintf( L"I am %d bit\n", sizeof(int*)==4 ? 32 : 64 );
	wprintf( L"Give %d arguments\n", argc-1 );
	for (i = 1; i < argc; ++i) wprintf( L"  #%d: %hs\n", i, argv[i] );

	device_specified = argc == 2 && !strstr( argv[1], "-psn" );
	if (device_specified)
	{
		strcpy( device_name, argv[1] );
	}

	devenum = enumerate_devices( probe_flags, enumerate_hints );
	if (!devenum)
	{
		wprintf( L"error enumerating devices\n" );
		names_count = 0;
	}

	names_count = get_device_count( devenum );
	if (names_count == -1)
	{
		wprintf( L"error enumerating device\n" );
		names_count = 0;
	}
	for (i = 0; i < names_count; ++i)
	{
		wprintf( L"device: %hs\n", get_device_name( devenum, i ) );
	}

	if (!device_specified)
	{
		if (names_count == 0)
		{
			// 255 is probably enough. The only problem may occure in Windows in case %USERNAME% is several hundred characters
			char path_to_virt_controller[255];
			wprintf( L"The real controller is not found or busy with another app.\nThe virtual controller is opening to check the operation of the library.\n" );
		#if defined(_WIN32) || defined(_WIN64)
			strcpy(path_to_virt_controller, getenv("USERPROFILE"));
			strcat(path_to_virt_controller, "\\virt_controller.bin");
			strcpy(device_name, "xi-emu:///");
			strcat(device_name, path_to_virt_controller);
			wprintf(L"Path to controller file to open: %ls\\virt_controller.bin\n", _wgetenv(L"USERPROFILE"));
		#else
			strcpy(path_to_virt_controller, getenv("HOME"));
			strcat(path_to_virt_controller, "/virt_controller.bin");
			strcpy(device_name, "xi-emu:///");
			strcat(device_name, path_to_virt_controller);
			wprintf(L"Path to controller file to open: ~/virt_controller.bin\n");
		#endif
		}
		else
			strcpy( device_name, get_device_name( devenum, 0 ) );
	}

	free_enumerate_devices( devenum );

	wprintf( L"\n\nOpening device...\n\n");

	device = open_device( device_name );
	if (device == device_undefined)
	{
		wprintf( L"error opening device\n" );
		wprintf(L"Press Enter to exit...");
		char  terminating_str[2];
		fgets(terminating_str, 2, stdin);
		return 1;
	}


	
	if ((result = get_status( device, &state )) != result_ok)
		wprintf( L"error getting status: %ls\n", error_string( result ) );
	print_state( &state );

	if ((result = get_device_information( device, &di )) != result_ok)
		wprintf( L"error getting di: %ls\n", error_string( result ) );
	wprintf( L"DI: manufacturer: %hs, id %hs, product %hs. Ver: %d.%d.%d\n",
			di.Manufacturer, di.ManufacturerId, di.ProductDescription,
			di.Major, di.Minor, di.Release);

	if ((result = command_zero( device )) != result_ok)
		wprintf( L"error zeroing: %ls\n", error_string( result ) );

	if ((result = get_status( device, &state )) != result_ok)
		wprintf( L"error getting status %ls\n", error_string( result ) );
	print_state( &state );

	if ((result = get_engine_settings( device, &engine_settings )) != result_ok)
		wprintf( L"error getting engine settings %ls\n", error_string( result ) );

	wprintf( L"engine: voltage %d current %d speed %d\n",
		engine_settings.NomVoltage, engine_settings.NomCurrent, engine_settings.NomSpeed );

	if ((result = get_emf_settings(device, &emf_settings)) != result_ok)
		wprintf(L"error getting emf settings %ls\n", error_string(result));

	wprintf(L"emf: Km %f L %f R %f\n",
		emf_settings.Km, emf_settings.L, emf_settings.R);

	eas_settings.stepcloseloop_Kw = 98;
	eas_settings.stepcloseloop_Kp_low = 99;
	eas_settings.stepcloseloop_Kp_high = 100;
	if ((result = set_engine_advansed_setup(device, &eas_settings)) != result_ok)
		wprintf(L"error getting eas settings %ls\n", error_string(result));


	eas_settings.stepcloseloop_Kw = 0;
	eas_settings.stepcloseloop_Kp_high = 0;

	if ((result = get_engine_advansed_setup(device, &eas_settings)) != result_ok)
		wprintf(L"error getting eas settings %ls\n", error_string(result));
	wprintf(L"eas: 1 %d 2 %d 3 %d\n",
		eas_settings.stepcloseloop_Kw, eas_settings.stepcloseloop_Kp_low, eas_settings.stepcloseloop_Kp_high);

	/*Reading motor parameters and output its type.  
	   MotorType = 0 motor not defined or not connected. 
	   MotorType = 1 step. 
	   MotorType = 2 DC. 
	   MotorType = 3 BLDC.
	*/
	if ((result = get_motor_settings(device, &motor_settings)) != result_ok)
		wprintf(L"error getting engine settings %ls\n", error_string(result));
	wprintf(L"Motor: Type %d\n",
		motor_settings.MotorType);

	calibration.A = 1;
	calibration.MicrostepMode = MICROSTEP_MODE_FULL;	

	if ((result = get_engine_settings_calb( device, &engine_settings_calb, &calibration )) != result_ok)
		wprintf( L"error getting engine calb settings %ls\n", error_string( result ) );
		
	wprintf( L"engine calb: voltage %d current %d speed %f\n",
		engine_settings_calb.NomVoltage, engine_settings_calb.NomCurrent, engine_settings_calb.NomSpeed );

	int sec = 2;
	wprintf( L"\n\nNow engine will rotate to the left for %d seconds...\n\n", sec);

	if ((result = command_left( device )) != result_ok)
		wprintf( L"error command left %ls\n", error_string( result ) );

	msec_sleep( sec*1000 );

	if ((result = get_status( device, &state )) != result_ok) {
		wprintf( L"error getting status %ls\n", error_string( result ) );
	}
	print_state( &state );

	char* ch1 = "Corr_table_example.tbl";

	// The device_t device parameter in this function is a C pointer, unlike most library functions that use this parameter
	if ((result = set_correction_table(device, ch1)) != result_ok)
		wprintf(L"error load_calibration_table %ls\n", error_string(result));

	if ((result = command_move_calb(device, 380, &calibration)) != result_ok)
		wprintf(L"error command_move_corr %ls\n", error_string(result));
	else wprintf(L"command_move_corr  %ls\n", error_string(result));
	
	if ((result = command_wait_for_stop( device, 100 )) != result_ok)
		wprintf( L"error command_wait_for_stop %ls\n", error_string( result ) );

	if ((result = get_position_calb(device, &get_pos, &calibration)) != result_ok)
		wprintf(L"error getting engine calb settings %ls\n", error_string(result));
	print_position_calb(&get_pos);

	if ((result = get_status_calb(device, &state1, &calibration)) != result_ok)
		wprintf(L"error getting status %ls\n", error_string(result));
	print_state_calb(&state1);


	if ((result = get_edges_settings_calb(device, &edges_settings_calb, &calibration)) != result_ok) {
		float LeftBorder = edges_settings_calb.LeftBorder;
		float RightBorder = edges_settings_calb.RightBorder;
		edges_settings_calb.LeftBorder = -900;
		edges_settings_calb.RightBorder = 1500;
		if ((result = set_edges_settings_calb(device, &edges_settings_calb, &calibration)) != result_ok)
			wprintf(L"error edges settings %ls\n", error_string(result));
		print_edges_calb(&edges_settings_calb);

		if ((result = get_edges_settings_calb(device, &edges_settings_calb, &calibration)) != result_ok)
			wprintf(L"error edges settings %ls\n", error_string(result));
		print_edges_calb(&edges_settings_calb);

		edges_settings_calb.LeftBorder = LeftBorder;
		edges_settings_calb.RightBorder = RightBorder;
		if ((result = set_edges_settings_calb(device, &edges_settings_calb, &calibration)) != result_ok)
			wprintf(L"error edges settings %ls\n", error_string(result));
	}
	if ((result = get_edges_settings(device, &edges_settings)) != result_ok)
		wprintf(L"error edges settings %ls\n", error_string(result));
	print_edges(&edges_settings);


	wprintf( L"\n\nStopping engine...\n\n");

	if ((result = command_stop( device )) != result_ok)
		wprintf( L"error command stop %ls\n", error_string( result ) );

	if ((result = get_status( device, &state )) != result_ok)
		wprintf( L"error getting status %ls\n", error_string( result ) );
	print_state( &state );
	if ((result = set_correction_table(device, NULL)) != result_ok)
		wprintf(L"error load_calibration_table %ls\n", error_string(result));
	if ((result = get_status_calb(device, &state1, &calibration)) != result_ok)
		wprintf(L"error getting status %ls\n", error_string(result));
	print_state_calb(&state1);

	msec_sleep( 2*1000 );

	// The id parameter device_t device this function is a C pointer, unlike most library functions that use this parameter
	if ((result = close_device( &device )) != result_ok)
		wprintf( L"error closing device %ls\n", error_string( result ) );

	wprintf( L"Done\n" );
	wprintf( L"Press Enter to exit...");
	char  terminating_str[2];
	fgets(terminating_str, 2, stdin);
	return 0;
}

