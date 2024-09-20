package ru.ximc.libximc;

@SuppressWarnings("serial")
public class XimcValueError extends XimcException {
    public XimcValueError() {
        super("Value access error");
    }

    public XimcValueError(String message) {
        super("Value access error " + message);
    }
}

// vim: etts=4 shiftwidth=4
