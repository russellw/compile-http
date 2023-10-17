
void decl(ostream& os, string name, int n) {
	os << "const unsigned char " << name << "Data[" << to_string(n) << ']';
}

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

			// data.hxx
			{
				ofstream os("data.hxx");
				os << "extern ";
				decl(os, name, header.size() + bytes.size());
				os << ';';
			}

			// data.cxx
			{
				ofstream os("data.cxx");
				os << "#include \"data.hxx\"\n";
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
