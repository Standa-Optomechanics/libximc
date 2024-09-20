//==============================================================================
//
// Title:		test1
// Purpose:		A short description of the command-line tool.
//
// Created on:	29.11.2020 at 22:43:09 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

//#include <ansi_c.h>
							/* Include files needed to compile DLL */
#include <windows.h>   
//#include <cvirte.h>		
//#include <userint.h>
#include <stdio.h>
#include "ximc.h"


int main (int argc, char *argv[])
{
	char version1[10];
	char device_name[256];
	
	printf("Started\n");
	
	//if (InitCVIRTE (0, argv, 0) == 0)
	//	return -1;
	
	ximc_version(version1);
	
	printf("Version %s\n", version1);	
	
	const int probe_flags = ENUMERATE_PROBE | ENUMERATE_NETWORK;
	device_enumeration_t devenum;
	const char* enumerate_hints = "addr=";
	
	devenum = enumerate_devices( probe_flags, enumerate_hints );
	
	strcpy( device_name, get_device_name( devenum, 0 ) );
	
	char *name = "xi-com:\\.\COM15";

	printf("Would open %s\n", name);

	device_t id = open_device(device_name); 
	printf("Handle id=%d\n", id);
	if (id<=0) {
		return -1;
	}
	Sleep(3000);
	
	printf("Moving\n");    
	command_movr(id, 1000, 0);
	command_wait_for_stop(id, 100);
			

	printf("Closing\n");
	close_device(&id);
	
	Sleep(5000);
	
	return 0;
}

