#include "devenum.h"


/* Function definition */

void add_device_to_ssdp_devices_struct(char* device_name, devices_enumerated_by_ssdp_t* ssdp_devices)
{
	if (ssdp_devices->count >= ssdp_devices->allocated_count) {
		ssdp_devices->allocated_count = (int)(ssdp_devices->allocated_count * 1.5);
		size_t new_size = ssdp_devices->allocated_count * sizeof(char*);
		char** device_names = (char**)realloc(ssdp_devices->device_names, new_size);
		if (!device_names) {
			log_error(L"Failed to increase memory to %d bytes for field 'device_names' in structure 'devices_enumerated_by_ssdp_t'. "
				L"The new device name will not be added to the SSDP devices array", new_size);
			return;
		}

		ssdp_devices->device_names = device_names;
	}

	char* p_device_name = (char*)malloc(MAX_NAME_LENGTH);
	if (!p_device_name) {
		log_error(L"Failed to allocate %d bytes of memory for string. The new device name will not be added to the SSDP devices array", MAX_NAME_LENGTH);
		return;
	}

	strcpy(p_device_name, device_name);
	ssdp_devices->device_names[ssdp_devices->count] = p_device_name;
	++ssdp_devices->count;
}


void add_server_to_xinet_servers_struct(char* server_address, adapter_info_t* adapter_info, xinet_servers_t* xinet_servers)
{
	if (!xinet_servers) {
		return;
	}

	int i;
	for (i = 0; i < xinet_servers->count; ++i) {
		if (is_same_device(server_address, xinet_servers->servers[i].address)) {
			log_debug(L"Skipping duplicate xinet server '%hs'", server_address);
			return;
		}
	}

	if (xinet_servers->count >= xinet_servers->allocated_count) {
		if (increase_memory_for_xinet_servers(xinet_servers) != result_ok) {
			log_error(L"Failed to add the server '%hs' to the xinet servers array", server_address);
			return;
		}
	}

	i = xinet_servers->count;
	xinet_servers->servers[i].adapter_info = adapter_info;
	memcpy(xinet_servers->servers[i].address, server_address, strlen(server_address));
	log_debug(L"The server added to the xinet servers array: index = %d, IP address = '%hs', adapter IP address = '%hs'", 
		i, xinet_servers->servers[i].address, xinet_servers->servers[i].adapter_info->address);
	xinet_servers->count++;
}


result_t allocate_memory_for_dev_enum(device_enumeration_opaque_t** dev_enum)
{
	const int allocated_count = 40;
	device_enumeration_opaque_t* p_dev_enum = (device_enumeration_opaque_t*)malloc(sizeof(device_enumeration_opaque_t));
	p_dev_enum->allocated_count = allocated_count;
	p_dev_enum->count = 0;
	p_dev_enum->flags = 0;
	p_dev_enum->names = NULL;
	p_dev_enum->raw_names = NULL;
	p_dev_enum->serials = NULL;
	p_dev_enum->infos = NULL;
	p_dev_enum->controller_names = NULL;
	p_dev_enum->stage_names = NULL;
	p_dev_enum->dev_net_infos = NULL;

	size_t size = allocated_count * sizeof(char*);
	p_dev_enum->names = (char**)malloc(size);
	if (!p_dev_enum->names) {
		log_error(L"Failed to allocate %d bytes of memory for field 'names' in structure 'device_enumeration_opaque_t'", size);
		return result_error;
	}

	p_dev_enum->raw_names = (char**)malloc(size);
	if (!p_dev_enum->raw_names) {
		log_error(L"Failed to allocate %d bytes of memory for field 'raw_names' in structure 'device_enumeration_opaque_t'", size);
		return result_error;
	}

	size = allocated_count * sizeof(uint32_t);
	p_dev_enum->serials = (uint32_t*)malloc(size);
	if (!p_dev_enum->serials) {
		log_error(L"Failed to allocate %d bytes of memory for field 'serials' in structure 'device_enumeration_opaque_t'", size);
		return result_error;
	}

	size = allocated_count * sizeof(device_information_t);
	p_dev_enum->infos = (device_information_t*)malloc(size);
	if (!p_dev_enum->infos) {
		log_error(L"Failed to allocate %d bytes of memory for field 'infos' in structure 'device_enumeration_opaque_t'", size);
		return result_error;
	}

	size = allocated_count * sizeof(controller_name_t);
	p_dev_enum->controller_names = (controller_name_t*)malloc(size);
	if (!p_dev_enum->controller_names) {
		log_error(L"Failed to allocate %d bytes of memory for field 'controller_names' in structure 'device_enumeration_opaque_t'", size);
		return result_error;
	}

	size = allocated_count * sizeof(stage_name_t);
	p_dev_enum->stage_names = (stage_name_t*)malloc(size);
	if (!p_dev_enum->stage_names) {
		log_error(L"Failed to allocate %d bytes of memory for field 'stage_names' in structure 'device_enumeration_opaque_t'", size);
		return result_error;
	}

	size = allocated_count * sizeof(device_network_information_t);
	p_dev_enum->dev_net_infos = (device_network_information_t*)malloc(size);
	if (!p_dev_enum->dev_net_infos) {
		log_error(L"Failed to allocate %d bytes of memory for field 'dev_net_infos' in structure 'device_enumeration_opaque_t'", size);
		return result_error;
	}

	memset(p_dev_enum->infos, 0, allocated_count * sizeof(device_information_t));
	memset(p_dev_enum->controller_names, 0, allocated_count * sizeof(controller_name_t));
	memset(p_dev_enum->stage_names, 0, allocated_count * sizeof(stage_name_t));
	memset(p_dev_enum->dev_net_infos, 0, allocated_count * sizeof(device_network_information_t));

	*dev_enum = p_dev_enum;
	return result_ok;
}


result_t allocate_memory_for_revealer_1_servers(int number, servers_enumerated_by_revealer_1_t** revealer_1_servers)
{
	size_t size = number * sizeof(servers_enumerated_by_revealer_1_t);
	servers_enumerated_by_revealer_1_t* p_revealer_1_servers = (servers_enumerated_by_revealer_1_t*)malloc(size);
	if (!p_revealer_1_servers) {
		log_error(L"Failed to allocate %d bytes of memory for array of structures 'servers_enumerated_by_revealer_1_t'", size);
		return result_error;
	}

	*revealer_1_servers = p_revealer_1_servers;
	return result_ok;
}


result_t allocate_memory_for_ssdp_devices(int number, devices_enumerated_by_ssdp_t** ssdp_devices)
{
	devices_enumerated_by_ssdp_t* p_ssdp_devices = (devices_enumerated_by_ssdp_t*)calloc(number, sizeof(devices_enumerated_by_ssdp_t));
	if (!p_ssdp_devices) {
		return result_error;
	}

	const int allocated_count = 40;
	int i;
	for (i = 0; i < number; i++) {
		p_ssdp_devices[i].allocated_count = allocated_count;
		p_ssdp_devices[i].count = 0;
		p_ssdp_devices[i].device_names = (char**)malloc(allocated_count * sizeof(char*));
		if (!p_ssdp_devices[i].device_names) {
			free_memory_for_ssdp_devices(number, p_ssdp_devices);
			return result_error;
		}
	}

	*ssdp_devices = p_ssdp_devices;
	return result_ok;
}


