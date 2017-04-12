# Protocol description
## message format
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
- Speed setting:    0x03

## Vehicle to client
### 1-byte commands
- Reset:    0x01
### 2-byte commands
- Battery voltage:  0x02
- Speed setting:    0x03
- Actual speed:     0x04