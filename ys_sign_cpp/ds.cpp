#include "common.h"
#include <iomanip>
#include <sstream>
#include <cstring>

class MD5 {
private:
	// 常量定义
	static const uint32_t s[64];
	static const uint32_t K[64];

	// 基础操作函数
	static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z)
	{
		return (x & y) | (~x & z);
	}

	static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z)
	{
		return (x & z) | (y & ~z);
	}

	static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z)
	{
		return x ^ y ^ z;
	}

	static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z)
	{
		return y ^ (x | ~z);
	}

	static inline uint32_t rotate_left(uint32_t x, uint32_t n)
	{
		return (x << n) | (x >> (32 - n));
	}

	// 主转换函数
	static void transform(uint32_t state[4], const uint8_t block[64])
	{
		uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
		uint32_t M[16];

		// 将 64 字节块转换为 16 个 32 位字
		for (int i = 0; i < 16; ++i)
		{
			M[i] = (block[i * 4]) | (block[i * 4 + 1] << 8) |
				(block[i * 4 + 2] << 16) | (block[i * 4 + 3] << 24);
		}

		// 主循环
		for (int i = 0; i < 64; ++i)
		{
			uint32_t f, g;

			if (i < 16)
			{
				f = F(b, c, d);
				g = i;
			}
			else if (i < 32)
			{
				f = G(b, c, d);
				g = (5 * i + 1) % 16;
			}
			else if (i < 48)
			{
				f = H(b, c, d);
				g = (3 * i + 5) % 16;
			}
			else {
				f = I(b, c, d);
				g = (7 * i) % 16;
			}

			uint32_t temp = d;
			d = c;
			c = b;
			b = b + rotate_left(a + f + K[i] + M[g], s[i]);
			a = temp;
		}

		state[0] += a;
		state[1] += b;
		state[2] += c;
		state[3] += d;
	}

public:
	static std::string hash(const std::string& input)
	{
		// 初始化状态
		uint32_t state[4] =
		{
			0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476
		};

		// 预处理：添加填充位和长度
		std::vector<uint8_t> message;
		message.reserve(input.size() + 64);
		message.assign(input.begin(), input.end());

		// 添加 1 比特
		message.push_back(0x80);

		// 填充 0 直到长度满足 mod 512 = 448
		while ((message.size() % 64) != 56)
		{
			message.push_back(0x00);
		}

		// 添加原始消息长度的 64 位表示（小端序）
		uint64_t bit_length = input.size() * 8;
		for (int i = 0; i < 8; ++i)
		{
			message.push_back((bit_length >> (i * 8)) & 0xFF);
		}

		// 处理每个 512 位块
		for (size_t i = 0; i < message.size(); i += 64)
		{
			transform(state, &message[i]);
		}

		// 转换为十六进制字符串（小端序）
		std::stringstream ss;
		ss << std::hex << std::setfill('0');
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ss << std::setw(2) << ((state[i] >> (j * 8)) & 0xFF);
			}
		}

		return ss.str();
	}
};

// 初始化常量
const uint32_t MD5::s[64] =
{
	7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
	6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

const uint32_t MD5::K[64] =
{
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

#include <chrono>
#include <random>
#include <format>

std::string getDS(std::string body)
{
	// 盐
	const std::string salt = "t0qEgfub6cvueAPgR5m9aQWWVciEer7v";
	// 时间戳
	const int t = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	// 随机数
	std::random_device rd;  // 随机种子
	std::mt19937_64 gen(rd()); // 使用 Mersenne Twister 算法
	std::uniform_int_distribution<int> dist(100000, 200000);

	int r = dist(gen);
	if (r == 100000)
	{
		r = 642367;
	}

	std::string str = std::format("salt={}&t={}&r={}&b={}", salt, t, r, body);
	std::string ds = MD5::hash(str);
	return std::format("{},{},{}", t, r, ds);
}
