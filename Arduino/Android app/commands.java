package testproject.test;

/**
 * Created by Dirk Vanbeveren on 2/04/2017.
 */

class commands {
    ///////if def TV/////////////////
    static final int FORWARD = 0x20;
    static final int BACKWARD = 0x21;
    static final int LEFT = 0x15;
    static final int RIGHT = 0x16;
    static final int BRAKE = 0x0C;
    static final int SPEEDUP = 0x10;
    static final int SPEEDDOWN = 0x11;
    static final int CHG_STATION = 0x30;
    ///////LIGHTS///////////////
    static final int LIGHTS_ON = 0x0E;
    static final int LIGHTS_OFF = 0x0D;
    static final int LIGHTS_AUTO = 0x38;

    static final int LIGHTS_RED = 0x37;
    static final int LIGHTS_GREEN = 0x36;
    static final int LIGHTS_YELLOW = 0x32;
    static final int LIGHTS_BLUE = 0x34;
    static final int LIGHTS_RAINBOW = 0x12;
}
