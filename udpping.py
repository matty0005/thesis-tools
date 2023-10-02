import socket
import time

# Constants
UDP_IP = "10.20.1.33"
UDP_PORT = 9999

MESSAGE = b"a" * 256
MESSAGE = b"PING!!!"
NUM_PACKETS = 1000
INTERVAL = 0.05  # 50ms in seconds

def send_ping():
    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    total_time = 0  # Total accumulated time for all packets
    total_sent = 0

    sock.settimeout(1)  # Set timeout to 1 second
    for _ in range(NUM_PACKETS):
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
            total_sent += 1

            # Wait for 50ms before sending the next packet
            time.sleep(INTERVAL)

        except Exception as e:
            print(f"Error occurred: {e}")

    average_rtt = total_time / total_sent
    print(f"\nAverage RTT for {NUM_PACKETS} packets: {average_rtt:.2f}ms, dropped {NUM_PACKETS - total_sent} packets")

    sock.close()

send_ping()
