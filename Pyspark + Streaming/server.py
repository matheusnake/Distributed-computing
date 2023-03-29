import socket
from time import sleep

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
HOST, PORT = 'localhost', 12345
sock.bind((HOST, PORT))

print("Waiting client to connect")
sock.listen(1)
(connection, a) = sock.accept()


while True:
    conn, addr = sock.accept()
    print('\nConnected with -> ', addr)
    try:
        print('\nReading file\n')
        with open('songsCharts.csv', encoding="utf8") as f:
            for line in f:
                out = line.encode('utf-8')
                print('Sending line',line)
                conn.send(out)
                sleep(0.3)
            print('End Of Stream.')
    except socket.error:
        print ('Error Occured.\n\nClient disconnected.\n')

conn.close()
