#include "adapters.h"
#include "common.h"
#include "util.h"


#if defined(_WIN32) || defined(WIN64)
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#endif


void allocate_adapter_infos(int number_of_adapters, adapter_info_t** adapter_infos)
{
	adapter_info_t* adapters = (adapter_info_t*)malloc(number_of_adapters * sizeof(adapter_info_t));
	*adapter_infos = adapters;
}


void allocate_memory_for_string_array(int number, int str_length, char*** str_array)
{
	char** p_str_array = (char**)malloc(number * sizeof(char*));
	int i;
	for (i = 0; i < number; i++) {
		p_str_array[i] = (char*)calloc(str_length * sizeof(char), sizeof(char));
	}
	*str_array = p_str_array;
}


#if defined(_WIN32) || defined(WIN64)

int calculate_number_of_adapters(IP_ADAPTER_INFO* adapter_info)
{
	int i = 0;
	IP_ADAPTER_INFO* adapter = adapter_info;
	while (adapter) {
		adapter = adapter->Next;
		i++;
	}
	return i;
}

#else  // for Linux

int calculate_number_of_adapters(struct ifaddrs* adapter_info)
{
	int i = 0;
	struct ifaddrs* adapter = adapter_info;
	while (adapter) {
		if (adapter->ifa_addr && adapter->ifa_addr->sa_family == AF_INET) {
			i++;
		}
		adapter = adapter->ifa_next;
	}
	return i;
}

#endif


int check_address_for_adapter(char* address, adapter_info_t* adapter)
{
	int real_parts_number;

	int address_parts[NUMBER_OF_PARTS_IN_IP_ADDRESS];
	real_parts_number = split_address_into_parts(address, NUMBER_OF_PARTS_IN_IP_ADDRESS, address_parts);
	if (real_parts_number != NUMBER_OF_PARTS_IN_IP_ADDRESS) {
		log_debug(L"IPv4 address '%hs' could not be split into %d numbers", address, NUMBER_OF_PARTS_IN_IP_ADDRESS);
		return 0;
	}

	int adapter_parts[NUMBER_OF_PARTS_IN_IP_ADDRESS];
	real_parts_number = split_address_into_parts(adapter->address, NUMBER_OF_PARTS_IN_IP_ADDRESS, adapter_parts);
	if (real_parts_number != NUMBER_OF_PARTS_IN_IP_ADDRESS) {
		log_debug(L"IPv4 address '%hs' could not be split into %d numbers", adapter->address, NUMBER_OF_PARTS_IN_IP_ADDRESS);
		return 0;
	}

	int mask_parts[NUMBER_OF_PARTS_IN_IP_ADDRESS];
	real_parts_number = split_address_into_parts(adapter->mask, NUMBER_OF_PARTS_IN_IP_ADDRESS, mask_parts);
	if (real_parts_number != NUMBER_OF_PARTS_IN_IP_ADDRESS) {
		log_debug(L"IPv4 address '%hs' could not be split into %d numbers", adapter->mask, NUMBER_OF_PARTS_IN_IP_ADDRESS);
		return 0;
	}

	int i;
	for (i = 0; i < NUMBER_OF_PARTS_IN_IP_ADDRESS; i++) {
		if ((address_parts[i] & mask_parts[i]) != (adapter_parts[i] & mask_parts[i])) {
			return 0;
		}
	}

	return 1;
}


#if defined(_WIN32) || defined(WIN64)

