#ifndef DEVENUM_H
#define DEVENUM_H

#include "common.h"

#include "ximc.h"
#include "util.h"
#include "loader.h"
#include "ximc-gen.h"
#include "metadata.h"
#include "platform.h"
#include "protosup.h"
#include "adapters.h"
#ifdef HAVE_XIWRAPPER
#include "wrapper.h"
#endif
#include <errno.h>

#if defined(WIN32) || defined(WIN64)
#include <io.h>
#include <iphlpapi.h>
#include <winioctl.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <ws2def.h>
#include <ws2def.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#ifdef __APPLE__
/* We need IOKit */
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>
#include <sys/sysctl.h>
#endif
#define MINIUPNP_STATICLIB
#include <miniupnpc/miniupnpc.h>


#define SSDP_DELAY_MS 2000
#define XINET_TIMEOUT_MS 2000
#define CHECK_DEVICE_TIMEOUT_MS 1000


/* Data types used */

/**
 * The structure that is used to determine the XIMC compatibility of a device in a separate thread.
 */
typedef struct enum_thread_state_t
{
	char* name;                         // URI of the device to connect to
	uint32_t serial;                    // serial number
	device_information_t* info;         //
	controller_name_t* controller_name; //
	stage_name_t* stage_name;           //
	int status;                         // 1 if the device is XIMC compatible, otherwise 0
	char* thread_expired;               // 1 if the time to determine device compatibility has expired, otherwise 0
} enum_thread_state_t;

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

PACK(
struct my_device
{
	char Manufacturer[4];
	char ManufacturerId[2];
	char ProductDescription[8];
	uint8_t Major;
	uint8_t Minor;
	uint16_t Release;
	uint8_t reserved[12];
});
typedef struct my_device my_device_information_t;

PACK(
struct my_cname
{
	char ControllerName[16];
	uint8_t CtrlFlags;
	uint8_t reserved[7];
});
typedef struct my_cname my_controller_name_t;

PACK(
struct my_sname
{
	char PositionerName[16];
	uint8_t reserved[8];
});
typedef struct my_sname my_stage_name_t;

PACK(
struct device_desc {
	uint32_t serial;
	my_device_information_t my_device_information;
	my_controller_name_t my_cname;
	my_stage_name_t my_sname;
	uint32_t ipv4; // passed in network byte order
	uint32_t reserved1;
	char nodename[16];
	uint32_t axis_state;
	char locker_username[16];
	char locker_nodename[16];
	uint64_t locked_time;
	uint8_t reserved2[30];
});
typedef struct device_desc device_description;

/**
 * The structure used when searching for devices via SSDP in separate threads for each adapter.
 */
typedef struct {
	adapter_info_t* adapter_info; // information about the adapter on which devices will be searched using SSDP
	int allocated_count;          // memory allocated for the given number of device names
	int count;                    // number of devices found
	char** device_names;          // names of found devices
} devices_enumerated_by_ssdp_t;

/**
 * Structure for storing an array of pointers to structure devices_enumerated_by_ssdp_t.
 */
typedef struct {
	int count;                                  // number of elements in the array
	devices_enumerated_by_ssdp_t* ssdp_devices; // pointer to array
} array_of_devices_enumerated_by_ssdp_t;

/**
 * The structure used to find devices using xinet when polling xinet servers in separate threads.
 */
typedef struct {
	adapter_info_t* adapter_info;			// information about the adapter on which devices will be searched using xinet
	char server_address[ADDRESS_LENGTH];	// the address of the xinet server that is queried to find devices
	int count;                           	// number of devices found
	int status;                          	//
	uint8_t** pbuf;                      	// pointer to a buffer that stores data about detected devices
	char* thread_expired;                	// 1 if the time to poll xinet servers has expired, otherwise 0
} devices_enumerated_by_xinet_t;

/**
 * Structure for storing an array of pointers to structure devices_enumerated_by_xinet_t.
 */
typedef struct {
	int count;                                    // number of elements in the array
	devices_enumerated_by_xinet_t* xinet_devices; // pointer to array
	mutex_t* mutex;                               // mutex
} array_of_devices_enumerated_by_xinet_t;

