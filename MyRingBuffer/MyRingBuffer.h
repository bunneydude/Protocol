#ifndef MYRINGBUFFER_H
#define MYRINGBUFFER_H
#include <stdint.h>


/*
#ifndef ENTER_CRITICAL_SECTION
#define ENTER_CRITICAL_SECTION __disable_interrupt()
#define EXIT_CRITICAL_SECTION __enable_interrupt()  
#endif
*/


class MyRingBuffer{
   public:
      MyRingBuffer(volatile uint8_t* mem_start, uint8_t length);
      uint8_t write(uint8_t data);
      uint8_t read(uint8_t* data);
      void flush();
      uint8_t getTail();
      uint8_t getHead();
   private:
      volatile uint8_t _head; //read here
      volatile uint8_t _tail; //write here
      volatile uint8_t* _memPtr; //location of data
      uint8_t _size; //size of buffer
};

#endif
