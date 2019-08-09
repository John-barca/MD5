#pragma once
#include <string>
#define N 64

class MD5 {
public:
	MD5();

	
	std::string GetFileMD5(const char* filename);

	std::string GetStringMD5(const std::string & str);
private:
	
	void init();

	void calculateMD5(size_t* chunk);

	void calculateMD5Final();

	std::string ChangeHex(size_t num);

	size_t F(size_t b, size_t c, size_t d) {
		return (b & c) | ((~b) & d);
	}

	size_t G(size_t b, size_t c, size_t d) {
		return (b & d) | (c & (~d));
	}

	size_t H(size_t b, size_t c, size_t d) {
		return b ^ c ^ d;
	}

	size_t I(size_t b, size_t c, size_t d) {
		return c ^ (b | (~d));
	}

	size_t ShiftLeftRotate(size_t num, size_t n) {
		//先左移n位
		//再右移位数 - n位
		//再将两数进行 或 运算得到循环左移的结果
		return (num << n) | (num >> (32 - n));
	}

private:
	/*_atemp = 0x67452301;
	_btemp = 0xefcdab89; 
	_ctemp = 0x98badcfe; 
	_dtemp = 0x10325476; */
	size_t _a;
	size_t _b;
	size_t _c;
	size_t _d;

	size_t _k[N];
	size_t _sft[N];

	const size_t _chunkByte;
	unsigned char _chunk[N];
	size_t _lastByte;
	unsigned long long _totalByte;
};