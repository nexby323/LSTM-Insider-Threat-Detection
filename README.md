# LADOHD ETW Insider Threat Detection

An end-to-end anomaly detection pipeline designed to identify insider threats utilizing a Long Short-Term Memory (LSTM) neural network. The system intercepts raw OS kernel events via Event Tracing for Windows (ETW), constructs chronological behavioral sequences, and evaluates them dynamically against a benign baseline.

## System Architecture
1. **ETW Agent (C++)**: A low-level kernel consumer operating in Ring 3, securely parsing `MachineName` and `Source` strings via the Trace Data Helper (TDH) API. Extracts are buffered in a sliding window mechanism (BPTT window size = 10) and transmitted via WinSock2 TCP.
2. **Inference Server (Python)**: A TensorFlow/Keras backend hosting the pre-trained LADOHD LSTM model. Evaluates incoming chronological tensors using a dynamic threshold calculation to classify standard execution versus anomalous deviation.

## Prerequisites
* Python 3.10+ (TensorFlow, NumPy, SciPy)
* CMake 3.10+ and MSVC Compiler (Windows)
* Administrative privileges (Required for ETW session allocation)

## Build Instructions
Generate the build files and compile the C++ agent utilizing CMake:
```cmd
mkdir build
cd build
cmake ..
cmake --build .