result_t allocate_memory_for_xinet_devices(int number, devices_enumerated_by_xinet_t** xinet_devices)
{
	devices_enumerated_by_xinet_t* p_xinet_devices = (devices_enumerated_by_xinet_t*)malloc(number * sizeof(devices_enumerated_by_xinet_t));
	if (!p_xinet_devices) {
		return result_error;
	}

	char* thread_expired = (char*)malloc(sizeof(char));
	if (!thread_expired) {
		free(p_xinet_devices);
		p_xinet_devices = NULL;
		return result_error;
	}
	*thread_expired = 0;

	int i;
	for (i = 0; i < number; i++) {
		memset(p_xinet_devices[i].server_address, 0, ADDRESS_LENGTH);
		p_xinet_devices[i].count = 0;
		p_xinet_devices[i].status = 0;
		p_xinet_devices[i].pbuf = (uint8_t**)malloc(sizeof(uint8_t*));
		*(p_xinet_devices[i].pbuf) = NULL;
		p_xinet_devices[i].thread_expired = thread_expired;
	}

	*xinet_devices = p_xinet_devices;
	return result_ok;
}


result_t allocate_memory_for_xinet_servers(xinet_servers_t** xinet_servers)
{
	xinet_servers_t* p_xinet_servers = (xinet_servers_t*)malloc(sizeof(xinet_servers_t));
	if (!p_xinet_servers) {
		log_error(L"Failed to allocate memory for structure 'xinet_servers_t'");
		return result_error;
	}

	p_xinet_servers->allocated_count = 10;
	p_xinet_servers->count = 0;
	p_xinet_servers->servers = (xinet_server_t*)malloc(p_xinet_servers->allocated_count * sizeof(xinet_server_t));
	if (!p_xinet_servers->servers) {
		log_error(L"Failed to allocate memory for array of structers 'xinet_server_t'");
		free(p_xinet_servers);
		p_xinet_servers = NULL;
		return result_error;
	}

	int i;
	for (i = 0; i < p_xinet_servers->allocated_count; i++) {
		memset(p_xinet_servers->servers[i].address, 0, IP_ADDRESS_SIZE);
	}

	*xinet_servers = p_xinet_servers;
	return result_ok;
}


int check_device_by_ximc_information(const char* name, device_information_t* info, uint32_t* serial, controller_name_t* controller_name, stage_name_t* stage_name)
{
	/* ACHTUNG!!! WE add a kludge to support STELM Manufacturer, #96430. Later it must be fixed! */
	log_debug(L"[Thread for '%hs'] Thread has been launched to check device '%hs'", name, name);

	device_information_t info_local;
	device_information_t* p_info = info ? info : &info_local;

	controller_name_t controller_name_local;
	controller_name_t* p_controller_name = controller_name ? controller_name : &controller_name_local;

	stage_name_t stage_name_local;
	stage_name_t* p_stage_name = stage_name ? stage_name : &stage_name_local;

	int is_ximc_device = 0;
	device_t device = open_device_impl(name, CHECK_DEVICE_TIMEOUT_MS);
	if (device != device_undefined) {
		log_debug(L"[Thread for '%hs'] Device opened", name);
		if (get_device_information_impl_unsynced(device, p_info) == result_ok) {
			log_debug(L"[Thread for '%hs'] Device information received", name);
			// ACHTUNG! The kludge! "!strcmp(pinfo->Manufacturer, "STLM")" must be removed later. #96430
			if (!strcmp(p_info->Manufacturer, "XIMC") || !strcmp(p_info->Manufacturer, "EPC ") || !strcmp(p_info->Manufacturer, "STLM")) {
				is_ximc_device = 1;
				if (serial) {
					if (get_serial_number(device, serial) != result_ok) {
						log_warning(L"[Thread for '%hs'] Failed to get serial number from device", name);
						*serial = 0;
					}
					else {
						log_debug(L"[Thread for '%hs'] Serial number received: %d", name, *serial);
					}

					if (get_stage_name(device, p_stage_name) != result_ok) {
						log_warning(L"[Thread for '%hs'] Failed to get stage name from device", name);
					}

					if (get_controller_name(device, p_controller_name) != result_ok) {
						log_warning(L"[Thread for '%hs'] Failed to get controller name from device", name);
					}
				}
			}
		}
		else {
			log_debug(L"[Thread for '%hs'] Failed to get device information", name);
		}

		close_device_impl(&device);
		log_debug(L"[Thread for '%hs'] Device closed", name);
		msec_sleep(ENUMERATE_CLOSE_TIMEOUT);
	}
	else {
		log_debug(L"[Thread for '%hs'] Failed to open device", name);
	}
	return is_ximc_device;
}


void check_device_in_thread(void* arg)
{
	enum_thread_state_t* state = (enum_thread_state_t*)arg;

	char* thread_expired = state->thread_expired;
	char name[MAX_NAME_LENGTH] = "";
	strcpy(name, state->name);
	controller_name_t* controller_name = (controller_name_t*)calloc(1, sizeof(controller_name_t));
	device_information_t* info = (device_information_t*)calloc(1, sizeof(device_information_t));
	stage_name_t* stage_name = (stage_name_t*)calloc(1, sizeof(stage_name_t));
	uint32_t serial = 0;
	if (!controller_name || !info || !stage_name) {
		free(controller_name);
		controller_name = NULL;
		free(info);
		info = NULL;
		free(stage_name);
		stage_name = NULL;

		log_error(L"Failed to allocate memory for local parameters to check device '%hs' in separate thread", name);
		return;
	}

	int status = check_device_by_ximc_information(name, info, &serial, controller_name, stage_name);

	lock_check_devices();
	if (*thread_expired == 0) {
		state->status = status;
		if (status) {
			memcpy(state->controller_name, controller_name, sizeof(controller_name_t));
			memcpy(state->info, info, sizeof(device_information_t));
			memcpy(state->stage_name, stage_name, sizeof(stage_name_t));
			state->serial = serial;
		}
		log_debug(L"Device '%hs' check completed before timeout", name);
	}
	else {
		log_debug(L"Device '%hs' check completed after timeout", name);
	}
	unlock_check_devices();

	free(controller_name);
	controller_name = NULL;
	free(info);
	info = NULL;
	free(stage_name);
	stage_name = NULL;
}


