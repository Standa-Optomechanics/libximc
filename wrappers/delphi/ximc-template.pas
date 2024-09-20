unit ximc;

interface

uses Windows, Sysutils;

const
	XimcDll = 'libximc.dll';

type
	Device = integer;

type
	XimcResult = integer;

type
	StringArray = array of string;

const
	DeviceUndefined = -1;

const
	ResultOk = 0;
	ResultError = -1;
	ResultNotImplemented = -2;
	ResultValueError = -3;
	ResultNoDevice = -4;

type
	PPAnsiChar = ^PAnsiChar;

type calibration_t = record
	A: double;
	MicrostepMode: integer;
end;

{ @@GENERATED_CODE@@ }

function open_device (name: PAnsiChar) : Device; stdcall; external XimcDll;
function close_device (var id: Device) : XimcResult; stdcall; external XimcDll;
function probe_device (name: PAnsiChar) : XimcResult; stdcall; external XimcDll;

function enumerate_devices(enumerate_flags: Integer; hints: PAnsiChar) : Pointer; 
				stdcall; external XimcDll;
function free_enumerate_devices(enumerate_devices: Pointer) : XimcResult; 
				stdcall; external XimcDll;
function get_device_count(device_enumeration: Pointer) : Integer;
				stdcall; external XimcDll;
function get_device_name(device_enumeration: Pointer; device_index: Integer) : PAnsiChar;
				stdcall; external XimcDll;
function get_enumerate_device_serial(device_enumeration: Pointer; device_index: Integer; 
				out serial: Cardinal) : XimcResult; stdcall; external XimcDll;
function get_enumerate_device_information(device_enumeration: Pointer; device_index: Integer; 
				out device_information: device_information_t) : XimcResult; stdcall; external XimcDll;

function reset_locks () : XimcResult; stdcall; external XimcDll;
function ximc_fix_usbser_sys (name: PAnsiChar) : XimcResult; stdcall; external XimcDll;
function get_device_information (id: Device; out device_information: device_information_t) : XimcResult; stdcall; external XimcDll;
function get_status (id: Device; out status: status_t) : XimcResult; stdcall; external XimcDll;
function get_status_calb (id: Device; out status: status_calb_t; var calibration: calibration_t) : XimcResult; stdcall; external XimcDll;

function goto_firmware(id: Device; out ret: Byte) : XimcResult; stdcall; external XimcDll;
function has_firmware(name: PAnsiChar; out ret: Byte) : XimcResult; stdcall; external XimcDll;
function write_key(name: PAnsiChar; key: Cardinal) : XimcResult; stdcall; external XimcDll;
function command_update_firmware(name: PAnsiChar; data: PByte; data_size: Cardinal) : XimcResult; stdcall; external XimcDll;
function command_reset(id: Device) : XimcResult; stdcall; external XimcDll;

procedure msec_sleep(msec: Cardinal); stdcall; external XimcDll;
procedure ximc_version(out version: PAnsiChar); stdcall; external XimcDll;

function command_wait_for_stop(id: Device; wait_interval_ms: Integer) : XimcResult; stdcall; external XimcDll;
procedure set_bindy_key(keyfilepath: PAnsiChar); stdcall; external XimcDll;

implementation

end.
