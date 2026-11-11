
import socket
import struct
import time

BUFFER_SIZE = 65536

sockfd = socket.socket(socket.AF_PACKET, socket.SOCK_RAW,
                       socket.htons(3))

print("RAW SOCKET PACKET CAPTURE")
print("Press Ctrl+C to stop...")

while True:
    packet, addr = sockfd.recvfrom(BUFFER_SIZE)

    if len(packet) < 14:
        continue

    dest_mac, src_mac, protocol = struct.unpack("!6s6sH", packet[:14])

    dest_mac = ":".join("%02X" % b for b in dest_mac)
    src_mac = ":".join("%02X" % b for b in src_mac)

    print("\n-----------------------------")
    print("Packet Length:", len(packet), "Bytes")

    print("\nEthernet Header")
    print("Destination MAC:", dest_mac)
    print("Source MAC:", src_mac)
    print("Protocol: 0x%04X" % protocol)

    if protocol == 0x0800:

        if len(packet) < 34:
            continue

        ip_header = packet[14:34]

        version_ihl = ip_header[0]
        version = version_ihl >> 4
        ihl = (version_ihl & 15) * 4

        if version != 4 or ihl < 20 or len(packet) < 14 + ihl:
            continue

        ttl = ip_header[8]
        proto = ip_header[9]

        src_ip = socket.inet_ntoa(ip_header[12:16])
        dst_ip = socket.inet_ntoa(ip_header[16:20])

        print("\nIP Header")
        print("Source IP:", src_ip)
        print("Destination IP:", dst_ip)
        print("TTL:", ttl)
        print("Protocol:", proto)

        transport_offset = 14 + ihl

        fragment = struct.unpack("!H", ip_header[6:8])[0]

        if fragment & 0x1FFF:
            continue

        if proto == 6:

            if len(packet) < transport_offset + 20:
                continue

            source_port, dest_port = struct.unpack(
                "!HH", packet[transport_offset:transport_offset + 4])

            print("\nTCP Header")
            print("Source Port:", source_port)
            print("Destination Port:", dest_port)

        elif proto == 17:

            if len(packet) < transport_offset + 8:
                continue

            source_port, dest_port = struct.unpack(
                "!HH", packet[transport_offset:transport_offset + 4])

            print("\nUDP Header")
            print("Source Port:", source_port)
            print("Destination Port:", dest_port)

        elif proto == 1:

            if len(packet) < transport_offset + 4:
                continue

            icmp_type, icmp_code = struct.unpack(
                "!BB", packet[transport_offset:transport_offset + 2])

            print("\nICMP Header")
            print("Type:", icmp_type)
            print("Code:", icmp_code)