void create_array_of_devices_enumerated_by_ssdp(adapters_t* adapters, array_of_devices_enumerated_by_ssdp_t** array_of_ssdp_devices)
{
	devices_enumerated_by_ssdp_t* ssdp_devices = NULL;
	if (allocate_memory_for_ssdp_devices(adapters->number_of_adapters, &ssdp_devices) != result_ok) {
		log_error(L"Failed to create array of structures 'devices_enumerated_by_ssdp_t'");
		return;
	}

	int i;
	for (i = 0; i < adapters->number_of_adapters; i++) {
		ssdp_devices[i].adapter_info = &(adapters->adapter_infos[i]);
	}

	array_of_devices_enumerated_by_ssdp_t* p_array_of_ssdp_devices = (array_of_devices_enumerated_by_ssdp_t*)malloc(sizeof(array_of_devices_enumerated_by_ssdp_t));
	if (!p_array_of_ssdp_devices) {
		log_error(L"Failed to allocate memory for structure 'array_of_devices_enumerated_by_ssdp_t'");
		free_memory_for_ssdp_devices(adapters->number_of_adapters, ssdp_devices);
		return;
	}

	p_array_of_ssdp_devices->count = adapters->number_of_adapters;
	p_array_of_ssdp_devices->ssdp_devices = ssdp_devices;

	*array_of_ssdp_devices = p_array_of_ssdp_devices;
}


void create_array_of_devices_enumerated_by_xinet(xinet_servers_t* xinet_servers, array_of_devices_enumerated_by_xinet_t** array_of_xinet_devices)
{
	if (!xinet_servers || xinet_servers->count == 0) {
		log_debug(L"There are no xinet servers to search for devices on");
		return;
	}

	log_debug(L"Devices will be searched using xinet on %d xinet servers:", xinet_servers->count);
	devices_enumerated_by_xinet_t* xinet_devices = NULL;
	if (allocate_memory_for_xinet_devices(xinet_servers->count, &xinet_devices) != result_ok) {
		log_error(L"Failed to create array of structures 'devices_enumerated_by_xinet_t'");
		return;
	}

	int i;
	for (i = 0; i < xinet_servers->count; i++) {
		devices_enumerated_by_xinet_t* xinet_device = &(xinet_devices[i]);
		strcpy(xinet_device->server_address, xinet_servers->servers[i].address);
		xinet_device->adapter_info = xinet_servers->servers[i].adapter_info;
		log_debug(L"%d. IP address of server = '%hs', IP address of adapter = '%hs'", i + 1, xinet_device->server_address, xinet_device->adapter_info->address);
	}

	array_of_devices_enumerated_by_xinet_t* p_array_of_xinet_devices = (array_of_devices_enumerated_by_xinet_t*)malloc(sizeof(array_of_devices_enumerated_by_xinet_t));
	if (!p_array_of_xinet_devices) {
		log_error(L"Failed to allocate memory for structure 'array_of_devices_enumerated_by_xinet_t'");
		free_memory_for_xinet_devices(xinet_servers->count, xinet_devices);
		return;
	}

	p_array_of_xinet_devices->count = xinet_servers->count;
	p_array_of_xinet_devices->xinet_devices = xinet_devices;

	*array_of_xinet_devices = p_array_of_xinet_devices;
}


void delete_array_of_devices_enumerated_by_ssdp(array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices)
{
	if (!array_of_ssdp_devices) {
		return;
	}

	free_memory_for_ssdp_devices(array_of_ssdp_devices->count, array_of_ssdp_devices->ssdp_devices);
	free(array_of_ssdp_devices);
	array_of_ssdp_devices = NULL;
}


void delete_array_of_devices_enumerated_by_xinet(array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices)
{
	if (!array_of_xinet_devices) {
		return;
	}

	free_memory_for_xinet_devices(array_of_xinet_devices->count, array_of_xinet_devices->xinet_devices);
	free(array_of_xinet_devices);
	array_of_xinet_devices = NULL;
}


void determine_adapters(const char* hints, adapters_t* adapters)
{
	char* adapter_addresses_from_hints = NULL;
	get_key_value_from_hints(hints, "adapter_addr", &adapter_addresses_from_hints);

	result_t result = result_ok;
	if (adapter_addresses_from_hints != NULL && strlen(adapter_addresses_from_hints) > 0) {
		char** addresses = NULL;
		int number_of_addresses = 0;
		split_string_into_parts(adapter_addresses_from_hints, ",", &number_of_addresses, &addresses);

		set_adapters(number_of_addresses, addresses, adapters);

		free_memory_for_string_array(number_of_addresses, addresses);
		free(adapter_addresses_from_hints);
		adapter_addresses_from_hints = NULL;
		log_debug(L"The adapter addresses is set by the user");
	}
	else {
		log_debug(L"Finding adapter addresses...");
		result = find_adapters(adapters);
		log_debug(L"Adapter addresses found");
	}

	if (result != result_ok) {
		adapters->adapter_infos = NULL;
		adapters->number_of_adapters = 0;
		adapters->adapter_set = 0;
	}
	print_adapters(adapters);
}


void discover_using_revealer_1_protocol(void* arg)
{
	servers_enumerated_by_revealer_1_t* revealer_1_servers = (servers_enumerated_by_revealer_1_t*)arg;
	log_debug(L"Start to search for servers using Revealer-1 protocol in separate thread on adapter '%hs'...", revealer_1_servers->adapter_info->address);
	bindy_searchByRevealer1Protocol(revealer_1_servers->adapter_info->address, &(revealer_1_servers->addresses));
	int number = 0;
	if (revealer_1_servers->addresses) {
		number = revealer_1_servers->addresses->count;
	}

	if (number > 0) {
		log_debug(L"%d servers found on adapter '%hs' using Revealer-1 protocol", number, revealer_1_servers->adapter_info->address);
	}
	else {
		log_debug(L"No servers found on adapter '%hs' using Revealer-1 protocol", revealer_1_servers->adapter_info->address);
	}
}


