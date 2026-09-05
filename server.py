import socket #for communicating through the network 
import struct #for packing and unpacking raw bytes data 
import numpy as np #for math operations efficently
import tensorflow as tf  #for ML infastructure
from scipy import stats #for scintific calculations 
import os # Added for path checking
import sys # Added for exit handling
import subprocess #for running other files (like training the model)

def detect_anomaly_dynamic(model, previous_sequence, actual_next_event):
    # Dynamic threshold logic (identical to your Colab implementation)
    seq_tensor = np.expand_dims(previous_sequence, axis=0)
    logits = model.predict(seq_tensor, verbose=0)
    probabilities = tf.nn.softmax(logits[0, -1, :]).numpy()
    rounded_probs = np.round(probabilities, decimals=3)
    
    mode_result = stats.mode(rounded_probs, keepdims=False)
    dynamic_threshold = mode_result.mode
    
    actual_event_prob = probabilities[actual_next_event]
    return actual_event_prob <= dynamic_threshold


print("Trying to load trained LADOHD model...")

MODEL_PATH = 'ladohd_model.keras'

# check if model exists, if not, trigger the training script automatically
if not os.path.exists(MODEL_PATH):
    print(f"[System] Model '{MODEL_PATH}' not found. Initiating automatic training...")
    try:
        # convert the Jupyter Notebook to a standard Python script dynamically
        print("[System] Converting Jupyter Notebook to Python script...")
        subprocess.run([sys.executable, "-m", "jupyter", "nbconvert", "--to", "python", "lstm_threat_model.ipynb"], check=True)
        
        # execute the newly generated python script
        print("[System] Executing training script...")
        subprocess.run([sys.executable, "lstm_threat_model.py"], check=True)
        
        print("[System] Training completed successfully. Proceeding to load model...")
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] Automatic training failed: {e}. Ensure internet is connected.")
        sys.exit(1)

print("Loading trained LADOHD model...")
model = tf.keras.models.load_model(MODEL_PATH)


HOST = '127.0.0.1'
PORT = 65432
WINDOW_SIZE = 10
# 4 bytes per 32-bit integer
BYTES_PER_WINDOW = WINDOW_SIZE * 4 

def start_inference_server():
    #opening socket in python
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, PORT))
        server_socket.listen()
        print(f"Server actively listening on {HOST}:{PORT}...")

        conn, addr = server_socket.accept()
        with conn:
            print(f"Connection established with C++ Agent at {addr}")
            while True:
                data = conn.recv(BYTES_PER_WINDOW)
                if not data:
                    break
                
                # Unpack the binary payload into a tuple of exactly 10 integers
                event_window = struct.unpack(f'{WINDOW_SIZE}i', data)
                sequence_array = np.array(event_window)
                
                print(f"\n[+] Received sliding window: {sequence_array}")
                
                # For this real-time simulation, the target event is the final action in the window
                target_event = sequence_array[-1]
                is_anomaly = detect_anomaly_dynamic(model, sequence_array, target_event)
                
                if is_anomaly:
                    print("🚨 ANOMALY DETECTED: Execution deviates from benign baseline!")
                else:
                    print("✅ Sequence recognized as benign.")

if __name__ == "__main__":
    start_inference_server()