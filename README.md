mitm_rogue_wifi_ap
==================

MITM Attack Example Code with Rogue Wi-Fi AP

This is actually a blundle of tools and codes for Rogue Wi-Fi AP MITM Attack demonstration.

NOTICE
  The startAP and stopAP script will over-write any iptables setting. Back them
  up ahead!
  Sniffing and attacking network without permission is illegal. Think twice
  before your conduct.


Tools used
  - ifconfig:           Wi-Fi card setup and IP configuration

  - iptables:           detour target packets to libnetfilter_queue

  - hostapd:            soft AP control program

  - dhcpd:              dhcp server tool to create a private network within AP

  - libnetfilter_queue: (or NQUEUE), do actual packet detour, exam and mangle

files
  - startAP:            setup Wi-Fi card(hostapd), private network(hostapd),
                        packet detour(iptables)

  - libnetfilter.c      compile this file and run after ./startAP. This file
                        will handle all packets through NQUEUE as you
                        programmed.

  - change_ttl.py       python version of NQUEUE mangling(sorry I don't
                        remember the sourse/author of this script)

  - stopAP:             kill the AP and clean up necesary settings.

  - dhcpd.conf          dhcpd configuraton file, used by startAP

  - hostapd.conf        hostapd configuraton file, used by startAP