void discover_using_ssdp_and_add_as_tcp(void* arg)
{
	devices_enumerated_by_ssdp_t* ssdp_devices = (devices_enumerated_by_ssdp_t*)arg;
	log_debug(L"Start to search for devices using SSDP in separate thread on adapter '%hs'...", ssdp_devices->adapter_info->address);

#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		return;
#endif

	char discovered_ip[64];
	strcpy(discovered_ip, "xi-tcp://");

	struct UPNPDev* dev_list = 0;
	int error = 0;
	if ((dev_list = upnpDiscoverAll(SSDP_DELAY_MS, ssdp_devices->adapter_info->address, NULL, 0, 0, 2, &error)) != NULL) {
		struct UPNPDev* device;
		for (device = dev_list; device; device = device->pNext) {
			if (strstr(device->server, "8SMC5-USB") != NULL || strstr(device->server, "mDrive") != NULL) {
				char* ip_start = strstr(device->descURL, "://");
				if (ip_start == NULL)
					continue;

				char* ip_end = strchr(ip_start + 3, ':');
				if (ip_end == NULL)
					ip_end = strchr(ip_start, '/');
				if (ip_end == NULL)
					ip_end = strchr(ip_start, 0);
				if (ip_end == NULL || ip_end < (ip_start + 3))
					continue;

				size_t ip_length;
				ip_length = ip_end - ip_start - 3;
				if (ip_end  < 3 + ip_start || ip_length > 63 - 9)
					continue; // "xi-tcp://"- len = 9

				memcpy(discovered_ip + 9, ip_start + 3, ip_length);
				// Add default port number for xi-tcp
				portable_snprintf(discovered_ip + 9 + ip_length, 64 - 9 - ip_length, ":%u", XIMC_TCP_PORT);

				add_device_to_ssdp_devices_struct(discovered_ip, ssdp_devices);
				log_debug(L"Device '%hs' found using SSDP on adapter '%hs'", discovered_ip, ssdp_devices->adapter_info->address);
			}
		}
		freeUPNPDevlist(dev_list);
		dev_list = 0;
	}

#ifdef _WIN32
	WSACleanup();
#endif
	log_debug(L"Search for devices using SSDP in separate thread on adapter '%hs' completed", ssdp_devices->adapter_info->address);
}


void discover_using_xinet(void* arg)
{
	devices_enumerated_by_xinet_t* xinet_devices = (devices_enumerated_by_xinet_t*)arg;
	char* thread_expired = xinet_devices->thread_expired;

	char adapter_address[IP_ADDRESS_SIZE] = "";
	if (xinet_devices->adapter_info) {
		strcpy(adapter_address, xinet_devices->adapter_info->address);
	}
	
	char server_address[IP_ADDRESS_SIZE] = "";
	if (0 != strcmp(xinet_devices->server_address, "")) {
		strcpy(server_address, xinet_devices->server_address);
	}

	uint8_t** pbuf = (uint8_t**)malloc(sizeof(uint8_t*));
	*pbuf = NULL;
	int count = 0;

	log_debug(L"Start to search for devices using xinet in separate thread on server '%hs' and adapter '%hs'...", server_address, adapter_address);
#ifdef HAVE_XIWRAPPER
	count = bindy_enumerate_specify_adapter(server_address, adapter_address, XINET_TIMEOUT_MS, pbuf);
#endif

	lock_xinet_data();
	if (*pbuf && *thread_expired == 0) {
		xinet_devices->count = count;
		*(xinet_devices->pbuf) = *pbuf;
		log_debug(L"Search for devices using xinet in separate thread on server '%hs' and adapter '%hs' completed", server_address, adapter_address);
	}
	else {
		bindy_free(pbuf);
		log_debug(L"Search for devices using xinet in separate thread on server '%hs' and adapter '%hs' completed after timeout or failed", server_address, adapter_address);
	}
	unlock_xinet_data();
	free(pbuf);
	pbuf = NULL;
}


result_t enumerate_devices_impl(device_enumeration_opaque_t** device_enumeration, int enumerate_flags, const char* hints)
{
	initialize_global_mutexes();

	// Allocate memory
	if (allocate_memory_for_dev_enum(device_enumeration) != result_ok) {
		return result_error;
	}

	device_enumeration_opaque_t* dev_enum = *device_enumeration;
	dev_enum->flags = enumerate_flags;

	log_info(L"Start searching for local devices...");
	if (discover_local_devices(store_device_name_from_local, dev_enum, enumerate_flags) != result_ok) {
		log_debug(L"Search for local devices failed");
		dev_enum->count = 0;
		return result_error;
	}
	log_info(L"Search for local devices completed");

	char* addr = NULL;
	if (enumerate_flags & ENUMERATE_NETWORK) {
		if (hints == NULL) {
			// NULL hints is fine
			log_error(L"'hints' argument is null");
			return result_ok;
		}

		get_key_value_from_hints(hints, "addr", &addr);

		const char xi_tcp[] = "xi-tcp";
		log_info(L"Start searching for '%hs' devices from the passed addresses...", xi_tcp);
		retrieve_uris_with_prefix(store_device_name_from_network, xi_tcp, addr, dev_enum);
		log_info(L"Search for '%hs' devices from the passed addresses completed", xi_tcp);

		const char xi_udp[] = "xi-udp";
		log_info(L"Start searching for '%hs' devices from the passed addresses...", xi_udp);
		retrieve_uris_with_prefix(store_device_name_from_network, xi_udp, addr, dev_enum);
		log_info(L"Search for '%hs' devices from the passed addresses completed", xi_udp);

		log_info(L"Start adapter determination...");
		adapters_t adapters;
		determine_adapters(hints, &adapters);
		log_info(L"Adapter determination completed");

		xinet_servers_t* xinet_servers = NULL;
		allocate_memory_for_xinet_servers(&xinet_servers);
#ifdef HAVE_XIWRAPPER
		if (!bindy_init()) {
			log_error(L"Network layer init failed");
			return result_error;
		}
		retrieve_xinet_server_addresses(addr, &adapters, xinet_servers);
		run_revealer_1_search_on_adapters_in_threads(&adapters, xinet_servers);
#endif // !HAVE_XIWRAPPER

		array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices = NULL;
		create_array_of_devices_enumerated_by_ssdp(&adapters, &array_of_ssdp_devices);
		array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices = NULL;
		create_array_of_devices_enumerated_by_xinet(xinet_servers, &array_of_xinet_devices);
		free_memory_for_xinet_servers(xinet_servers);

		run_ssdp_and_xinet_search_in_threads(array_of_ssdp_devices, array_of_xinet_devices);

		print_ssdp_devices(array_of_ssdp_devices);
		store_ssdp_devices_to_dev_enum(array_of_ssdp_devices, dev_enum);
		delete_array_of_devices_enumerated_by_ssdp(array_of_ssdp_devices);

		lock_xinet_data();
		print_xinet_devices(array_of_xinet_devices);
		store_xinet_devices_to_dev_enum(array_of_xinet_devices, dev_enum);
		delete_array_of_devices_enumerated_by_xinet(array_of_xinet_devices);
		unlock_xinet_data();

		free_adapters(&adapters);
	}

	if (enumerate_flags & ENUMERATE_PROBE) {
		run_device_check_in_threads(dev_enum);
	}

	if (addr) {
		free(addr);
		addr = NULL;
	}
	log_info(L"%d devices found", dev_enum->count);
	return result_ok;
}


void free_memory_for_revealer_1_servers(int number, servers_enumerated_by_revealer_1_t* revealer_1_servers)
{
	int i;
	for (i = 0; i < number; i++) {
		bindy_freeMemoryForRevealer1Addresses(revealer_1_servers[i].addresses);
	}
	free(revealer_1_servers);
	revealer_1_servers = NULL;
}


