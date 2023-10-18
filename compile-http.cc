#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
using filesystem::path;

#ifdef NDEBUG
#define debug(a)
#else
#define debug(a) cout << __FILE__ << ':' << __LINE__ << ": " << __func__ << ": " << #a << ": " << (a) << '\n'
#endif

void decl(ostream& os, string name, size_t n) {
	os << "const unsigned char " << name << "Data[" << to_string(n) << ']';
}

int main(int argc, char** argv) {
	try {
		vector<string> files;
		for (int i = 1; i < argc; ++i) {
			auto s = argv[i];
			if (*s == '-') {
				while (*s == '-')
					++s;
				switch (*s) {
				case 'V':
				case 'v':
					cout << "compile-http version 0\n";
					return 0;
				case 'h':
					cout << "Usage: compile-http [options] files\n";
					cout << "Writes compiled-http.cxx,hxx\n";
					cout << "\n";
					cout << "-h  Show help\n";
					cout << "-V  Show version\n";
					return 0;
				}
				throw runtime_error(string(argv[i]) + ": unknown option");
			}
			files.push_back(s);
		}

		remove("compiled-http.cxx");
		remove("compiled-http.hxx");
		for (auto& file: files) {
			auto name = path(file).stem().string();

			// input file
			ifstream is(file, ios::binary);
			vector<unsigned char> bytes{istreambuf_iterator<char>(is), istreambuf_iterator<char>()};

			// HTTP header
			auto header = "HTTP/1.1 200\r\n"
						  "Cache-Control:public,max-age=31536000,immutable\r\n"
						  "Content-Type:image/png\r\n"
						  "Content-Length:" +
						  to_string(bytes.size()) + "\r\n\r\n";

			// hxx
			{
				ofstream os("compiled-http.hxx", ios::app);

				os << "extern ";
				decl(os, name, header.size() + bytes.size());
				os << ';';
			}

			// cxx
			{
				ofstream os("compiled-http.cxx", ios::app);

				os << "extern ";
				decl(os, name, header.size() + bytes.size());
				os << ';';

				decl(os, name, header.size() + bytes.size());
				os << '{';
				for (auto c: header)
					os << (int)c << ',';
				for (auto c: bytes)
					os << (int)c << ',';
				os << "};";
			}
		}
		return 0;
	} catch (exception& e) {
		cerr << e.what() << '\n';
		return 1;
	}
}
