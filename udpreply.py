import socket
import time

# Constants
UDP_IP = "10.20.1.34"
UDP_PORT = 1337
MESSAGE = b"hello world!"


def send_reply():
    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    total_time = 0  # Total accumulated time for all packets
    total_sent = 0

    sock.settimeout(1)  # Set timeout to 1 second
    try:
        start_time = time.time()  # Record the current time

        # Send data
        sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))

        # Receive response
        data, server = sock.recvfrom(100)
        end_time = time.time()  # Record the time after receiving the response

        # Calculate the round-trip time for this packet
        rtt = (end_time - start_time) * 1000  # in milliseconds
        total_time += rtt

        print(f"Received response from {server} in {rtt:.2f}ms: {data.decode()}")
        
    except Exception as e:
        print(f"Error occurred: {e}")

    sock.close()

send_reply()
