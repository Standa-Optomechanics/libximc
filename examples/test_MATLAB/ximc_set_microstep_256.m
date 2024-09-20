function [ ] = ximc_set_microstep_256(device_id)

% To use the libximc library commands get_engine_settings, set_engine_settings,
% you need to initialize the engine_settings_t structure used by the commands.
% Here is a trick.
% It is necessary to initiate an arbitrary structure(dummy_struct) with a field that is present in the structure engine_settings_t.
% Based on which the engine_settings_t structure should be set.
dummy_struct = struct('MicrostepMode',0);
parg_struct = libpointer('engine_settings_t', dummy_struct);

% read current engine settings from motor
[result, engine_settings] = calllib('libximc','get_engine_settings', device_id, parg_struct);

clear parg_struct
if result ~= 0
    disp(['Command failed with code', num2str(result)]);
end

% 9 is the value of the constant MICROSTEP_MODE_FRAC_256
% See all MICROSTEP_MODE_FRAC_ constants in documentation
% For example, you should write 5 instead of 9 for MICROSTEP_MODE_FRAC_16
engine_settings.MicrostepMode = 9;

% write engine settings to controller
result= calllib('libximc', 'set_engine_settings', device_id, engine_settings)
if result ~= 0
    disp(['Command failed with code', num2str(result)]);
end

end