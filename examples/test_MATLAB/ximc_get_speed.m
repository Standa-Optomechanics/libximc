function [ speed, uspeed ] = ximc_get_speed(device_id)

% To use the libximc library command get_move_settings,
% you need to initialize the move_settings_t structure used by the commands.
% Here is a trick.
% It is necessary to initiate an arbitrary structure(dummy_struct) with a field that is present in the structure move_settings_t.
% Based on which the move_settings_t structure should be set.
dummy_struct = struct('Speed',0);
parg_struct = libpointer('move_settings_t', dummy_struct);

% read current engine settings from motor
[result, move_settings] = calllib('libximc','get_move_settings', device_id, parg_struct);

clear parg_struct
if result ~= 0
    disp(['Command failed with code', num2str(result)]);
end

speed = move_settings.Speed;
uspeed = move_settings.uSpeed;

end