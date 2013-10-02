import nfqueue, socket
from scapy.all import IP

def cb(dummy, payload):
	pkt = IP(payload.get_data())
	# set the TTL
	pkt.ttl = 24
	# clear the IP checksum so that Scapy recalculates it, since we modified the IP header
	del pkt.chksum
	# reinject the packet!
	payload.set_verdict_modified(nfqueue.NF_ACCEPT, str(pkt), len(pkt))


q = nfqueue.queue()
q.open()
q.bind(socket.AF_INET)
q.set_callback(cb)
q.create_queue(0) 
try:
	q.try_run()
except KeyboardInterrupt:
	print "Exiting..." 
q.unbind(socket.AF_INET)
q.close()
