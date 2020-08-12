/*
 * ip_arp_udp_tcp.h
 *
 *  Created on: Aug 11, 2020
 *      Author: quoclaptk
 */

#ifndef IP_ARP_UDP_TCP_H_
#define IP_ARP_UDP_TCP_H_

#include "pgmspace.h"
#include "net.h"
#include "enc28j60.h"

class ip_arp_udp_tcp {
private:
	ENC28J60 eth;
	uint8_t wwwport=80;
	uint8_t macaddr[6];
	uint8_t ipaddr[4];
	int16_t info_hdr_len=0;
	int16_t info_data_len=0;
	uint8_t seqnum=0xa; // my initial tcp sequence number
	uint16_t ip_identifier = 1;
	uint16_t checksum(uint8_t *buf, uint16_t len,uint8_t type);
	void make_eth(uint8_t *buf);
	void make_eth_ip_new(uint8_t *buf, uint8_t* dst_mac);
	void fill_ip_hdr_checksum(uint8_t *buf);
	void make_ip_tcp_new(uint8_t *buf, uint16_t len,uint8_t *dst_ip);
	void make_ip(uint8_t *buf);
	void make_tcphead(uint8_t *buf,uint16_t rel_ack_num,uint8_t mss,uint8_t cp_seq);

public:
	ip_arp_udp_tcp();
	virtual ~ip_arp_udp_tcp();

	void init_ip_arp_udp_tcp(uint8_t *mymac,uint8_t *myip,uint8_t wwwp);

	uint8_t eth_type_is_arp_and_my_ip(uint8_t *buf,uint16_t len);
	uint8_t eth_type_is_ip_and_my_ip(uint8_t *buf,uint16_t len);
	void make_arp_answer_from_request(uint8_t *buf);
	void make_echo_reply_from_request(uint8_t *buf,uint16_t len);
	void make_udp_reply_from_request(uint8_t *buf,char *data,uint8_t datalen,uint16_t port);

	void make_tcp_synack_from_syn(uint8_t *buf);
	void init_len_info(uint8_t *buf);
	uint16_t get_tcp_data_pointer(void);
	uint16_t fill_tcp_data_p(uint8_t *buf,uint16_t pos, const char *progmem_s PROGMEM);
	uint16_t fill_tcp_data(uint8_t *buf,uint16_t pos, const char *s);
	void make_tcp_ack_from_any(uint8_t *buf);
	void make_tcp_ack_with_data(uint8_t *buf,uint16_t dlen);
	void make_arp_request(uint8_t *buf, uint8_t *server_ip);
	uint8_t arp_packet_is_myreply_arp ( uint8_t *buf );
	void tcp_client_send_packet(uint8_t *buf,uint16_t dest_port, uint16_t src_port, uint8_t flags, uint8_t max_segment_size,
	uint8_t clear_seqck, uint16_t next_ack_num, uint16_t dlength, uint8_t *dest_mac, uint8_t *dest_ip);
	uint16_t tcp_get_dlength ( uint8_t *buf );
};

#endif /* IP_ARP_UDP_TCP_H_ */