void free_memory_for_ssdp_devices(int number, devices_enumerated_by_ssdp_t* ssdp_devices)
{
	if (!ssdp_devices) {
		return;
	}

	int i;
	for (i = 0; i < number; i++) {
		int j = 0;
		for (j = 0; j < ssdp_devices[i].count; j++) {
			free(ssdp_devices[i].device_names[j]);
			ssdp_devices[i].device_names[i] = NULL;
		}
		free(ssdp_devices[i].device_names);
		ssdp_devices[i].device_names = NULL;
	}
	free(ssdp_devices);
	ssdp_devices = NULL;
}


void free_memory_for_xinet_devices(int number, devices_enumerated_by_xinet_t* xinet_devices)
{
	if (!xinet_devices) {
		return;
	}

	int i;
	for (i = 0; i < number; i++) {
		if (*(xinet_devices[i].pbuf) != NULL) {
			bindy_free(xinet_devices[i].pbuf);
		}
		free(xinet_devices[i].pbuf);
		xinet_devices[i].pbuf = NULL;
	}
	free(xinet_devices);
	xinet_devices = NULL;
}


void free_memory_for_xinet_servers(xinet_servers_t* xinet_servers)
{
	if (!xinet_servers) {
		return;
	}

	free(xinet_servers->servers);
	xinet_servers->servers = NULL;
	free(xinet_servers);
	xinet_servers = NULL;
}


char* get_address_without_prefix(char* addr, const char* prefix)
{
	size_t prefix_length = strlen(prefix);
	if (prefix_length) {
		prefix_length += 3;
	}

	char* address_without_prefix = (char*)malloc(strlen(addr) + 1);
	if (!address_without_prefix) {
		return NULL;
	}

	size_t length_to_copy = strlen(addr) - prefix_length;
	memcpy(address_without_prefix, addr + prefix_length, length_to_copy);
	address_without_prefix[length_to_copy] = 0;
	return address_without_prefix;
}


void get_addresses_with_prefix(char* addr, const char* prefix, char** addr_with_prefix)
{
	if (!addr) {
		return;
	}

	size_t length = strlen(addr) + 1;
	char* p_addr = (char*)malloc(length);
	memset(p_addr, 0, length);
	*addr_with_prefix = p_addr;

	char* new_ptr;
	char* ptr = addr;
	size_t len = 0, len_out = 0;
	while (ptr != NULL) {
		new_ptr = strchr(ptr, ',');
		if (new_ptr != NULL)
			len = new_ptr - ptr;
		else
			len = strlen(ptr);

		if (len) {
			char* substr = (char*)malloc(len);
			memcpy(substr, ptr, len);
			if ((strlen(prefix) == 0 && strstr(substr, ":/") == NULL) ||
				(strlen(prefix) != 0 && portable_strncasecmp(substr, prefix, strlen(prefix)) == 0)) {
				memcpy(p_addr, ptr, len);
				memcpy(p_addr + len, ",", 1);
				p_addr += len + 1;
			}
			free(substr);
			substr = NULL;
		}

		if (new_ptr != NULL)
			ptr = new_ptr + 1; // continue with string after the comma
		else
			ptr = NULL;
	}

	if (len_out == 0)
		len_out++;

	if (p_addr > *addr_with_prefix) {
		*(p_addr - 1) = 0;
	}

	if (strlen(*addr_with_prefix) == 0) {
		free(*addr_with_prefix);
		*addr_with_prefix = NULL;
	}

	if (*addr_with_prefix != NULL) {
		log_debug(L"Addresses with prefix '%hs' from key 'addr': '%hs'", prefix, *addr_with_prefix);
	}
	else {
		log_debug(L"No addresses with prefix '%hs' found in key 'addr'", prefix);
	}
}


void get_key_value_from_hints(const char* hints, const char* key, char** value)
{
	if (!hints) {
		return;
	}

#ifdef HAVE_XIWRAPPER
	int hint_length = (int)strlen(hints);
	char* p_value = malloc(hint_length + 1);
	memset(p_value, 0, hint_length + 1);

	if (!find_key(hints, key, p_value, hint_length)) {
		free(p_value);
		p_value = NULL;
	}

	if (p_value != NULL) {
		log_debug(L"The '%hs' key in 'hints' is '%hs'", key, p_value);
	}
	else {
		log_debug(L"No '%hs' key in 'hints' argument", key);
	}
	*value = p_value;
#endif
}


void increase_memory_for_dev_enum(device_enumeration_opaque_t* dev_enum)
{
	int allocated_count = (int)(dev_enum->allocated_count * 1.5);
	dev_enum->allocated_count = allocated_count;
	dev_enum->names = (char**)realloc(dev_enum->names, allocated_count * sizeof(char*));
	dev_enum->raw_names = (char**)realloc(dev_enum->raw_names, allocated_count * sizeof(char*));
	dev_enum->serials = (uint32_t*)realloc(dev_enum->serials, allocated_count * sizeof(uint32_t));
	dev_enum->infos = (device_information_t*)realloc(dev_enum->infos, allocated_count * sizeof(device_information_t));
	dev_enum->controller_names = (controller_name_t*)realloc(dev_enum->controller_names, allocated_count * sizeof(controller_name_t));
	dev_enum->stage_names = (stage_name_t*)realloc(dev_enum->stage_names, allocated_count * sizeof(stage_name_t));
	dev_enum->dev_net_infos = (device_network_information_t*)realloc(dev_enum->dev_net_infos, allocated_count * sizeof(device_network_information_t));
}


result_t increase_memory_for_xinet_servers(xinet_servers_t* xinet_servers)
{
	xinet_servers->allocated_count = (int)(1.5 * xinet_servers->allocated_count);
	size_t new_size = xinet_servers->allocated_count * sizeof(xinet_server_t);
	xinet_server_t* new_servers = (xinet_server_t*)realloc(xinet_servers->servers, new_size);
	if (!new_servers) {
		log_error(L"Failed to increase memory to %d bytes for field 'servers' of structure 'xinet_servers_t'", new_size);
		return result_error;
	}

	xinet_servers->servers = new_servers;
	int i;
	for (i = xinet_servers->count; i < xinet_servers->allocated_count; i++) {
		memset(xinet_servers->servers[i].address, 0, IP_ADDRESS_SIZE);
	}
	return result_ok;
}


void initialize_global_mutexes()
{
	// Ensure one-thread mutex init
	lock_metadata();
	unlock_metadata();
	lock_xinet_data();
	unlock_xinet_data();
	lock_check_devices();
	unlock_check_devices();
}

void print_ssdp_devices(array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices)
{
	if (!array_of_ssdp_devices || array_of_ssdp_devices->count == 0) {
		log_info(L"No devices found using SSDP");
		return;
	}

	int i;
	for (i = 0; i < array_of_ssdp_devices->count; i++) {
		devices_enumerated_by_ssdp_t* ssdp = &(array_of_ssdp_devices->ssdp_devices[i]);
		char* adapter_address = ssdp->adapter_info ? ssdp->adapter_info->address : NULL;

		if (ssdp->count > 0) {
			log_info(L"Using SSDP found %d devices on adapter '%hs':", ssdp->count, adapter_address);
			int j;
			for (j = 0; j < ssdp->count; j++) {
				log_info(L"%d. Device name: '%hs'", j + 1, ssdp->device_names[j]);
			}
		}
		else {
			log_info(L"No devices found using SSDP on adapter '%hs'", adapter_address);
		}
	}
}


