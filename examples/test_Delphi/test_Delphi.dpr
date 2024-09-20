program test_Delphi;

{$APPTYPE CONSOLE}

uses
  SysUtils,
  ximc;

type
        XimcException = class(Exception);

procedure ShowStatus(dev: Device);
var
        res: XimcResult;
        status: ximc.status_t;
begin
        res := get_status(dev, status);
        if res <> ResultOk then
                raise XimcException.Create('Result ' + IntToStr(res));
        WriteLn(
                ' rpm: ' + IntToStr(status.CurSpeed) +
                ' pos: ' + IntToStr(status.CurPosition) +
                ' flags: ' + IntToStr(status.Flags));
end;

procedure ShowStatusCalb(dev: Device);
var
        res: XimcResult;
		engine_settings: ximc.engine_settings_t;
		status_calb: ximc.status_calb_t;
		calibration: ximc.calibration_t;
begin
        res := get_engine_settings(dev, engine_settings);
        if res <> ResultOk then
                raise XimcException.Create('Result ' + IntToStr(res));
        calibration.MicrostepMode := engine_settings.MicrostepMode;
        calibration.A := 0.1;
        res := get_status_calb(dev, status_calb, calibration);
        if res <> ResultOk then
                raise XimcException.Create('Result ' + IntToStr(res));
        WriteLn(
                ' calb speed: ' + FloatToStr(status_calb.CurSpeed) + ' mm/s ' +
                ' calb pos: ' + FloatToStr(status_calb.CurPosition) + ' mm '
		);
end;


procedure Test;
var
        res: XimcResult;
        dev: Device;
        device_names: StringArray;
        device_count, i: Integer;
        dev_enum: Pointer;
        probe_flags: Integer;
        enum_hints: String;
begin
        WriteLn('Hello');
        dev := DeviceUndefined;
        device_names := nil;

        try
                probe_flags := 1 + 4; // ENUMERATE_PROBE | ENUMERATE_NETWORK
                enum_hints := 'addr=192.168.0.1,172.16.2.3';

                dev_enum := enumerate_devices(probe_flags, pansichar(enum_hints));
                device_count := get_device_count(dev_enum);
                SetLength(device_names, device_count);
                for i := 0 to device_count-1 do
                        device_names[i] := get_device_name(dev_enum, i);

                WriteLn('Found devices... ', Length(device_names));
                if Length(device_names) = 0 then
                        raise XimcException.Create('No devices found');

                WriteLn('Using device ', device_names[0]);
                dev := open_device(PAnsiChar(device_names[0]));
                if (dev = DeviceUndefined) then
                        raise XimcException.Create('No such device');
                WriteLn('Device is ', dev);
                ShowStatus(dev);

                WriteLn('Running...');
                res := command_right(dev);
                if (res <> ResultOk) then
                        raise XimcException.Create('Error command ' + IntToStr(res));

                Sleep(1*1000);
                ShowStatus(dev);
                ShowStatusCalb(dev);
                Sleep(10*1000);
                ShowStatus(dev);

                WriteLn('Stopping...');
                res := command_stop(dev);
                if (res <> ResultOk) then
                        raise XimcException.Create('Error command ' + IntToStr(res));
                ShowStatus(dev);

                close_device(dev);
        except
                on exc: XimcException do begin
                        WriteLn('Error happens ', exc.Message);
                        if dev <> DeviceUndefined then
                                close_device(dev);
                end
        end;
        WriteLn('Done.');
end;

begin
        Test;
end.
