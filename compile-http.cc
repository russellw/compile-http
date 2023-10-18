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
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;
using filesystem::path;

#ifdef NDEBUG
#define debug(a)
#else
#define debug(a) cout << __FILE__ << ':' << __LINE__ << ": " << __func__ << ": " << #a << ": " << (a) << '\n'
#endif

// input
string file;
string text;

void readText() {
	ifstream is(file, ios::in);
	text = {istreambuf_iterator<char>(is), istreambuf_iterator<char>()};

	// make sure input ends with a newline, to simplify parser code
	if (text.empty() || text.back() != '\n')
		text += '\n';
}

// SORT
void decl(ostream& os, string name, size_t n) {
	os << "const unsigned char " << name << "Data[" << to_string(n) << ']';
}

bool eq(const char* s, const char* t) {
	for (auto i = strlen(t); i--;)
		if (*s++ != *t++)
			return 0;
	return 1;
}

string esc(string s) {
	string o = "\"";
	for (auto c: s) {
		if (isprint((unsigned char)c)) {
			if (c == '"')
				o += '\\';
			o += c;
			continue;
		}
		char buf[7];
		sprintf_s(buf, sizeof buf, "\\x%02x\"\"", (unsigned char)c);
		o += buf;
	}
	return o + '"';
}

//
int main(int argc, char** argv) {
	try {
		for (int i = 1; i < argc; ++i) {
			file = argv[i];
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
