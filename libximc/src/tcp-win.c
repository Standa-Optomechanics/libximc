#include "common.h"

#include <winioctl.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <ws2def.h>

#include "ximc.h"

#include "util.h"
#include "metadata.h"
#include "platform.h"

#include "protosup.h"

// uses device_meatada_t virtual_XXX fields to save some TCP-connection params
// virtual_state - allocated for socket_in data of a controller-server
// virtual_scratchpad - all accepted data
// virtual_packet_actual - unread data size

#define PTCP_SOCKET_IN metadata -> virtual_state

#define TCP_BUFFER_LEN VIRTUAL_SCRATCHPAD_SIZE

#define TCP_BUFFER metadata ->	virtual_scratchpad

#define TCP_UNREAD_LEN metadata -> virtual_packet_actual

result_t open_tcp(device_metadata_t *metadata, const char* ip4_port)
{
	
	// check parameter ip4_port : address:port
	char saddress[64];
	int optval = 1; // set option =true
	memset(saddress, 0, 64);
	strncpy_s(saddress, 64, ip4_port, strlen(ip4_port));
	char * port_start = strchr(saddress, ':');
	if (port_start == NULL)
	{
		port_start = strchr(saddress, 0);
		portable_snprintf(port_start, 64 - strlen(saddress), ":%u", XIMC_TCP_PORT);
	}
	unsigned int port;
	if (sscanf_s(port_start + 1, "%u", &port) != 1) return result_error;
	*port_start = 0;
	ULONG addr = inet_addr(saddress);
	if (addr == INADDR_NONE) return result_error;

	// init actions to init some windows dll with tcp from Microsoft example  
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) 
	{
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

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		//printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return result_error;
	}

	
	// creating a new connection resource

	metadata->handle = (handle_t)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ((SOCKET)metadata->handle == INVALID_SOCKET)
	{
		return result_error;
	}
	
	SOCKADDR_IN *sa;
	PTCP_SOCKET_IN = (sa = (SOCKADDR_IN *)malloc(sizeof(SOCKADDR_IN)));
	sa->sin_family = AF_INET;
	sa->sin_port = htons((USHORT)(port));
	sa->sin_addr.s_addr = addr;
	
	int result  = connect((SOCKET)metadata->handle, (const SOCKADDR *)sa, sizeof(SOCKADDR_IN));
	if (result != SOCKET_ERROR)
	{
		DWORD timeout = metadata->timeout;
		result = setsockopt((SOCKET)metadata->handle, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
		if (result != SOCKET_ERROR)
		{
			result = setsockopt((SOCKET)metadata->handle, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		}
		if (result != SOCKET_ERROR)
		{
			result = setsockopt((SOCKET)metadata->handle, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(int));
		}
	}
	if (result == SOCKET_ERROR)
	{
		closesocket((SOCKET)metadata->handle);
		free(PTCP_SOCKET_IN);
		WSACleanup();
		return result_error;
	}
	
	metadata->type = dtTcp;
	TCP_UNREAD_LEN = 0;
	return result_ok;
}

result_t close_tcp(device_metadata_t *metadata)
{
	closesocket((SOCKET)metadata->handle);
	free(PTCP_SOCKET_IN);
	// free some windows dll
	WSACleanup();
	TCP_UNREAD_LEN = 0;
	return result_ok;
}


ssize_t write_tcp(device_metadata_t *metadata, const byte* command, size_t command_len)
{
	TCP_UNREAD_LEN = 0;
	int iResult;
	iResult = send((SOCKET)metadata->handle, (const char *)command, (int)command_len, 0);
	return  (iResult == SOCKET_ERROR) ? -1 : iResult;
}

// assume amount is the required number of data bytes - non max buffer size
ssize_t read_tcp(device_metadata_t *metadata, void *buf, size_t amount)
{
	if (TCP_UNREAD_LEN > 0)
	{
		if ((size_t)TCP_UNREAD_LEN >= amount)
		{
			memcpy(buf, TCP_BUFFER, amount);
			memmove(TCP_BUFFER, TCP_BUFFER + amount, (TCP_UNREAD_LEN = TCP_UNREAD_LEN - amount));
			return amount;
		}
		
	}
	// this is blocking tcp reading (recvfrom); if nothing to receive - will hang
	int real_len = recv((SOCKET)metadata->handle, (char *)(TCP_BUFFER + TCP_UNREAD_LEN), (int)(TCP_BUFFER_LEN - TCP_UNREAD_LEN), 0);
	if (real_len == SOCKET_ERROR) return -1;
	if (amount <= (size_t)(real_len + TCP_UNREAD_LEN)) // calling context wants too little of bytes
	{
		memcpy(buf, TCP_BUFFER, amount);
		// rewrite the rest of buffer data to the buffer array start  
		memmove(TCP_BUFFER, TCP_BUFFER + amount, (TCP_UNREAD_LEN = real_len + TCP_UNREAD_LEN - amount));
		return amount;
	}
	else // calling context wants too match - unread bytes plus just real read ones will be written and their amount will be return
	{
		memcpy(buf, TCP_BUFFER, real_len + TCP_UNREAD_LEN);
		TCP_UNREAD_LEN = 0;
		return real_len + TCP_UNREAD_LEN;
	}

}