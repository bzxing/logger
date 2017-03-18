#!/usr/bin/python

import socket
import sys
s = socket.socket()
host = socket.gethostname()
port = 9876

s.connect((host,port))
s.settimeout(2);

print "Connected!"

while True:
	print ">",
	line = raw_input()
	if line:
		s.send(line + "\r\n")
	else:
		try:
			data = s.recv(1024)
			print data;
		except socket.timeout:
			print "Done!"


