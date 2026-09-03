import socket
import struct
import numpy as np
import tensorflow as tf  
from scipy import stats

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

print("Loading trained LADOHD model...")
#need ladohd_model.keras for the model TODO: if not exist activate the model
model = tf.keras.models.load_model('ladohd_model.keras')

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