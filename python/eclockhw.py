
import analogio
import board
from digitalio import DigitalInOut, Direction
import time

class Battery():
    def __init__(self):
        self._adc = analogio.AnalogIn(board.VOLTAGE_MONITOR)

    def voltage(self):
        return self._adc.value * 3.3 / 65536.0 * 2.0

class PowerSwitch():
    def __init__(self):
        self._done = DigitalInOut(board.D5)
        self._done.direction = Direction.OUTPUT
        self._done.value = False
    
    def turn_off(self):
        # TPL5111 triggers on rising edge.
        self._done.value = True
        time.sleep(0.5)
        # Will normally not reach this point.
        # Repeat the signal, if necessary
        self._done.value = False
        time.sleep(0.5)
        self._done.value = True
