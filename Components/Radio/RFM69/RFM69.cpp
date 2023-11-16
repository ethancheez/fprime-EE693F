// ======================================================================
// \title  RFM69.cpp
// \author ethanchee
// \brief  cpp file for RFM69 component implementation class
// ======================================================================

#include <Components/Radio/RFM69/RFM69.hpp>
#include <FpConfig.hpp>
#include <Os/Log.hpp>

namespace Radio {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  RFM69 ::
    RFM69(
        const char *const compName
    ) : RFM69ComponentBase(compName),
        rfm69(RFM69_CS, RFM69_INT),
        radio_state(Fw::On::OFF),
        pkt_rx_count(0),
        pkt_tx_count(0),
        numHelloWorldRX(0),
        numHelloWorldSent(0),
        numHelloWorldToSend(0),
        sendHelloWorldPkts(false)
  {
    
  }

  RFM69 ::
    ~RFM69()
  {

  }

  bool RFM69 ::
    send(const U8* payload, NATIVE_UINT_TYPE len)
  {
    FW_ASSERT(payload != nullptr);

    NATIVE_UINT_TYPE offset = 0;
    while(len > RH_RF69_MAX_MESSAGE_LEN)
    {
      rfm69.send(&payload[offset], RH_RF69_MAX_MESSAGE_LEN);
      if(!rfm69.waitPacketSent(500))
      {
        return false;
      } 
      delay(1);
      offset += RH_RF69_MAX_MESSAGE_LEN;
      len -= RH_RF69_MAX_MESSAGE_LEN;
    }

    rfm69.send(&payload[offset], len);
    if(!rfm69.waitPacketSent(500))
    {
      return false;
    }

    pkt_tx_count++;
    this->tlmWrite_NumPacketsSent(pkt_tx_count);
    this->log_DIAGNOSTIC_PayloadMessageTX(len);

    Fw::Success radioSuccess = Fw::Success::SUCCESS;
    if (this->isConnected_comStatus_OutputPort(0)) {
        this->comStatus_out(0, radioSuccess);
    }

    return true;
  }

  void RFM69 ::
    recv()
  {
    if (rfm69.available()) {
      U8 buf[RH_RF69_MAX_MESSAGE_LEN];
      U8 bytes_recv = RH_RF69_MAX_MESSAGE_LEN;

      if (rfm69.recv(buf, &bytes_recv)) {

        String bufString(reinterpret_cast<CHAR*>(buf));
        if (bufString.indexOf("Hello World") >= 0) {
          this->numHelloWorldRX++;
          return;
        }

        Fw::Buffer recvBuffer = this->allocate_out(0, bytes_recv);
        memcpy(recvBuffer.getData(), buf, bytes_recv);
        recvBuffer.setSize(bytes_recv);
        pkt_rx_count++;

        this->log_DIAGNOSTIC_PayloadMessageRX(recvBuffer.getSize());

        this->tlmWrite_NumPacketsReceived(pkt_rx_count);
        this->tlmWrite_RSSI(rfm69.lastRssi());
        this->tlmWrite_NumHelloWorldReceived(numHelloWorldRX);

        this->comDataOut_out(0, recvBuffer, Drv::RecvStatus::RECV_OK);
      }
    }
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  Drv::SendStatus RFM69 ::
    comDataIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &sendBuffer
    )
  {
    if((radio_state == Fw::On::ON) && (sendBuffer.getSize() > 0) && (not this->send(sendBuffer.getData(), sendBuffer.getSize())))
    {
      radio_state = Fw::On::OFF;
    }
    deallocate_out(0, sendBuffer);

    return Drv::SendStatus::SEND_OK;  // Always send ok to deframer as it does not handle this anyway
  }

  void RFM69 ::
    run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    this->tlmWrite_Status(radio_state);

    if(radio_state == Fw::On::OFF)
    {
      if(this->isConnected_gpioReset_OutputPort(0))
      {
        this->gpioReset_out(0, Fw::Logic::HIGH);
        delay(10);
        this->gpioReset_out(0, Fw::Logic::LOW);
        delay(10);
      }
      
      if(!rfm69.init())
      {
        return;
      }

      rfm69.setFrequency(RFM69_FREQ);
      rfm69.setTxPower(20, true);
      // rfm69.setPreambleLength(16);
      // U8 syncWord[] = { 0x12, 0x34, 0x56, 0x78 };
      // rfm69.setSyncWords(syncWord, sizeof(syncWord));
      // U8 key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
      // rfm69.setEncryptionKey(key);

      Fw::Success radioSuccess = Fw::Success::SUCCESS;
      if (this->isConnected_comStatus_OutputPort(0)) {
          this->comStatus_out(0, radioSuccess);
      }

      radio_state = Fw::On::ON;
    }
    
    this->recv();
    this->sendHelloWorld();
  }

  void RFM69 ::
    SEND_HELLO_WORLD_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U16 numToSend,
        U16 interval
    )
  {
    delay(3000);
    this->sendInterval = interval;
    this->numHelloWorldSent = 0;
    this->numHelloWorldToSend = numToSend;
    this->sendHelloWorldPkts = true;
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void RFM69 ::
    RESET_HELLO_WORLD_TLM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    this->numHelloWorldRX = 0;
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void RFM69::sendHelloWorld() {
    this->tlmWrite_NumHelloWorldSent(numHelloWorldSent);

    if (!sendHelloWorldPkts) {
      return;
    }

    if (this->numHelloWorldSent >= this->numHelloWorldToSend) {
      this->sendHelloWorldPkts = false;
      return;
    }

    if (this->sendIntervalTimer < this->sendInterval) {
      return;
    }

    U8 helloWorld[] = "Hello World";
    rfm69.send(helloWorld, sizeof(helloWorld));

    Fw::Logger::logMsg("[%d] Sending Hello World...\n", this->numHelloWorldSent);
    this->numHelloWorldSent++;
    this->sendIntervalTimer = 0;
  }

} // end namespace Radio
