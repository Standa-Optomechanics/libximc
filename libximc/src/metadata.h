#ifndef INC_METADATA_H
#define INC_METADATA_H

/*
 * Device metadata
 */

typedef enum { dtUnknown, dtSerial, dtVirtual, dtNet, dtUdp, dtTcp } device_type_t;

struct mutex_t;

typedef struct device_corr_table_t
{
	/* The length of the adjustment table. */
	uint32_t len_table;
	/* Coordinate of the grid. */
	float* X;
	/* Deviation. */
	float* dX;
} device_corr_table_t;

#define VIRTUAL_SCRATCHPAD_SIZE 256

typedef struct device_metadata_t
{
	/* device type */
	device_type_t type;
	/* serial port handle */
	handle_t handle;
	/* serial port timeout */
	int port_timeout;
	/* logical timeout */
	int timeout;
	struct mutex_t* device_mutex;
	/* bindy serial */
	uint32_t serial;
	/* bindy id */
	uint32_t conn_id;
	/* Corrective table. */
	device_corr_table_t table;

	/* virtual devices metadata*/
	/* in-memory device state */
	void *virtual_state;
	/* state file */
	FILE *virtual_file;
	/* virtual scratchpad for request/response data */
	uint8_t virtual_scratchpad[VIRTUAL_SCRATCHPAD_SIZE];
	/* scratchpad size */
	size_t virtual_packet_size;
	/* actual scratchpad size */
	size_t virtual_packet_actual;
} device_metadata_t;

device_metadata_t* get_metadata(device_t device);
void remove_metadata(device_t device);

#endif
