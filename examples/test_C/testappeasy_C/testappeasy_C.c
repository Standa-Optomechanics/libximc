#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
#include <libximc/ximc.h>
#else
#include <ximc.h>
#endif

int main (int argc, char* argv[])
{
/*
	Variables declaration.
	device_t, status_t, engine_settings_t, status_calb and calibration_t are types provided by the libximc library.
*/
	device_t device;
	engine_settings_t engine_settings;
	status_t status;
	status_calb_t status_calb;
	calibration_t calibration;
	const char* units = "mm";
	int names_count;
	char device_name[256];
	const int probe_flags = ENUMERATE_PROBE | ENUMERATE_NETWORK;
	const char* enumerate_hints = "addr=192.168.1.1,172.16.2.3";
	// const char* enumerate_hints = "addr="; // this hint will use broadcast enumeration, if ENUMERATE_NETWORK flag is enabled
	char ximc_version_str[32];
	const int seconds = 3;
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
		free_enumerate_devices( devenum );
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

	printf( "Getting status parameters: " );
//	Read device status from a device
	get_status( device, &status );
	printf( "position %d, encoder %lld, speed %d\n", status.CurPosition, status.EncPosition, status.CurSpeed );

	printf( "Getting engine parameters: " );
//	Read engine settings from a device
	get_engine_settings( device, &engine_settings );
	printf( "voltage %d, current %d, speed %d\n", engine_settings.NomVoltage, engine_settings.NomCurrent, engine_settings.NomSpeed );

	printf( "Rotating to the left for %d seconds...", seconds);
//	Send "rotate left" command to a device
	command_left( device );
	msec_sleep( seconds*1000 );
	printf( "\n" );

	printf( "Getting status parameters: " );
//	Read device status from a device
	get_status( device, &status );
	printf( "position %d, encoder %lld, speed %d\n", status.CurPosition, status.EncPosition, status.CurSpeed );

	printf( "Getting calibrated parameters: " );
	// Setting calibration constant to 0.1 (one controller step equals this many units)
	calibration.A = 0.1;
	// We have to set microstep mode to convert microsteps to calibrated units correctly
	calibration.MicrostepMode = engine_settings.MicrostepMode;

	// Read calibrated device status from a device
	get_status_calb( device, &status_calb, &calibration);
	printf( "calibrated position %.3f %s, calibrated speed %.3f %s/s\n", status_calb.CurPosition, units, status_calb.CurSpeed, units );

	printf( "Stopping engine..." );
//	Send "stop" command to a device
	command_stop( device );
	printf( "done.\n" );

	printf( "Getting status parameters: " );
//	Read device status from a device
	get_status( device, &status );
	printf( "position %d, encoder %lld, speed %d\n", status.CurPosition, status.EncPosition, status.CurSpeed );

	printf( "Closing device..." );
//	Close specified device
	close_device( &device );
	printf( "done.\n" );
	printf( "Press Enter to exit...");
	char  terminating_str[2];
	fgets(terminating_str, 2, stdin);
	return 0;
}
