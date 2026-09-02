# Insider Threat Detection: LADOHD Framework Implementation

This repository implements an LSTM-based Anomaly Detector Over High-dimensional Data (LADOHD), directly referencing the architectural framework proposed in the TDSC paper by Miguel Villarreal-Vasquez et al. The primary objective is solving the Order-Aware Recognition (OAR) problem to identify stealthy insider threats that camouflage malicious intent within sequences of legitimate system events.

## Theoretical Foundation
Based on the LADOHD methodology, the system avoids static thresholding. Instead, it computes the probability distribution of sequential events and establishes a dynamic anomaly threshold utilizing the statistical mode of the network's output logits. This effectively partitions expected baseline behavior from statistically improbable sequences, identifying threats without generating excessive false positives.

## Current Implementation State
The project is configured for Google Colab deployment, utilizing TensorFlow/Keras.
* Data Preprocessing: Implements dynamic vocabulary building and sliding-window tensor generation for BPTT.
* Model Architecture: Features a 16-dimensional embedding layer, three stacked LSTM layers, and dense classification layers.
* Dynamic Evaluation: Implements the mode-based dynamic thresholding algorithm for real-time anomaly flagging.

## Future Integration (Work in Progress)
* Kaggle Dataset Integration: Integrating the CERT Insider Threat dataset to transition from simulated dummy data to real-world benign and malicious sequences.
* Low-Level OS Telemetry: Developing a system-level agent to capture raw execution logs directly from the host operating system to feed the neural network in real time.

## How to Run
1. Click the "Open In Colab" badge to launch the environment.
2. Select a GPU runtime (T4 recommended).
3. Execute the cells sequentially to initialize the model and observe the dynamic thresholding logic.

Author: Maayan Oshri