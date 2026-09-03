# LADOHD ETW Insider Threat Detection

An end-to-end anomaly detection pipeline designed to identify insider threats utilizing a Long Short-Term Memory (LSTM) neural network. The system intercepts raw OS kernel events via Event Tracing for Windows (ETW), constructs chronological behavioral sequences, and evaluates them dynamically against a benign baseline.

## System Architecture

* **ETW Agent (C++)**: A low-level kernel consumer operating in Ring 3. It securely parses `MachineName` and `Source` strings via the Trace Data Helper (TDH) API. Extracts are mapped to a predefined vocabulary and buffered in a sliding window mechanism (size = 10) before being transmitted via WinSock2 TCP.
* **Inference Server (Python)**: A containerized TensorFlow/Keras backend hosting the pre-trained LADOHD LSTM model. It evaluates incoming sequences using a dynamic threshold calculation (based on statistical mode) to classify execution anomalies.

## Prerequisites

* **Server Environment**: Docker Engine (utilizes `python:3.11-slim`).
* **Agent Environment**: Windows OS, CMake 3.10+, and MSVC Compiler (C++17 standard).
* **Permissions**: Administrative privileges are strictly required for ETW trace session allocation.

## Deployment & Execution

### 1. Launch the Inference Server (Docker)
Build the container and bind the listening socket to port 65432[cite: 2]. 
```bash
docker build -t ladohd-server .
docker run -p 65432:65432 ladohd-server
```
### 2. Compile the ETW Agent
Generate the build files and compile the C++ agent, ensuring linkage with ws2_32.lib and tdh.lib[cite: 2]. Run this from a Developer Command Prompt on the Windows host:
```cmd
mkdir build
cd build
cmake ..
cmake --build .
```
### 3. Pipeline Activation
Launch the system asynchronously using the provided batch script. This script automatically handles UAC elevation prompts for the C++ agent[cite: 2]:
```cmd
.\run_system.bat
```
