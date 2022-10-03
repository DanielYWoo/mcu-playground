# schematic

```
PIN_YL 2 //servo[0]  left leg
PIN_YR 3 //servo[1]  right leg
PIN_RL 4 //servo[2]  left foot
PIN_RR 5 //servo[3]  right foot
PIN_Buzzer   6
PIN_Trigger  8  //TRIGGER pin (8)
PIN_Echo     9  //ECHO pin (9)
PIN_SOFT_RX  10  to BT05 TX
PIN_SOFT_TX  11  to BT05 RX with a voltage divider 5v -> 3.3v
PIN_BT_STATUS  12 to BT05 STATE

```


# bluetooth pairing
HC05 does not work on the latest MacOS, has to re-pair everytime.
HC05 does not support BLE so it does not work with iPhone.
BT05 works with the latest MacOS and iPhone, but it does not work with the terminal with the virtal serial port. It needs something like BlueSee.
BT05 has a service FFE1, we can write to it, or read data from it.

BT05 needs to be set up with a serial USB TTL like CH340G, e.g., set the baud rate to 9600, then in your code you can `BT.begin(9600)`.

# control
To keep it simple, single byte command.

