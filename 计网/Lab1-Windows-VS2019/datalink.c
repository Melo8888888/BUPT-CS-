#include <stdio.h>
#include <string.h>
#include "datalink.h"
#include "protocol.h"


// 序号增加宏：当序号达到上限时回绕归零，这种写法简单直观一点点
#define inc(k) ((k) = ((k) < MAX_SEQ ? (k) + 1 : 0))
// 定义各个定时器超时时间（单位：毫秒）
// DATA_TIMER：用于数据帧超时检测；这个值直接影响协议的响应速度和鲁棒性 
#define DATA_TIMER 2500
// ACK_TIMER：用于确认帧超时检测；较短的超时值能及时反馈ACK信息 调参发现250-340之间更好
#define ACK_TIMER 290
// 最大帧序号，决定了序列号的循环范围； 本例中 六位序列号  为63
#define MAX_SEQ 63
// 帧缓冲区的大小，由最大序号决定，一般为序号空间的一半
#define NR_BUFS ((MAX_SEQ + 1) / 2)





typedef unsigned char frame_kind;      // 帧类型（如数据帧、确认帧、拒绝帧）
typedef unsigned char seq_nr;          // 帧序号
typedef unsigned char packet[PKT_LEN]; // 数据包：一个数组，长度为PKT_LEN
typedef unsigned char boolean;         // 布尔类型，0表示假，非0表示真

// 封装了帧类型、序号、确认号、数据及填充位
typedef struct {
	frame_kind kind; // 帧的类型（数据、ACK、NAK）
	seq_nr ack;      // 携带的确认序号，表示期望下一个正确接收的帧序号
	seq_nr seq;      // 数据帧的序号，用于区分不同帧
	unsigned char data[PKT_LEN]; // 数据部分，存放要传输的实际内容
	unsigned int padding; // 填充位，用于对齐或占位（不影响帧的主要功能！）
} Frame;

// 全局变量
static int phl_ready = 0;  // 表示物理层是否空闲，初始时认为物理层不可用
static boolean no_nak = 1; // 表示是否已经发送过NAK；1表示还未发送


// 这是滑动窗口协议中判断帧是否在期望范围内的核心函数 判断序号是否在区间内（左闭右开区间）
static boolean between(seq_nr a, seq_nr b, seq_nr c)
{
	return ((a <= b && b < c) || (c < a && b < c) || (c < a && a <= b));
}

// 函数：计算并附加校验和后，传送给物理层
// 校验和算法使用CRC32，保证数据传输的完整性，学生认为这部分代码充分体现了网络协议中数据可靠性的重要性
static void put_frame(unsigned char *frame, int len)
{
	// 将CRC32校验码放在帧末尾（4字节），方便接收方检测传输错误
	*(unsigned int *)(frame + len) = crc32(frame, len);
	// 发送帧（数据+校验码）
	send_frame(frame, len + 4);
	// 发送后默认认为物理层缓存已满，需要等待物理层回调通知
	phl_ready = 0;
}

// 构建并发送一个帧（可以是数据帧、ACK帧或NAK帧）
// 通过frame_expected倒推出最近成功接收的帧的序号作为ACK返回，同时从发送缓冲区中取出对应的数据包发送
static void send__frame(frame_kind fk, seq_nr frame_nr, seq_nr frame_expected,
			packet buffer[])
{
	Frame s; // 临时帧变量，用于构造要发送的帧

	s.kind = fk;
	s.seq = frame_nr;
	// 计算ACK值：当前接收窗口下界的前一个帧的序号作为ACK值返回
	// 这里采用了模运算以适应序号循环的特性
	s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
	// 一旦ACK捎带发送，就停止ACK定时器，这有助于减少不必要的重复ACK
	stop_ack_timer();

	if (fk == FRAME_DATA) {
		// 数据帧：从发送缓冲区中拷贝出对应的数据到帧中
		memcpy(s.data, buffer[frame_nr % NR_BUFS], PKT_LEN);

		dbg_frame("We Send DATA %d %d, ID %d\n", s.seq, s.ack,
			  *(short *)s.data);
		// 发送数据帧：这里的3为frame_kind, seq和ack三个字节，加上数据部分长度
		put_frame((unsigned char *)&s, 3 + PKT_LEN);
		// 开始数据帧定时器，等待对该帧的ACK；如果超时则会重传
		start_timer(frame_nr, DATA_TIMER);
	} else if (fk == FRAME_ACK) {
		// ACK帧：仅包含帧类型和ACK序号，数据部分不发送
		dbg_frame("We Send ACK  %d\n", s.ack);
		put_frame((unsigned char *)&s, 2);
	} else {
		// NAK帧：用于请求重传错误帧。注意：NAK的序号为当前期望的帧序号
		dbg_frame("We Send NAK  %d\n", frame_expected);
		no_nak = 0; // 标记已经发送过NAK，避免重复发送
		put_frame((unsigned char *)&s, 2);
	}
}

