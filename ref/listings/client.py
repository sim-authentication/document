#!/usr/bin/python

import socket
from binascii import *
from card.utils import *
from optparse import OptionParser
from card.USIM import USIM
from card.SIM import SIM

def handle_usim(options, rand_bin, autn_bin):
	u = USIM()
	if not u:
		print "Error opening USIM"
		exit(1)

	if options.debug:
		u.dbg = 2;

	imsi = u.get_imsi()
	ret = u.authenticate(rand_bin, autn_bin, ctx='3G')
	if len(ret) == 1:
		print "AUTS:\t%s" % b2a_hex(byteToString(ret[0]))
	else:
		print "RES:\t%s" % b2a_hex(byteToString(ret[0]))
		print "CK:\t%s" % b2a_hex(byteToString(ret[1]))
		print "IK:\t%s" % b2a_hex(byteToString(ret[2]))
		if len(ret) == 4:
			print "Kc:\t%s" % b2a_hex(byteToString(ret[3]))

	#ret = u.authenticate(rand_bin, autn_bin, ctx='2G')
	#if not len(ret) == 2:
	#	print "Error during 2G authentication"
	#	exit(1)
	#print "SRES:\t%s" % b2a_hex(byteToString(ret[0]))
	#print "Kc:\t%s" % b2a_hex(byteToString(ret[1]))

if __name__ == "__main__":
	u = USIM()
	u.debug = 2
	imsi = u.get_imsi()

	s = socket.socket()
	host = socket.gethostname()
	#host = '192.168.2.254'
	port = 12345
	s.connect((host, port))
	authenticated = False
	status = 0

	while True:
		print "\n"
		if authenticated == False:
			if status == 2:
				print "## auth=false; status=2 - trying to send imsi+auts"
				s.send("3"+imsi+auts)
			else:
				print "## auth=false; status!=2 - trying to send imsi only"
				s.send("1"+imsi)
			recvmsg = s.recv(1024)
			status = recvmsg[0:1]
			rand = recvmsg[16:48]
			print "RAND: "+rand
			autn = recvmsg[48:80]
			print "AUTN: "+autn

			rand_bin = stringToByte(a2b_hex(rand))
			autn_bin = stringToByte(a2b_hex(autn))

			ret = u.authenticate(rand_bin, autn_bin, ctx='3G')
			if len(ret) == 1:
				print "######## auth=false; status!=2; len(ret)=1 - trying to authenticate -> got auts"
				auts = b2a_hex(byteToString(ret[0]))
				print "AUTS: " + auts
				status = 2
			else:
				print "######## auth=false; status!=2, len(ret)>1 - tyring to authenticate -> got triple"
				res = b2a_hex(byteToString(ret[0]))
				print "RES: " + res
				ck = b2a_hex(byteToString(ret[1]))
				print "CK: " + ck
				ik = b2a_hex(byteToString(ret[2]))
				print "IK :" + ik
				authenticated = True

	s.close
