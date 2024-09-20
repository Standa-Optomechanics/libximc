package ru.ximc.libximc;

@SuppressWarnings("serial")
abstract class XimcException extends RuntimeException {
    public XimcException(String message) {
        super(message);
    }
}

/** libximc wrapper */
public class JXimc {

	/** Special device id */
	public static final int DEVICE_UNDEFINED = -1;

	public static class calibration_t
	{
		public double A;
		public int MicrostepMode;
	}

	public static class firmware_version_t
	{
		public int major;
		public int minor;
		public short release;
	}

	public static class bootloader_version_t
	{
		public int major;
		public int minor;
		public short release;
	}

/* @@GENERATED_CODE@@ */

	/** Hand-crafted functions */
	public static native int open_device (String name);
	public static native void close_device (int id);
	public static native void probe_device (String name);

	public static native void reset_locks ();
	public static native void ximc_fix_usbser_sys (String name);
	public static native boolean goto_firmware(int id);
	public static native boolean has_firmware(String name);
	public static native void write_key(String name, byte[] key);
	public static native void command_reset(int id);
	public static native void command_update_firmware(String name, byte[] data, int data_size);
	public static native void msec_sleep(int id);
	public static native String ximc_version();

	public static native long enumerate_devices(int enumerate_flags, String hints);
	public static native void free_enumerate_devices(long device_enumeration);
	public static native int get_device_count(long device_enumeration);

	public static native String get_device_name(long device_enumeration, int device_index);
	public static native device_information_t get_enumerate_device_information(long device_enumeration, int device_index);
	public static native long get_enumerate_device_serial(long device_enumeration, int device_index);

	public static native status_t get_status (int id);
	public static native status_calb_t get_status_calb (int id, calibration_t calibration);
	public static native device_information_t get_device_information (int id);
	
	public static native void command_wait_for_stop(int id, int wait_interval_ms);
	public static native void set_bindy_key(String keyfilepath);
}

// vim: ts=4 shiftwidth=4
