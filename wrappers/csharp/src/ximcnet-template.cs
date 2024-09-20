using System;
using System.Runtime.InteropServices;
using System.Text;

namespace ximc
{
	public enum Result { ok = 0, error = -1, not_implemented = -2, value_error = -3, no_device = -4 };

	[StructLayout(LayoutKind.Sequential)]
	public struct calibration_t
	{
		public double A;
		public uint MicrostepMode;
	};
	
	/* @@GENERATED_CODE@@ */


	public partial class API
	{   
		private class Impl
		{
			[DllImport("libximc.dll")]
			public static extern IntPtr get_device_name(IntPtr device_enumeration, int device_index);
		};

		[DllImport("libximc.dll", CharSet = CharSet.Ansi)]
		public static extern int open_device ([MarshalAs(UnmanagedType.LPStr)] String name);
		[DllImport("libximc.dll")]
		public static extern Result close_device (ref int id);
		[DllImport("libximc.dll", CharSet = CharSet.Ansi)]
		public static extern Result probe_device ([MarshalAs(UnmanagedType.LPStr)] String name);
		
		[DllImport("libximc.dll")]
		public static extern Result reset_locks ();
		[DllImport("libximc.dll", CharSet = CharSet.Ansi)]
		public static extern Result ximc_fix_usbser_sys ([MarshalAs(UnmanagedType.LPStr)] String name);
		[DllImport("libximc.dll")]
		public static extern Result get_device_information (int id, out device_information_t device_information);
		[DllImport("libximc.dll")]
		public static extern Result get_status (int id, out status_t status);
		[DllImport("libximc.dll")]
		public static extern Result get_status_calb (int id, out status_calb_t status, ref calibration_t calibration);
		[DllImport("libximc.dll")]
		public static extern Result goto_firmware(int id, out byte ret);
		[DllImport("libximc.dll", CharSet = CharSet.Ansi)]
		public static extern Result has_firmware([MarshalAs(UnmanagedType.LPStr)] String name, out byte ret);
		[DllImport("libximc.dll", CharSet = CharSet.Ansi)]
		public static extern Result write_key([MarshalAs(UnmanagedType.LPStr)] String name, byte[] key);
		[DllImport("libximc.dll")]
		public static extern Result command_reset(int id);
		[DllImport("libximc.dll", CharSet = CharSet.Ansi)]
		public static extern Result command_update_firmware([MarshalAs(UnmanagedType.LPStr)] String name, byte[] data, uint data_size);
		[DllImport("libximc.dll")]
		public static extern void msec_sleep(int id);
		[DllImport("libximc.dll", CharSet = CharSet.Ansi)]
		public static extern void ximc_version(StringBuilder version);
		
		[DllImport("libximc.dll", CharSet = CharSet.Ansi)]
		public static extern IntPtr enumerate_devices(int enumerate_flags, [MarshalAs(UnmanagedType.LPStr)] String hints);
		[DllImport("libximc.dll")]
		public static extern Result enumerate_devices(IntPtr device_enumeration);
		[DllImport("libximc.dll")]
		public static extern Result free_enumerate_devices(IntPtr device_enumeration);
		[DllImport("libximc.dll")]
		public static extern int get_device_count(IntPtr device_enumeration);

		public static String get_device_name(IntPtr device_enumeration, int device_index)
		{
			return System.Runtime.InteropServices.Marshal.PtrToStringAnsi(
					Impl.get_device_name(device_enumeration, device_index));
		}


		[DllImport("libximc.dll")]
		public static extern Result get_enumerate_device_information(IntPtr device_enumeration, int device_index, 
				out device_information_t device_information);
		[DllImport("libximc.dll")]
		public static extern Result get_enumerate_device_serial(IntPtr device_enumeration, int device_index,
				out UInt32 serial);

		public enum LogLevel { error = 0x01, warning = 0x02, info = 0x03, debug = 0x04 };

		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		public delegate void LoggingCallback ([MarshalAs(UnmanagedType.I4)] LogLevel loglevel,
				[MarshalAs(UnmanagedType.LPWStr)] string message, IntPtr user_data);
		
		[DllImport("libximc.dll")]
		public static extern void set_logging_callback([MarshalAs(UnmanagedType.FunctionPtr)] LoggingCallback logging_callback, IntPtr user_data);
	
		[DllImport("libximc.dll")]
		public static extern Result command_wait_for_stop(int id, int wait_interval_ms);

		[DllImport("libximc.dll")]
		public static extern Result set_bindy_key([MarshalAs(UnmanagedType.LPStr)] String keyfilepath);
	};

};

