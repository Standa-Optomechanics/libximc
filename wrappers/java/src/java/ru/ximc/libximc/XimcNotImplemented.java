package ru.ximc.libximc;

@SuppressWarnings("serial")
public class XimcNotImplemented extends XimcException {
    public XimcNotImplemented() {
        super("Not implemented");
    }

    public XimcNotImplemented(String message) {
        super("Not implemented " + message);
    }
}

// vim: et ts=4 shiftwidth=4
