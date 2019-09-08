#include "ring_buffer.h"

RingBuffer::RingBuffer()
{
    for (uint8_t i = 0; i < N; i++) {
      _aucBuffer[i] = 0;
    }

    clear();
}

void RingBuffer::store_char( uint8_t c )
{
  int i = nextIndex(_iHead);

  if ( i != _iTail )
  {
    _aucBuffer[_iHead] = c ;
    _iHead = i ;
  }
}

void RingBuffer::clear()
{
  _iHead = 0;
  _iTail = 0;
}

int RingBuffer::read_char()
{
  if(_iTail == _iHead)
    return -1;

  uint8_t value = _aucBuffer[_iTail];
  _iTail = nextIndex(_iTail);

  return value;
}

int RingBuffer::available()
{
  int delta = _iHead - _iTail;

  if(delta < 0)
    return N + delta;
  else
    return delta;
}

int RingBuffer::availableForStore()
{
  if (_iHead >= _iTail)
    return N - 1 - _iHead + _iTail;
  else
    return _iTail - _iHead - 1;
}

int RingBuffer::peek()
{
  if(_iTail == _iHead)
    return -1;

  return _aucBuffer[_iTail];
}

int RingBuffer::nextIndex(int index)
{
  return (uint32_t)(index + 1) % N;
}

bool RingBuffer::isFull()
{
  return (nextIndex(_iHead) == _iTail);
}