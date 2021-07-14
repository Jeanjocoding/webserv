import socket
import time

# create an INET, STREAMing socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# now connect to the web server on port 80 - the normal http port
s.connect(("127.0.0.1", 8001))
#to_send = "aa\r\na\r\nabb\r\nbbccccggg\r\nghh\r\nhhssssyyyykaritokar\r\nitokaritokarito\r\nkarito\r\nkaritokaritokari\r\ntokaritokaritokaritokar\r\nitokaritokaritokaritokaritokaritokarito\r\n\r\n"
#to_send = "aaaabbbbccccgggghhhhssssyyyykaritokaritokaritokaritokaritokaritokaritokaritokaritokaritokaritokaritokaritokaritokaritokaritokaritokarito\r\n\r\n"
#to_send = "aaaabbbbccccgggghhhhssssyyyy\r\nbbbbcccc\r\nq\r\nddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd\r\n"

mega_Str = "0".zfill(10);
print("len : " + str(len(mega_Str)))
#while (1):
to_send = "GET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     goodbye      \r\nConnection: c\r\n\r\nGET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     2oodbye2  \r\n\r\nGET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     3oodbye3\r\n\r\nGET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     4ofbye4\r\n\r\nGET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     5oodbye5      \r\n\r\n"
#to_send = "GET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     goodbye      \r\nContent-Length: 1 \r\nConnection: c\r\n\r\n1GET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     2oodbye2  \r\ncontent-length: 2    \r\n\r\n22GET /FZEFGEZ/RGERgergerG/erzfe HTTP/1.1\r\nTransfer-Encoding: gzip, chunked\r\nTrailer: bobo\r\n\r\n5\r\nhhhhh\r\nA\r\nbbbbbbbbbC\r\na\r\n" + mega_Str + "\r\n0\r\nbobo: bonjour\r\n\r\nGET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     3oodbye3\r\n\r\nGET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     4ofbye4\r\n\r\nGET /FZEFGEZ/RGERgergerG/gerge HTTP/1.1\r\nhello:     5oodbye5      \r\n\r\n"
#	to_send = "GET /FZEFGEZ/RGERgergerG/erzfe HTTP/1.1\r\nTransfer-Encoding: gzip, chunked\r\nTrailer: bobo\r\n\r\n5\r\nhhhhh\r\nA\r\nbbbbbbbbbC\r\na\r\n" + mega_Str + "\r\n0\r\nbobo: bonjour\r\n\r\n"
#	print(to_send)
s.send(to_send.encode())
resp = s.recv(1000);
print("response from server: " + resp.decode("ascii"))
time.sleep(10);

#s.close();