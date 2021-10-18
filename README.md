# PiPico_AnalogVoltageOut
This Sketch is able to output clean sawtooth waves & attempts to output triangle and sin waves but they get distorted for currently unknown reasons

The code that converts values to GPIO 8 bit binary does work its just when an array gets sent to the GPIO does distortion happen

--------------------------------------------------------------------------
If the value variable is assigned a number from 0-255 an analog voltage will be present if the GPIOs are connected together with a r2r resistor ladder
