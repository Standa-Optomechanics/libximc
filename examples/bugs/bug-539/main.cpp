#include <ximc.h>
#include <errno.h>
#include <iostream>
#include <time.h>
using namespace std;

void msleep(int msec)
{
	// POSIX 1.b
	struct timespec ts;
	ts.tv_sec = msec / 1E3;
	ts.tv_nsec = msec*1E6 - ts.tv_sec*1E9;
	if (nanosleep( &ts, NULL ) != 0)
		fprintf( stderr, "nanosleep failed with %d\n", errno );
}

int main(int argc, char* argv[])
{
	int name_count;
	char **names;
	result_t result;
	
	/*//enumerate devices
	if(enumerate_devices(&name_count, &names, 0) == result_ok)
		printf("enumerate ok\r\n");
	else{
		printf("enumerate error\r\n");
		cin.get();
		return 0;
	}

	if(name_count == 0){
		printf("no devices\r\n");
		cin.get();
		return 0;
	}*/

	//open device
	device_t device = open_device(argv[0]);
	if(device > 0)
		printf("open device ok\r\n");
	else{
		printf("open device error\r\n");
		//free_enumerate_devices(name_count, names);
		cin.get();
		return 0;
	}

	//init
	if(init_device(device) == result_ok)
		printf("init ok\r\n");
	else{
		printf("init error\r\n");
		free_enumerate_devices(name_count, names);
		cin.get();
		return 0;
	}
	
	msleep( 1000 );

	state_t state;
	if(get_status(device, &state) == result_ok)
	{
		printf("status: %d", state.Sts);
		switch (state.Sts)
		{
			case DevStateOff: printf( "off" ); break;
			case DevStateStop: printf( "stop" ); break;
			case DevStateMoving: printf( "run" ); break;
			case DevStateTune: printf( "tune" ); break;
			case DevStateHoming: printf( "homing" ); break;
			case DevStateCalibr: printf( "calibrating" ); break;
			case DevStateAlarm: printf( "alarm" ); break;
			default: printf( "UNKNOWN" );
		}
		printf("\r\n");
	}
	else
		printf("GETS error\r\n");
	
	msleep( 1000 );

	//get device info
	char man[16], man_id[4], desc[16];
	if(get_device_information(device, man, man_id, desc) == result_ok)
		printf("GETI ok (%s, %s, %s)\r\n", man, man_id, desc);
	else 
		printf("GETI error\r\n");

	//get serial
	uint32_t serial;
	if(get_serial_number(device, &serial) == result_ok)
		printf("GSER ok (%i)\r\n", serial);
	else
		printf("GSER error\r\n");

	//set serial
	if(set_serial_number(device, serial, 3712757057) == result_ok)
		printf("SSER ok (%i)\r\n", serial);
	else{
		printf("SSER error\r\n");
		result = set_serial_number(device, serial, 3712757057);
	}

	//has firmware
	uint8_t ret;
	if(has_firmware(device, &ret) == result_ok)
		if(ret != 0)printf("  G1 ok (firmware is loaded)\r\n");
		else printf("  G1 ok (firmware is not loaded)\r\n");
	else
		printf("  G1 error\r\n");

	//get firmware version
	unsigned int major, minor, release;
	if(get_firmware_version(device, &major, &minor, &release) == result_ok)
		printf("GFMW ok (%i.%i.%i)\r\n", major, minor, release);
	else 
		printf("GFMW error\r\n");
	
	msleep( 1000 );


	//move
	if(command_move(device, 1000, 0) == result_ok)
		printf("MOVE ok\r\n");
	else
		printf("MOVE error\r\n");

	//movr
	if(command_movr(device, 1000, 0) == result_ok)
		printf("MOVR ok\r\n");
	else
		printf("MOVR error\r\n");

	//loft
	if(command_loft(device) == result_ok)
		printf("LOFT ok\r\n");
	else
		printf("LOFT error\r\n");
	
	msleep( 1000 );

	//left
	if(command_left(device) == result_ok)
		printf("LEFT ok\r\n");
	else
		printf("LEFT error\r\n");

	//right
	if(command_right(device) == result_ok)
		printf("RIGT ok\r\n");
	else
		printf("RIGT error\r\n");

	//home
	if(command_home(device) == result_ok)
		printf("HOME ok\r\n");
	else
		printf("HOME error\r\n");

	
	msleep( 1000 );

	//stop
	if(command_stop(device) == result_ok)
		printf("STOP ok\r\n");
	else{
		printf("STOP error\r\n");
		result=command_stop(device);
	}

	//zero
	if(command_zero(device) == result_ok)
		printf("ZERO ok\r\n");
	else
		printf("ZERO error\r\n");

	//SENC
	if(command_setencoder(device, 0) == result_ok)
		printf("SENC ok\r\n");
	else
		printf("SENC error\r\n");

	
	msleep( 1000 );

	//get status
	if(get_status(device, &state) == result_ok)
		printf("GETS ok\r\n");
	else
		printf("GETS error\r\n");

	//get chart data
	chart_data_t chart_data;
	if(get_chart_data(device, &chart_data) == result_ok)
		printf("GETC ok\r\n");
	else
		printf("GETC error\r\n");

	//get additional data
	additional_state_t additional_state;
	if(get_additional_status(device, &additional_state) == result_ok)
		printf("GETA ok\r\n");
	else
		printf("GETA error\r\n");

	//analog data
	analog_data_t ad;
	if(get_analog_data(device, &ad) == result_ok)
		printf("RDAN ok\r\n");
	else
		printf("RDAN error\r\n");

	//get pwm freq
	unsigned int pwm_freq=15000;
	if(get_pwm_freq(device, &pwm_freq) == result_ok)
		printf("GPWM ok\r\n");
	else
		printf("GPWM error\r\n");

	//set pwm freq
	if(get_pwm_freq(device, &pwm_freq) == result_ok)
		printf("SPWM ok\r\n");
	else
		printf("SPWM error\r\n");

	//get feedback settings
	unsigned int ips=1000, feedback_type=0, feedback_flags=0;
	if(get_feedback_settings (device, &ips, &feedback_type, &feedback_flags) == result_ok)
		printf("GFBS ok\r\n");
	else
		printf("GFBS error\r\n");

	//set feedback settings
	if(set_feedback_settings (device, ips, feedback_type, feedback_flags) == result_ok)
		printf("SFBS ok\r\n");
	else
		printf("SFBS error\r\n");

	//get move settings
	move_settings_t move_settings;
	if(get_move_settings(device, &move_settings) == result_ok)
		printf("GMOV ok\r\n");
	else
		printf("GMOV error\r\n");

	//set move settings
	if(set_move_settings(device, &move_settings) == result_ok)
		printf("SMOV ok\r\n");
	else
		printf("SMOV error\r\n");

	//get engine settings
	engine_settings_t engine_settings;
	if(get_engine_settings (device, &engine_settings) == result_ok)
		printf("GENG ok\r\n");
	else
		printf("GENG error\r\n");

	//set engine settings
	if(set_engine_settings (device, &engine_settings) == result_ok)
		printf("SENG ok\r\n");
	else
		printf("SENG error\r\n");

	//get engine type settings
	unsigned int engine_type, driver_type;
	if(get_entype_settings (device, &engine_type, &driver_type) == result_ok)
		printf("GENT ok\r\n");
	else
		printf("GENT error\r\n");

	//set engine type settings
	if(set_entype_settings (device, engine_type, driver_type) == result_ok)
		printf("SENT ok\r\n");
	else
		printf("SENT error\r\n");

	//get power settings
	power_settings_t power_settings;
	if(get_power_settings (device, &power_settings) == result_ok)
		printf("GPWR ok\r\n");
	else
		printf("GPWR error\r\n");

	//set power settings
	if(set_power_settings (device, &power_settings) == result_ok)
		printf("SPWR ok\r\n");
	else
		printf("SPWR error\r\n");

	//get brake settings
	brake_settings_t brake_settings;
	if(get_brake_settings (device, &brake_settings) == result_ok)
		printf("GBRK ok\r\n");
	else
		printf("GBRK error\r\n");

	//set brake settings
	if(set_brake_settings (device, &brake_settings) == result_ok)
		printf("SBRK ok\r\n");
	else
		printf("SBRK error\r\n");

	//get control settings
	control_settings_t control_settings;
	if(get_control_settings (device, &control_settings) == result_ok)
		printf("GCTL ok\r\n");
	else
		printf("GCTL error\r\n");

	//set control settings
	if(set_control_settings (device, &control_settings) == result_ok)
		printf("SCTL ok\r\n");
	else
		printf("SCTL error\r\n");

	//get control position settings
	ctp_settings_t ctp_settings;
	if(get_ctp_settings (device, &ctp_settings) == result_ok)
		printf("GCTP ok\r\n");
	else
		printf("GCTP error\r\n");

	//set control position settings
	if(set_ctp_settings (device, &ctp_settings) == result_ok)
		printf("SCTP ok\r\n");
	else
		printf("SCTP error\r\n");

	//get secure settings
	unsigned int critical_curr, critical_voltage, critical_temp;
	if(get_secure_settings (device, &critical_curr, &critical_voltage, &critical_temp) == result_ok)
		printf("GSEC ok\r\n");
	else
		printf("GSEC error\r\n");

	//set secure settings
	if(set_secure_settings (device, critical_curr, critical_voltage, critical_temp) == result_ok)
		printf("SSEC ok\r\n");
	else
		printf("SSEC error\r\n");

	//get edges settings
	edges_settings_t edges_settings;
	if(get_edges_settings (device, &edges_settings) == result_ok)
		printf("GEDS ok\r\n");
	else
		printf("GEDS error\r\n");

	//set edges settings
	if(set_edges_settings (device, &edges_settings) == result_ok)
		printf("SEDS ok\r\n");
	else
		printf("SEDS error\r\n");

	//get pid settings
	pid_settings_t pid_settings;
	if(get_pid_settings (device, &pid_settings) == result_ok)
		printf("GPID ok\r\n");
	else
		printf("GPID error\r\n");

	//set pid settings
	if(set_pid_settings (device, &pid_settings) == result_ok)
		printf("SPID ok\r\n");
	else
		printf("SPID error\r\n");

	//get sync settings
	sync_settings_t sync_settings;
	if(get_sync_settings (device, &sync_settings) == result_ok)
		printf("GSYN ok\r\n");
	else
		printf("GSYN error\r\n");

	//set sync settings
	if(set_sync_settings (device, &sync_settings) == result_ok)
		printf("SSYN ok\r\n");
	else
		printf("SSYN error\r\n");

	//get calibration coeffs
	calibration_settings_t cs;
	if(get_calibration_coeffs(device, &cs) == result_ok)
		printf("GCAL ok\r\n");
	else
		printf("GCAL error\r\n");

	//set calibration coeffs
	if(set_calibration_coeffs(device, &cs) == result_ok)
		printf("SCAL ok\r\n");
	else
		printf("SCAL error\r\n");

	//get home settings
	home_settings_t home_settings;
	if(get_home_settings (device, &home_settings) == result_ok)
		printf("GHOM ok\r\n");
	else
		printf("GHOM error\r\n");

	//set home settings
	if(set_home_settings (device, &home_settings) == result_ok)
		printf("SHOM ok\r\n");
	else
		printf("SHOM error\r\n");

	//read settings
	clock_t t1 = clock();
	if(command_read_settings(device) == result_ok)
		printf("READ ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("READ error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	//save settings
	t1 = clock();
	if(command_save_settings(device) == result_ok)
		printf("SAVE ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("SAVE error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	//EE read settings
	t1 = clock();
	if(command_eeread_settings(device) == result_ok)
		printf("EERD ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("EERD error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	//EE save settings
	t1 = clock();
	if(command_eesave_settings(device) == result_ok)
		printf("EESV ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("EESV error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	unsigned int uart_data;
	if(command_read_uart(device, &uart_data) == result_ok)
		printf("RDUT ok\r\n");
	else{
		printf("RDUT error\r\n");
		result = command_read_uart(device, &uart_data);
	}

	if(command_write_uart(device, uart_data) == result_ok)
		printf("WRUT ok\r\n");
	else
		printf("WRUT error\r\n");

	//get stage settings
	stage_information_t stage_information;
	dc_information_t dc_information;
	step_information_t step_information;
	encoder_information_t encoder_information;
	gear_information_t gear_information;

	t1 = clock();
	if(get_stage_information (device, &stage_information) == result_ok)
		printf("GSTG ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("GSTG error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	t1 = clock();
	if(get_dc_information (device, &dc_information) == result_ok)
		printf("GDCI ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("GDCI error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	t1 = clock();
	if(get_step_information (device, &step_information) == result_ok)
		printf("GSTI ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("GSTI error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	t1 = clock();
	if(get_encoder_information (device, &encoder_information) == result_ok)
		printf("GENI ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("GENI error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	t1 = clock();
	if(get_gear_information (device, &gear_information) == result_ok)
		printf("GGRI ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("GGRI error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	//set stage settings
	t1 = clock();
	if(set_stage_information (device, &stage_information) == result_ok)
		printf("SSTG ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("SSTG error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	t1 = clock();
	if(set_dc_information (device, &dc_information) == result_ok)
		printf("SDCI ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("SDCI error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	t1 = clock();
	if(set_step_information (device, &step_information) == result_ok)
		printf("SSTI ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("SSTI error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	t1 = clock();
	if(set_encoder_information (device, &encoder_information) == result_ok)
		printf("SENI ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("SENI error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	t1 = clock();
	if(set_gear_information (device, &gear_information) == result_ok)
		printf("SGRI ok (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);
	else
		printf("SGRI error (%i msec)\r\n", 1000*(clock()-t1)/CLOCKS_PER_SEC);

	//calibrate
	if(command_calibrate(device) == result_ok)
		printf("CALB ok\r\n");
	else
		printf("CALB error\r\n");

	//reset
	/*if(command_reset(device) == result_ok)
		printf("RESET ok\r\n");
	else
		printf("RESET error\r\n");
*/
	printf("\r\n");

	if(reset_locks() == result_ok)
		printf("reset locks ok\r\n");
	else
		printf("reset locks error\r\n");

	if(close_device(device) == result_ok)
		printf("close device ok\r\n");
	else
		printf("close device error\r\n");

/*	if(free_enumerate_devices(name_count, names) == result_ok)
		printf("memory free ok\r\n");
	else
		printf("memory free error\r\n");*/
	cin.get();
	return 0;
}