result_t find_adapters(adapters_t* adapters)
{
	IP_ADAPTER_INFO* adapter_info = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (adapter_info == NULL) {
		log_error(L"Error allocating memory needed to call GetAdaptersinfo");
		return result_error;
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	ULONG buf_size = sizeof(IP_ADAPTER_INFO);
	if (GetAdaptersInfo(adapter_info, &buf_size) == ERROR_BUFFER_OVERFLOW) {
		FREE(adapter_info);
		adapter_info = (IP_ADAPTER_INFO*)MALLOC(buf_size);
		if (adapter_info == NULL) {
			log_error(L"Error allocating memory needed to call GetAdaptersinfo");
			return result_error;
		}
	}

	result_t total_result = result_ok;
	DWORD result = 0;
	int number_of_adapters = 0;
	adapter_info_t* adapter_infos = NULL;
	if ((result = GetAdaptersInfo(adapter_info, &buf_size)) == NO_ERROR) {
		number_of_adapters = calculate_number_of_adapters(adapter_info);
		allocate_adapter_infos(number_of_adapters, &adapter_infos);

		int i = 0;
		IP_ADAPTER_INFO* ip_adapter_info = adapter_info;
		while (ip_adapter_info) {
			strcpy(adapter_infos[i].address, ip_adapter_info->IpAddressList.IpAddress.String);
			strcpy(adapter_infos[i].mask, ip_adapter_info->IpAddressList.IpMask.String);
			strcpy(adapter_infos[i].gateway, ip_adapter_info->GatewayList.IpAddress.String);
			ip_adapter_info = ip_adapter_info->Next;
			i++;
		}
	}
	else {
		total_result = result_error;
		log_error(L"GetAdaptersInfo failed with error: %d", result);
	}

	if (adapter_info) {
		FREE(adapter_info);
		adapter_info = NULL;
	}

	adapters->adapter_infos = adapter_infos;
	adapters->number_of_adapters = number_of_adapters;
	adapters->adapter_set = 0;
	return total_result;
}

#else  // for Linux

result_t find_adapters(adapters_t* adapters)
{
	struct ifaddrs* adapter_info;
	getifaddrs(&adapter_info);

	int number_of_adapters = calculate_number_of_adapters(adapter_info);
	adapter_info_t* adapter_infos = NULL;
	allocate_adapter_infos(number_of_adapters, &adapter_infos);

	int i = 0;
	struct sockaddr_in* sock_addr;
	struct ifaddrs* ip_adapter_info;
	for (ip_adapter_info = adapter_info; ip_adapter_info; ip_adapter_info = ip_adapter_info->ifa_next) {
		if (ip_adapter_info->ifa_addr && ip_adapter_info->ifa_addr->sa_family == AF_INET) {
			sock_addr = (struct sockaddr_in*)ip_adapter_info->ifa_addr;
			strcpy(adapter_infos[i].address, inet_ntoa(sock_addr->sin_addr));
			sock_addr = (struct sockaddr_in*)ip_adapter_info->ifa_netmask;
			strcpy(adapter_infos[i].mask, inet_ntoa(sock_addr->sin_addr));
			strcpy(adapter_infos[i].gateway, "");
			i++;
		}
	}

	freeifaddrs(adapter_info);

	adapters->adapter_infos = adapter_infos;
	adapters->number_of_adapters = number_of_adapters;
	adapters->adapter_set = 0;
	return result_ok;
}

#endif


void free_adapters(adapters_t* adapters)
{
	if (adapters && adapters->adapter_infos) {
		free(adapters->adapter_infos);
		adapters->adapter_infos = NULL;
	}
}


void free_memory_for_string_array(int number, char** str_array)
{
	int i;
	for (i = 0; i < number; i++) {
		free(str_array[i]);
		str_array[i] = NULL;
	}
	free(str_array);
	str_array = NULL;
}


adapter_info_t* get_adapter_for_address(char* address, adapters_t* adapters)
{
	int i;
	for (i = 0; i < adapters->number_of_adapters; i++) {
		adapter_info_t* adapter_info = &adapters->adapter_infos[i];
		if (check_address_for_adapter(address, adapter_info)) {
			return adapter_info;
		}
	}

	/* The address does not fall into the mask of any adapter.
	Therefore, the address is in the external network.
	You need to select an adapter with the main gateway */
	for (i = 0; i < adapters->number_of_adapters; i++) {
		adapter_info_t* adapter_info = &adapters->adapter_infos[i];
		if (strlen(adapter_info->gateway) > 0 && strcmp(adapter_info->gateway, "0.0.0.0") != 0) {
			return adapter_info;
		}
	}

	return NULL;
}


char* get_substr(char* str, size_t pos, size_t count)
{
	static char buffer[BUFSIZ];
	memset(buffer, 0, BUFSIZ);
	if (count >= BUFSIZ) {
		log_error(L"Failed to get substring of size %lu from string '%hs': substring size is greater than BUFSIZ = %d", count, str + pos, BUFSIZ);
		return NULL;
	}

	return strncpy(buffer, str + pos, count);
}


void print_adapters(adapters_t* adapters)
{
	if (adapters == NULL) {
		log_info(L"Adapters not found");
		return;
	}

	log_info(L"%d adapters found:", adapters->number_of_adapters);
	int i;
	for (i = 0; i < adapters->number_of_adapters; i++) {
		adapter_info_t* adapter_info = &(adapters->adapter_infos[i]);
		log_info(L"#%d. Adapter IP address: '%hs', mask: '%hs', gateway: '%hs'", i + 1, adapter_info->address, adapter_info->mask, adapter_info->gateway);
	}
}


void set_adapters(int number_of_addresses, char** addresses, adapters_t* adapters)
{
	if (number_of_addresses == 0 || !addresses) {
		return;
	}

	adapter_info_t* adapter_infos = NULL;
	allocate_adapter_infos(number_of_addresses, &adapter_infos);

	int i;
	for (i = 0; i < number_of_addresses; i++) {
		strcpy(adapter_infos[i].address, addresses[i]);
		strcpy(adapter_infos[i].mask, "");
		strcpy(adapter_infos[i].gateway, "");
	}	

	adapters->adapter_infos = adapter_infos;
	adapters->number_of_adapters = number_of_addresses;
	adapters->adapter_set = 1;
}


int split_address_into_parts(char* address, int number_of_parts, int* parts)
{
	char* p_address = address;
	int i = 0;
	while (p_address && i < number_of_parts) {
		char* pos = strstr(p_address, ".");
		char* part;
		if (pos) {
			part = get_substr(p_address, 0, pos - p_address);
			pos++;
		}
		else {
			part = p_address;
		}
		parts[i] = atoi(part);
		p_address = pos;
		i++;
	}

	return i;
}


void split_string_into_parts(char* str, const char* delimiter, int* number_of_parts, char*** parts)
{
	int number = 0;
	char* ptr = str;
	while (ptr != NULL) {
		char* found_ptr = strstr(ptr, delimiter);
		size_t length = 0;
		if (found_ptr != NULL) {
			found_ptr++;
			length = found_ptr - ptr - 1;
		}
		else {
			length = strlen(ptr);
		}

		if (length > 0)
			number++;

		ptr = found_ptr;
	}

	*number_of_parts = number;
	if (number == 0) {
		return;
	}

	allocate_memory_for_string_array(number, ADDRESS_LENGTH, parts);
	char* token = strtok(str, delimiter);
	int i = 0, j = 0;
	while (i < number) {
		if (token != NULL) {
			memcpy((*parts)[j], token, strlen(token));
			j++;
		}
		token = strtok(NULL, delimiter);
		i++;
	}
}
