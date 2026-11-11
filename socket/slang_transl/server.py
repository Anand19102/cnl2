
import socket

PORT = 12345

dictionary = {
    "tbh": "to be honest",
    "ig": "I guess",
    "tbf": "to be fair",
    "atm": "at the moment",
    "irl": "in real life",
    "lol": "laughing out loud",
    "asap": "as soon as possible",
    "omg": "oh my God",
    "ttyl": "talk to you later",
    "idk": "I don't know",
    "nvm": "never mind",
    "idc": "I don't care"
}

sockfd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sockfd.bind(("0.0.0.0", PORT))

print("UDP Server running...")

data, client_addr = sockfd.recvfrom(1024)

message = data.decode().rstrip("\x00")

print("Received:", message)

words = message.split()

translated = []

for word in words:
    translated.append(dictionary.get(word, word))

result = " ".join(translated)

print("Translated:", result)

sockfd.sendto(result.encode() + b"\0", client_addr)

sockfd.close()

