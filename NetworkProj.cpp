#include <iostream>
#include <string>
#include <vector>
#include <cstring> //memory operations
#include <cstdlib> //standard library
#include <cstdio> //standard input/output
#include <unistd.h> //unix standard
#include <sys/socket.h> //socket library
#include <arpa/inet.h> //inet_addr
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>


using namespace std;

// Function to parse and display an Ethernet header
void parseEthernetHeader(const u_char* buffer) {
    struct ethhdr *eth = (struct ethhdr *)buffer;
    cout << "Ethernet Header:" << endl;
    cout << "Source MAC: ";
    for (int i = 0; i < 6; i++) {
        printf("%02X", eth->h_source[i]);
        if (i < 5) cout << ":";
    }
    cout << endl;

    cout << "Destination MAC: ";
    for (int i = 0; i < 6; i++) {
        printf("%02X", eth->h_dest[i]);
        if (i < 5) cout << ":";
    }
    cout << endl;
    cout << "Protocol: " << ntohs(eth->h_proto) << endl;
}

// Function to parse an IP header
void parseIPHeader(const u_char* buffer) {
    struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in source, dest;

    source.sin_addr.s_addr = ip->saddr;
    dest.sin_addr.s_addr = ip->daddr;

    cout << "\nIP Header:" << endl;
    cout << "Source IP: " << inet_ntoa(source.sin_addr) << endl;
    cout << "Destination IP: " << inet_ntoa(dest.sin_addr) << endl;
    cout << "Protocol: " << (unsigned int)ip->protocol << endl;
}

// Packet capture function
void capturePackets() {
    int sock_raw;
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);
    unsigned char *buffer = new unsigned char[65536];

    // Create a raw socket
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Socket Error");
        exit(EXIT_FAILURE);
    }

    cout << "Starting to capture packets..." << endl;

    while (true) {
        // Receive a packet
        int data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr, (socklen_t*)&saddr_len);
        if (data_size < 0) {
            perror("Recvfrom Error");
            break;
        }

        // Parse Ethernet header
        parseEthernetHeader(buffer);

        // Parse IP header if present
        parseIPHeader(buffer);

        cout << "-------------------------------------------" << endl;
    }

    // Close the socket
    close(sock_raw);
    delete[] buffer;
}

int main() {
    cout << "Network Monitoring Program" << endl;

    try {
        capturePackets();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}