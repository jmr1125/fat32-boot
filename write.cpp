#include "fat32.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
vector<DWORD> fat;
bootsector bootsec;
const vector<pair<QWORD, DWORD>> cluster_map = {{66600, 0},        //<=66600
                                                {32680, 2},        //<=32680
                                                {262144, 4},       //<=262144
                                                {16777216, 8},     //<=16777216
                                                {33554432, 16},    //<=33554432
                                                {67108864, 32},    //<=67108864
                                                {0xFFFFFFFF, 64}}; //>67108864
// size vs. SecPerClus

QWORD disk_size = (QWORD)1024 * 1024 * 1024; // 64 MiB
int BytesPerSec = 512;
int SecPerClus = 0;
int heads = 0, SecPerTrk = 0;
int Media = 0xf8;
// upper_bound(cluster_map.begin(), cluster_map.end(), disk_size / BytesPerSec,
//             [](long long x, pair<QWORD, DWORD> y) { return x < y.first; })
//     ->second;
int total_clusters = 2;

struct filename {
  string x;
  operator string() const { return x; }
  auto substr(int a, int b = -1) const { return x.substr(a, b); }
  auto find_last_of(char c) const { return x.find_last_of(c); }
  auto find(char c) const { return x.find(c); }
  auto size() const { return x.size(); }
  auto operator[](int i) const { return x[i]; }
  auto &operator[](int i) { return x[i]; }
  filename(string s) : x(s) {}
};
namespace std {
template <> struct hash<filename> {
  auto operator()(const filename &f) const { return hash<string>()(f.x); }
};
} // namespace std
ostream &operator<<(ostream &os, const filename &f) { return os << f.x; }
bool operator<(const filename &a, const filename &b) { return a.x < b.x; }
bool operator==(const filename &a, const filename &b) { return a.x == b.x; }
bool operator==(const filename &a, const char *b) {
  return strcmp(a.x.c_str(), b) == 0;
}

