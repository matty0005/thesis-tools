import socket

def receive_udp_data(port):
    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Bind the socket to the port
    server_address = ('', port)
    print(f'Starting up on {server_address[0]} port {server_address[1]}')
    sock.bind(server_address)

    while True:
        print('\nWaiting to receive message')
        data, address = sock.recvfrom(4096)

        print(f'Received {len(data)} bytes from {address}')
        print(data.decode())


if __name__ == "__main__":
    receive_udp_data(10000)
