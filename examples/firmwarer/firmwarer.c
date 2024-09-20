#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
// include path to framework
#include <libximc/ximc.h>
#else
#include <ximc.h>
#endif

void print_state (status_t* state)
{
	printf( " rpm: %d", state->CurSpeed );
	printf( " pos: %d", state->CurPosition );
	printf( " upwr: %d", state->Upwr );
	printf( " ipwr: %d", state->Ipwr );
	printf( " flags: %x", state->Flags );
	if (state->Flags & STATE_ALARM)
		printf( " ALARM" );
	if (state->Flags & STATE_ERRC)
		printf( " ERRC" );
	if (state->Flags & STATE_ERRD)
		printf( " ERRD" );
	printf( "\n" );
}


void report_firmware_version (device_t device)
{
	result_t result;
	unsigned int firmware_ver[3];

	if ((result = get_firmware_version( device, firmware_ver+0, firmware_ver+1, firmware_ver+2 )) != result_ok)
		printf( "error command %d\n", result );
}

void report_has_firmware (const char* device_name)
{
	result_t result;
	uint8_t is_firmware = 0;

	if ((result = has_firmware( device_name, &is_firmware )) != result_ok) { printf( "error command %d\n", result ); }

	if (is_firmware)
		printf( "firmware is presented\n" );
	else
		printf( "firmware is not presented\n" );
}

/* returns 0 on success */
int read_file (const char* filename, uint8_t** data, int* datalen)
{
	FILE* f;

	if (!filename || !data || !datalen)
		return 1;

	f = fopen( filename, "rb" );
	if (!f)
	{
		printf( "Can't open file %s\n", filename );
		return 1;
	}
	fseek( f, 0, SEEK_END );
	*datalen = ftell( f );
	rewind( f );
	*data = malloc( *datalen );
	if (*datalen != (long)fread( *data, 1, *datalen, f ))
	{
		printf( "Can't read file %s\n", filename );
		return 1;
	}
	fclose( f );
	return 0;
}

int upload_firmware(char* device_name, char* firmware_file, char* key_file, int only_key)
{
	uint8_t* data;
	int datalen;
	uint8_t* key;
	int keylen;
	result_t result;
	int upload_result = 0;

	// REMOVED sleep so device will leave the loader
	//msec_sleep( 2000 );

	if (key_file)
	{
		if (!read_file( key_file, &key, &keylen ))
		{
			printf( "Can't load file\n" );
			return 1;
		}

		if (write_key( device_name, key ) != result_ok) {
			printf( "error command\n" );
			upload_result = 1;
		}
		else
			printf( "key is written\n" );

		printf( "waiting for reset after a key write..\n" );
		msec_sleep( 1000 );

		free( key );
	}

	if (!only_key && upload_result == 0)
	{
		if (!read_file( firmware_file, &data, &datalen ))
		{
			printf( "Can't load file\n" );
			return 1;
		}
		printf( "Initiating upload\n" );
		if ((result = command_update_firmware( device_name, data, datalen )) != result_ok)
		{
			printf( "Error uploading firmware %d\n", result );
			upload_result = 1;
		}
		else
		{
			printf( "Firmware uploaded\n" );
		}
		free( data );
	}

	return upload_result;
}

int main (int argc, char* argv[])
{
	char device_name[256];
	device_t device;
	status_t state;
	int names_count = 16;
	int i;
	int key_specified = 0;
	int only_key = 0;
	char* firmware_file = NULL;
	char* key_file = NULL;


	device_enumeration_t devenum;

	/* Inherit system locale */
	setlocale(LC_ALL,"");
	/* It is an ANSI version of program */
	set_logging_callback( logging_callback_stderr_narrow, NULL );

	printf( "Hello! I'm a firmware updater!\n" );
	printf( "I am %s bit\n", sizeof(int*)==4 ? "32" : "64" );
	printf( "Give %d arguments\n", argc-1 );
	for (i = 1; i < argc; ++i) printf( "  #%d: %s\n", i, argv[i] );

	if (argc > 5 || argc < 3)
	{
		printf( "usage: firmware firmware_file device [keyfile] [onlykey?]\n" );
		return 1;
	}
	firmware_file = argv[1];
	key_specified = argc >= 4;
	only_key = argc >= 5 && !strcmp( argv[4], "onlykey" );

	strncpy( device_name, argv[2], sizeof(device_name) );
	if (key_specified)
		key_file = argv[3];

	devenum = enumerate_devices( 0 );
	if (!devenum)
	{
		printf( "error opening\n" );
		return 1;
	}

	names_count = get_device_count( devenum );

	if (names_count == 0)
	{
		printf( "No devices found" );
	}

	for (i = 0; i < names_count; ++i)
	{
		printf( "device: %s\n", get_device_name( devenum, i ) );
	}
	free_enumerate_devices( devenum );
	printf( "Using device: %s\n", device_name );

	printf( "Starting updating firmware\n" );
	if (upload_firmware( device_name, firmware_file, key_file, only_key ) == 0)
	{
		printf( "Update completed, sleep for a while...\n" );
		msec_sleep( 6*1000 );

		// check out what we have done
		printf( "Check for firmware\n" );

		report_has_firmware( device_name );

		printf( "Check device\n" );

		device = open_device( device_name );
		if (device == device_undefined)
		{
			printf( "error opening\n" );
			return 1;
		}

		report_firmware_version( device );

		if (get_status( device, &state ) != result_ok)
		{
			printf( "error command. maybe updating failed\n" );
		}
		else
		{
			print_state( &state );
			printf( "device seems to be ok!\n" );
		}

		if (close_device( &device ) != result_ok) printf( "error closing\n" );
	}
	printf( "Done\n" );
	return 0;
}

