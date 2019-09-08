#ifndef TENSORFLOW_LITE_EXPERIMENTAL_MICRO_EXAMPLES_DATA_PROVIDER_H_
#define TENSORFLOW_LITE_EXPERIMENTAL_MICRO_EXAMPLES_DATA_PROVIDER_H_

#include "tensorflow/lite/c/c_api_internal.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"

TfLiteStatus GetData(tflite::ErrorReporter* error_reporter, float* x_val);

#endif  // TENSORFLOW_LITE_EXPERIMENTAL_MICRO_EXAMPLES_DATA_PROVIDER_H_
