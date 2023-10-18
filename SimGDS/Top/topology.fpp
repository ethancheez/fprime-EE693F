module SimGDS {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

    enum Ports_RateGroups {
      rateGroup1
    }

    enum Ports_StaticMemory {
      downlink
      uplink
    }

  topology SimGDS {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance commDriver
    instance eventLogger
    instance rateDriver
    instance rateGroup1
    instance rateGroupDriver
    instance staticMemory
    instance textLogger
    instance timeHandler

    instance rfm69
    instance gpioRadioReset
    instance streamCrossoverUplink
    instance streamCrossoverDownlink

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    event connections instance eventLogger

    time connections instance timeHandler

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections RateGroups {
      # Block driver
      rateDriver.CycleOut -> rateGroupDriver.CycleIn

      # Rate group 1
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1.CycleIn
      rateGroup1.RateGroupMemberOut[0] -> commDriver.schedIn
      rateGroup1.RateGroupMemberOut[1] -> rfm69.run
    }

    connections Comms {

      # Downlink
      rfm69.allocate -> staticMemory.bufferAllocate[Ports_StaticMemory.downlink]
      rfm69.comDataOut -> streamCrossoverDownlink.streamIn
      streamCrossoverDownlink.streamOut -> commDriver.send
      commDriver.deallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.downlink]

      streamCrossoverDownlink.errorDeallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.downlink]

      # Uplink
      commDriver.allocate -> staticMemory.bufferAllocate[Ports_StaticMemory.uplink]
      commDriver.$recv -> streamCrossoverUplink.streamIn
      streamCrossoverUplink.streamOut -> rfm69.comDataIn
      rfm69.deallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.uplink]
      
      streamCrossoverUplink.errorDeallocate -> staticMemory.bufferDeallocate[Ports_StaticMemory.uplink]

    }

    connections SimGDS {
      # Add here connections to user-defined components
      rfm69.gpioReset -> gpioRadioReset.gpioWrite
    }

  }

}
