import socket

# create an INET, STREAMing socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# now connect to the web server on port 80 - the normal http port
s.connect(("127.0.0.1", 8001))

to_send = "aaaabbbbccccgggghhhhssssyyyy\r\n\r\n"
#to_send = "aaaabbbbccccgggghhhhssssyyyy\r\nbbbbcccc\r\nq\r\nddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd\r\n"
s.send(to_send.encode())



resp = s.recv(1000);

print("response from server: " + resp.decode("usascii"))

s.close();