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
	//��ʼ��Ԫ��
	_a = 0x67452301;
	_b = 0xefcdab89;
	_c = 0x98badcfe;
	_d = 0x10325476;
	
	//ѭ�����Ƶ�λ��
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

//�������һ������
void MD5::calculateMD5Final() {
	//_lastByte:��ʾ���һ�����ݵ��ֽ���(< 64bytes)
	unsigned char* p = _chunk + _lastByte;//��Ҫ������ʼλ��
	//���λ��ǰ8bit��1000 0000
	*p++ = 0x80;
	//remainFillByte �����һ���ֽں�ʣ��Ҫ�������ݿ�Ĵ�С���ֽ�Ϊ��λ��
	size_t remainFillByte = _chunkByte - _lastByte - 1;
	//remainFillByte < 8,��ζ����һ������ʣ������ݲ�����¼ԭ�ĵĳ���
	//��Ҫ���������ʣ��Ĳ���ȫ������0��Ȼ������ͬ���ķ�ʽȥ����
	//����´�һ���µ�64�ֽ����ݿ�
	//ǰ448λ�������Ϊ0��ʣ��64λ��¼ԭ�ĵĳ���
	if (remainFillByte < 8) {
		//��������ƴ��
		memset(p, 0, remainFillByte);//ʣ�ಿ��ȫ����0
		calculateMD5((size_t*)_chunk);//����ÿ�����
		memset(_chunk, 0, _chunkByte);//�����µ����ݿ�,ȫ����0
	}
	else {
		memset(p, 0, remainFillByte);//ʣ���ֽڴ���8�������㹻
	}

	//����8byte���ԭʼ�ĵ���bit����
	((unsigned long long*)_chunk)[7] = _totalByte * 8;
	calculateMD5((size_t*)_chunk);
}

void MD5::calculateMD5(size_t* chunk) {
	size_t a = _a;
	size_t b = _b;
	size_t c = _c;
	size_t d = _d;
	size_t f, g;//chunk[g] f:��ϣ��������ֵ
	//64�α任��4�ֲ�����ÿһ�ֲ�����16���Ӳ���
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
		//64�β�����ÿһ�μ������ϣ�����ķ���ֵ֮�󣬸���a,b,c,d��ֵ
		size_t dtemp = d;//����dδ����֮ǰ��ֵ
		d = c;
		c = b;
		//ѭ�����Ƶ����ݿ�
		b = b + ShiftLeftRotate(a + f + _k[i] + chunk[g], _sft[i]);
		a = dtemp;
	}

	//��һ��64�ֽڵ����ݿ鴦����Ϻ󣬸���MD buffer��ֵ
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