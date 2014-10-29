#include <stdint.h>
#include <MyRingBuffer.h>
#include "Arduino.h"
#include "Protocol.h"

//TODO add flush to uartRxBuffer

Protocol::Protocol(uint8_t port) : _uartTxBuffer(&_uartTxData[0], RING_SIZE)
{  
//   ringBufferInit(&uartRxBuffer, &uartRxData[0], 16);
//   ringBufferInit(&uartTxBuffer, &uartTxData[0], 16);

   _byte_num = 0;
   _cmd = 0;
   _addr = 0;
   _data = 0;
   _checksum = 0;
   _return_data = 0;
   _executePacket = 0;

   _port = port;
   _special = 0;
   if(port == 0){
      _channel = &Serial;
   }else if(port == 1){
      _channel = &Serial1;
   }
}


uint8_t Protocol::serial_respond(){
   if(Serial1.available()){
      uint8_t rxBuffer[5];
      uint8_t response[5];   
      uint8_t index = 0;
      uint8_t i = 0;
      uint8_t sendResponse = 0;

      while(Serial1.available() < 2); //wait for 2 bytes
      rxBuffer[index++] = Serial1.read(); //cmd
      rxBuffer[index++] = Serial1.read(); //size

      for(i=0; i<rxBuffer[1]; i++){ //get rest of bytes
         while(Serial1.available() == 0); //wait for a byte
         rxBuffer[index++] = Serial1.read();
      }

      while(Serial1.available() == 0); //wait for checksum
      rxBuffer[index++] = Serial1.read();

      //parse packet
      sendResponse = parse_packet(rxBuffer, response);

      if(sendResponse == 1){
         for(i=0; i<response[1]+3; i++){
            _uartTxBuffer.write(response[i]);
         }
         uartTx();
      }
      return 1;
   }
   return 0;
}

void Protocol::serial_transmit(uint8_t cmd, uint8_t addr, uint8_t data){
   uint8_t response[5];
   form_packet(response, cmd, addr, data);
   for(uint8_t i=0; i<response[1]+3; i++){
      _uartTxBuffer.write(response[i]);
   }
   uartTx();
}

uint8_t Protocol::parse_packet(uint8_t* buf, uint8_t* response){
   uint8_t returnCode = 0;
   _cmd = buf[0];
   _size = buf[1];
   _checksum = _cmd + _size;
   for(uint8_t i=0; i<_size; i++){
      _remainderBuf[i] = buf[2+i];
      _checksum += _remainderBuf[i];
   }
   _checksum += buf[2+_size]; //get last byte, the checksum

   if(_checksum == 0){
      _executePacket = 1;
   }else{
      Serial.print("Bad checksum: ");
      Serial.println(_checksum);
      form_packet(response, NACK, 0, BAD_CHECKSUM);
      returnCode = 1;
      _executePacket = 0;
      _byte_num = 0;
      _checksum = 0;
   }

   if(_executePacket == 1){
      _executePacket = 0;
      _checksum = 0;
      switch(_cmd){
         case(READ_REG): //read_reg
            Serial.println("Got read_reg");
            _addr = _remainderBuf[0];

            form_packet(response, ACK, _addr, serial_registers[_addr]);
            returnCode = 1;
            break;

         case(WRITE_REG): 
            Serial.println("Got write_reg");
            _addr = _remainderBuf[0];
            _data = _remainderBuf[1];
            serial_registers[_addr] = _data; //write value

            form_packet(response, ACK, _addr, serial_registers[_addr]);
            returnCode = 1;
            break;

         case(ACK): //ack
            Serial.println("Got ack");
            Serial.print("Addr = ");Serial.println(_remainderBuf[0]);
            Serial.print("Data = ");Serial.println(_remainderBuf[1]);
            Serial.print("\n");
            returnCode = 0;
            break;

         case(NACK): //nack
            Serial.println("Got nack");
            Serial.print("Addr = ");Serial.println(_remainderBuf[0]);
            Serial.print("Data = ");Serial.println(_remainderBuf[1]);
            Serial.print("\n");
            returnCode = 0;
            break;

         case(NOP): //nop
            Serial.println("Got nop");
            _special = 1;
            returnCode = 0;            
            break;

         default: //unknown command
            Serial.println("Got unknown command. Send nack");
            form_packet(response, NACK, 0, BAD_COMMAND);
            returnCode = 1;
            break;
      }//end switch
   }//end if executePacket==1
   return returnCode;
}//end parse_packet

uint8_t Protocol::form_packet(uint8_t* buf, uint8_t cmd, uint8_t addr, uint8_t data){
   uint8_t index = 0;
   uint8_t checksum = cmd;

   buf[index++] = cmd;

   if(cmd == READ_REG){
      buf[index++] = (1);
      buf[index++] = (addr);

      checksum += 1;
      checksum += addr;

   }else if(cmd == WRITE_REG){
      buf[index++] = (2);
      buf[index++] = (addr);
      buf[index++] = (data);

      checksum += 2;
      checksum += addr;
      checksum += data;

   }else if(cmd == ACK){
      buf[index++] = (2);
      buf[index++] = (addr);
      buf[index++] = (data);

      checksum += 2;
      checksum += addr;
      checksum += data;

   }else if(cmd == NACK){
      buf[index++] = (2);
      buf[index++] = (addr);
      buf[index++] = (data);

      checksum += 2;
      checksum += addr;
      checksum += data;

   }else if(cmd == NOP){
      buf[index++] = ((uint8_t)0);

   }else{
      printf("Unknown command: %d\n",cmd);
      return 0;
   }

   buf[index++] = ((~checksum)+1);

   return 1;
}

void Protocol::uartTx(){
   uint8_t temp;
   while(_uartTxBuffer.read(&temp)){
//         _channel->write(temp);
         Serial1.write(temp);
   }
}
