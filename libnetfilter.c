#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>	/* for NF_ACCEPT */
#include <libnetfilter_queue/libnetfilter_queue.h>
/* add -lnetfilter_queue to link library */

#define QUEUEMAXLEN 1024

/* returns packet id */
static u_int32_t
print_pkt (struct nfq_data *tb)
{
  int id = 0;
  struct nfqnl_msg_packet_hdr *ph;
  int pktLen, iByte;
  unsigned char *data;

  ph = nfq_get_msg_packet_hdr (tb);
  if (ph)
    {
      id = ntohl (ph->packet_id);
    }

  pktLen = nfq_get_payload (tb, &data);	// This payload is the IPv4 packet
  if (pktLen >= 0)
    {
      //printf("payload_len=%d ", pktLen);
      for (iByte = 0; iByte < pktLen; ++iByte)
	{
	  if (iByte % 16 == 0)
	    {
	      printf ("\n%05X  ", iByte);
	    }
	  else if (iByte % 8 == 0)
	    {
	      printf (" ");
	    }
	  printf ("%02X ", data[iByte]);
	}
    }


  return id;
}

// this is the callback function by nf_queue packet handler
static int
cb (struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
    struct nfq_data *nfa, void *data)
{
  //printf("entering callback\n");
  //u_int32_t id = print_pkt(nfa);
  return nfq_set_verdict (qh, id, NF_ACCEPT, 0, NULL);
}


int
main (int argc, char **argv)
{
  struct nfq_handle *h;
  struct nfq_q_handle *qh;
  struct nfnl_handle *nh;
  int fd;
  int rv;
  char buf[4096] __attribute__ ((aligned));

  printf ("opening library handle\n");
  h = nfq_open ();
  if (!h)
    {
      fprintf (stderr, "error during nfq_open()\n");
      exit (1);
    }

  printf ("unbinding existing nf_queue handler for AF_INET (if any)\n");
  if (nfq_unbind_pf (h, AF_INET) < 0)
    {
      fprintf (stderr, "error during nfq_unbind_pf()\n");
      exit (1);
    }

  printf ("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
  if (nfq_bind_pf (h, AF_INET) < 0)
    {
      fprintf (stderr, "error during nfq_bind_pf()\n");
      exit (1);
    }

  printf ("binding this socket to queue '0'\n");
  //                                           0 is nf_queue_number, you can define multiple queues
  qh = nfq_create_queue (h, 0, &cb, NULL);
  if (!qh)
    {
      fprintf (stderr, "error during nfq_create_queue()\n");
      exit (1);
    }

  printf ("setting copy_packet mode\n");
  if (nfq_set_mode (qh, NFQNL_COPY_PACKET, 0xffff) < 0)
    {
      fprintf (stderr, "can't set packet_copy mode\n");
      exit (1);
    }

  printf ("setting kernel queue maxlen\n");
  if (nfq_set_queue_maxlen (qh, QUEUEMAXLEN) < 0)
    {
      fprintf (stderr, "can't set kernel queue maxlen\n");
      exit (1);
    }


  fd = nfq_fd (h);

  while ((rv = recv (fd, buf, sizeof (buf), 0)) && rv >= 0)
    {
      printf ("\npkt received\n");
      nfq_handle_packet (h, buf, rv);
    }

  //printf("unbinding from queue 0\n");
  nfq_destroy_queue (qh);

#ifdef INSANE
  /* normally, applications SHOULD NOT issue this command, since
   * it detaches other programs/sockets from AF_INET, too ! */
  printf ("unbinding from AF_INET\n");
  nfq_unbind_pf (h, AF_INET);
#endif

  printf ("closing library handle\n");
  nfq_close (h);

  exit (0);
}
