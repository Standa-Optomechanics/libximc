using System;
using System.Text;
using System.Threading;
using System.Reflection;
using ximc;

namespace test_CSharp
{
    class testapp
    {
        private static API.LoggingCallback callback;
        private static void MyLog (API.LogLevel loglevel, string message, IntPtr user_data)
        {
            Console.WriteLine("MyLog {0}: {1}", loglevel, message);
        }

        private static void print_status(status_t status)
        {
            Console.WriteLine("rpm: {0} pos: {1} flags: {2}",
                        status.CurSpeed, status.CurPosition, status.Flags);
        }

        private static void print_status_calb(status_calb_t status_calb)
        {
            Console.WriteLine("speed: {0} {3}/s position: {1} {3} flags: {2}",
                        status_calb.CurSpeed, status_calb.CurPosition, status_calb.Flags, "mm");
        }

        static void Main(string[] args)
        {
            int device = -1;
            Result res;
            try
            {
                Console.WriteLine("testapp CLR runtime version: " + Assembly.GetExecutingAssembly().ImageRuntimeVersion);
                foreach (Assembly a in AppDomain.CurrentDomain.GetAssemblies())
                    if (a.GetName().Name.Equals("ximcnet"))
                        Console.WriteLine("ximcnet CLR runtime version: " + a.ImageRuntimeVersion);
                Console.WriteLine("Current CLR runtime version: " + Environment.Version.ToString());

                callback = new API.LoggingCallback(MyLog);
                API.set_logging_callback(callback, IntPtr.Zero);

                // Pointer to device enumeration structure
                IntPtr device_enumeration;

                // Probe flags, used to enable various enumeration options
                const int probe_flags = (int)(Flags.ENUMERATE_PROBE | Flags.ENUMERATE_NETWORK);

                // Enumeration hint, currently used to indicate ip address for network enumeration
                String enumerate_hints = "addr=192.168.1.1,172.16.2.3";
                // String enumerate_hints = "addr="; // this hint will use broadcast enumeration, if ENUMERATE_NETWORK flag is enabled

                // Enumerates all devices
                device_enumeration = API.enumerate_devices(probe_flags, enumerate_hints);
                if (device_enumeration == null)
                    throw new Exception("Error enumerating devices");

                // Gets found devices count
                int device_count = API.get_device_count(device_enumeration);

                // List all found devices
                for (int i = 0; i < device_count; ++i)
                {
                    // Gets device name 
                    String dev = API.get_device_name(device_enumeration, i);
                    System.Console.WriteLine("Found device {0}", dev);
                }
        
                // Get first device name or command-line arg
                String deviceName;
                if (device_count > 0)
                    deviceName = API.get_device_name(device_enumeration, 0);
                else if (args.Length > 0)
                    deviceName = args[0];
                else
                    throw new Exception("No devices");
                System.Console.WriteLine("Using device {0}", deviceName);

                // Open this device
                device = API.open_device(deviceName);
                Console.WriteLine("Device {0}", device);

                StringBuilder versb = new StringBuilder(256);
                API.ximc_version(versb);
                Console.WriteLine("XIMC version: {0}", versb.ToString());

                status_t status;
                // Read device status
                res = API.get_status(device, out status);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());
                print_status(status);

                device_information_t di;
                res = API.get_device_information(device, out di);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());
                Console.WriteLine("Manufacturer {0}", di.Manufacturer);

                // Send "zero" command to device
                res = API.command_zero(device);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());
                Thread.Sleep(2);

                // Read device status
                res = API.get_status(device, out status);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());
                print_status(status);

                Console.WriteLine("running...");
                // Send "move to the right" command to the device
                res = API.command_right(device);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());

                int shift = 0;
                Thread.Sleep(3 * 1000);
                // Read device status
                res = API.get_status(device, out status);
                shift -= status.CurPosition;
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());
                print_status(status);

                status_calb_t status_calb;
                engine_settings_t engine_settings;
                res = API.get_engine_settings(device, out engine_settings);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());

                calibration_t calibration = new calibration_t();
                calibration.A = 0.1;
                calibration.MicrostepMode = Math.Max(1, engine_settings.MicrostepMode);

                // Read calibrated device status
                res = API.get_status_calb(device, out status_calb, ref calibration);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());
                print_status_calb(status_calb);

                res = API.get_status(device, out status);
                shift += status.CurPosition;
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());

                Console.WriteLine("waiting for stop...");
                res = API.command_move(device, shift, 0);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());
                // Wait for stop of the device
                res = API.command_wait_for_stop(device, 100);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());

                // Read device status
                res = API.get_status(device, out status);
                if (res != Result.ok)
                    throw new Exception("Error " + res.ToString());
                print_status(status);

                Console.WriteLine("Done...");
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception " + e.Message);
                Console.WriteLine(e.StackTrace.ToString());
            }
            finally
            {
                if (device != -1)
                    API.close_device(ref device);
            }
            Console.WriteLine("Press Enter to exit...");
            Console.ReadLine();
        }
    }
}
