import socket
import time

# Constants
TCP_IP = "10.20.1.34"
TCP_PORT = 1337
MESSAGE = b"Here is a message of asdas"

def send_reply():
    # Create a TCP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    total_time = 0  # Total accumulated time for all packets
    total_sent = 0

    sock.settimeout(1)  # Set timeout to 1 second

    try:
        # Establish a connection
        sock.connect((TCP_IP, TCP_PORT))

        start_time = time.time()  # Record the current time

        # Send data
        sock.send(MESSAGE)

        # Receive response
        data = sock.recv(1000)
        end_time = time.time()  # Record the time after receiving the response

        hex_data = ' '.join([f'{byte:02x}' for byte in data])

        # Calculate the round-trip time for this packet
        rtt = (end_time - start_time) * 1000  # in milliseconds
        total_time += rtt

        print(f"Received response in {rtt:.2f}ms: {hex_data}")
        
    except Exception as e:
        print(f"Error occurred: {e}")

    sock.close()

send_reply()
