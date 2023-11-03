module SimGDS {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

    enum Ports_RateGroups {
      rateGroup1
      rateGroup2
    }

    enum Ports_StaticMemory {
      downlink
      uplink
    }

  topology SimGDS {

    # ----------------------------------------------------------------------
    # Instances used in the topology
    # ----------------------------------------------------------------------

    instance gnd_commDriver
    instance gnd_eventLogger
    instance gnd_framer
    instance gnd_rateDriver
    instance gnd_rateGroup1
    instance gnd_rateGroup2
    instance gnd_rateGroupDriver
    instance gnd_staticMemory
    instance gnd_textLogger
    instance gnd_timeHandler
    instance gnd_tlmSend

    instance gnd_rfm69
    instance gnd_gpioRadioReset
    instance gnd_streamCrossoverUplink
    instance gnd_streamCrossoverDownlink

    # ----------------------------------------------------------------------
    # Pattern graph specifiers
    # ----------------------------------------------------------------------

    event connections instance gnd_eventLogger

    time connections instance gnd_timeHandler

    telemetry connections instance gnd_tlmSend

    # ----------------------------------------------------------------------
    # Direct graph specifiers
    # ----------------------------------------------------------------------

    connections RateGroups {
      # Block driver
      gnd_rateDriver.CycleOut -> gnd_rateGroupDriver.CycleIn

      # Rate group 1
      gnd_rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup1] -> gnd_rateGroup1.CycleIn
      gnd_rateGroup1.RateGroupMemberOut[0] -> gnd_commDriver.schedIn
      gnd_rateGroup1.RateGroupMemberOut[1] -> gnd_rfm69.run

      gnd_rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup2] -> gnd_rateGroup2.CycleIn
      gnd_rateGroup2.RateGroupMemberOut[0] -> gnd_tlmSend.Run
    }

    connections Comms {

      gnd_tlmSend.PktSend -> gnd_framer.comIn

      gnd_framer.framedAllocate -> gnd_staticMemory.bufferAllocate[Ports_StaticMemory.downlink]
      gnd_framer.framedOut -> gnd_commDriver.send

      # Downlink
      gnd_rfm69.allocate -> gnd_staticMemory.bufferAllocate[Ports_StaticMemory.downlink]
      gnd_rfm69.comDataOut -> gnd_streamCrossoverDownlink.streamIn
      gnd_streamCrossoverDownlink.streamOut -> gnd_commDriver.send
      gnd_commDriver.deallocate -> gnd_staticMemory.bufferDeallocate[Ports_StaticMemory.downlink]

      gnd_streamCrossoverDownlink.errorDeallocate -> gnd_staticMemory.bufferDeallocate[Ports_StaticMemory.downlink]

      # Uplink
      gnd_commDriver.allocate -> gnd_staticMemory.bufferAllocate[Ports_StaticMemory.uplink]
      gnd_commDriver.$recv -> gnd_streamCrossoverUplink.streamIn
      gnd_streamCrossoverUplink.streamOut -> gnd_rfm69.comDataIn
      gnd_rfm69.deallocate -> gnd_staticMemory.bufferDeallocate[Ports_StaticMemory.uplink]
      
      gnd_streamCrossoverUplink.errorDeallocate -> gnd_staticMemory.bufferDeallocate[Ports_StaticMemory.uplink]

    }

    connections SimGDS {
      # Add here connections to user-defined components
      gnd_rfm69.gpioReset -> gnd_gpioRadioReset.gpioWrite
    }

  }

}
