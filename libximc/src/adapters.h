#ifndef ADAPTER_H
#define ADAPTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN64)
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#else
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#endif

#include "ximc.h"


#define ADDRESS_LENGTH 100
#define IP_ADDRESS_SIZE 16
#define MAX_NAME_LENGTH 4096
#define NUMBER_OF_PARTS_IN_IP_ADDRESS 4


/* Data types used */

/**
 * Structure for storing data about the network adapter.
 */
typedef struct {
	char address[IP_ADDRESS_SIZE];  // adapter IP address
	char mask[IP_ADDRESS_SIZE];     // adapter mask
	char gateway[IP_ADDRESS_SIZE];  // default gateway
} adapter_info_t;

/**
 * Structure for storing an array of data about network adapters.
 */
typedef struct {
	adapter_info_t* adapter_infos;  // pointer to an array with information about adapters
	int number_of_adapters;         // number of adapters in the array
	int adapter_set;                // if 1, then the adapter is specified by the user, otherwise adapters are found
} adapters_t;


/* Function declaration */

/**
 * The function allocates memory for an array of structures with information about adapters.
 * @param[in] number_of_adapters Number of elements in the array.
 * @param[out] adapter_infos Pointer to array.
 */
void allocate_adapter_infos(int number_of_adapters, adapter_info_t** adapter_infos);

/**
* The function allocates memory for an array of strings.
* @param[in] number Number of elements in the array.
* @param[in] str_size Length of strings in array.
* @param[out] str_array Pointer to an array of strings.
*/
void allocate_memory_for_string_array(int number, int str_length, char*** str_array);

/**
 * The function calculates the number of adapters found.
 * \return Number of adapters found.
 */
#if defined(_WIN32) || defined(WIN64)
int calculate_number_of_adapters(IP_ADAPTER_INFO* adapter_info);
#else  // for Linux
int calculate_number_of_adapters(struct ifaddrs* adapter_info);
#endif

/**
 * The function checks whether the IP address and adapter are in the same network.
 * @param[in] address IP address.
 * @param[in] adapter Pointer to a structure with adapter data.
 * \return 1 if the address matches the adapter, otherwise 0.
 */
int check_address_for_adapter(char* address, adapter_info_t* adapter);

/**
 * The function finds network adapters.
 * @param[out] adapters Pointer to a structure with an array of data about the adapters found.
 */
result_t find_adapters(adapters_t* adapters);

/**
 * The function frees the memory allocated for the array of structures with adapter data.
 * @param[in] adapters Pointer to a structure with an array of adapter data.
 */
void free_adapters(adapters_t* adapters);

/**
* The function frees the memory allocated for the string array.
* @param[in] number Number of elements in the array.
* @param[in] str_array Pointer to an array of strings.
*/
void free_memory_for_string_array(int number, char** str_array);

/**
 * The function determines which adapter the IP address corresponds to.
 * @param[in] address IP address.
 * @param[in] adapters Pointer to a structure with an array of adapter data.
 * \return Pointer to a structure containing data for the adapter that is on the same network as the IP address.
 */
adapter_info_t* get_adapter_for_address(char* address, adapters_t* adapters);

/**
 * The function returns a substring of the given string.
 * @param[in] str The string from which to return a substring.
 * @param[in] pos Start position of substring in string.
 * @param[in] count Number of characters in a substring.
 * \return Substring. If such a substring cannot be obtained, NULL is returned.
 */
char* get_substr(char* str, size_t pos, size_t count);

/**
 * The function displays information about adapters.
 * @param[in] adapters Pointer to a structure with an array of adapter data.
 */
void print_adapters(adapters_t* adapters);

/**
 * The function saves the IP addresses as the adapter addresses.
 * @param[in] number_of_addresses The number of addresses that will need to be saved as adapter addresses.
 * @param[in] addresses IP address array.
 * @param[out] adapters Pointer to a structure with an array of adapter data. IP address will be stored in this array.
 */
void set_adapters(int number_of_addresses, char** addresses, adapters_t* adapters);

/**
 * The function splits the IP address by dot into numbers and writes these numbers into an array.
 * @param[in] address IP address.
 * @param[in] number_of_parts The number of elements in the array in which the numbers from the IP address are stored.
 * @param[out] parts An array in which the numbers from the IP address will be saved.
 * \return The number of numbers into which the IP address is divided by a dot.
 */
int split_address_into_parts(char* address, int number_of_parts, int* parts);

/**
 * The function splits a string into parts by a given delimiter and saves them in an array.
 * @param[in] str The string to be split.
 * @param[in] delimiter The separator by which to split the string.
 * @param[out] number_of_parts Array size.
 * @param[out] parts The array in which the substrings will be stored.
 */
void split_string_into_parts(char* str, const char* delimiter, int* number_of_parts, char*** parts);

#endif // !ADAPTER_H
