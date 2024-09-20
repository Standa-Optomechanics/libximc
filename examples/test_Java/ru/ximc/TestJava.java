package ru.ximc;
import ru.ximc.libximc.*;

class TestJava
{
	private static final int PROBE_FLAGS = 1 + 4; //ENUMERATE_PROBE | ENUMERATE_NETWORK;
	private static final String ENUM_HINTS = "addr=192.168.0.1,172.16.2.3";

	private static void printState(JXimc.status_t state) {
		System.out.println(
				"state_t:" +
				"\n\tCurSpeed=" + state.CurSpeed +
				"\n\tCurPosition=" + state.CurPosition +
				"\n\tUpwr=" + state.Upwr +
				"\n\tIpwr=" + state.Ipwr +
				"\n\tFlags=" + state.Flags +
				"\n");
	}

	private static void printEngineSettings(JXimc.engine_settings_t engine_settings) {
				System.out.println("engine_settings:" + 
						"\n\tNomVoltage=" + engine_settings.NomVoltage +
						"\n\tNomCurrent=" + engine_settings.NomCurrent +
						"\n\tNomSpeed=" + engine_settings.NomSpeed +
						"\n");
	}

	public static void main(String args[])
	{
		try {
			String jniName = System.getProperty("os.name").toLowerCase().startsWith("windows") ? "libjximc" : "jximc";
			System.loadLibrary(jniName);
			System.out.println("Hello! I am a stupid test program");
			System.out.println("libximc version " + JXimc.ximc_version());
			System.out.println("Arch " + System.getProperty("os.arch"));

			for (String arg : args) {
				System.out.println(" " + arg);
			}

			String deviceName = "";

			// Find the device

			boolean deviceSpecified = args.length == 1 && !args[0].startsWith("-psn");
			if (deviceSpecified)
				deviceName = args[0];
			
			long deviceEnumeration = JXimc.enumerate_devices(PROBE_FLAGS, ENUM_HINTS);
			if (deviceEnumeration == 0)
				throw new RuntimeException("Error enumerating devices");
			int namesCount = JXimc.get_device_count(deviceEnumeration);
			for (int i = 0; i < namesCount; i++) {
				String name = JXimc.get_device_name(deviceEnumeration, i);
				if (name == null)
					throw new RuntimeException("Should not happen");
				System.out.println("device: " + name);
			}

			if (!deviceSpecified)
			{
				if (namesCount == 0)
					throw new RuntimeException("Not enough devices");
				deviceName = JXimc.get_device_name(deviceEnumeration, 0);
			}

			JXimc.free_enumerate_devices(deviceEnumeration);
			System.out.println("Opening device " + deviceName + "...");

			int device = JXimc.open_device(deviceName);
			if (device == JXimc.DEVICE_UNDEFINED)
				throw new RuntimeException("Cannot open a device");

			// Run simple tests

			try {

				printState(JXimc.get_status(device));

				JXimc.command_zero(device);

				printState(JXimc.get_status(device));

				System.out.println("\nNow engine will rotate to the left for 5 seconds...\n");

				JXimc.command_zero(device);
				JXimc.command_left(device);

				JXimc.msec_sleep(5*1000);
				printState(JXimc.get_status(device));

				JXimc.status_t status = JXimc.get_status(device);
				
				JXimc.command_move(device, 0, (short)0);
				System.out.println("\nWaiting for stop...\n");

				JXimc.command_wait_for_stop(device, 100);
				printState(JXimc.get_status(device));

				System.out.println("\nTesting get/set commands...\n");
				JXimc.engine_settings_t engine_settings = JXimc.get_engine_settings(device);
				printEngineSettings(engine_settings);
				engine_settings.NomSpeed += 1;
				JXimc.set_engine_settings(device, engine_settings);
				printEngineSettings(engine_settings);
				engine_settings.NomSpeed -= 1;
				JXimc.set_engine_settings(device, engine_settings);
				printEngineSettings(engine_settings);

				JXimc.calibration_t calb = new JXimc.calibration_t();
				calb.A = 1.0;
				calb.MicrostepMode = 1;
				JXimc.engine_settings_calb_t engine_settings_calb = JXimc.get_engine_settings_calb(device, calb);

				JXimc.msec_sleep(3*1000);


			} finally {
				System.out.println("Closing device");
				JXimc.close_device(device);
			}

		} catch (RuntimeException e) {
			System.err.println("XIMC failed with " + e.toString());
			e.printStackTrace();
		}

		System.out.println("Done");
	}
}
/* vim: set ts=2 sw=2: */
