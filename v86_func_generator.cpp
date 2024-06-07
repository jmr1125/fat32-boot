#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
int main() {
  std::ifstream fin("v86_func", std::ios::in|std::ios::binary);
  std::cout << "char v86_func_context[]={";
  char c;
  int count=0;
  while ((c = fin.get()) != EOF) {
    std::cout << (int)c << ",";
    ++count;
  }
  ++count;
  std::cout<<"};" << std::endl;
  std::cout<<"int v86_func_context_size = " << count << ";" << std::endl;
  //std::cout<<"int aaa[10] " << count << ";" << std::endl;
  return 0;
}
