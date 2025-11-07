#!/usr/bin/env python3
import socket

try:
    # Create socket and connect
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('localhost', 42842))
    print("Connected to localhost:42842")

    stuff = s.recv(1024)  # Wait for server response
    print("Received response from server")

    print("Server says:", stuff.decode())

    print("Closing socket")
    
except ConnectionRefusedError:
    print("Connection refused - server not running")
except Exception as e:
    print(f"Error: {e}")