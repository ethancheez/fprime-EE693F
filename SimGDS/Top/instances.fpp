module SimGDS {

  constant GND_TOPOLOGY_BASE = 0x10000000

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {
    constant QUEUE_SIZE = 3
    constant STACK_SIZE = 64 * 1024
  }

  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------

  instance gnd_eventLogger: Svc.ActiveLogger base id GND_TOPOLOGY_BASE + 0x0200 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 98

  instance gnd_tlmSend: Svc.TlmChan base id GND_TOPOLOGY_BASE + 0x0300 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 97

  # ----------------------------------------------------------------------
  # Queued component instances
  # ----------------------------------------------------------------------

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  instance gnd_rateGroup1: Svc.PassiveRateGroup base id GND_TOPOLOGY_BASE + 0x1000

  instance gnd_rateGroup2: Svc.PassiveRateGroup base id GND_TOPOLOGY_BASE + 0x1100

  instance gnd_commDriver: Arduino.StreamDriver base id GND_TOPOLOGY_BASE + 0x4000

  instance gnd_framer: Svc.Framer base id 0x4100

  instance gnd_timeHandler: Svc.Time base id GND_TOPOLOGY_BASE + 0x4400 \
    type "Svc::ArduinoTimeImpl" \
    at "../../lib/arduino/fprime-arduino/Arduino/ArduinoTime/ArduinoTimeImpl.hpp"

  instance gnd_rateGroupDriver: Svc.RateGroupDriver base id GND_TOPOLOGY_BASE + 0x4500

  instance gnd_staticMemory: Svc.StaticMemory base id GND_TOPOLOGY_BASE + 0x4600

  instance gnd_textLogger: Svc.PassiveTextLogger base id GND_TOPOLOGY_BASE + 0x4700

  instance gnd_rateDriver: Arduino.HardwareRateDriver base id GND_TOPOLOGY_BASE + 0x4A00

  instance gnd_streamCrossoverUplink: Drv.StreamCrossover base id GND_TOPOLOGY_BASE + 0x5000

  instance gnd_streamCrossoverDownlink: Drv.StreamCrossover base id GND_TOPOLOGY_BASE + 0x5100

  instance gnd_rfm69: Radio.RFM69 base id GND_TOPOLOGY_BASE + 0x10000

  instance gnd_gpioRadioReset: Arduino.GpioDriver base id GND_TOPOLOGY_BASE + 0x10100

}
