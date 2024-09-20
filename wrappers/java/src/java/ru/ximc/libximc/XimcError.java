package ru.ximc.libximc;

@SuppressWarnings("serial")
public class XimcError extends XimcException {
    public XimcError() {
        super("Generic error");
    }

    public XimcError(String message) {
        super("Generic error " + message);
    }
}

// vim: et ts=4 shiftwidth=4