/**
 * The structure with adapter and array of servers found on this adapter using Revealer-1 protocol.
 */
typedef struct {
	adapter_info_t* adapter_info;  // information about the adapter on which server was found using the Revealer-1 protocol
	Revealer1Addresses* addresses; // pointer to a structure with an array of xinet servers
} servers_enumerated_by_revealer_1_t;

/**
* The structure with adapter and xinet server address found on this adapter using Revealer-1 protocol.
*/
typedef struct {
	adapter_info_t* adapter_info;  // information about the adapter on which server was found using the Revealer-1 protocol
	char address[IP_ADDRESS_SIZE]; // xinet server address
} xinet_server_t;

/**
 * The structure for storing an array of xinet servers.
 */
typedef struct {
	int allocated_count;     // the number of array elements for which memory is allocated
	int count;               // number of xinet servers in the array
	xinet_server_t* servers; // pointer to array of xinet servers
} xinet_servers_t;


/* Exported function declaration */

device_enumeration_t XIMC_API enumerate_devices(int enumerate_flags, const char* hints);
result_t XIMC_API free_enumerate_devices(device_enumeration_t device_enumeration);
int XIMC_API get_device_count(device_enumeration_t device_enumeration);
pchar XIMC_API get_device_name(device_enumeration_t device_enumeration, int device_index);
result_t XIMC_API get_enumerate_device_controller_name(device_enumeration_t device_enumeration, int device_index, controller_name_t* controller_name);
result_t XIMC_API get_enumerate_device_information(device_enumeration_t device_enumeration, int device_index, device_information_t* device_information);
result_t XIMC_API get_enumerate_device_network_information(device_enumeration_t device_enumeration, int device_index, device_network_information_t* device_network_information);
result_t XIMC_API get_enumerate_device_serial(device_enumeration_t device_enumeration, int device_index, uint32_t* serial);
result_t XIMC_API get_enumerate_device_stage_name(device_enumeration_t device_enumeration, int device_index, stage_name_t* stage_name);
result_t XIMC_API probe_device(const char* uri);


/* Function declaration */

/**
 * The function adds the device name to the structure storing the names of devices found using SSDP on the specified adapter.
 * @param[in] device_name The name of the device to be added.
 * @param[in] ssdp_devices Pointer to the structure to which the device name should be added.
 */
void add_device_to_ssdp_devices_struct(char* device_name, devices_enumerated_by_ssdp_t* ssdp_devices);

/**
 * The function adds a xinet server to the array of xinet servers.
 * @param[in] server_address IP address of xinet server.
 * @param[in] adapter_info Pointer to a structure with information about the adapter that the xinet server corresponds to.
 * @param[in] Pointer to a structure with an array of xinet servers
 */
void add_server_to_xinet_servers_struct(char* server_address, adapter_info_t* adapter_info, xinet_servers_t* xinet_servers);

/**
 * The function allocates memory for the dev_enum structure.
 * @param[out] dev_enum Pointer to structure.
 * \return result_ok if memory is allocated, otherwise result_error.
 */
result_t allocate_memory_for_dev_enum(device_enumeration_opaque_t** dev_enum);

/**
 * The function allocates memory for an array of structures servers_enumerated_by_revealer_1_t.
 * @param[in] number Number of elements in the array.
 * @param[out] revealer_1_servers Pointer to array.
 * \return result_ok if memory is allocated, otherwise result_error.
 */
result_t allocate_memory_for_revealer_1_servers(int number, servers_enumerated_by_revealer_1_t** revealer_1_servers);

/**
 * The function allocates memory for an array of structures that will be used to search for devices using SSDP.
 * @param[in] number Number of elements in the array.
 * @param[in] ssdp_devices Pointer to array.
 * \return result_ok if memory is allocated, otherwise result_error.
 */
result_t allocate_memory_for_ssdp_devices(int number, devices_enumerated_by_ssdp_t** ssdp_devices);

/**
 * The function allocates memory for an array of structures that will be used to search for devices using xinet.
 * @param[in] number Number of elements in the array.
 * @param[out] xinet_devices Pointer to array.
 * \return result_ok if memory is allocated, otherwise result_error.
 */
