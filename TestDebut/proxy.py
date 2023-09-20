# coding=utf-8
import socket

IRC_SERVER = '127.0.0.1'
#IRC_SERVER = 'chat.freenode.net'
IRC_PORT = 6667  # le port sur lequel votre serveur IRC est en écoute

PROXY_PORT = 6668  # le port sur lequel le proxy est en écoute

def handle_client(client_socket):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.connect((IRC_SERVER, IRC_PORT))

    while True:
        data = client_socket.recv(4096)
        if not data: break

        # Interceptez ou modifiez les données ici si nécessaire
        print("Data from client:", data.decode())

        server_socket.send(data)
        response = server_socket.recv(4096)
        
        # Interceptez ou modifiez les données ici si nécessaire
        print("Data from server:", response.decode())

        client_socket.send(response)

    client_socket.close()
    server_socket.close()

def main():
    proxy = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    proxy.bind(('0.0.0.0', PROXY_PORT))
    proxy.listen(5)
    
    print(f"Proxy listening on port {PROXY_PORT}")

    while True:
        client, addr = proxy.accept()
        print(f"Accepted connection from {addr[0]}:{addr[1]}")
        handle_client(client)

main()

