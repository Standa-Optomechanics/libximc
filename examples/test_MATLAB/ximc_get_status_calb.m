function [ res_struct ] = ximc_get_status_calb(device_id, calibration)

% To use the libximc library command get_status_calb,
% you need to initialize the status_calb_t structure used by the commands.
% Here is a trick.
% It is necessary to initiate an arbitrary structure(dummy_struct) with a field that is present in the structure status_calb_t.
% Based on which the status_calb_t structure should be set.
dummy_struct = struct('Flags',999);
parg_struct = libpointer('status_calb_t', dummy_struct);
parg2_struct = libpointer('calibration_t', calibration);
[result, res_struct] = calllib('libximc','get_status_calb', device_id, parg_struct, parg2_struct);
clear parg_struct
clear parg2_struct
if result ~= 0
    disp(['Command failed with code', num2str(result)]);
    res_struct = 0;
end

end

