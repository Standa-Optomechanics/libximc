#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// msvc doesn't care about stdint
#ifndef _MSC_VER
#include <stdint.h>
#endif

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
// include path to framework
#include <libximc/ximc.h>
#else
#include <ximc.h>
#endif

void print_state (state_t* state)
{
	printf( "state: " );
	switch (state->Sts)
	{
		case DevStateOff: printf( "off" ); break;
		case DevStateStop: printf( "stop" ); break;
		case DevStateMoving: printf( "run" ); break;
		case DevStateTune: printf( "tune" ); break;
		case DevStateCalibr: printf( "calibrating" ); break;
		case DevStateAlarm: printf( "alarm" ); break;
		default: printf( "UNKNOWN" );
	}
	printf( "  pwm: " );
	switch (state->pwm_state)
	{
		case PwmStateOff      : printf( "off" ); break;
		case PwmStateBrak    	: printf( "brake" ); break;
		case PwmStateRunfwd	 	: printf( "runfwd" ); break;
		case PwmStateRunbck		: printf( "runbck" ); break;
		case PwmStateInvbrak	: printf( "invbrake" ); break;
		default: printf( "UNKNOWN" );
	}
	printf( " rpm: %d", state->current_speed );
	printf( " pos: %d", state->current_position );
	printf( " veng: %d", state->current_engine_voltage );
	printf( " vpwr: %d", state->current_power_voltage );
	printf( " currr: %d", state->current_input );
	printf( " flags: %x", state->flags );
	if (state->flags & STATE_ERRC)
		printf( " ERRC" );
	if (state->flags & STATE_ERRD)
		printf( " ERRD" );
	printf( "\n" );
}

const char* error_string (result_t result)
{
	switch (result)
	{
		case result_error: return "error";
		case result_not_implemented: return "not implemented";
		case result_nodevice: return "no device";
		default: return "success";
	}
}

int main (int argc, char* argv[])
{
	char device_name[256];
	device_t device;
	state_t state;
	result_t result;
	int names_count = 16;
	char** names = NULL;
	int i;
	int device_specified = 0;

	result = result_ok;

	printf( "Hello! I'm a stupid test program!\n" );
	printf( "I am %s bit\n", sizeof(int*)==4 ? "32" : "64" );
	printf( "Give %d arguments\n", argc-1 );
	for (i = 1; i < argc; ++i) printf( "  #%d: %s\n", i, argv[i] );

	device_specified = argc == 2 && strstr( argv[1], "-psn" );
	if (device_specified)
		strncpy( device_name, argv[1], sizeof(device_name) );

	names_count = 0;
	/*if ((result = enumerate_devices( &names_count, &names, 1 )) != result_ok)
	{
		printf( "error opening device %s\n", error_string( result ) );
		return 1;
	}
*/
	if (!device_specified && names_count == 0)
	{
		printf( "No devices found\n" );
		return 1;
	}

	if (!device_specified)
		strncpy( device_name, names[0], sizeof(device_name) );

	for (i = 0; i < names_count; ++i)
	{
		printf( "device: %s\n", names[i] );
	}
	free_enumerate_devices( names_count, names );

	printf( "\n\nOpening device...\n\n");

	device = open_device( device_name );
	if (device == device_undefined)
	{
		printf( "error opening device\n" );
		return 1;
	}

	printf("getting state for 15 seconds\n");

	time_t starttime = time(0);
	do{
		if ((result = get_status( device, &state )) != result_ok)
			printf( "error getting status %s\n", error_string( result ) );
		else
			printf(".");
		msec_sleep( 50 );
		fflush(stdout);

	}while(time(0) - starttime < 15);
	printf("\n");

	if ((result = close_device( device )) != result_ok)
		printf( "error closing device %s\n", error_string( result ) );

	printf( "Done\n" );
	return 0;
}