void print_xinet_devices(array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices)
{
	if (!array_of_xinet_devices || array_of_xinet_devices->count == 0) {
		log_info(L"No devices found using xinet");
		return;
	}

	int i;
	for (i = 0; i < array_of_xinet_devices->count; i++) {
		devices_enumerated_by_xinet_t* xinet = &(array_of_xinet_devices->xinet_devices[i]);
		char* adapter_address = xinet->adapter_info ? xinet->adapter_info->address : NULL;

		if (xinet->count > 0) {
			log_info(L"Using xinet found %d devices on xinet server '%hs' and adapter '%hs':", xinet->count, xinet->server_address, adapter_address);
			int j;
			for (j = 0; j < xinet->count; j++) {
				device_description* desc = (((device_description*)*(xinet->pbuf)) + j);
				char* name = (char*)malloc(MAX_NAME_LENGTH);
				uint8_t* ip_bytes = (uint8_t*)&desc->ipv4;
				portable_snprintf(name, MAX_NAME_LENGTH - 1, "xi-net://%d.%d.%d.%d/%08X", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3], desc->serial);
				name[MAX_NAME_LENGTH - 1] = '\0';
				log_info(L"%d. Device name: '%hs', serial: %d", j + 1, name, desc->serial);
				free(name);
			}
		}
		else {
			log_info(L"No devices found using xinet on xinet server '%hs' and adapter '%hs'", xinet->server_address, adapter_address);
		}
	}
}


void retrieve_uris_with_prefix(enumerate_devices_directory_callback_t callback, const char* prefix, char* addr, device_enumeration_opaque_t* dev_enum)
{
	char* addresses_with_prefix = NULL;
	get_addresses_with_prefix(addr, prefix, &addresses_with_prefix);
	if (!addresses_with_prefix) {
		return;
	}

	char* new_ptr;
	char* ptr = addresses_with_prefix;
	while (ptr != NULL) {
		new_ptr = strchr(ptr, ',');
		if (new_ptr != NULL) {
			*new_ptr = 0;
		}
		callback(ptr, dev_enum);

		if (new_ptr != NULL) {
			ptr = new_ptr + 1; // continue with string after the comma
		}
		else {
			ptr = NULL;
		}
	}
	free(addresses_with_prefix);
	addresses_with_prefix = NULL;
}


void retrieve_xinet_server_addresses(char* addr, adapters_t* adapters, xinet_servers_t* xinet_servers)
{
	retrieve_xinet_server_addresses_with_prefix(addr, "", adapters, xinet_servers);
	retrieve_xinet_server_addresses_with_prefix(addr, "xi-net", adapters, xinet_servers);
}


void retrieve_xinet_server_addresses_with_prefix(char* addr, const char* prefix, adapters_t* adapters, xinet_servers_t* xinet_servers)
{
	char* addresses_with_prefix = NULL;
	get_addresses_with_prefix(addr, prefix, &addresses_with_prefix);

	char** server_addresses = NULL;
	int number_of_servers = 0;
	split_string_into_parts(addresses_with_prefix, ",", &number_of_servers, &server_addresses);

	if (number_of_servers > 0) {
		int i;
		for (i = 0; i < number_of_servers; i++) {
			char* address_without_prefix = get_address_without_prefix(server_addresses[i], prefix);
			if (!address_without_prefix) {
				log_error(L"Failed to get address without prefix '%hs' from address '%hs'", prefix, server_addresses[i]);
				continue;
			}

			adapter_info_t* adapter_info = get_adapter_for_address(address_without_prefix, adapters);
			add_server_to_xinet_servers_struct(address_without_prefix, adapter_info, xinet_servers);
			free(address_without_prefix);
			address_without_prefix = NULL;
		}
	}

	free(addresses_with_prefix);
	addresses_with_prefix = NULL;
	free_memory_for_string_array(number_of_servers, server_addresses);
}


void run_device_check_in_threads(device_enumeration_opaque_t* dev_enum)
{
	// Check all found devices in threads
	log_debug(L"%d threads will be launched to check the found devices", dev_enum->count);
	enum_thread_state_t* states = (enum_thread_state_t*)malloc(dev_enum->count * sizeof(enum_thread_state_t));
	char* thread_expired = (char*)malloc(sizeof(char));
	*thread_expired = 0;

	int i;
	for (i = 0; i < dev_enum->count; ++i) {
		states[i].name = dev_enum->names[i];
		states[i].info = &dev_enum->infos[i];
		states[i].controller_name = &dev_enum->controller_names[i];
		states[i].stage_name = &dev_enum->stage_names[i];
		states[i].status = 0;
		states[i].thread_expired = thread_expired;
	}

	if (fork_join_with_timeout(check_device_in_thread, dev_enum->count, states, sizeof(enum_thread_state_t), CHECK_DEVICE_TIMEOUT_MS, NULL) != result_ok) {
		log_error(L"Failed to check devices in separate threads");
		dev_enum->count = 0;
	}

	lock_check_devices();
	*thread_expired = 1;

	int k;
	for (i = 0, k = 0; i < dev_enum->count; ++i) {
		// Rewrite devenum array
		if (states[i].status) {
			log_debug(L"After checking the device '%hs' is XIMC-compatible", dev_enum->names[i]);
			dev_enum->names[k] = states[i].name;
			dev_enum->serials[k] = states[i].serial;
			if (&dev_enum->infos[k] != states[i].info)
				memcpy(&dev_enum->infos[k], states[i].info, sizeof(device_information_t));
			if (&dev_enum->controller_names[k] != states[i].controller_name)
				memcpy(&dev_enum->controller_names[k], states[i].controller_name, sizeof(controller_name_t));
			if (&dev_enum->stage_names[k] != states[i].stage_name)
				memcpy(&dev_enum->stage_names[k], states[i].stage_name, sizeof(stage_name_t));
			++k;
		}
		else {
			log_debug(L"After checking the device '%hs' is not XIMC-compatible", dev_enum->names[i]);
			free(dev_enum->names[i]);
			dev_enum->names[i] = NULL;
			free(dev_enum->raw_names[i]);
			dev_enum->raw_names[i] = NULL;
		}
	}
	dev_enum->count = k;
	free(states);
	states = NULL;
	unlock_check_devices();
}


