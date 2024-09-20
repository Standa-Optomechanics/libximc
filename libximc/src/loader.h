#ifndef INC_LOADER_H
#define INC_LOADER_H

#if defined(__cplusplus)
extern "C" {
#endif


typedef enum
{
	RESULT_OK					= 0x00,
	RESULT_ERROR				= 0x01,
	RESULT_FLASH_ERROR			= 0x02,
	RESULT_HAS_FIRMWARE			= 0x04,
	RESULT_NO_FIRMWARE			= 0x05,
	RESULT_ALREADY_IN_FIRMWARE	= 0x06,
	RESULT_SOFT_ERROR			= 0xE1,
	RESULT_HARD_ERROR			= 0xE2,
} service_result_t;

void dump_service_result (const wchar_t* prefix, service_result_t sresult);

#if defined(__cplusplus)
};
#endif


#endif

// vim: syntax=c tabstop=4 shiftwidth=4
