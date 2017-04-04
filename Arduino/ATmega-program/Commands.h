#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef TV

// Drive
#define FORWARD 0x20
#define BACKWARD 0x21
#define LEFT 0x15
#define RIGHT 0x16
#define BRAKE 0x0C
#define SPEEDUP 0x10
#define SPEEDDOWN 0x11
#define CHG_STATION 0x30

// Lights
#define LIGHTS_ON 0x0E
#define LIGHTS_OFF 0x0D
#define LIGHTS_AUTO 0x38

#define LIGHTS_RED 0x37
#define LIGHTS_GREEN 0x36
#define LIGHTS_YELLOW 0x32
#define LIGHTS_BLUE 0x34
#define LIGHTS_RAINBOW 0x12

#elif defined DVD

// Drive
#define FORWARD 0x61
#define BACKWARD 0x60
#define LEFT 0x50
#define RIGHT 0x51
#define BRAKE 0x62 // Ok
#define SPEEDUP 0x10 // Vol +
#define SPEEDDOWN 0x11 // Vol -
#define CHG_STATION 0x4E // Menu
#define MANUAL 0x4D // Power

// Lights
#define LIGHTS_ON 0x63 // PP 
#define LIGHTS_OFF 0x0D // Mute
#define LIGHTS_AUTO 0x69 // AV 

#define LIGHTS_RED 0x76
#define LIGHTS_GREEN 0x72
#define LIGHTS_YELLOW 0x78
#define LIGHTS_BLUE 0x74
#define LIGHTS_RAINBOW 0x41 // TV

// Numbers

#define NB1 0x52
#define NB2 0x43
#define NB3 0x42
#define NB4 0x4A
#define NB5 0x46
#define NB6 0x45
#define NB7 0x40
#define NB8 0x49
#define NB9 0x48
#define NB0 0x47

// Other

#define INFO 0x7F
#define A_B 0x70

#define P_P 0x6B
#define PLUS10 0x44

#define RETURN 0x61
#define TIME 0x77

#define PREV 0x56
#define NEXT 0x55
#define BEGINNING 0x6C
#define END 0x6D

#define PLAY 0x6E
#define STOP 0x6F
#define TITLE 0x7C
#define OPEN 0x4C

#endif

#endif
