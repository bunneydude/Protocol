#include <stdint.h>
#include "Arduino.h"
#include "MyRingBuffer.h"


MyRingBuffer::MyRingBuffer(volatile uint8_t* mem_start, uint8_t length){
   _head = 0;
   _tail = 0;
   _memPtr = mem_start;
   _size = length-1;
}

uint8_t MyRingBuffer::write(uint8_t data){   
//   ENTER_CRITICAL_SECTION;
   if( ( (_tail + 1) & (_size) ) != _head){//leave bubble
      *(_memPtr +_tail*sizeof(uint8_t)) = data;
      _tail = (_tail+1) & (_size); 
 //     EXIT_CRITICAL_SECTION;
      return 1;
   }else{
  //    EXIT_CRITICAL_SECTION;
      return 0;
   }

}

uint8_t MyRingBuffer::read(uint8_t* data){
  // ENTER_CRITICAL_SECTION;
   if(_head != _tail){      
      *data = *(_memPtr +_head*sizeof(uint8_t));
      _head = (_head+1) & (_size);
    //  EXIT_CRITICAL_SECTION;
      return 1;
   }else 
      //EXIT_CRITICAL_SECTION;
      return 0;

}

void MyRingBuffer::flush(){
   //ENTER_CRITICAL_SECTION
   _head = 0;
   _tail = 0;
   //EXIT_CRITICAL_SECTION
}

uint8_t MyRingBuffer::getTail(){
   return _tail;
}

uint8_t MyRingBuffer::getHead(){
   return _head;
}
