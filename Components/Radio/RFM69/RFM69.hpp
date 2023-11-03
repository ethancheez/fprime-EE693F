// ======================================================================
// \title  RFM69.hpp
// \author ethanchee
// \brief  hpp file for RFM69 component implementation class
// ======================================================================

#ifndef RFM69_HPP
#define RFM69_HPP

#include "Components/Radio/RFM69/RFM69ComponentAc.hpp"
#include "RH_RF69.h"
#include <FprimeArduino.hpp>

namespace Radio {

  class RFM69 :
    public RFM69ComponentBase
  {

    public:

      static const NATIVE_INT_TYPE RFM69_FREQ = 915;
      static const NATIVE_INT_TYPE RFM69_CS   = 10;
      static const NATIVE_INT_TYPE RFM69_INT  = 20;
      static const NATIVE_INT_TYPE RFM69_RST  = 14;

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object RFM69
      //!
      RFM69(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object RFM69
      //!
      ~RFM69();

      bool send(const U8* payload, NATIVE_UINT_TYPE len);
      void recv();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for comDataIn
      //!
      Drv::SendStatus comDataIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &sendBuffer 
      );

      //! Handler implementation for run
      //!
      void run_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< 
      The call order
      */
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for SEND_HELLO_WORLD command handler
      //! 
      void SEND_HELLO_WORLD_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U16 numToSend, 
          U16 interval 
      );

      //! Implementation for RESET_HELLO_WORLD_TLM command handler
      //! 
      void RESET_HELLO_WORLD_TLM_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      void sendHelloWorld();

      RH_RF69 rfm69;
      Fw::On radio_state;
      U16 pkt_rx_count;
      U16 pkt_tx_count;

      elapsedMillis sendIntervalTimer;
      U16 sendInterval;
      U16 numHelloWorldRX;
      U16 numHelloWorldSent;
      U16 numHelloWorldToSend;
      bool sendHelloWorldPkts;
    };

} // end namespace Radio

#endif
