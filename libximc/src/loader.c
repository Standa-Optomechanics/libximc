#include "common.h"

#include "ximc.h"
#include "util.h"
#include "metadata.h"
#include "platform.h"
#include "protosup.h"
#include "loader.h"
#include "ximc-gen.h"

#if defined(__cplusplus)
extern "C" {
#endif

static int is_debug_logging()
{
	#ifdef DEBUG_TRACE
	return 1;
	#else
	return 0;
	#endif
}


void dump_service_result (const wchar_t* prefix, service_result_t sresult)
{
	const wchar_t* str_sresult;
	switch (sresult)
	{
		case RESULT_OK: 					str_sresult=L"RESULT_OK"; break;
		case RESULT_ERROR: 					str_sresult=L"RESULT_ERROR"; break;
		case RESULT_FLASH_ERROR: 			str_sresult=L"RESULT_FLASH_ERROR"; break;
		case RESULT_HAS_FIRMWARE: 			str_sresult=L"RESULT_HAS_FIRMWARE"; break;
		case RESULT_NO_FIRMWARE: 			str_sresult=L"RESULT_NO_FIRMWARE"; break;
		case RESULT_ALREADY_IN_FIRMWARE:	str_sresult=L"RESULT_ALREADY_IN_FIRMWARE"; break;
		case RESULT_SOFT_ERROR:				str_sresult=L"RESULT_SOFT_ERROR"; break;
		case RESULT_HARD_ERROR:				str_sresult=L"RESULT_HARD_ERROR"; break;
		default:							str_sresult=L"RESULT UNKNOWN";
	}
	log_error( L"%ls: service function error %ls", prefix, str_sresult );
}

// all commands with service_ prefix must not use lock

// must not lock
// pre-condition: id should refer to an closed device
// post-condition: id should refer to an opened device
result_t update_firmware_reopen_cycled(device_t* id, const char* name)
{
	result_t result;
	int waited_time = 0;
	const int sleep_time = XISM_PORT_DETECT_TIME/5;

	do
	{
		*id = open_device_impl( name, DEFAULT_TIMEOUT_TIME );
		if(*id != device_undefined)
		{
			if (is_debug_logging()) log_debug ( L"UF try connect..." );
			if ((result = service_command_connect_impl( *id )) == result_ok)
				return result;

			if (is_debug_logging()) log_debug ( L"UF reopen port..." );

			result = close_device_impl( id );
			if (result != result_ok)
				log_warning( L"updater: error closing port non-fatal" );
		}
		msec_sleep( sleep_time );
		waited_time += sleep_time;
	}
	while (waited_time < XISM_PORT_DETECT_TIME);

	if (*id != device_undefined)
		close_device_impl( id );

	return result_nodevice;
}

result_t XIMC_API command_update_firmware(const char* uri, const uint8_t* data, uint32_t data_len)
{
	result_t result;
	device_t id;
	const uint8_t* current = data;

	lock_global();

	if (data == NULL || data_len == 0)
	{
		log_error( L"updater: wrong arguments passed" );
		return unlocker_global( result_error );
	}

	if ((data_len % XIMC_METALEN_SERVICE_COMMAND_WRITE_DATA_IMPL_DATA) != 0)
	{
		log_error( L"updater: wrong data size, should be a multiplier of 128" );
		return unlocker_global( result_error );
	}

	id = open_device_impl( uri, DEFAULT_TIMEOUT_TIME );
	if (id == device_undefined)
	{
		log_error( L"error opening raw device" );
		return unlocker_global( result_nodevice );
	}

	if (is_debug_logging()) log_debug ( L"UF reset (updf)..." );
	//if ((result = service_command_reset( id )) != result_ok)
	if ((result = service_command_updf( id )) != result_ok)
	{
		log_error( L"updater: can't reset" );
		close_device_impl( &id );
		return unlocker_global( result );
	}
	close_device_impl( &id );

	// TODO I think it is a bug. Controller shouldn't wait longer than RESET_TIME (+3000 ms for Luminary usb driver)
	// With "reset" it's a lot faster
	if (is_debug_logging()) log_debug ( L"UF waiting to reboot..." );
	// increased to 5000 in 14.2
	msec_sleep( (int)(RESET_TIME*1.2 + 5000) );
	if (is_debug_logging()) log_debug ( L"UF wish it had rebooted..." );

	if ((result = update_firmware_reopen_cycled( &id, uri )) != result_ok)
	{
		log_error( L"updater: can't reopen port" );
		if (id != device_undefined)
			close_device_impl( &id );
		return unlocker_global( result );
	}

	if (is_debug_logging()) log_debug ( L"UF provide %d bytes...", data_len );
	for (current = data; current < data + data_len; current += XIMC_METALEN_SERVICE_COMMAND_WRITE_DATA_IMPL_DATA)
	{
		if (is_debug_logging()) log_debug ( L"UF write data bunch..." );
		if ((result = service_command_write_data_impl( id, current )) != result_ok)
		{
			log_error( L"write_data failed" );
			close_device_impl( &id );
			return unlocker_global( result );
		}
	}

	if (is_debug_logging()) log_debug ( L"UF disconnect..." );
	if ((result = service_command_disconnect_impl( id )) != result_ok)
	{
		log_error( L"updater: can't disconnect" );
		close_device_impl( &id );
		return unlocker_global( result );
	}

	if (is_debug_logging()) log_debug ( L"UF done..." );
	close_device_impl( &id );
	return unlocker_global( result_ok );
}

// must not lock
// pre- and post-condition: refer to closed device
result_t write_key_reopen_cycled(device_t* id, const char* name, uint8_t* key)
{
	result_t result;
	int waited_time = 0;
	const int sleep_time = XISM_PORT_DETECT_TIME/10;

	do
	{
		*id = open_device_impl( name, DEFAULT_TIMEOUT_TIME );
		if(*id != device_undefined)
		{
			if (is_debug_logging()) log_debug ( L"try connect..." );

			result = service_command_write_key_impl( *id, key );
			if(result == result_ok)
				return result;

			if (is_debug_logging()) log_debug ( L"reopen port..." );

			if (close_device_impl( id ) != result_ok)
				log_warning( L"write key: error closing port non-fatal" );
		}
		msec_sleep( sleep_time );
		waited_time += sleep_time;
	}
	while (waited_time < XISM_PORT_DETECT_TIME);

	return result_nodevice;
}

result_t XIMC_API write_key (const char* uri, uint8_t* key)
{
	result_t result;
	device_t id;

	lock_global();

	id = open_device_impl( uri, DEFAULT_TIMEOUT_TIME );
	if (id == device_undefined)
	{
		log_error( L"error opening raw device" );
		return unlocker_global( result_nodevice );
	}

	if ((result = service_command_updf( id )) != result_ok)
	{
		log_error( L"can't reset" );
		close_device_impl( &id );
		return unlocker_global( result );
	}
	close_device_impl( &id );

	// TODO I think it is a bug. Controller shouldn't wait longer than RESET_TIME (+3000 ms for Luminary usb driver)
	// With "reset" it's a lot faster
	if (is_debug_logging()) log_debug ( L"waiting to reboot..." );
	msec_sleep( (int)(RESET_TIME*1.2 + 3000) );
	if (is_debug_logging()) log_debug ( L"wish it had rebooted..." );

	if ((result = write_key_reopen_cycled( &id, uri, key )) != result_ok)
		log_error( L"write key: can't reopen port" );

	if(id != device_undefined)
		close_device_impl( &id );

	msec_sleep( (int)(RESET_TIME*1.2 + 3000) );

	return unlocker_global( result );
}

result_t XIMC_API command_reset(device_t id)
{
	lock( id );
	return unlocker( id, service_command_reset_impl( id ) );
}

result_t XIMC_API command_clear_fram(device_t id)
{
	lock( id );
	return unlocker( id, service_command_clear_fram_impl( id ) );
}

result_t XIMC_API goto_firmware(device_t id, uint8_t* ret)
{
	result_t result;
	service_result_t sresult;

	lock( id );
	result = service_command_goto_firmware_impl( id, &sresult );

	if (result == result_ok)
	{
		switch (sresult)
		{
			case RESULT_OK:
				*ret = 1;
				result = result_ok;
				break;
			case RESULT_ALREADY_IN_FIRMWARE:
				*ret = 1;
				result = result_ok;
				break;
			case RESULT_NO_FIRMWARE:
				*ret = 0;
				result = result_ok;
				break;
			default:
				dump_service_result( L"gofw", sresult );
				result = result_error;
		}
	}

	return unlocker( id, result );
}

result_t has_firmware_reopen_cycled(device_t* id, const char* name, uint8_t* ret)
{
	result_t result;
	service_result_t sresult;
	int waited_time = 0;
	const int sleep_time = XISM_PORT_DETECT_TIME/10;

	do
	{
		*id = open_device_impl( name, DEFAULT_TIMEOUT_TIME );
		if(*id != device_undefined)
		{
			if (is_debug_logging()) log_debug ( L"try connect..." );

			result = service_command_has_firmware_impl( *id, &sresult );
			if (result == result_ok)
			{
				switch (sresult)
				{
					case RESULT_HAS_FIRMWARE:
						*ret = 1;
						result = result_ok;
						break;
					case RESULT_NO_FIRMWARE:
						*ret = 0;
						result = result_ok;
						break;
					default:
						dump_service_result( L"hasf", sresult );
						result = result_error;
				}
				return result;
			}

			if (is_debug_logging()) log_debug ( L"reopen port..." );

			if (close_device_impl( id ) != result_ok)
				log_warning( L"write key: error closing port non-fatal" );
		}
		msec_sleep( sleep_time );
		waited_time += sleep_time;
	}
	while (waited_time < XISM_PORT_DETECT_TIME);

	return result_nodevice;
}

result_t XIMC_API has_firmware(const char* uri, uint8_t* ret)
{
	result_t result;
	device_t id;

	if (!ret)
		return result_error;

	lock_global();

	id = open_device_impl( uri, DEFAULT_TIMEOUT_TIME );
	if (id == device_undefined)
	{
		log_error( L"error opening raw device" );
		return unlocker_global( result_nodevice );
	}

	if ((result = service_command_updf( id )) != result_ok)
	{
		log_error( L"can't reset" );
		close_device_impl( &id );
		return unlocker_global( result );
	}
	close_device_impl( &id );

	// wait
	if (is_debug_logging()) log_debug ( L"waiting to reboot..." );
	msec_sleep( (int)(RESET_TIME*1.2 + 3000) );
	if (is_debug_logging()) log_debug ( L"wish it had rebooted..." );

	if ((result = has_firmware_reopen_cycled( &id, uri, ret )) != result_ok)
		log_error( L"has_firmware can't reopen port" );

	if(id != device_undefined)
		close_device_impl( &id );

	// should wait because exterior caller don't know about rebooting
	msec_sleep( (int)(RESET_TIME*1.2 + 3000) );

	return unlocker_global( result );
}

result_t XIMC_API get_device_information (device_t id, device_information_t* device_information)
{
	lock( id );
	return unlocker( id, get_device_information_impl( id, device_information ) );
}

result_t XIMC_API command_wait_for_stop(device_t id, uint32_t refresh_interval_ms)
{
	status_t status;
	do
	{
		result_t result = get_status( id, &status );
		if (result != result_ok)
			return result;
		msec_sleep( refresh_interval_ms );
	}
	while ((status.MvCmdSts & MVCMD_RUNNING) != 0); // check for stop status
	return result_ok;
}

result_t XIMC_API command_homezero(device_t id)
{
	result_t result;
	result = command_home(id);
	if (result != result_ok)
			return result;
	
	result = command_wait_for_stop(id, 10); // 10 milliseconds is a recomended time for most applications
	if (result != result_ok)
			return result;
	
	return command_zero(id); // No matter was it ok or not ok for the last action
}

#if defined(__cplusplus)
};
#endif

// vim: syntax=c tabstop=4 shiftwidth=4
