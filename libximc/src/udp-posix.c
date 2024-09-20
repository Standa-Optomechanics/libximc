#include "common.h"

#include <stdio.h>	
#include <string.h>    
#include <stdlib.h>    
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifdef HAVE_LOCKS
#include <semaphore.h>
#endif

#include "ximc.h"
#include "util.h"
#include "metadata.h"
#include "platform.h"

#include "protosup.h"
#include "wrapper.h"

#ifdef STRERROR_R_CHAR_P
#error GNU variant of strerror_r is not supported
#endif

#ifdef __APPLE__
/* We need IOKit */
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>
#include <sys/sysctl.h>
/* and time too */
#include <mach/clock.h>
#include <mach/mach.h>
#endif


/*
 * Udp support
 */
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
    strncpy(saddress, ip4_port, strlen(ip4_port));
    char * port_start = strchr(saddress, ':');
	if (port_start == NULL)
	{
		port_start = strchr(saddress, 0);
		portable_snprintf(port_start, 64 - strlen(saddress), ":%u", XIMC_UDP_PORT);
	}
    unsigned int port;
    if (sscanf(port_start + 1, "%u", &port) != 1) return result_error;
    *port_start = 0;
    in_addr_t addr = inet_addr(saddress);
    if (addr == INADDR_NONE) return result_error;

    // creating a new connection resource

    metadata->handle = (uint32_t)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ((int)metadata->handle == -1) return result_error;

    struct timeval timeout;
    timeout.tv_sec = metadata->timeout / 1000;            // second part of timeout (which ordinary in milliseconds)
    timeout.tv_usec = (metadata->timeout % 1000) * 1000;  // millisecond part of timeout in microseconds
    int result = setsockopt((int)metadata->handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    if (result != -1)
    {
        result = setsockopt((int)metadata->handle, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (struct timeval));
    }
    if (result == -1)
    {
        close((int)metadata->handle);
        return result_error;
    }

    metadata->type = dtUdp;

    struct sockaddr_in * sa;

    PUDP_SOCKET_IN = (sa = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in)));

    sa->sin_family = AF_INET;
    sa->sin_port = htons((port));
    sa->sin_addr.s_addr = addr;
    UDP_UNREAD_LEN = 0;
    return result_ok;
}

result_t close_udp(device_metadata_t *metadata)
{
	close((int)metadata->handle);
	free(PUDP_SOCKET_IN);
	UDP_UNREAD_LEN = 0;
	return result_ok;
}


ssize_t write_udp(device_metadata_t *metadata, const byte* command, size_t command_len)
{
	return (ssize_t)sendto((int)metadata->handle, (const char *)command, (int)command_len, 0, (struct sockaddr *)metadata->virtual_state, (socklen_t) sizeof (struct sockaddr_in));
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
	int real_len = (int)recvfrom((int)metadata->handle, (char *)(UDP_BUFFER + UDP_UNREAD_LEN), UDP_BUFFER_LEN - UDP_UNREAD_LEN, 0, NULL, NULL);
	if (real_len == -1) return -1;
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
