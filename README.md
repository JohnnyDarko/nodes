# nodes:
Nodes put the abstraction in Hardware Abstraction Layer (HAL). This form of loose coupling allows nodes to be aggregation points. One node may utilize multiple drivers.

example of nodes:

    Gps: utilizes a serial line reader driver for nmea sentences.
    Ahrs: provides one point of access to - gyroscope, accelerometer, magnometer, barometer
    Servo: handles control driver for servo as well as performng simple transformations
    Motor: handles motor speed control driver as well as positional feedback driver
    