// 主函数：程序入口，主要负责滑动窗口协议的发送和接收逻辑
int main(int argc, char **argv)
{
	// 滑动窗口变量初始化：
	seq_nr ack_expected = 0; // 发送窗口的下限，即下一个未被确认的数据帧序号
	seq_nr next_frame_to_send =
		0; // 发送窗口的上限+1，表示下一个要发送的数据帧序号
	seq_nr frame_expected = 0; // 接收窗口的下限，表示期望接收的下一帧序号
	seq_nr too_far = NR_BUFS;  // 接收窗口的上界+1

	// 定义发送和接收缓冲区，用于存放数据包
	packet in_buf[NR_BUFS];  // 接收窗口缓冲，用于临时存储接收的帧
	packet out_buf[NR_BUFS]; // 发送窗口缓冲，用于存储待发送的数据包
	boolean arrived
		[NR_BUFS]; // 标记接收缓冲区中每个位置是否已有数据，初始时均为空
	seq_nr nbuffered = 0; // 记录发送但未确认的帧的个数

	// 初始化接收缓冲区的标记为0，表示所有位置均为空
	for (int i = 0; i < NR_BUFS; ++i)
		arrived[i] = 0;

	int event;   // 用于存储事件类型
	int arg;     // 用于存储定时器序号或其它事件参数
	Frame f;     // 用于存储接收到的帧
	int len = 0; // 接收帧的长度

	// 初始化协议（例如，网络和物理层的设置）
	protocol_init(argc, argv);
	lprintf("Melo && Songshuyu : " __DATE__ "  "__TIME__
		"\n");

	// 刚开始禁止网络层，以防发送缓冲区满
	disable_network_layer();

	// 进入主循环，等待并处理各种事件
	for (;;) {
		// 等待事件发生（如网络层数据、物理层准备好、帧接收等）
		event = wait_for_event(&arg);

		switch (event) {
		case NETWORK_LAYER_READY:
			// 网络层准备好数据，获取数据包存入发送缓冲区
			get_packet(out_buf[next_frame_to_send % NR_BUFS]);
			nbuffered++; // 增加待发送帧计数
			// 发送数据帧，同时捎带ACK（告知接收方已成功接收的最新帧）
			send__frame(FRAME_DATA, next_frame_to_send,
				    frame_expected, out_buf);
			// 更新发送窗口的上界，注意序号回绕
			inc(next_frame_to_send);
			break;

		case PHYSICAL_LAYER_READY:
			// 当物理层空闲后，设置标志位，允许继续发送
			phl_ready = 1;
			break;

		case FRAME_RECEIVED:
			// 当接收到一帧数据时，调用recv_frame读取数据到f中，并返回数据长度
			len = recv_frame((unsigned char *)&f, sizeof f);

			// 检查帧长度是否足够，以及CRC校验是否通过，确保数据未被破坏
			if (len < 6 || crc32((unsigned char *)&f, len) != 0) {
				dbg_event(
					"****  Error, Bad CRC Checksum\n");
				// 出现错误且之前未发送NAK，发送NAK请求重传
				if (no_nak) {
					send__frame(FRAME_NAK, 0,
						    frame_expected, out_buf);
				}
				// 错误帧直接丢弃，不进行进一步处理
				break;
			}

			// 处理ACK帧：仅记录ACK信息，并打印调试信息
			if (f.kind == FRAME_ACK)
				dbg_frame("Recv ACK  %d\n", f.ack);

			// 处理数据帧
			if (f.kind == FRAME_DATA) {
				dbg_frame("Recv DATA %d %d, ID %d\n", f.seq,
					  f.ack, *(short *)f.data);

				// 如果接收到的帧序号不是期望的，并且之前没有发送NAK，则主动发送NAK请求重传
				if (f.seq != frame_expected && no_nak) {
					dbg_event(
						"Recv frame is not  expected, NAK sent back\n");
					send__frame(FRAME_NAK, 0,
						    frame_expected, out_buf);
				}

				// 判断接收到的帧是否在当前接收窗口内，并且该缓冲区位置还未被占用
				if (between(frame_expected, f.seq, too_far) &&
				    (arrived[f.seq % NR_BUFS] == 0)) {
					arrived[f.seq % NR_BUFS] =
						1; // 标记该位置已接收数据

					// 注意：len = PKT_LEN + 3 + 4 (CRC) - 4，所以数据长度为 len - 7
					memcpy(in_buf[f.seq % NR_BUFS], f.data,
					       len - 7);

					// 当接收缓冲中从期望帧开始连续有数据时，交付网络层，并更新窗口
					while (arrived[frame_expected %
						       NR_BUFS]) {
						dbg_event(
							"Put packet to network layer seq:%d, ID: %d\n",
							frame_expected,
							*(short *)(in_buf[frame_expected %
									  NR_BUFS]));
						put_packet(
							in_buf[frame_expected %
							       NR_BUFS],
							len - 7);
						arrived[frame_expected %
							NR_BUFS] =
							0; // 释放缓冲区位置
						no_nak =
							1; // 交付后，重置NAK标志
						inc(frame_expected); // 窗口下界前移
						inc(too_far); // 窗口上界前移
						// 每交付一帧就启动ACK定时器，确保ACK能及时发送
						start_ack_timer(ACK_TIMER);
					}
				}
			}

			// 处理NAK帧：如果收到NAK，且被请求重传的帧在发送窗口中，则重发该帧
			if (f.kind == FRAME_NAK &&
			    between(ack_expected, (f.ack + 1) % (MAX_SEQ + 1),
				    next_frame_to_send)) {
				dbg_frame("Recv NAK  %d --%dbyte\n",
					  (f.ack + 1) % (MAX_SEQ + 1), len);
				// 重发被NAK请求的帧
				send__frame(FRAME_DATA,
					    (f.ack + 1) % (MAX_SEQ + 1),
					    frame_expected, out_buf);
			}

			// 处理累计确认：通过一个循环，利用收到的（可能带有累计确认信息的）ACK f.ack，
			// 逐个处理从当前窗口下界 ack_expected 开始的、所有已被该 ACK 确认的帧。每处理一个帧，
			// 就减少缓存计数、停止其定时器，并将窗口下界向前推进一步。
			// 直到 ack_expected 指向一个尚未被 f.ack 确认的帧时，循环停止
			while (between(ack_expected, f.ack,
				       next_frame_to_send)) {
				nbuffered--;              // 减少待确认计数
				stop_timer(ack_expected); // 停止对应定时器
				inc(ack_expected);        // 发送窗口下界前移
			}
			break;

		case DATA_TIMEOUT:
			// 数据帧超时：说明对应帧的ACK未能及时收到，触发重传机制
			dbg_event("---- DATA %d timeout, resend ----\n", arg);
			send__frame(FRAME_DATA, arg, frame_expected, out_buf);
			break;

		case ACK_TIMEOUT:
			// ACK定时器超时：及时发送独立ACK帧，确保通信双方同步
			send__frame(FRAME_ACK, 0, frame_expected, out_buf);
			break;
		}

		// 流量控制：如果发送窗口未满且物理层准备好，则通知网络层继续发送数据
		if (nbuffered < NR_BUFS && phl_ready)
			enable_network_layer();
		else
			disable_network_layer();
	}
}