void run_revealer_1_search_on_adapters_in_threads(adapters_t* adapters, xinet_servers_t* xinet_servers)
{
	log_info(L"Start searching for xinet servers using Revealer-1 protocol...");
	servers_enumerated_by_revealer_1_t* revealer_1_servers = NULL;
	if (allocate_memory_for_revealer_1_servers(adapters->number_of_adapters, &revealer_1_servers) != result_ok) {
		return;
	}

	int i;
	for (i = 0; i < adapters->number_of_adapters; i++) {
		revealer_1_servers[i].adapter_info = &(adapters->adapter_infos[i]);
		revealer_1_servers[i].addresses = NULL;
	}

	if (fork_join(discover_using_revealer_1_protocol, adapters->number_of_adapters, revealer_1_servers, sizeof(servers_enumerated_by_revealer_1_t)) != result_ok) {
		log_error(L"Failed to start xinet servers discovery using Revealer-1 protocol in separate threads");
	}

	for (i = 0; i < adapters->number_of_adapters; i++) {
		Revealer1Addresses* revealer_1_addresses = revealer_1_servers[i].addresses;
		if (!revealer_1_addresses) {
			continue;
		}

		adapter_info_t* adapter_info = &(adapters->adapter_infos[i]);
		int j;
		for (j = 0; j < revealer_1_addresses->count; j++) {
			add_server_to_xinet_servers_struct(revealer_1_addresses->addresses[j], adapter_info, xinet_servers);
		}
	}

	free_memory_for_revealer_1_servers(adapters->number_of_adapters, revealer_1_servers);
	log_info(L"Searching for xinet servers using Revealer-1 protocol completed");
}


void run_ssdp_and_xinet_search_in_threads(array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices, array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices)
{
	fork_join_2_threads(run_ssdp_search_on_adapters_in_threads, array_of_ssdp_devices, run_wrapper_for_xinet_search_on_adapters_in_threads, array_of_xinet_devices);
}


void run_ssdp_search_on_adapters_in_threads(void* arg)
{
	array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices = (array_of_devices_enumerated_by_ssdp_t*)arg;
	if (!array_of_ssdp_devices || array_of_ssdp_devices->count == 0) {
		log_info(L"There are no adapters to search for devices on them using SSDP");
		return;
	}

	log_info(L"Start to search for devices using SSDP in separate threads...");
	if (fork_join(discover_using_ssdp_and_add_as_tcp, array_of_ssdp_devices->count, array_of_ssdp_devices->ssdp_devices, sizeof(devices_enumerated_by_ssdp_t)) != result_ok) {
		log_error(L"Failed to start device discovery using SSDP in separate threads");
		delete_array_of_devices_enumerated_by_ssdp(array_of_ssdp_devices);
	}
	else {
		log_info(L"Search for devices using SSDP in separate threads completed");
	}
}


void run_wrapper_for_xinet_search_on_adapters_in_threads(void* arg)
{
	array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices = (array_of_devices_enumerated_by_xinet_t*)arg;
	if (!array_of_xinet_devices || array_of_xinet_devices->count == 0) {
		log_info(L"There are no xinet servers to search for devices on them using xinet");
		return;
	}

	mutex_t* mutex = mutex_init(0);
	array_of_xinet_devices->mutex = mutex;

	mutex_lock(mutex);
	single_thread_launcher(run_xinet_search_on_adapters_in_threads, arg);
	mutex_lock(mutex); // blocks this thread until enumerate controller thread unlocks the mutex after a timeout
	mutex_unlock(mutex);
	mutex_close(mutex);
}


XIMC_RETTYPE XIMC_CALLCONV run_xinet_search_on_adapters_in_threads(void* arg)
{
	array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices = (array_of_devices_enumerated_by_xinet_t*)arg;
	if (!array_of_xinet_devices || array_of_xinet_devices->count == 0) {
		log_info(L"There are no xinet servers to search for devices on them using xinet");
		return (XIMC_RETTYPE)0;
	}

	log_info(L"Start to search for devices using xinet in separate threads...");
	fork_join_with_timeout(discover_using_xinet, array_of_xinet_devices->count, array_of_xinet_devices->xinet_devices, sizeof(devices_enumerated_by_xinet_t),
		XINET_TIMEOUT_MS, array_of_xinet_devices->mutex);
	log_info(L"Search for devices using xinet in separate threads completed");

	lock_xinet_data();
	*(array_of_xinet_devices->xinet_devices[0].thread_expired) = 1;
	unlock_xinet_data();
	return (XIMC_RETTYPE)0;
}


void store_device_name(char* name, void* arg, int connected_via_usb)
{
	device_enumeration_opaque_t* dev_enum = (device_enumeration_opaque_t*)arg;
	int i;
	for (i = 0; i < dev_enum->count; ++i) {
		if (is_same_device(name, dev_enum->raw_names[i])) {
			log_debug(L"Skipping duplicate device '%hs'", name);
			return;
		}
	}

	log_debug(L"Storing device '%hs'", name);
	if (dev_enum->count >= dev_enum->allocated_count) {
		increase_memory_for_dev_enum(dev_enum);
	}

	char* encoded_name = uri_copy(name);
	dev_enum->names[dev_enum->count] = (char*)malloc(MAX_NAME_LENGTH);

	if (connected_via_usb) {
		if (*encoded_name && *encoded_name == '/') {
			/* absolute path - make file:// uri, like xi-com:///dev/tty */
			/* skip first slash for absolute pathes */
			portable_snprintf(dev_enum->names[dev_enum->count], MAX_NAME_LENGTH - 1, "xi-com://%s", encoded_name);
		}
		else {
			/* simple name - make uri with empty host and path component, like xi-com:///COM42
			* use instead more clear URI without hier path, like xi-com:COM42, xi-com:%5C%5C.%5CCOM42 */
			portable_snprintf(dev_enum->names[dev_enum->count], MAX_NAME_LENGTH - 1, "xi-com:%s", encoded_name);
		}
	}
	else
		portable_snprintf(dev_enum->names[dev_enum->count], MAX_NAME_LENGTH - 1, "%s", encoded_name);

	dev_enum->names[dev_enum->count][MAX_NAME_LENGTH - 1] = '\0';
	dev_enum->raw_names[dev_enum->count] = portable_strdup(name);
	free(encoded_name);
	encoded_name = NULL;
	++dev_enum->count;
}


void store_device_name_from_local(char* name, void* arg)
{
	store_device_name(name, arg, 1);
}


void store_device_name_from_network(char* name, void* arg)
{
	store_device_name(name, arg, 0);
}


void store_ssdp_devices_to_dev_enum(array_of_devices_enumerated_by_ssdp_t* array_of_ssdp_devices, device_enumeration_opaque_t* dev_enum)
{
	if (!array_of_ssdp_devices || array_of_ssdp_devices->count == 0) {
		log_debug(L"No devices found using SSDP to save to the general device list dev_enum");
		return;
	}

	int i;
	for (i = 0; i < array_of_ssdp_devices->count; i++) {
		int j;
		for (j = 0; j < array_of_ssdp_devices->ssdp_devices[i].count; j++) {
			store_device_name_from_network(array_of_ssdp_devices->ssdp_devices[i].device_names[j], dev_enum);
		}
	}
}


