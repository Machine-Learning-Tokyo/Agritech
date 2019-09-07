#include "tensorflow/lite/experimental/micro/examples/suspended_bot/suspended_bot_model_quantized.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/experimental/micro/testing/micro_test.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

TF_LITE_MICRO_TESTS_BEGIN

TF_LITE_MICRO_TEST(LoadModelAndPerformInference) {
  // Set up logging
  tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = ::tflite::GetModel(g_suspended_bot_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.\n",
        model->version(), TFLITE_SCHEMA_VERSION);
  }

  // This pulls in all the operation implementations we need
  tflite::ops::micro::AllOpsResolver resolver;

  // Create an area of memory to use for input, output, and intermediate arrays.
  // Finding the minimum value for your model may require some trial and error.
  const int tensor_arena_size = 3 * 1024;
  uint8_t tensor_arena[tensor_arena_size];

  // Build an interpreter to run the model with
  tflite::MicroInterpreter interpreter(model, resolver, tensor_arena,
                                       tensor_arena_size, error_reporter);

  // Allocate memory from the tensor_arena for the model's tensors
  interpreter.AllocateTensors();

  // Obtain a pointer to the model's input tensor
  TfLiteTensor* input = interpreter.input(0);

  // Make sure the input has the properties we expect
  TF_LITE_MICRO_EXPECT_NE(nullptr, input);
  // The property "dims" tells us the tensor's shape. It has one element for
  // each dimension. Our input is a 2D tensor containing 1 element, so "dims"
  // should have size 2.
  TF_LITE_MICRO_EXPECT_EQ(2, input->dims->size);
  // The value of each element gives the length of the corresponding tensor.
  // We should expect two single element tensors (one is contained within the
  // other).
  TF_LITE_MICRO_EXPECT_EQ(1, input->dims->data[0]);
  TF_LITE_MICRO_EXPECT_EQ(10, input->dims->data[1]);
  // The input is a 32 bit floating point value
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteFloat32, input->type);
//(array([6. , 2. , 4. , 6.5, 4.5, 4.5, 0. , 0. , 1. , 0. ]),


  // Provide an input value
  input->data.f[0] = 6.0;
  input->data.f[1] = 2.0;
  input->data.f[2] = 4.0;
  input->data.f[3] = 6.5;
  input->data.f[4] = 4.5;
  input->data.f[5] = 4.5;
  input->data.f[6] = 0.0;
  input->data.f[7] = 0.0;
  input->data.f[8] = 1.0;
  input->data.f[9] = 0.0;
  
  // Run the model on this input and check that it succeeds
  TfLiteStatus invoke_status = interpreter.Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed\n");
  }
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);

  // Obtain a pointer to the output tensor and make sure it has the
  // properties we expect. It should be the same as the input tensor.
  TfLiteTensor* output = interpreter.output(0);
  TF_LITE_MICRO_EXPECT_EQ(2, output->dims->size);
  TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[0]);
  TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[1]);
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteFloat32, output->type);

  // Obtain the output value from the tensor
  // [-0.37784552]
  float value = output->data.f[0];
  error_reporter->Report("output= %f", value);
  // Check that the output value is within 0.05 of the expected value
  TF_LITE_MICRO_EXPECT_NEAR(-0.377, value, 0.05);

  // // Run inference on several more values and confirm the expected outputs
  // input->data.f[0] = 1.;
  // interpreter.Invoke();
  // value = output->data.f[0];
  // TF_LITE_MICRO_EXPECT_NEAR(0.841, value, 0.05);

  // input->data.f[0] = 3.;
  // interpreter.Invoke();
  // value = output->data.f[0];
  // TF_LITE_MICRO_EXPECT_NEAR(0.141, value, 0.05);

  // input->data.f[0] = 5.;
  // interpreter.Invoke();
  // value = output->data.f[0];
  // TF_LITE_MICRO_EXPECT_NEAR(-0.959, value, 0.05);
}

TF_LITE_MICRO_TESTS_END
