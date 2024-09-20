package ru.ximc.libximc;

@SuppressWarnings("serial")
public class XimcNoDevice extends XimcException {
    public XimcNoDevice() {
        super("Device is not found");
    }

    public XimcNoDevice(String message) {
        super("Device is not found " + message);
    }
}

// vim: et ts=4 shiftwidth=4
