function [ serial ] = ximc_get_serial_number(device_id)
    serial_number = 0;
    serial_number_ptr = libpointer('uint32Ptr', serial_number);
    [result, serial] = calllib('libximc','get_serial_number', device_id, serial_number_ptr);
    clear serial_number_ptr
    if result ~= 0
        disp(['Command failed with code', num2str(result)]);
        serial = 0;
    end
end