result_t allocate_memory_for_xinet_devices(int number, devices_enumerated_by_xinet_t** xinet_devices);

/**
 * The function allocates memory for a structure containing an array of xinet servers.
 * @param[out] xinet_servers Pointer to a structure with an array of xinet servers.
 * \return result_ok if memory is allocated, otherwise result_error.
 */
result_t allocate_memory_for_xinet_servers(xinet_servers_t** xinet_servers);

/**
 * The function checks that the device is XIMC-compatible. The manufacturer name is used.
 * @param[in] name Device name.
 * @param[out] info Pointer to a structure in which information about the device will be stored.
 * @param[out] serial Pointer to where the device serial number will be saved.
 * @param[out] controller_name Pointer to where the controller name will be saved.
 * @param[out] stage_name Pointer to where the custom device name will be saved.
 * \return 1 if the device is XIMC-compatible, otherwise 0.
 */
int check_device_by_ximc_information(const char* name, device_information_t* info, uint32_t* serial, controller_name_t* controller_name, stage_name_t* stage_name);

/*
 * The function checks in the thread that the device is XIMC-compatible.
 * @param[in] arg Pointer to a structure with the device name. This structure will store the manufacturer name,
 * serial number, and other information for XIMC-compatible devices.
 */
void check_device_in_thread(void* arg);

/**
 * The function creates an array of 'devices_enumerated_by_ssdp_t' structures that is used to search for devices using SSDP on individual adapters.
 * @param[in] adapters Pointer to a structure with an array of data about adapters on which to search for devices.
 * @param[out] array_of_ssdp_devices Pointer to an array of structures.
 */
void create_array_of_devices_enumerated_by_ssdp(adapters_t* adapters, array_of_devices_enumerated_by_ssdp_t** array_of_ssdp_devices);

/**
 * The function creates an array of 'devices_enumerated_by_xinet_t' structures that is used to search for devices using xinet on individual xinet servers.
 * @param[in] xinet_servers Pointer to a structure with an array of xinet servers.
 * @param[out] array_of_xinet_devices Pointer to an array of structures.
 */
void create_array_of_devices_enumerated_by_xinet(xinet_servers_t* xinet_servers, array_of_devices_enumerated_by_xinet_t** array_of_xinet_devices);

/**
 * The function deletes an array of 'devices_enumerated_by_ssdp_t' structures.
 * @param[in] array_of_ssdp_devices Pointer to an array of structures.
 */
void delete_array_of_devices_enumerated_by_ssdp(array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices);

/**
 * The function deletes an array of 'devices_enumerated_by_xinet_t' structures.
 * @param[in] array_of_xinet_devices Pointer to an array of structures.
 */
void delete_array_of_devices_enumerated_by_xinet(array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices);

/**
 * The function determines which adapters should be used to search for devices.
 * @param[in] hints The 'hints' argument to the 'enumerate_devices' function. This string may contain the
 * 'addapter_addr' key with the specific address of the adapter to search for.
 * @param[out] adapters Pointer to a structure with an array of data about adapters on which to search for devices.
 */
void determine_adapters(const char* hints, adapters_t* adapters);

/**
 * The function searches for xinet servers using the Revealer-1 protocol on the specified adapter.
 * @param[in] arg Pointer to a structure where found xinet servers will be written.
 */
void discover_using_revealer_1_protocol(void* arg);

/**
 * The function searches for devices using SSDP for the specified adapter.
 * @param[in] arg Pointer to the structure in which the names of found devices will be stored.
 */
void discover_using_ssdp_and_add_as_tcp(void* arg);

/**
 * The function searches for devices using xinet for the specified adapter.
 * @param[in] arg Pointer to the structure in which the names of found devices will be stored.
 */
void discover_using_xinet(void* arg);

/**
 * Enumerate devices main function.
 * @param[out] device_enumeration Pointer to a structure in which found devices will be stored.
 * @param[in] enumerate_flags Flags for searching devices.
 * @param[in] hints String with keys and values. The string must be of the form 'key1=value1_1,value1_2, \n key2=value2_1,value2_2'.
 * \return 
 */
result_t enumerate_devices_impl(device_enumeration_opaque_t** device_enumeration, int enumerate_flags, const char* hints);

