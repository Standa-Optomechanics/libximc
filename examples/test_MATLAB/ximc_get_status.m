function [ res_struct ] = ximc_get_status(device_id)

% To use the libximc library command get_status,
% you need to initialize the status_t structure used by the commands.
% Here is a trick.
% It is necessary to initiate an arbitrary structure(dummy_struct) with a field that is present in the structure status_t.
% Based on which the status_t structure should be set.
dummy_struct = struct('Flags',999);
parg_struct = libpointer('status_t', dummy_struct);
[result, res_struct] = calllib('libximc','get_status', device_id, parg_struct);
clear parg_struct
if result ~= 0
    disp(['Command failed with code', num2str(result)]);
    res_struct = 0;
end

end

