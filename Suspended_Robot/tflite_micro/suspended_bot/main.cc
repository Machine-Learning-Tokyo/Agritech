#include "tensorflow/lite/experimental/micro/examples/suspended_bot/data_provider.h"
#include "tensorflow/lite/experimental/micro/examples/suspended_bot/output_handler.h"
#include "tensorflow/lite/experimental/micro/examples/suspended_bot/suspended_bot_model_quantized.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "uart.h"

int main(int argc, char* argv[]) {
  Uart g_uart;
  g_uart.begin(115200);

  // Set up logging
  tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;
  error_reporter->Report("hiiii\n");

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = ::tflite::GetModel(g_suspended_bot_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.\n",
        model->version(), TFLITE_SCHEMA_VERSION);
    return 1;
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

  // Obtain pointers to the model's input and output tensors
  TfLiteTensor* input = interpreter.input(0);
  TfLiteTensor* output = interpreter.output(0);

  // Keep track of how many inferences we have performed
  // int inference_count = 0;
  char str[128];
  uint8_t ch, i = 0;

  // Loop indefinitely
  while (true) {
    if (g_uart.available() > 0) {
      ch = g_uart.read();

      if (ch != ';') {
        str[i++] = ch;
      } else {
        str[i] = '\0';
        i = 0;

        //error_reporter->Report("%s\n", str);

        char* tok = strtok(str, ",");
        int j = 0;

        while (tok != NULL) {
          input->data.f[j++] = atof(tok);
          tok = strtok(NULL, ",");
        }

        // Run inference, and report any error
        TfLiteStatus invoke_status = interpreter.Invoke();
        if (invoke_status != kTfLiteOk) {
          error_reporter->Report("Invoke failed on x_val");
          continue;
        }

        // Read the predicted y value from the model's output tensor
        float y_val = output->data.f[0];

        HandleOutput(error_reporter, input->data.f, y_val);
      }
    }
  }
}