/**
 * The function frees the memory allocated for the array of structures servers_enumerated_by_revealer_1_t.
 * @param[in] number Number of elements in the array.
 * @param[out] revealer_1_servers Pointer to array.
 */
void free_memory_for_revealer_1_servers(int number, servers_enumerated_by_revealer_1_t* revealer_1_servers);

/**
 * The function frees the memory allocated for the array of structures for searching devices using SSDP.
 * @param[in] number Number of elements in the array.
 * @param[in] ssdp_devices Pointer to array.
 */
void free_memory_for_ssdp_devices(int number, devices_enumerated_by_ssdp_t* ssdp_devices);

/**
 * The function frees the memory allocated for the array of structures for searching devices using xinet.
 * @param[in] number Number of elements in the array.
 * @param[out] xinet_devices Pointer to array.
 */
void free_memory_for_xinet_devices(int number, devices_enumerated_by_xinet_t* xinet_devices);

/**
 * The function frees the memory allocated for a structure containing an array of xinet servers.
 * @param[in] xinet_servers Pointer to a structure with an array of xinet servers.
 */
void free_memory_for_xinet_servers(xinet_servers_t* xinet_servers);

/**
 * The function returns the address from which the prefix has been removed.
 * @param[in] addr Address with prefix.
 * @param[in] prefix Prefix.
 * \return Address without prefix.
 */
char* get_address_without_prefix(char* addr, const char* prefix);

/**
 * The function gets addresses with a given prefix.
 * @param[in] addr A string containing a comma-separated list of addresses.
 * @param[in] prefix Prefix with which to get addresses. For example, "xi-tcp", "xi-udp".
 * @param[out] addr_with_prefix A string of addresses with the given prefix, separated by commas.
 */
void get_addresses_with_prefix(char* addr, const char* prefix, char** addr_with_prefix);

/**
 * The function gets the value of the key from the given string.
 * For example, if you specify the string "key1=value1_1,value1_2 \n key2=value2"
 * and the key "key1", then you will get the value "value1_1,value1_2".
 * The xiwrapper library is needed because the 'find_key' function from this library is used.
 * @param[in] hints String with keys and values.
 * @param[in] key The name of the key whose value you want to get.
 * @param[out] value Pointer to the string into which the key value will be written.
 */
void get_key_value_from_hints(const char* hints, const char* key, char** value);

/**
 * The function increases the memory allocated to the structure device_enumeration_opaque_t.
 * @param[in] dev_enum Pointer to the structure for which memory needs to be increased.
 */
void increase_memory_for_dev_enum(device_enumeration_opaque_t* dev_enum);

/**
 * The function increases the memory allocated to the xinet server array.
 * @param[in] xinet_servers Pointer to a structure with an array of xinet servers.
 * \return result_ok if the function is executed without errors, otherwise result_error.
 */
result_t increase_memory_for_xinet_servers(xinet_servers_t* xinet_servers);

/**
 * The function initializes global mutexes.
 */
void initialize_global_mutexes();

/**
 * The function displays information about devices found using xinet.
 * @param[in] array_of_ssdp_devices Pointer to an array of structures that store data about devices found using SSDP.
 */
void print_ssdp_devices(array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices);

/**
 * The function displays information about devices found using xinet.
 * @param[in] array_of_xinet_devices Pointer to an array of structures that store data about devices found using xinet.
 */
void print_xinet_devices(array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices);

/**
 * The function gets addresses with a given prefix from a string 'addr' and stores them in 'dev_enum'.
 * @param[in] callback Function that stores addresses with a prefix in dev_enum.
 * @param[in] prefix Prefix that must be in the address.
 * @param[in] addr A string containing a comma-separated list of addresses.
 * @param[out] dev_enum Pointer to 'dev_enum' structure.
 */
void retrieve_uris_with_prefix(enumerate_devices_directory_callback_t callback, const char* prefix, char* addr, device_enumeration_opaque_t* dev_enum);

/**
 * The function extracts the IP addresses of xinet servers from a string of addresses separated by commas and adds them to the array of xinet servers.
 * @param[in] addr A string of IP addresses, separated by commas.
 * @param[in] adapters Pointer to a structure with an array of adapter data.
 * @param[in] xinet_servers Pointer to a structure with an array of xinet servers.
 */
