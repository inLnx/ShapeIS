#include <ak/math.h>
#include <ak/string.h>
#include <errno/error.h>
#include <memory/vmm.h>
#include <net/ethernet.h>
#include <net/ip.h>
#include <net/net.h>
#include <net/buffer.h>
#include <proc/task.h>
#include "udp.h"

#define MAX_UDP_HEADER (sizeof(struct ethernet_packet) + sizeof(struct ip4_packet) + sizeof(struct udp_packet))

/**
 * @brief udp calculate checksum
 * 
 * @param udp 
 * @param packet_len 
 * @param source_ip 
 * @param dest_ip 
 * @return uint16_t 
 */
uint16_t udp_calculate_checksum(struct udp_packet *udp, uint16_t packet_len, uint32_t source_ip, uint32_t dest_ip) {
	udp->checksum = 0;
	return transport_calculate_checksum(udp, packet_len, IP4_PROTOCAL_UDP, source_ip, dest_ip);
}

/**
 * @brief udp validate header
 * 
 * @param udp 
 * @param source_ip 
 * @param dest_ip 
 * @return int 
 */
int udp_validate_header(struct udp_packet *udp, uint32_t source_ip, uint32_t dest_ip) {
	uint16_t received_checksum = udp->checksum;
	if (!received_checksum)
		return 0;

	uint16_t packet_checksum = udp_calculate_checksum(udp, ntohs(udp->length), source_ip, dest_ip);
	udp->checksum = received_checksum;

	return packet_checksum != received_checksum ? -EPROTO : 0;
}

/**
 * @brief udp build header
 * 
 * @param udp 
 * @param packet_len 
 * @param source_ip 
 * @param source_port 
 * @param dest_ip 
 * @param dest_port 
 */
void udp_build_header(struct udp_packet *udp, uint16_t packet_len, uint32_t source_ip, uint16_t source_port, uint32_t dest_ip, uint16_t dest_port) {
	udp->source_port = htons(source_port);
	udp->dest_port = htons(dest_port);
	udp->length = htons(packet_len);
	udp->checksum = udp_calculate_checksum(udp, packet_len, source_ip, dest_ip);
}

/**
 * @brief udp bind
 * 
 * @param sock 
 * @param myaddr 
 * @param sockaddr_len 
 * @return int 
 */
int udp_bind(struct socket *sock, struct sockaddr *myaddr, int sockaddr_len) {
	struct inet_sock *isk = inet_sk(sock->sk);
	memcpy(&isk->ssin, myaddr, sockaddr_len);
	return 0;
}

/**
 * @brief udp connect
 * 
 * @param sock 
 * @param vaddr 
 * @param sockaddr_len 
 * @return int 
 */
int udp_connect(struct socket *sock, struct sockaddr *vaddr, int sockaddr_len) {
	struct inet_sock *isk = inet_sk(sock->sk);
	memcpy(&isk->dsin, vaddr, sockaddr_len);

	sock->state = SS_CONNECTED;
	return 0;
}

/**
 * @brief udp sendmsg
 * 
 * @param sock 
 * @param msg 
 * @param msg_len 
 * @return int 
 */
int udp_sendmsg(struct socket *sock, void *msg, size_t msg_len) {
	if (sock->state == SS_DISCONNECTED)
		return -ESHUTDOWN;

	struct inet_sock *isk = inet_sk(sock->sk);
	struct sk_buff *skb = skb_alloc(MAX_UDP_HEADER, msg_len);
	skb->sk = sock->sk;
	skb->dev = isk->sk.dev;

	skb_put(skb, msg_len);
	memcpy(skb->data, msg, msg_len);

	skb_push(skb, sizeof(struct udp_packet));
	skb->h.udph = (struct udp_packet *)skb->data;
	udp_build_header(skb->h.udph, skb->len, isk->ssin.sin_addr, isk->ssin.sin_port, isk->dsin.sin_addr, isk->dsin.sin_port);

	skb_push(skb, sizeof(struct ip4_packet));
	skb->nh.iph = (struct ip4_packet *)skb->data;
	ip4_build_header(skb->nh.iph, skb->len, IP4_PROTOCAL_UDP, isk->ssin.sin_addr, isk->dsin.sin_addr, rand());

	ip4_sendmsg(sock, skb);
	return 0;
}

/**
 * @brief udp recvmsg
 * 
 * @param sock 
 * @param msg 
 * @param msg_len 
 * @return int 
 */
int udp_recvmsg(struct socket *sock, void *msg, size_t msg_len) {
	if (sock->state == SS_DISCONNECTED)
		return -ESHUTDOWN;

	struct sock *sk = sock->sk;
	struct sk_buff *skb = NULL;

	while (!skb)
	{
		skb = list_first_entry_or_null(&sk->rx_queue, struct sk_buff, sibling);
		if (!skb)
		{
			update_thread(sk->owner_thread, THREAD_WAITING);
			schedule();
		}
	}

	list_del(&skb->sibling);

	uint32_t payload_len = min(msg_len, htons(skb->h.udph->length) - sizeof(struct udp_packet));
	memcpy(msg, (uint8_t *)skb->h.udph + sizeof(struct udp_packet), payload_len);

	skb_free(skb);
	return payload_len;
}

/**
 * @brief udp handler
 * 
 * @param sock 
 * @param skb 
 * @return int 
 */
int udp_handler(struct socket *sock, struct sk_buff *skb) {
	if (sock->state == SS_DISCONNECTED)
		return -ESHUTDOWN;

	struct inet_sock *isk = inet_sk(sock->sk);
	int32_t ret = ethernet_rcv(skb);
	if (ret < 0)
		return ret;

	ret = ip4_rcv(skb);
	if (ret < 0)
		return ret;

	if (skb->nh.iph->protocal != IP4_PROTOCAL_UDP)
		return -EPROTO;

	struct udp_packet *udp = (struct udp_packet *)skb->data;
	ret = udp_validate_header(udp, ntohl(skb->nh.iph->source_ip), ntohl(skb->nh.iph->dest_ip));
	if (ret < 0)
		return ret;

	if (htonl(skb->nh.iph->dest_ip) == isk->ssin.sin_addr && htons(udp->dest_port) == isk->ssin.sin_port &&
		htonl(skb->nh.iph->source_ip) == isk->dsin.sin_addr && htons(udp->source_port) == isk->dsin.sin_port)
	{
		skb->h.udph = udp;

		list_add_tail(&skb->sibling, &sock->sk->rx_queue);
		update_thread(sock->sk->owner_thread, THREAD_READY);
	}
	return 0;
}

struct proto_ops udp_proto_ops = {
	.family = PF_INET,
	.obj_size = sizeof(struct inet_sock),
	.bind = udp_bind,
	.ioctl = inet_ioctl,
	.connect = udp_connect,
	.sendmsg = udp_sendmsg,
	.recvmsg = udp_recvmsg,
	.shutdown = socket_shutdown,
	.handler = udp_handler,
};
