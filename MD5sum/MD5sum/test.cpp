#include <iostream>
#include "MD5.h"

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "eg:" << std::endl;
		std::cout << argv[0] << "\t0\tfilename" << std::endl;
		std::cout << argv[0] << "\t1\tfilename" << std::endl;
		return 0;
	}

	char option = *argv[1];

	MD5 md5;
	switch (option) {
	case'0':
		std::cout << md5.GetFileMD5(argv[2]) << std::endl;
	case'1':
		std::cout << md5.GetStringMD5(argv[2]) << std::endl;
	};

 	system("pause");
	return 0;
}