void retrieve_xinet_server_addresses(char* addr, adapters_t* adapters, xinet_servers_t* xinet_servers);

/**
* The function extracts the IP addresses (with given prefix) of xinet servers from a string of addresses separated by commas and adds them to the array of xinet servers.
* @param[in] addr A string of IP addresses, separated by commas.
* @param[in] prefix IP address prefix.
* @param[in] adapters Pointer to a structure with an array of adapter data.
* @param[in] xinet_servers Pointer to a structure with an array of xinet servers.
*/
void retrieve_xinet_server_addresses_with_prefix(char* addr, const char* prefix, adapters_t* adapters, xinet_servers_t* xinet_servers);

/*
 * The function runs a check of devices for XIMC-compatibility in separate threads.
 * Non XIMC-compatible devices will be excluded from the dev_enum structure.
 * @param[in] dev_enum Pointer to a structure that stores the devices to be checked.
 */
void run_device_check_in_threads(device_enumeration_opaque_t* dev_enum);

/**
 * The function searches for xinet servers using the Revealer-1 protocol on adapters in separate threads.
 * @param[in] adapters Pointer to a structure containing information about adapters on which to search for servers using Revealer-1 protocol.
 * @param[in] xinet_servers Pointer to a structure with an array of xinet servers.
 */
void run_revealer_1_search_on_adapters_in_threads(adapters_t* adapters, xinet_servers_t* xinet_servers);

/**
 * @param[in] array_of_ssdp_devices Pointer to an array of structures that stores devices found on adapters using SSDP.
 * @param[in] array_of_xinet_devices Pointer to an array of structures that stores devices found on xinet servers using xinet.
 */
void run_ssdp_and_xinet_search_in_threads(array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices, array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices);

/**
 * The function starts searching using SSDP on adapters in separate threads.
 * @param[in] arg Pointer to an array of structures that stores devices found on adapters using SSDP.
 */
void run_ssdp_search_on_adapters_in_threads(void* arg);

/**
 * A wrapper function that will run in a separate thread to search for devices on xinet servers.
 * @param[in] arg Pointer to an array of structures that stores devices found on xinet servers using xinet.
 */
void run_wrapper_for_xinet_search_on_adapters_in_threads(void* arg);

/**
 * The function starts searching using xinet on adapters in separate threads.
 * @param[in] arg Pointer to an array of structures that stores devices found on xinet servers using xinet.
 */
XIMC_RETTYPE XIMC_CALLCONV run_xinet_search_on_adapters_in_threads(void* arg);

/*
 * The function saves the device name to the structure dev_enum.
 * @param[in] name Device name.
 * @param[in] arg Pointer to a dev_enum structure in which to store the device name.
 * @param[in] connected_via_usb If 1, the device is connected via USB, otherwise via network.
 */
void store_device_name(char* name, void* arg, int connected_via_usb);

/**
 * The function saves the device name to the structure dev_enum. The device is connected via USB.
 * @param[in] name Device name.
 * @param[in] arg Pointer to a dev_enum structure.
 */
void store_device_name_from_local(char* name, void* arg);

/**
* The function saves the device name to the structure dev_enum. The device is connected via network.
* @param[in] name Device name.
* @param[in] arg Pointer to a dev_enum structure.
*/
void store_device_name_from_network(char* name, void* arg);

/**
 * The function saves the names of devices from the array of devices found using SSDP into the dev_enum structure.
 * @param[in] array_of_ssdp_devices Pointer to an array of devices found using SSDP.
 * @param[in] dev_enum Pointer to a dev_enum structure.
 */
void store_ssdp_devices_to_dev_enum(array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices, device_enumeration_opaque_t* dev_enum);

/**
 * The function saves the names of devices from the array of devices found using xinet into the dev_enum structure.
 * @param[in] array_of_xinet_devices Pointer to an array of devices found using xinet.
 * @param[in] dev_enum Pointer to a dev_enum structure.
 */
void store_xinet_devices_to_dev_enum(array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices, device_enumeration_opaque_t* dev_enum);

#endif // !DEVENUM_H
