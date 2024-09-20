#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
#include <libximc/ximc.h>
#else
#include <ximc.h>
#endif

// This line includes a c-profile for the "8MT173-25" stage
#include "8MT173-25.h"

int main (int argc, char* argv[])
{
/*
	Variables declaration.
	device_t, status_t, engine_settings_t, status_calb and calibration_t are types provided by the libximc library.
*/
	device_t device;
	result_t result;
	int names_count;
	char device_name[256];
	const int probe_flags = ENUMERATE_PROBE;
	const char* enumerate_hints = "";
	char ximc_version_str[32];
	device_enumeration_t devenum;

// unused variables
	(void)argc;
	(void)argv;

	printf( "This is a ximc test program.\n" );
//	ximc_version returns library version string.
	ximc_version( ximc_version_str );
	printf( "libximc version %s\n", ximc_version_str );

//	Device enumeration function. Returns an opaque pointer to device enumeration data.
	devenum = enumerate_devices( probe_flags, enumerate_hints );

//	Gets device count from device enumeration data
	names_count = get_device_count( devenum );

//	Terminate if there are no connected devices
	if (names_count <= 0)
	{
		printf( "No devices found\n" );
	//	Free memory used by device enumeration data
		free_enumerate_devices(devenum);
		printf( "Press Enter to exit...");
		char  terminating_str[2];
		fgets(terminating_str, 2, stdin);
		return 1;
	}

//	Copy first found device name into a string
	strcpy( device_name, get_device_name( devenum, 0 ) );
//	Free memory used by device enumeration data
	free_enumerate_devices( devenum );

	printf( "Opening device...");
//	Open device by device name
	device = open_device( device_name );
	printf( "done.\n" );

//	Load c-profile
	printf( "Setting profile for 8MT173-25... ");
	result = set_profile_8MT173_25(device);
	printf( "done. Result = %d\n", result );

	printf( "Closing device..." );
//	Close specified device
	close_device( &device );
	printf( "done.\n" );
	printf( "Press Enter to exit...");
	char  terminating_str[2];
	fgets(terminating_str, 2, stdin);
	return 0;
}
