#include "uart.h"

Uart *uart_handle = 0;
uint8_t tXBuffer[MAX_UART_PACKET_SIZE];

Uart::Uart()
{
    am_util_stdio_printf("Constructor called\n");
    _handle = NULL;
    _config = ap3_uart_config_default;
    _config.pui8TxBuffer = tXBuffer;
    _config.ui32TxBufferSize = sizeof(tXBuffer);
}

void Uart::begin(const uint32_t baudrate)
{
    _config.ui32BaudRate = baudrate;

    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    if (_handle == NULL)
    {
        am_hal_uart_initialize(0, &_handle);
    }

    am_hal_uart_power_control(_handle, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(_handle, &_config);
    UARTn(0)->LCRH_b.FEN = 0; // Disable that pesky FIFO

   // Enable the UART pins.
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    // Enable RX interrupts
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + 0));
    am_hal_uart_interrupt_enable(_handle, (AM_HAL_UART_INT_RX));
    am_hal_interrupt_master_enable();

    am_util_stdio_printf_init(uart_print);
    uart_handle = this;
}

void Uart::end()
{
    if (_handle != NULL)
    {
        flush();

        // Power down the UART, and surrender the handle.
        am_hal_uart_power_control(_handle, AM_HAL_SYSCTRL_DEEPSLEEP, false);
        am_hal_uart_deinitialize(_handle);

        // Disable the UART pins.
        am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_HAL_GPIO_DISABLE);

        _handle = NULL;
    }
}

int Uart::available()
{
    return _rx_buffer.available();
}

int Uart::peek()
{
    return _rx_buffer.peek();
}

int Uart::read()
{
    return _rx_buffer.read_char();
}

void Uart::flush()
{
    // Make sure the UART has finished sending everything it's going to send.
    am_hal_uart_tx_flush(_handle);
}

size_t Uart::write(const uint8_t data)
{
    return write(&data, 1);
}

size_t Uart::write(const uint8_t *buffer, size_t size)
{
    uint32_t ui32BytesWritten = 0;

    // todo: use a local buffer to guarantee lifespan of data (maybe txbuffer, but maybe not a ring buffer? b/c of efficiency + not breaking up transfers)

    const am_hal_uart_transfer_t sUartWrite =
        {
            .ui32Direction = AM_HAL_UART_WRITE,
            .pui8Data = (uint8_t *)buffer,
            .ui32NumBytes = size,
            .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,
            .pui32BytesTransferred = (uint32_t *)&ui32BytesWritten,
        };

    am_hal_uart_transfer(_handle, &sUartWrite);

    return ui32BytesWritten;
}

inline void Uart::rx_isr(void)
{

    uint32_t ui32Status;

    // Read the masked interrupt status from the UART.
    am_hal_uart_interrupt_status_get(_handle, &ui32Status, true);
    am_hal_uart_interrupt_clear(_handle, ui32Status);
    am_hal_uart_interrupt_service(_handle, ui32Status, 0);

    if (ui32Status & AM_HAL_UART_INT_RX)
    {
        uint32_t ui32BytesRead = 0x00;
        uint8_t rx_c = 0x00;

        am_hal_uart_transfer_t sRead =
            {
                .ui32Direction = AM_HAL_UART_READ,
                .pui8Data = (uint8_t *)&rx_c,
                .ui32NumBytes = 1,
                .ui32TimeoutMs = 0,
                .pui32BytesTransferred = &ui32BytesRead,
            };
        am_hal_uart_transfer(_handle, &sRead);

        if (ui32BytesRead)
        {
            _rx_buffer.store_char(rx_c);
        }
    }
}
// ISR implementations for the UART peripheral on the Apollo3
extern "C" void am_uart_isr(void)
{
    if (uart_handle != NULL)
    {
        uart_handle->rx_isr();
    }
}

extern "C" void uart_print(char *buffer)
{
    uint32_t size = 0;

    while (buffer[size] != '\0')
    {
        size++;
    }

    uart_handle->write(buffer, size);
}
