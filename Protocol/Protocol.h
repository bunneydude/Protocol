#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <Arduino.h>
#include <MyRingBuffer.h>

#define ADC_NUM_SAMPLES 30

#define UART_TXD   0x04                     // TXD on P1.2
#define UART_RXD   0x02                     // RXD on P1.1

#define SERIAL_REGISTER_NUM 16
#define RING_SIZE 16

//commands
#define READ_REG 0x1
#define WRITE_REG 0x2
#define ACK 0x3
#define NACK 0x4
#define NOP 0x5
#define READ_MEM 0x6
#define WRITE_MEM 0x7
#define ADC_SINGLE 0x8
#define ADC_RESULT 0x9
#define ADC_LOOP 0xA
#define ADC_END 0xB
#define SPI_CMD 0xC

//error codes
#define BAD_CHECKSUM 0x1
#define BAD_COMMAND 0x2
#define FEATURE_WIP 0x3
#define CAT 0x4
#define OUT_OF_RANGE 0x5
#define BP_TIMEOUT 0x6

class Protocol{

   public:
      Protocol(uint8_t port);
      uint8_t serial_registers[SERIAL_REGISTER_NUM];
      uint8_t serial_respond();
      void serial_transmit(uint8_t cmd, uint8_t addr, uint8_t data);      
      uint8_t form_packet(uint8_t* buf, uint8_t cmd, uint8_t addr, uint8_t data);
      uint8_t parse_packet(uint8_t* buf, uint8_t* response);
      void uartTx();
      MyRingBuffer _uartTxBuffer;
      uint8_t _special;
   private:
      TTYUARTClass *_channel;
      uint8_t _cmd;
      uint8_t _size;
      uint8_t _addr;
      uint8_t _data;  
      uint8_t _checksum;
      uint8_t _byte_num;
      uint8_t _remainderBuf[RING_SIZE];
      uint8_t _executePacket;
      uint8_t _return_data;
      uint8_t _port;      
      //static uart rx and tx arrays
      volatile uint8_t _uartTxData[RING_SIZE];
      //buffers to manage the data arrays
};

#endif
