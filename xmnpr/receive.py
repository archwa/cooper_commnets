import receiver
import socket
import string
import sys
import zlib

# custom Receiver class
class Receiver(receiver.Receiver):
	BYTES_PER_PACKET = 256
	WINDOW_SZ = 512

	# initialize receiver
	def __init__(self):
		super(Receiver, self).__init__(timeout=1.5)
	
	# use sequence number and data to form checksum
	def checksum(self, seqNum, data):
		filledSeqNum = bin(seqNum)[2:].zfill(32).encode('utf-8')
		filledData = string.join([string.zfill(n, 8) for n in map(lambda s: s[2:], map(bin, data))], '').encode('utf-8')
		checksum = zlib.adler32(filledSeqNum + filledData) & 0xFFFFFFFF
		return checksum

	def receive (self):
		quit = False

		totalLength = 0
		rcvPkt = [None]

		while True:
			try:
				initLen = self.simulator.u_receive()
				init_seqNum = int(''.join([string.zfill(s, 8) for s in map(lambda n: n[2:], map(bin, initLen[0:4]))]), 2)
				
				checksum = self.checksum(init_seqNum, initLen[4:-4])
				init_checksum = int(''.join([string.zfill(s, 8) for s in map(lambda n: n[2:], map(bin, initLen[-4:]))]), 2)
				
				if checksum == init_checksum:
					totalLength = int(''.join([string.zfill(s, 8) for s in map(lambda n: n[2:], map(bin, initLen[4:-4]))]), 2)
					
					rcvPkt = [None]*totalLength

					checksum = bin(self.checksum(0, [0, 0, 0, 0]))[2:].zfill(32)
					packetedChecksum = bytearray([int(s, 2) for s in [checksum[:8], checksum[8:16], checksum[16:24], checksum[24:]]])

					self.simulator.u_send(bytearray([0, 0, 0, 0, 0]) + packetedChecksum)
					break

			except socket.timeout:
				pass

		while True:
			try:
				# receive packet
				pkt = self.simulator.u_receive()
				
				trans_and = 128
				for i in range(0, len(pkt)):
					trans_and = trans_and & pkt[i]
				
				if trans_and == 128:
					self.simulator.u_send(bytearray([128, 128, 128, 128, 128, 128, 128, 128]))
					quit = True

				else:
					seqNum = int(''.join([string.zfill(s, 8) for s in map(lambda n: n[2:], map(bin, pkt[0:4]))]), 2)

					checksum = self.checksum(seqNum, pkt[4:-4])
					pkt_checksum = int(''.join([string.zfill(s, 8) for s in map(lambda n: n[2:], map(bin, pkt[-4:]))]), 2)



					if checksum == pkt_checksum:
						components = bytearray(pkt[0:4]) + bytearray([ 255 ])
						checksum = bin(self.checksum(seqNum, [ 255 ]))[2:]
						packetedChecksum = bytearray([int(s, 2) for s in [checksum[:8], checksum[8:16], checksum[16:24], checksum[24:]]])

						packet = bytearray(components + packetedChecksum)
						
						self.simulator.u_send(packet)

						# save packet
						rcvPkt[seqNum] = pkt

			except socket.timeout:
				if quit:
					break
				else:
					pass

		for packet in rcvPkt:
			if packet != None:
				sys.stdout.write(packet[4:-4])

		sys.exit()

if __name__ == "__main__":
	rcvr = Receiver()
	rcvr.receive()
