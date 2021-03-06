## Suspended Bot position inferencing using tensorflow lite quantized model at the edge
Currently the input data is fed into the model over the serial communication, UART (Universal Asynchronous Receiver-Transmitter).

![Development Board](sparkfun_edge/sparkfun_edge.jpeg)

### TensorFlow Lite for Microcontrollers
TensorFlow Lite for Microcontrollers is an experimental port of TensorFlow Lite aimed at microcontrollers and other 
devices with only kilobytes of memory. It is designed to be portable even to "bare metal" systems, so it doesn't require 
operating system support, any standard C or C++ libraries, or dynamic memory allocation.


## Development Board
### SparkFun Edge
* 32-bit ARM Cortex-M4F processor (Ambiq Apollo3)
* 48MHz CPU clock (can be overclocked to 96MHz)
* Extremely low-power usage: 6uA/MHz (48 * 6 = 288uA)
* 1MB Flash
* 384KB SRAM

## Workflow

### Create TensorFlow model

The model must be small enough to fit on the target device after conversion, and it can only use supported operations. 
Provide your own implementations to use operations that are not currently supported.

### Convert the model to a TensorFlow Lite FlatBuffer

The model is converted into the standard TensorFlow Lite format with quantization using the TensorFlow Lite converter. 

```
import tensorflow as tf

converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.OPTIMIZE_FOR_SIZE]

tflite_model = converter.convert()

open("suspened_bot_quantized.tflite", "wb").write(tflite_model)

```

### Convert the FlatBuffer to a C byte array

Models are kept in read-only program memory and provided in the form of a simple C file. 
Standard tools like *xxd* can be used to convert the FlatBuffer into a C array (hex dump).
```
xxd -i suspened_bot_quantized.tflite > suspened_bot_quantized.cc

```

### Integrate the TensorFlow Lite for Microcontrollers C++ library

See [main.cc](https://github.com/Machine-Learning-Tokyo/Agritech/blob/naveen/Suspended_Robot/tflite_micro/suspended_bot/main.cc) for implementation.

### Deploy to Sparkfun Edge device

```
export DEVICENAME=/dev/cu.wchusbserial1440

export BAUD_RATE=921600

$ make -f tensorflow/lite/experimental/micro/tools/make/Makefile TARGET=sparkfun_edge suspended_bot_bin

$ cp tensorflow/lite/experimental/micro/tools/make/downloads/AmbiqSuite-Rel2.0.0/tools/apollo3_scripts/keys_info0.py \
tensorflow/lite/experimental/micro/tools/make/downloads/AmbiqSuite-Rel2.0.0/tools/apollo3_scripts/keys_info.py

$ python3 tensorflow/lite/experimental/micro/tools/make/downloads/AmbiqSuite-Rel2.0.0/tools/apollo3_scripts/create_cust_image_blob.py \
--bin tensorflow/lite/experimental/micro/tools/make/gen/sparkfun_edge_cortex-m4/bin/suspended_bot.bin \
--load-address 0xC000 \
--magic-num 0xCB \
-o main_nonsecure_ota \
--version 0x0

$ python3 tensorflow/lite/experimental/micro/tools/make/downloads/AmbiqSuite-Rel2.0.0/tools/apollo3_scripts/create_cust_wireupdate_blob.py \
--load-address 0x20000 \
--bin main_nonsecure_ota.bin \
-i 6 \
-o main_nonsecure_wire \
--options 0x1

$ python3 tensorflow/lite/experimental/micro/tools/make/downloads/AmbiqSuite-Rel2.0.0/tools/apollo3_scripts/uart_wired_update.py \
-b ${BAUD_RATE} ${DEVICENAME} \
-r 1 \
-f main_nonsecure_wire.bin \
-i 6

```
