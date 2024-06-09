#include <fstream>
#include <iostream>
using namespace std;
int main(int argc, char **argv) {
  if (argc != 3) {
    cerr << "usage : " << argv[0] << " symbol [file]" << endl;
    cerr << "generate char [symbol]_context[]={...};int "
            "[symbol]_size=...;"
         << endl;
    return 1;
  }
  ifstream in(argv[2], ios::in | ios::binary);
  if (!in) {
    cerr << "Cannot open file " << argv[1] << endl;
    return 2;
  }
  cout << "unsigned char " << argv[1] << "_context[]={" << endl;
  int count = 0;
  while (in) {
    int c=in.get();
    if(c==EOF) break;
    cout << hex << "0x" << c << ",";
    ++count;
  }
  cout << "};";
  cout << "int " << argv[1] << "_size=" << dec << count << ";" << endl;
  return 0;
}
