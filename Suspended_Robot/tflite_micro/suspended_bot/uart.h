#ifndef UART_H_
#define UART_H_

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ring_buffer.h"

#define MAX_UART_PACKET_SIZE   2048

extern "C" void uart_print(char *buffer);

const am_hal_uart_config_t ap3_uart_config_default = {
    // Standard UART settings: 115200-8-N-1
    .ui32BaudRate = 9600,
    .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
    .ui32Parity = AM_HAL_UART_PARITY_NONE,
    .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
    .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

    // Set TX and RX FIFOs to interrupt at half-full.
    .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                        AM_HAL_UART_RX_FIFO_1_2),

    // Buffers
    .pui8TxBuffer = 0,
    .ui32TxBufferSize = 0,
    .pui8RxBuffer = 0,
    .ui32RxBufferSize = 0,
};

class Uart 
{
    public:
	    Uart();
        void begin(const uint32_t baudrate);
        void end();
        int available();
        int peek();
        int read();
        void flush();
        size_t write(const uint8_t data);
        size_t write(const uint8_t *buffer, size_t size);
        void rx_isr(void);

    public:						  
        void *_handle;
        am_hal_uart_config_t _config;
        RingBuffer _rx_buffer;
};

#endif // UART_H_
