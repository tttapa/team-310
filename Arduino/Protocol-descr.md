# Protocol description
## Message format
```0b 1ccc cccc  0ddd dddd```  
`c`: command (7-bit)  
`d`: data (7-bits)  
First byte has MSB = 1, second byte has MSB = 0, this means that if one byte is dropped (on the Serial connection, for instance), the system can't get out of sync and interpret a data byte as a command byte.
If the command doesn't require a data byte, the second byte can be omitted. 

## Timeouts
### IR
Two movement commands should arrive within 200ms after the previous command. If no subsequent command has been received after 200ms, the vehicle should automatically brake.
This prevents that it keeps on driving when the remote is out of range, the battery dies, or when the IR beam is obstructed.  
If a system with start and stop commands were to be used, a missed _stop_ command due to one of the factors mentioned above would mean that the vehicle would keep on moving out of control, possibly with disasterous consequences ...

### UDP
Commands sent over UDP should be treated the same way as IR commands, because there is no guarantee that a _stop_ command will arrive, and there is no way to tell if a client has disconnected.

### WebSocket 
Given that the WebSocket protocol is based upon TCP, packet delivery is guaranteed, and a disconnect from the client is always detected. This allows for a system with a start and stop command, so no continuous stream of repeating commands is required.  
When a client is disconnected, the vehicle should immediately brake, unless a command from either a UDP client or an IR remote overrides this action.  

## Client to vehicle
### 1-byte commands
#### IR or UDP
- Forward:  0x61
- Backward: 0x60
- Left:     0x50
- Right:    0x51
- Speed +:  0x10 (vol +)
- Speed -:  0x11 (vol -)

#### WebSocket
- Forward:  0x6B
- Backward: 0x6A
- Left:     0x5A
- Right:    0x5B

#### General
- Reset:    0x01
- Brake:    0x62
- Auto:     0x4E (Menu)
- Power:    0x4D
- Lights on:    0x63 (PP) 
- Lights off:   0x0D (Mute)
- Lights auto:  0x69 (AV)
- Red:      0x76
- Green:    0x72
- Yellow:   0x78
- Blue:     0x74
- Rainbow:  0x41 (TV)
### 2-byte commands
- Speed setting:     0x03
- Left wheel power:  0x04
- Right wheel power: 0x05
## Vehicle to client
### 1-byte commands
- Reset:    0x01
### 2-byte commands
- Battery voltage:   0x02
- Speed setting:     0x03
- Actual speed:      0x06

## Encoding data bytes
In order to increase accuracy, speed and battery level values are encoded: d = 0x00 represents the minimum value, and d = 0x7F represents the maximum value. 
```
if(raw <= min)
  data = 0;
else if(raw >= max)
  data = 0x7F;
else 
  data = (raw - min) * 0x7F / (max - min);
```
On the receiver side, this becomes:
```
raw = data * (max - min) / 0x7F + min;
```

## Android Client App
### Communication
The app should use the WebSocket protocol to communicate with the vehicle. 
### Drive commands
The app should contain four buttons to send the forward, backward, left and right commands. When one of these buttons is pressed, it should send the appropriate direction command, when the button is released, a brake command is to be sent.  

Alternatively, a joystick-like control may be implemented. The _Left and Right wheel power_ commands can be used to send the right instructions to the vehicle.
### Lights
The color should be initialized to white, and the mode to _off_.
### Speed
The app should display the actual speed, both as a decimal value, and on a speedometer gauge.
Setting the speed level can be done using a slider with n positions, or n separate buttons.
Increment/decrement commands may not be used, only absolute _speed setting_ commands.
### Battery
The app should display the battery level of the vehicle, and produce a warning message when the level is too low.
