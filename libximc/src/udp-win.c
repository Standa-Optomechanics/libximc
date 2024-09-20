#include "common.h"

#include <winioctl.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#include "ximc.h"

#include "util.h"
#include "metadata.h"
#include "platform.h"

#include "protosup.h"

// uses device_meatada_t virtual_XXX fields to save some udp-connection params
// virtual_state - allocated for socket_in data of a controller-server
// virtual_scratchpad - all accepted data
// virtual_packet_actual - unread data size

#define PUDP_SOCKET_IN metadata -> virtual_state

#define UDP_BUFFER_LEN VIRTUAL_SCRATCHPAD_SIZE

#define UDP_BUFFER metadata ->	virtual_scratchpad

#define UDP_UNREAD_LEN metadata -> virtual_packet_actual

result_t open_udp(device_metadata_t *metadata, const char* ip4_port)
{
	
	// check parameter ip4_port : address:port
	char saddress[64];
	memset(saddress, 0, 64);
	strncpy_s(saddress, 64, ip4_port, strlen(ip4_port));
	char * port_start = strchr(saddress, ':');
	if (port_start == NULL)
	{
		port_start = strchr(saddress, 0);
		portable_snprintf(port_start, 64 - strlen(saddress), ":%u", XIMC_UDP_PORT);
	}
	unsigned int port;
	if (sscanf_s(port_start + 1, "%u", &port) != 1) return result_error;
	*port_start = 0;
	ULONG addr = inet_addr(saddress);
	if (addr == INADDR_NONE) return result_error;

	// init actions to init some windows dll with udp from Microsoft example  
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		//printf("WSAStartup failed with error: %d\n", err);
		return result_error;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		//printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return result_error;
	}


	// creating a new connection resource

	metadata->handle = (handle_t)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if ((SOCKET)metadata->handle == INVALID_SOCKET) return result_error;

	// устанавлмваем опцию сокета
	int OptVal = 1;
	int OptLen = sizeof(int);

	DWORD timeout = metadata->timeout;
    int rez = setsockopt((SOCKET)metadata->handle, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    if (rez != SOCKET_ERROR)
    {
        rez = setsockopt((SOCKET)metadata->handle, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    }
    if (rez == SOCKET_ERROR)
    {
        closesocket((SOCKET)metadata->handle);
        return result_error;
    }

	metadata->type = dtUdp;

	SOCKADDR_IN * sa;

	PUDP_SOCKET_IN = (sa = (SOCKADDR_IN *)malloc(sizeof(SOCKADDR_IN)));

	sa->sin_family = AF_INET;
	sa->sin_port = htons((USHORT)(port));
	sa->sin_addr.s_addr = addr;

	UDP_UNREAD_LEN = 0;
	return result_ok;
}

result_t close_udp(device_metadata_t *metadata)
{
	closesocket((SOCKET)metadata->handle);
	free(PUDP_SOCKET_IN);
	// free some windows dll
	WSACleanup();
	UDP_UNREAD_LEN = 0;
	return result_ok;
}


ssize_t write_udp(device_metadata_t *metadata, const byte* command, size_t command_len)
{
	int iResult;
	iResult = sendto((SOCKET)metadata->handle, (const char *)command, (int)command_len, 0, (SOCKADDR *)PUDP_SOCKET_IN, (int) sizeof (SOCKADDR_IN));
	return  (iResult == SOCKET_ERROR) ? -1 : iResult;
}

// assume amount is the required number of data bytes - non max buffer size
ssize_t read_udp(device_metadata_t *metadata, void *buf, size_t amount)
{
	if (UDP_UNREAD_LEN > 0)
	{
		if ((size_t)UDP_UNREAD_LEN >= amount)
		{
			memcpy(buf, UDP_BUFFER, amount);
			memmove(UDP_BUFFER, UDP_BUFFER + amount, (UDP_UNREAD_LEN = UDP_UNREAD_LEN - amount));
			return amount;
		}
	}
	// this is blocking udp reading (recvfrom); if nothing to receive - will hang
	int real_len = recvfrom((SOCKET)metadata->handle, (char *)(UDP_BUFFER + UDP_UNREAD_LEN), (int)(UDP_BUFFER_LEN - UDP_UNREAD_LEN), 0, NULL, NULL);
	if (real_len == SOCKET_ERROR) return -1;
	if (amount <= (size_t)(real_len + UDP_UNREAD_LEN)) // calling context wants too little of bytes
	{
		memcpy(buf, UDP_BUFFER, amount);
		// rewrite the rest of buffer data to the buffer array start  
		memmove(UDP_BUFFER, UDP_BUFFER + amount, (UDP_UNREAD_LEN = real_len + UDP_UNREAD_LEN - amount));
		return amount;
	}
	else // calling context wants too match - unread bytes plus just real read ones will be written and their amount will be return
	{
		memcpy(buf, UDP_BUFFER, real_len + UDP_UNREAD_LEN);
		UDP_UNREAD_LEN = 0;
		return real_len + UDP_UNREAD_LEN;
	}
}