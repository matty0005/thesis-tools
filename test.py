import socket
import time
import platform
import requests
import subprocess

# Constants
IP_ADDR = "10.20.1.34"
TCP_PORT = 1337
MESSAGE = b"RISCy Firewall Message"
UDP_PORT = 1337

def send_tcp_message(payload, ip, port):
    # Create a TCP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    total_time = 0  # Total accumulated time for all packets

    sock.settimeout(1)  # Set timeout to 1 second

    try:
        # Establish a connection
        sock.connect((ip, port))

        start_time = time.time()  # Record the current time

        # Send data
        sock.send(payload)

        # Receive response
        data = sock.recv(1000)
        end_time = time.time()  # Record the time after receiving the response

        hex_data = ' '.join([f'{byte:02x}' for byte in data])

        # Calculate the round-trip time for this packet
        rtt = (end_time - start_time) * 1000  # in milliseconds
        total_time += rtt

        print(f"TCP received response in {rtt:.2f}ms: {hex_data}")
        
    except Exception as e:
        print(f"TCP: ({ip}, {port}) Error occurred: {e}")

    sock.close()



def send_udp_message(payload, ip, port):
    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    total_time = 0  # Total accumulated time for all packets

    sock.settimeout(1)  # Set timeout to 1 second
    try:
        start_time = time.time()  # Record the current time

        # Send data
        sock.sendto(payload, (ip, port))

        # Receive response
        data, server = sock.recvfrom(100)
        end_time = time.time()  # Record the time after receiving the response

        # Calculate the round-trip time for this packet
        rtt = (end_time - start_time) * 1000  # in milliseconds
        total_time += rtt

        print(f"UDP received response from {server} in {rtt:.2f}ms: {data.decode()}")
        
    except Exception as e:
        print(f"UDP: ({ip}, {port}) Error occurred: {e}")

    sock.close()


def icmp_ping(host, count=1):
    """
    Sends an ICMP ping to the specified host and returns the time taken.

    Parameters:
    - host (str): The target host to ping.
    - count (int): The number of pings to send. Default is 1.

    Returns:
    - float: Time taken for the ping in milliseconds. Returns None if failed.
    """

    # Check the system type to adapt the ping command accordingly
    param = "-n" if platform.system().lower() == "windows" else "-c"

    # Execute the ping command and capture the output
    try:
        output = subprocess.check_output(["ping", param, str(count), host], universal_newlines=True)
        
        if platform.system().lower() == "windows":
            # Extract time for Windows (time=xms format)
            for line in output.splitlines():
                if "time=" in line:
                    return float(line.split("time=")[1].split("ms")[0])
        else:
            # Extract time for Linux/Mac (time=x.xx ms format)
            for line in output.splitlines():
                if "time=" in line:
                    return float(line.split("time=")[1].split(" ms")[0])

    except subprocess.CalledProcessError:
        return None

def test_icmp_ping(address):
    # Test the function
    time_taken = icmp_ping(address, 1)
    if time_taken:
        print(f"Ping to {address} took {time_taken} ms")
    else:
        print(f"{address} is not reachable")


def http_request(url):
    """
    Sends an HTTP GET request to the given URL.

    Parameters:
    - url (str): The target URL for the GET request.

    Returns:
    - str: Response content if the request was successful.
    - None: If the request failed.
    """
    try:
        response = requests.get(url, timeout=2)
        response.raise_for_status()  # Raise an exception for HTTP errors

        return response.text

    except requests.RequestException as e:
        return None


def test_http_request(url):
    start_time = time.time()  # Record the current time

    response_content = http_request(url)
    if response_content:
        end_time = time.time()  # Record the time after receiving the response
        rtt = (end_time - start_time) * 1000  # in milliseconds
        print(f"HTTP received from {url} in {round(rtt, 2)}ms")
    else:
        print(f"Failed to retrieve content from {url}")

def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('8.8.8.8', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP

print(f"Testing from IP: {get_local_ip()}")

test_icmp_ping(IP_ADDR)
send_tcp_message(MESSAGE, IP_ADDR, TCP_PORT)
send_udp_message(MESSAGE, IP_ADDR, UDP_PORT)
send_udp_message(MESSAGE, IP_ADDR, UDP_PORT)
send_udp_message(MESSAGE, IP_ADDR, 9999)
test_http_request(f"http://{IP_ADDR}/")
test_http_request(f"http://{IP_ADDR}/")

