# coding=utf-8
import socket

IRC_SERVER = '127.0.0.1'
#IRC_SERVER = 'chat.freenode.net'
IRC_PORT = 6667  # le port sur lequel votre serveur IRC est en écoute

PROXY_PORT = 6668  # le port sur lequel le proxy est en écoute

# Codes de couleurs
RED = "\033[91m"
BLUE = "\033[94m"
ENDC = "\033[0m"

def handle_client(client_socket):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.connect((IRC_SERVER, IRC_PORT))

    while True:
        data = client_socket.recv(4096)
        if not data: break

        decoded_data = data.decode()

        # Capturer et afficher les commandes NICK et USER
#        if "NICK" in decoded_data or "USER" in decoded_data:
#            print(f"{RED}NICK or USER command received:{ENDC} {decoded_data}")

        # Interceptez ou modifiez les données ici si nécessaire
        print(f"{RED}Data from client:{ENDC}", decoded_data)

        server_socket.send(data)
        response = server_socket.recv(4096)
        
        # Interceptez ou modifiez les données ici si nécessaire
        print(f"{BLUE}Data from server:{ENDC}", response.decode())

        client_socket.send(response)

    client_socket.close()
    server_socket.close()

def main():
    proxy = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    proxy.bind(('0.0.0.0', PROXY_PORT))
    proxy.listen(5)
    
    print(f"{BLUE}Proxy listening on port {PROXY_PORT}{ENDC}")

    while True:
        client, addr = proxy.accept()
        print(f"{BLUE}Accepted connection from {addr[0]}:{addr[1]}{ENDC}")
        handle_client(client)

if __name__ == '__main__':
    main()