struct file_node {
  vector<DWORD> clusters;
  virtual ~file_node() = default;
  virtual int type() = 0;
  virtual void print(int) = 0;
  virtual void alloc_cluster() = 0;
  virtual void generate_fat() = 0;
  virtual void generate_fileentry(ostream &, QWORD, QWORD) = 0;
  virtual FileEntry get_fe() = 0;
};
struct file : public file_node {
  int size;
  file(int s) : size(s) {}
  virtual ~file() override = default;
  virtual int type() override { return 1; }
  virtual void print(int i = 0) override {
    cout << string(i, ' ') << "File: " << size << " bytes\n";
    cout << string(i, ' ') << "Clusters: ";
    for (auto &i : clusters) {
      cout << i << " ";
    }
    cout << "\n";
  }
  virtual void alloc_cluster() override {
    for (int i = 0; i <= size / BytesPerSec / SecPerClus; ++i) {
      clusters.push_back(total_clusters++);
    }
  }
  virtual void generate_fat() override {
    for (int i = 0; i < clusters.size() - 1; ++i) {
      fat.at(clusters[i]) = clusters[i + 1];
    }
    fat.at(clusters.back()) = 0xfffffff8;
  }
  virtual void generate_fileentry(ostream &, QWORD, QWORD) override { return; }
};
struct directory : public file_node {
  map<filename, file_node *> files;
  virtual ~directory() override {
    for (auto &i : files) {
      delete i.second;
    }
  }
  virtual int type() override { return 2; }
  virtual void print(int x = 0) override {
    cout << string(x, ' ') << "Clusters: ";
    for (auto &i : clusters) {
      cout << i << " ";
    }
    cout << "\n";
    for (auto &i : files) {
      cout << string(x, ' ') << "Name: " << i.first << "\n";
      i.second->print(x + 2);
    }
  }
  virtual void alloc_cluster() override {
    int size = files.size() * FileEntrySize;
    if (size == 0) {
      return;
    }
    for (int i = 0; i <= size / BytesPerSec / SecPerClus; ++i) {
      clusters.push_back(total_clusters++);
    }
    for (auto &i : files) {
      i.second->alloc_cluster();
    }
  }
  virtual void generate_fat() override {
    if (clusters.size() == 0)
      return;
    for (int i = 0; i < clusters.size() - 1; ++i) {
      fat.at(clusters[i]) = clusters[i + 1];
      cout << "=====" << i << ' ' << clusters[i] << ' ' << clusters[i + 1]
           << endl;
    }
    fat.at(clusters.back()) = 0xfffffff8;
    for (auto &i : files) {
      i.second->generate_fat();
    }
  }
#define check()                                                                \
  if (now == BytesPerSec * SecPerClus / 32) {                                  \
    now = 0;                                                                   \
    clust = fat[clust] & 0x0fffffff;                                           \
    if (clust >= 0xfffffff8) {                                                 \
      cerr << "Error: " << __FILE__ << ":" << __LINE__ << endl;                \
    }                                                                          \
  }
  virtual void generate_fileentry(ostream &os, QWORD base,
                                  QWORD parent) override {
    static int flag = 0;
    int clust = clusters[0], now = 0;
    if (flag == 0) {
      flag = 1;
      ShortFileName se;
      for (int i = 0; i < 11; ++i) {
        se.FileName[i] = bootsec.BS_VolLab[i];
      }
      se.Extention[0] = ' ';
      se.Extention[1] = ' ';
      se.Extention[2] = ' ';
      se.Attr = 0x28;
      os.seekp(base, ios::beg);
      ++now;
      write(os, se);
    }
    for (auto &i : files) {
      if (i.first == ".") {
        ShortFileName se;
        se.FileName[0] = '.';
        se.FileName[1] = se.FileName[2] = se.FileName[3] = se.FileName[4] =
            se.FileName[5] = se.FileName[6] = se.FileName[7] = se.Extention[0] =
                se.Extention[1] = se.Extention[2] = ' ';
        se.Attr = (BYTE)0x10;
        se.ClsH16 = clusters[0] >> 16;
        se.ClsL16 = clusters[0] & 0xffff;
        se.FileLen = 0;
        check();
        os.seekp(base + (clust - 2) * BytesPerSec * SecPerClus +
                     now * FileEntrySize,
                 ios::beg);
        ++now;
        write(os, se);
        continue;
      } else if (i.first == "..") {
        ShortFileName se;
        se.FileName[0] = se.FileName[1] = '.';
        se.FileName[2] = se.FileName[3] = se.FileName[4] = se.FileName[5] =
            se.FileName[6] = se.FileName[7] = se.Extention[0] =
                se.Extention[1] = se.Extention[2] = ' ';
        se.Attr = (BYTE)0x10;
        se.ClsH16 = parent >> 16;
        se.ClsL16 = parent & 0xff;
        se.FileLen = 0;
        check();
        os.seekp(base + (clust - 2) * BytesPerSec * SecPerClus +
                     now * FileEntrySize,
                 ios::beg);
        ++now;
        write(os, se);
        continue;
      } else {
        string name = i.first.substr(0, i.first.find_last_of('.'));
        string ext = i.first.substr(i.first.find_last_of('.') + 1);
        if (i.first.find('.') == string::npos) {
          name = i.first;
          ext = "";
        }
        FileEntry fe;
        if (name.length() > 8 || ext.length() > 3) { // long filename
          cerr << "Error: " << __FILE__ << ":" << __LINE__
               << " long filename not supported" << endl;
        } else { // short filename
          for (int i = 0; i < 8; ++i) {
            if (i < name.length()) {
              fe.a.FileName[i] = name[i];
            } else {
              fe.a.FileName[i] = ' ';
            }
          }
          for (int i = 0; i < 3; ++i) {
            if (i < ext.length()) {
              fe.a.Extention[i] = ext[i];
            } else {
              fe.a.Extention[i] = ' ';
            }
          }
          if (i.second->type() == 0) {
            for (int j = 0; j < 8; ++j) {
              if (j < i.first.size())
                fe.a.FileName[j] = i.first[j];
              else
                fe.a.FileName[j] = ' ';
            }
            fe.a.FileLen = 0;
            fe.a.Attr = 0x28;
          } else {
            fe.a.Attr = (i.second->type() == 2) ? 0x10 : 0x00;
            fe.a.ClsH16 = i.second->clusters[0] >> 16;
            fe.a.ClsL16 = i.second->clusters[0] & 0xffff;
            fe.a.FileLen = (i.second->type() == 1)
                               ? dynamic_cast<file *>(i.second)->size
                               : 0;
          }
          check();
          os.seekp(base + (clust - 2) * BytesPerSec * SecPerClus +
                       now * FileEntrySize,
                   ios::beg);
          ++now;
          write(os, fe.a);
          if (i.second->type() == 2) {
            dynamic_cast<directory *>(i.second)->generate_fileentry(
                os, base, clusters[0] == 2 ? 0 : clusters[0]);
          }
        }
      }
    }
  }
#undef check
};
vector<string> split(const string &s) {
  vector<string> result;
  regex r(R"(/[^/]+)");
  regex_iterator<string::const_iterator> it(s.begin(), s.end(), r);
  regex_iterator<string::const_iterator> end;
  for (; it != end; it++) {
    result.push_back(it->str().substr(1));
  }
  return result;
}
int main() { // run $ dd if=/dev/zero of=res.img bs=8192 count=8192
  for (auto &i : cluster_map) {
    if (disk_size / BytesPerSec <= i.first) {
      SecPerClus = i.second;
      break;
    }
  }
  QWORD Totsecs = disk_size / BytesPerSec;
  if (Totsecs <= 524288) {
    heads = Totsecs <= 32768    ? 2
            : Totsecs <= 65536  ? 4
            : Totsecs <= 262144 ? 8
                                : 16;
    SecPerTrk = Totsecs <= 4096 ? 16 : 32;
  } else {
    heads = Totsecs <= 16 * 63 * 1024    ? 16
            : Totsecs <= 32 * 63 * 1024  ? 32
            : Totsecs <= 64 * 63 * 1024  ? 64
            : Totsecs <= 128 * 63 * 1024 ? 128
                                         : 255;
    SecPerTrk = 63;
  }
  cout << "=====" << endl;
  cout << "SecPerClus: " << SecPerClus << endl;
  cout << "BytesPerSec: " << BytesPerSec << endl;
  cout << "TotalSecs: " << Totsecs << endl;
  cout << "TotalClusters: " << disk_size / BytesPerSec / SecPerClus << endl;
  fstream f("touch.txt", ios::in);
  directory root;
  string line;
  while (getline(f, line)) {
    string l = line.substr(0, line.find(":"));
    string r = line.substr(line.find(":") + 1);
    if (l == "dir") {
      auto d = &root;
      for (auto &i : split(r)) {
        if (d->files.count(i) == 0) {
          d->files[i] = new directory;
        }
        d = dynamic_cast<directory *>(d->files[i]);
      }
    }
    if (l == "file") {
      auto d = &root;
      auto x = split(r);
      for (int I = 0; I < x.size() - 2; ++I) {
        auto i = x[I];
        if (d->files.count(i) == 0) {
          d->files[i] = new directory;
        }
        d = dynamic_cast<directory *>(d->files[i]);
      }
      d->files[x[x.size() - 2]] = new file(stoi(x[x.size() - 1]));
    }
  }
  f.close();
  root.alloc_cluster();
  fat.resize(total_clusters);
  root.print();
  root.generate_fat();
  fat[0] = 0x0fffff00;
  fat[1] = 0x0fffff0f;
  // print FAT
  cout << "FAT\n";
  for (int i = 0; i < fat.size(); ++i) {
    cout << hex << i << ": " << hex << fat[i] << "\n";
  }
  // code = 0xebf4 00fd
  bootsec = {
      .jmpcmd = {0xeb, 0x58, 0x90},
      .BS_OEMNAME = {'d', 'n', 'g', 'o', 's', '1', '.', '0'},
      .BPB_BytsPerSec = static_cast<WORD>(BytesPerSec),
      .BPB_SecPerClus = static_cast<BYTE>(SecPerClus),
      .BPB_RsvdSecCnt = 0x20,
      .BPB_NumFATs = 2,
      .BPB_RootEntCnt = 0,
      .BPB_TotSec16 = 0,
      .BPB_Media = static_cast<BYTE>(Media),
      .BPB_FATSz16 = 0,
      .BPB_SecPerTrk = static_cast<WORD>(SecPerTrk),
      .BPB_NumHeads = static_cast<WORD>(heads),
      .BPB_HiddSec = 0,
      .BPB_TotSec32 =
          static_cast<DWORD>(disk_size / BytesPerSec) - 2 - 6 /*bk boot sec*/,
      .BPB_FATSz32 = static_cast<DWORD>(ceil(1.0 * disk_size / BytesPerSec /
                                             SecPerClus * 32)) /
                     BytesPerSec,
      .BPB_ExtFlags = 0,
      .BPB_FSVer = 0,
      .BPB_RootClus = 0x2,
      .BPB_FSInfo = 0x1,
      .BPB_BkBootSec = 0x6,
      .BPB_Reserved = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      .BS_DrvNum = 0x80,
      .BS_Reserved = 0,
      .BS_BootSig = 0x29,
      .BS_VolID = 0x11223344,
      .BS_VolLab = {'D', 'g', 'n', 'o', 's', ' ', ' ', ' ', ' ', ' ', ' '},
      .BS_BootSign = 0xaa55};
  {
    ifstream bootcode("boot", ios::binary | ios::in);
    bootcode.seekg(90, ios::beg);
    for (int i = 0; i < 420; ++i) {
      bootsec.BS_BootCode32[i] = bootcode.get();
    }
  }
  FSInfo fsinfo;
  fsinfo.FSI_Nxt_Free = total_clusters - 1;
  // fsinfo.FSI_Free_Count = bootsec.BPB_TotSec32 - total_clusters;
  fsinfo.FSI_Free_Count = 0xffffffff;
  ofstream output("res.img", ios::out | ios::binary);
  output.seekp(disk_size - 1, ios::beg);
  output.put(0);
  output.seekp(0, ios::beg);
  write(output, bootsec);
  write(output, fsinfo);
  output.seekp(bootsec.BPB_BytsPerSec * bootsec.BPB_BkBootSec,
               ios::beg); // write bkbootsec
  write(output, bootsec);
  write(output, fsinfo);
  output.seekp(bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt,
               ios::beg); // write FAT
  cout << "FAT locate: 0x" << hex
       << bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt << endl;
  for (int i = 0; i < fat.size(); ++i) {
    write(output, fat[i]);
  }
  output.seekp(bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt +
                   bootsec.BPB_NumFATs * bootsec.BPB_FATSz32 *
                       bootsec.BPB_BytsPerSec,
               ios::beg); // write file
  cout << "File locate: 0x" << hex
       << bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt +
              bootsec.BPB_NumFATs * bootsec.BPB_FATSz32 * bootsec.BPB_BytsPerSec
       << endl;
  root.generate_fileentry(output,
                          bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt +
                              bootsec.BPB_NumFATs * bootsec.BPB_FATSz32 *
                                  bootsec.BPB_BytsPerSec,
                          0);
  output.close();
  return 0;
}
