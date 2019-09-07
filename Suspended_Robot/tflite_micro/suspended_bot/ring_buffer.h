#ifndef _RING_BUFFER_
#define _RING_BUFFER_

#include <stdint.h>
#define N 128

class RingBuffer
{
  public:
    uint8_t _aucBuffer[N] ;
    volatile int _iHead ;
    volatile int _iTail ;

  public:
    RingBuffer( void ) ;
    void store_char( uint8_t c ) ;
    void clear();
    int read_char();
    int available();
    int availableForStore();
    int peek();
    bool isFull();

  private:
    int nextIndex(int index);
};

#endif /* _RING_BUFFER_ */
