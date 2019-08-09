#include <memory.h>
#include <cmath>
#include <fstream>
#include "MD5.h"


MD5::MD5()
	:_chunkByte(N)
{
	init();

	memset(_chunk, 0, _chunkByte);
	_totalByte = _lastByte = 0;
}

void MD5::init() {
	//初始化元素
	_a = 0x67452301;
	_b = 0xefcdab89;
	_c = 0x98badcfe;
	_d = 0x10325476;
	
	//循环左移的位数
	size_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7,
		12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
		15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

	memcpy(_sft, s, sizeof(s));
	//_k[i] 2^32*abs(sin(i + 1))
	for (int i = 0; i < 64; i++) {
		_k[i] = (size_t)(pow(2, 32) * (abs(sin(i + 1))));
	}
}

//处理最后一块数据
void MD5::calculateMD5Final() {
	//_lastByte:表示最后一块数据的字节数(< 64bytes)
	unsigned char* p = _chunk + _lastByte;//需要填充的起始位置
	//填充位的前8bit：1000 0000
	*p++ = 0x80;
	//remainFillByte 填充完一个字节后剩余要填充的数据块的大小（字节为单位）
	size_t remainFillByte = _chunkByte - _lastByte - 1;
	//remainFillByte < 8,意味着这一块数据剩余的数据不够记录原文的长度
	//需要将这块数据剩余的部分全部填充成0，然后再用同样的方式去处理
	//最后新创一块新的64字节数据块
	//前448位数据填充为0，剩余64位记录原文的长度
	if (remainFillByte < 8) {
		//进行数据拼接
		memset(p, 0, remainFillByte);//剩余部分全部填0
		calculateMD5((size_t*)_chunk);//处理该块数据
		memset(_chunk, 0, _chunkByte);//构造新的数据块,全部填0
	}
	else {
		memset(p, 0, remainFillByte);//剩余字节大于8，长度足够
	}

	//最后的8byte存放原始文档的bit长度
	((unsigned long long*)_chunk)[7] = _totalByte * 8;
	calculateMD5((size_t*)_chunk);
}

void MD5::calculateMD5(size_t* chunk) {
	size_t a = _a;
	size_t b = _b;
	size_t c = _c;
	size_t d = _d;
	size_t f, g;//chunk[g] f:哈希函数返回值
	//64次变换，4轮操作，每一轮操作是16个子操作
	for (size_t i = 0; i < 64; i++) {
		if (0 <= i && i < 16) {
			f = F(b, c, d);
			g = i;
		}
		else if (i >= 16 && i < 32) {
			f = G(b, c, d);
			g = (5 * i + 1) % 16;
		}
		else if (i >= 32 && i < 48) {
			f = H(b, c, d);
			g = (3 * i + 5) % 16;
		}
		else {
			f = I(b, c, d);
			g = (7 * i) % 16;
		}
		//64次操作，每一次计算完哈希函数的返回值之后，更新a,b,c,d的值
		size_t dtemp = d;//保存d未更新之前的值
		d = c;
		c = b;
		//循环左移的数据块
		b = b + ShiftLeftRotate(a + f + _k[i] + chunk[g], _sft[i]);
		a = dtemp;
	}

	//当一个64字节的数据块处理完毕后，更新MD buffer的值
	_a += a;
	_b += b;
	_c += c;
	_d += d;
}

std::string MD5::GetFileMD5(const char* filename) {
	std::ifstream fin(filename, std::ifstream::binary);
	if (fin.is_open()) {
		while (!fin.eof()) {
			fin.read((char*)_chunk, _chunkByte);
			if (_chunkByte != fin.gcount()) {
				break;
			}

			_totalByte += _chunkByte;
			calculateMD5((size_t*)_chunk);

		}

		_lastByte = (size_t)fin.gcount();
		_totalByte += _lastByte;
		calculateMD5Final();
	}

	return ChangeHex(_a) + ChangeHex(_b) + ChangeHex(_c) + ChangeHex(_d);
}


std::string MD5::GetStringMD5(const std::string& str) {
	if (str.empty()) {
		return "";
	}
	else {
		unsigned char* pstr = (unsigned char*)str.c_str();
		size_t numChunk = str.size() / _chunkByte;
		for (size_t i = 0; i < numChunk; ++i) {
			_totalByte += _chunkByte;
			calculateMD5((size_t*)(pstr + i * _chunkByte));
		}

		_lastByte = str.size() % _chunkByte;
		memcpy(_chunk, pstr + _totalByte, _lastByte);
		calculateMD5Final();
	}
	return ChangeHex(_a) + ChangeHex(_b) + ChangeHex(_c) + ChangeHex(_d);
}

std::string MD5::ChangeHex(size_t num) {
	static std::string strMap = "0123456789abcdef";
	std::string ret;
	std::string bytestr;
	for (int i = 0; i < 4; i++) {
		bytestr = "";
		size_t b = (num >> (i * 8)) & 0xff;
		for (int j = 0; j < 2; j++) {
			bytestr.insert(0, 1, strMap[b % 16]);
			b /= 16;
		}
		ret += bytestr;
	}
	return ret;
}