void store_xinet_devices_to_dev_enum(array_of_devices_enumerated_by_xinet_t* array_of_xinet_devices, device_enumeration_opaque_t* dev_enum)
{
	if (!array_of_xinet_devices || array_of_xinet_devices->count == 0) {
		log_debug(L"No devices found using xinet to save to the general device list dev_enum");
		return;
	}

	int i;
	for (i = 0; i < array_of_xinet_devices->count; i++) {
		devices_enumerated_by_xinet_t* xinet = &(array_of_xinet_devices->xinet_devices[i]);
		int j;
		for (j = 0; j < xinet->count; j++) {
			device_description* desc = (((device_description*)*(xinet->pbuf)) + j);
			char* name = (char*)malloc(MAX_NAME_LENGTH);
			uint8_t* ip_bytes = (uint8_t*)&desc->ipv4;
			portable_snprintf(name, MAX_NAME_LENGTH - 1, "xi-net://%d.%d.%d.%d/%08X", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3], desc->serial);
			name[MAX_NAME_LENGTH - 1] = '\0';
			store_device_name_from_network(name, dev_enum);
			free(name);
			name = NULL;
		}
	}
}


/* Exported function definition */

/* We do not have read/write lock so lock all DE functions with one global lock */

device_enumeration_t XIMC_API enumerate_devices(int enumerate_flags, const char* hints)
{
	device_enumeration_opaque_t* de;
	result_t result;
	lock_global();
	result = enumerate_devices_impl(&de, enumerate_flags, hints);
	unlock_global();
	return result == result_ok ? (device_enumeration_t)de : 0;
}


result_t XIMC_API free_enumerate_devices(device_enumeration_t device_enumeration)
{
	device_enumeration_opaque_t* de = (device_enumeration_opaque_t*)device_enumeration;
	int i;
	lock_global();
	if (de) {
		if (de->names) {
			for (i = 0; i < de->count; ++i) {
				if (de->names[i]) {
					free(de->names[i]);
					de->names[i] = NULL;
				}
			}
			free(de->names);
			de->names = NULL;
		}

		if (de->raw_names) {
			for (i = 0; i < de->count; ++i) {
				if (de->raw_names[i]) {
					free(de->raw_names[i]);
					de->raw_names[i] = NULL;
				}
			}
			free(de->raw_names);
			de->raw_names = NULL;
		}

		if (de->serials) {
			free(de->serials);
			de->serials = NULL;
		}

		if (de->infos) {
			free(de->infos);
			de->infos = NULL;
		}

		if (de->controller_names) {
			free(de->controller_names);
			de->controller_names = NULL;
		}

		if (de->stage_names) {
			free(de->stage_names);
			de->stage_names = NULL;
		}

		if (de->dev_net_infos) {
			free(de->dev_net_infos);
			de->dev_net_infos = NULL;
		}

		de->count = 0;
		free(de);
		de = NULL;
	}
	unlock_global();
	return result_ok;
}


int XIMC_API get_device_count(device_enumeration_t device_enumeration)
{
	device_enumeration_opaque_t* de = (device_enumeration_opaque_t*)device_enumeration;
	if (!de)
		return result_error;

	lock_global();
	return unlocker_global(de->count);
}


pchar XIMC_API get_device_name(device_enumeration_t device_enumeration, int device_index)
{
	device_enumeration_opaque_t* de = (device_enumeration_opaque_t*)device_enumeration;
	if (!de)
		return NULL;

	char* device_name = NULL;
	lock_global();
	if (device_index >= 0 && device_index < de->count) {
		device_name = de->names[device_index];
	}
	unlock_global();
	return device_name;
}


result_t XIMC_API get_enumerate_device_controller_name(device_enumeration_t device_enumeration, int device_index, controller_name_t* controller_name)
{
	device_enumeration_opaque_t* de = (device_enumeration_opaque_t*)device_enumeration;
	result_t result;
	if (!de)
		return result_error;

	lock_global();
	if ((de->flags & ENUMERATE_PROBE) && device_index >= 0 && device_index < de->count) {
		memcpy(controller_name, &de->controller_names[device_index], sizeof(controller_name_t));
		result = result_ok;
	}
	else {
		result = result_error;
	}
	return unlocker_global(result);
}


result_t XIMC_API get_enumerate_device_information(device_enumeration_t device_enumeration, int device_index, device_information_t* device_information)
{
	device_enumeration_opaque_t* de = (device_enumeration_opaque_t*)device_enumeration;
	result_t result;
	if (!de)
		return result_error;

	lock_global();
	if ((de->flags & ENUMERATE_PROBE) && device_index >= 0 && device_index < de->count) {
		memcpy(device_information, &de->infos[device_index], sizeof(device_information_t));
		result = result_ok;
	}
	else {
		result = result_error;
	}
	return unlocker_global(result);
}


result_t XIMC_API get_enumerate_device_network_information(device_enumeration_t device_enumeration, int device_index, device_network_information_t* device_network_information)
{
	device_enumeration_opaque_t* de = (device_enumeration_opaque_t*)device_enumeration;
	result_t result;
	if (!de)
		return result_error;

	lock_global();
	if ((de->flags & ENUMERATE_PROBE) && device_index >= 0 && device_index < de->count) {
		memcpy(device_network_information, &de->dev_net_infos[device_index], sizeof(device_network_information_t));
		result = result_ok;
	}
	else {
		result = result_error;
	}
	return unlocker_global(result);
}


result_t XIMC_API get_enumerate_device_serial(device_enumeration_t device_enumeration, int device_index, uint32_t* serial)
{
	device_enumeration_opaque_t* de = (device_enumeration_opaque_t*)device_enumeration;
	result_t result;
	if (!de)
		return result_error;

	lock_global();
	if ((de->flags & ENUMERATE_PROBE) && device_index >= 0 && device_index < de->count) {
		*serial = de->serials[device_index];
		result = result_ok;
	}
	else {
		result = result_error;
	}
	return unlocker_global(result);
}


result_t XIMC_API get_enumerate_device_stage_name(device_enumeration_t device_enumeration, int device_index, stage_name_t* stage_name)
{
	device_enumeration_opaque_t* de = (device_enumeration_opaque_t*)device_enumeration;
	result_t result;
	if (!de)
		return result_error;

	lock_global();
	if ((de->flags & ENUMERATE_PROBE) && device_index >= 0 && device_index < de->count) {
		memcpy(stage_name, &de->stage_names[device_index], sizeof(stage_name_t));
		result = result_ok;
	}
	else {
		result = result_error;
	}
	return unlocker_global(result);
}


result_t XIMC_API probe_device(const char* uri)
{
	result_t result;
	lock_global();
	result = check_device_by_ximc_information(uri, NULL, NULL, NULL, NULL) ? result_ok : result_nodevice;
	return unlocker_global(result);
}

