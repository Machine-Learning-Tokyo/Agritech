SUSPENDED_BOT_TEST_SRCS := \
tensorflow/lite/experimental/micro/examples/suspended_bot/suspended_bot_test.cc \
tensorflow/lite/experimental/micro/examples/suspended_bot/suspended_bot_model_quantized.cc

SUSPENDED_BOT_TEST_HDRS := \
tensorflow/lite/experimental/micro/examples/suspended_bot/suspended_bot_model_quantized.h

OUTPUT_HANDLER_TEST_SRCS := \
tensorflow/lite/experimental/micro/examples/suspended_bot/output_handler_test.cc \
tensorflow/lite/experimental/micro/examples/suspended_bot/output_handler.cc

OUTPUT_HANDLER_TEST_HDRS := \
tensorflow/lite/experimental/micro/examples/suspended_bot/output_handler.h 

SUSPENDED_BOT_SRCS := \
tensorflow/lite/experimental/micro/examples/suspended_bot/main.cc \
tensorflow/lite/experimental/micro/examples/suspended_bot/suspended_bot_model_quantized.cc \
tensorflow/lite/experimental/micro/examples/suspended_bot/output_handler.cc \
tensorflow/lite/experimental/micro/examples/suspended_bot/ring_buffer.cc \
tensorflow/lite/experimental/micro/examples/suspended_bot/uart.cc  \
tensorflow/lite/experimental/micro/examples/suspended_bot/data_provider.cc 


SUSPENDED_BOT_HDRS := \
tensorflow/lite/experimental/micro/examples/suspended_bot/suspended_bot_model_quantized.h \
tensorflow/lite/experimental/micro/examples/suspended_bot/output_handler.h \
tensorflow/lite/experimental/micro/examples/suspended_bot/constants.h \
tensorflow/lite/experimental/micro/examples/suspended_bot/ring_buffer.h \
tensorflow/lite/experimental/micro/examples/suspended_bot/uart.h \
tensorflow/lite/experimental/micro/examples/suspended_bot/data_provider.h 


#Find any platform-specific rules for this example.
include $(wildcard tensorflow/lite/experimental/micro/examples/suspended_bot/*/Makefile.inc)

# Tests loading and running the suspended_bot model.
$(eval $(call microlite_test,suspended_bot_test,\
$(SUSPENDED_BOT_TEST_SRCS),$(SUSPENDED_BOT_TEST_HDRS)))

# Tests producing an output.
$(eval $(call microlite_test,output_handler_test,\
$(OUTPUT_HANDLER_TEST_SRCS),$(OUTPUT_HANDLER_TEST_HDRS)))

# Builds a standalone binary.
$(eval $(call microlite_test,suspended_bot,\
$(SUSPENDED_BOT_SRCS),$(SUSPENDED_BOT_HDRS)))
