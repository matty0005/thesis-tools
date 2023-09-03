import socket

def send_udp_data(ip, port, message):
    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    try:
        # Send data
        print(f'Sending {message} to {ip}:{port}')
        sent = sock.sendto(message.encode(), (ip, port))

    finally:
        print('Closing socket')
        sock.close()


if __name__ == "__main__":
    send_udp_data('10.20.1.120', 10000, 'Here is some UDP data over port 10,000!')
