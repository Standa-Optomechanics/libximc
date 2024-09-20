/* Include files needed to compile DLL */
#include <windows.h>   
#include <cvirte.h>		
#include <userint.h>
#include "simple.h"
#include "ximc.h"
#include <stdio.h>
#include <stdlib.h>


device_t id;

static int panelHandle;

int main (int argc, char *argv[])
{
	char version1[10];
	char device_name[256];
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "simple.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	
  
	ximc_version(version1);
	SetCtrlVal (panelHandle, PANEL_TEXTMSG_2, version1);
	
	const int probe_flags = ENUMERATE_PROBE | ENUMERATE_NETWORK;
	device_enumeration_t devenum;
	const char* enumerate_hints = "addr=";
		
	devenum = enumerate_devices( probe_flags, enumerate_hints );
	
	strcpy( device_name, get_device_name( devenum, 0 ) );
	
	//char* device_name = "xi-com:\\.\COM11";

	id = open_device(device_name); 
	
	get_position_t get_pos;
	get_position(id, &get_pos);
	SetCtrlVal (panelHandle, PANEL_NUMERIC, get_pos.Position);

	if (id>0) 
	{
		RunUserInterface ();
		close_device(id); 
	}
	
	 
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK Button1 (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	//char version1[10];
	int Position1;
	switch (event)
	{
		case EVENT_COMMIT:

			break;
		case EVENT_LEFT_CLICK:

			GetCtrlVal (panelHandle, PANEL_NUMERIC, &Position1);
;   
			command_move(id, Position1, 0); 
			
			break;
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}


 int CVICALLBACK QuitCallback (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	exit(0);
}
