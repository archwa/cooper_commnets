import sender
import socket
import string
import sys
import zlib

# custom Sender class
class Sender(sender.Sender):
	BYTES_PER_PACKET = 256
	WINDOW_SZ = 512
	
	# initialize sender
	def __init__(self):
		super(Sender, self).__init__(timeout=1)

	# use sequence number and data to form checksum
	def checksum(self, seqNum, data):
		filledSeqNum = bin(seqNum)[2:].zfill(32).encode('utf-8')
		filledData = string.join([string.zfill(n, 8) for n in map(lambda s: s[2:], map(bin, data))], '').encode('utf-8')
		checksum = zlib.adler32(filledSeqNum + filledData) & 0xFFFFFFFF
		return checksum

	# create packet array using data
	def packetizeData(self, data):
		packets = []

		for i in range(0, len(data), self.BYTES_PER_PACKET):
			upper = i + self.BYTES_PER_PACKET

			if upper > len(data):
				upper = len(data)

			# creating packet components
			seqNum = (i / self.BYTES_PER_PACKET) % 2**32
			packetedSeqNum = bytearray([(seqNum & 0xFF000000) >> 24, (seqNum & 0xFF0000) >> 16, (seqNum & 0xFF00) >> 8, seqNum & 0xFF])

			packetedData = data[i:upper]

			checksum = bin(self.checksum(seqNum, packetedData))[2:].zfill(32)
			packetedChecksum = bytearray([int(s, 2) for s in [checksum[:8], checksum[8:16], checksum[16:24], checksum[24:]]])

			packet = packetedSeqNum + packetedData + packetedChecksum
			packets.append({ "ackd": False, "pkt": bytearray(packet), "sent": False })

		return packets

	# override send method
	def send(self, data):
		packets = self.packetizeData(data)
		
		lower = 0
		windowSz = self.WINDOW_SZ

		if len(packets) < windowSz:
			windowSz = len(packets)

		# create initial packet detailing how long the transmission will be
		initPktData = bytearray([(len(packets) & 0xFF000000) >> 24, (len(packets) & 0xFF0000) >> 16, (len(packets) & 0xFF00) >> 8, len(packets) & 0xFF])

		lenChecksum = bin(self.checksum(0, initPktData))[2:].zfill(32)
		packetedLenChecksum = bytearray([int(s, 2) for s in [lenChecksum[:8], lenChecksum[8:16], lenChecksum[16:24], lenChecksum[24:]]])

		initPkt = bytearray([0, 0, 0, 0]) + initPktData + packetedLenChecksum

		# send transmission length data
		while True:
			try:
				self.simulator.u_send(initPkt)
				ack = self.simulator.u_receive()

				# verify checksum of ack packet
				checksum = self.checksum(0, [ 0, 0, 0, 0 ])
				ack_checksum = int(''.join([string.zfill(s, 8) for s in map(lambda n: n[2:], map(bin, ack[-4:]))]), 2)

				if checksum == ack_checksum:
					break

				else:
					pass
			except socket.timeout:
				pass

		# send all the data
		while True:
			try:
				upper = lower + windowSz

				if upper > len(packets):
					upper = len(packets)
				

				# send all unsent packets
				for i in range(lower, upper):
					if not packets[i]['sent']:
						self.simulator.u_send(packets[i]['pkt'])
						packets[i]['sent'] = True

				while True:
					ack = self.simulator.u_receive()
					
					ack_seqNum = int(''.join([string.zfill(s, 8) for s in map(lambda n: n[2:], map(bin, ack[0:4]))]), 2)
					checksum = self.checksum(ack_seqNum, ack[4:-4])
					ack_checksum = int(''.join([string.zfill(s, 8) for s in map(lambda n: n[2:], map(bin, ack[-4:]))]), 2)

					if checksum == ack_checksum:
						packets[ack_seqNum]['ackd'] = True

					all_ackd = True

					for i in range(lower, upper):
						all_ackd = packets[i]['ackd'] and all_ackd
						if not all_ackd:
							break

					if not all_ackd:
						pass
					
					else:
						lower += windowSz
						break
				
			except socket.timeout:
				unackd = 0
				for i in range(lower, upper):
					if not packets[i]['ackd']:
						self.simulator.u_send(packets[i]['pkt'])
						unackd += 1

				if unackd != 0:
					continue
				elif lower >= upper:
					break
					
		# end transmission
		while True:
			self.simulator.u_send(bytearray([128, 128, 128, 128, 128, 128, 128, 128]))

			try:
				ack = self.simulator.u_receive()
			except socket.timeout:
				continue
			
			trans_and = 128
			
			for i in range(0, len(ack)):
				trans_and = trans_and & ack[i]
			
			if trans_and == 128:
				break

			else:
				pass
		
		sys.exit()

if __name__ == "__main__":
	data = bytearray(sys.stdin.read())
	s = Sender()
	s.send(data)
