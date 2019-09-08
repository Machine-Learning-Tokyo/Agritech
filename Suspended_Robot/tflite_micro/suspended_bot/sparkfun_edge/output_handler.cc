#include "tensorflow/lite/experimental/micro/examples/suspended_bot/output_handler.h"

#include "am_bsp.h"  // NOLINT

void HandleOutput(tflite::ErrorReporter* error_reporter, float *x_value,
                  float y_value) {
  // The first time this method runs, set up our LEDs correctly
  static bool is_initialized = false;
  if (!is_initialized) {
    // Set up LEDs as outputs
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_RED, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_BLUE, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_GREEN, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_LED_YELLOW, g_AM_HAL_GPIO_OUTPUT_12);
    // Ensure all pins are cleared
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_RED);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_BLUE);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_GREEN);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_YELLOW);
    is_initialized = true;
  }

  // Set the LEDs to represent negative values
  if (y_value < 0) {
    // Clear unnecessary LEDs
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_GREEN);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_YELLOW);
    // The blue LED is lit for all negative values
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_BLUE);
    // The red LED is lit in only some cases
    if (y_value <= -0.75) {
      am_hal_gpio_output_set(AM_BSP_GPIO_LED_RED);
    } else {
      am_hal_gpio_output_clear(AM_BSP_GPIO_LED_RED);
    }
    // Set the LEDs to represent positive values
  } else if (y_value > 0) {
    // Clear unnecessary LEDs
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_RED);
    am_hal_gpio_output_clear(AM_BSP_GPIO_LED_BLUE);
    // The green LED is lit for all positive values
    am_hal_gpio_output_set(AM_BSP_GPIO_LED_GREEN);
    // The yellow LED is lit in only some cases
    if (y_value >= 0.75) {
      am_hal_gpio_output_set(AM_BSP_GPIO_LED_YELLOW);
    } else {
      am_hal_gpio_output_clear(AM_BSP_GPIO_LED_YELLOW);
    }
  }
  
  // Log the current X and Y values
  error_reporter->Report("x_value: [%f, %f, %f, %f, %f, %f, %f, %f, %f, %f] -> y_value: %f\n", 
    x_value[0], x_value[1], x_value[2], x_value[3], x_value[4], 
    x_value[5], x_value[6], x_value[7], x_value[8], x_value[9], y_value);
}
