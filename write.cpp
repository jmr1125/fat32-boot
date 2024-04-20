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
  int id;
  string x;
  operator string() const { return x; }
  auto substr(int a, int b = -1) const { return x.substr(a, b); }
  auto find_last_of(char c) const { return x.find_last_of(c); }
  auto find(char c) const { return x.find(c); }
  auto Entrysize() const { return FileEntrySize; }
  auto operator[](int i) const { return x[i]; }
  auto &operator[](int i) { return x[i]; }
  filename(string s) : x(s), id(256) {}
  filename(string s, int i) : x(s), id(i) {}
};
namespace std {
template <> struct hash<filename> {
  auto operator()(const filename &f) const { return hash<string>()(f.x); }
};
} // namespace std
ostream &operator<<(ostream &os, const filename &f) { return os << f.x; }
bool operator<(const filename &a, const filename &b) {
  return a.id < b.id || (a.id == b.id && a.x < b.x);
}
bool operator==(const filename &a, const filename &b) {
  return a.id < b.id || (a.id == b.id && a.x == b.x);
}
bool operator==(const filename &a, const char *b) {
  return strcmp(a.x.c_str(), b) == 0;
}

struct file_node {
  vector<DWORD> clusters;
  virtual ~file_node() = default;
  virtual int Entrysize() = 0;
  virtual void print(int) = 0;
  virtual void alloc_cluster() = 0;
  virtual void generate_fat() = 0;
  virtual void generate_fileentry(ostream &, QWORD, QWORD) = 0;
  virtual vector<FileEntry> get_fe(string name, string ext,
                                   int grand_parent_cluster,
                                   int parent_cluster) = 0;
  virtual void fill_file(ostream &, QWORD base) = 0;
};
struct LFN : public file_node {
  vector<LongFileName> Names;
  LFN() = default;
  LFN(string name, BYTE chk) { set_name(name, chk); }
  virtual ~LFN() override = default;
  virtual int Entrysize() override { return FileEntrySize * Names.size(); }
  virtual void print(int i = 0) override {
    cout << string(i, ' ') << "LFN\n";
    {
      int i = 0;
      for (auto &n : Names) {
        cout << string(i + 2, ' ') << i++ << "\n";
        cout << string(i + 2, ' ') << FileEntry{.b = n} << "\n";
      }
    }
  }
  virtual void alloc_cluster() override { return; }
  virtual void generate_fat() override { return; }
  void set_name(string s, BYTE chk) {
    for (int i = 0; i < s.size(); i += 13) {
      string cur = s.substr(i, 13);
      LongFileName tmp = {
          .Num =
              static_cast<BYTE>((i / 13 + 1) | (i + 13 > s.size() ? 0x40 : 0)),
          .Flag = (BYTE)0x0f,
          .Rsv = 0,
          .Check = chk,
          .Sect = 0,
      };
      for (int i = 0; i < 10; i++) {
        if (i % 2 == 1)
          tmp.UnicodeName[i] = 0;
        else {
          if (i / 2 >= cur.size())
            tmp.UnicodeName[i] = 0;
          else
            tmp.UnicodeName[i] = cur[i / 2];
        }
      }
      if (10 / 2 >= cur.size()) {
        for (int i = 0; i < 12; i++)
          tmp.UnicodeName1[i] = 0xff;
      } else
        for (int i = 0; i < 12; i++) {
          if (i % 2 == 1)
            tmp.UnicodeName1[i] = 0;
          else {
            if ((10 + i) / 2 >= cur.size())
              tmp.UnicodeName1[i] = 0;
            else
              tmp.UnicodeName1[i] = cur[(10 + i) / 2];
          }
        }
      if (22 / 2 >= cur.size()) {
        for (int i = 0; i < 4; i++)
          tmp.UnicodeName2[i] = 0xff;
      } else
        for (int i = 0; i < 4; i++) {
          if (i % 2 == 1)
            tmp.UnicodeName2[i] = 0;
          else {
            if ((22 + i) / 2 >= cur.size())
              tmp.UnicodeName2[i] = 0;
            else
              tmp.UnicodeName2[i] = cur[(22 + i) / 2];
          }
        }
      Names.push_back(tmp);
    }
  }
  virtual void generate_fileentry(ostream &, QWORD, QWORD) override { return; }
  virtual void fill_file(ostream &, QWORD) override {}
  virtual vector<FileEntry> get_fe(string, string, int, int) override {
    vector<FileEntry> res;
    for (auto &n : Names) {
      res.push_back({.b = n});
    }
    reverse(res.begin(), res.end());
    return res;
  }
};
struct volume : public file_node {
  virtual ~volume() override = default;
  virtual int Entrysize() override { return FileEntrySize; }
  virtual void print(int i = 0) override {
    cout << string(i, ' ') << "Volume\n";
  }
  virtual void alloc_cluster() override { return; }
  virtual void generate_fat() override { return; }
  virtual void generate_fileentry(ostream &, QWORD, QWORD) override { return; }
  virtual void fill_file(ostream &, QWORD) override {}
  virtual vector<FileEntry> get_fe(string name, string ext, int, int) override {
    ShortFileName fe;
    for (int i = 0; i < 8; ++i) {
      if (i < name.size())
        fe.FileName[i] = name[i];
      else
        fe.FileName[i] = ' ';
    }
    for (int i = 0; i < 3; ++i) {
      if (i < ext.size())
        fe.Extention[i] = ext[i];
      else
        fe.Extention[i] = ' ';
    }
    fe.Attr = (BYTE)0x8;
    fe.FileLen = fe.ClsH16 = fe.ClsL16 = 0;
    return {FileEntry{.a = fe}};
  }
};
struct file : public file_node {
  int size;
  string real_file;
  file(int s) : size(s) {}
  virtual ~file() override = default;
  virtual int Entrysize() override { return FileEntrySize; }
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
  virtual void fill_file(ostream &o, QWORD base) override {
    ifstream f(real_file, ios::in | ios::binary);
    if (!f) {
      cout << "File " << real_file << " not found\n";
      return;
    } else {
      cout << real_file << endl;
    }
    for (auto &i : clusters) {
      o.seekp(base + (i - 2) * BytesPerSec * SecPerClus);
      char buf[BytesPerSec * SecPerClus];
      f.read(buf, BytesPerSec * SecPerClus);
      o.write(buf, BytesPerSec * SecPerClus);
    }
  }
  virtual vector<FileEntry> get_fe(string name, string ext, int, int) override {
    ShortFileName fe;
    for (int j = 0; j < 8; ++j) {
      if (j < name.size())
        fe.FileName[j] = name[j];
      else
        fe.FileName[j] = ' ';
    }
    for (int j = 0; j < 3; ++j) {
      if (j < ext.size())
        fe.Extention[j] = ext[j];
      else
        fe.Extention[j] = ' ';
    }
    fe.Attr = (BYTE)0x20;
    fe.ClsH16 = clusters[0] >> 16;
    fe.ClsL16 = clusters[0] & 0xffff;
    fe.FileLen = size;
    return {FileEntry{.a = fe}};
  }
};
struct directory : public file_node {
  map<filename, file_node *> files;
  virtual ~directory() override {
    for (auto &i : files) {
      delete i.second;
    }
  }
  virtual int Entrysize() override { return FileEntrySize; }
  virtual void fill_file(ostream &o, QWORD base) override {
    for (auto &i : files) {
      i.second->fill_file(o, base);
    }
  }
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
    int size = 0;
    for (auto &i : files) {
      size += i.second->Entrysize();
    }
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
  virtual vector<FileEntry> get_fe(string name, string ext,
                                   int grand_parent_cluster,
                                   int parent_cluster) override {
    ShortFileName fe;
    for (int j = 0; j < 8; ++j) {
      if (j < name.size())
        fe.FileName[j] = name[j];
      else
        fe.FileName[j] = ' ';
    }
    for (int j = 0; j < 3; ++j) {
      if (j < ext.size())
        fe.Extention[j] = ext[j];
      else
        fe.Extention[j] = ' ';
    }
    fe.Attr = (BYTE)0x10;
    if (name == "..") {
      fe.ClsH16 = grand_parent_cluster >> 16;
      fe.ClsL16 = grand_parent_cluster & 0xffff;
    } else if (name == ".") {
      fe.ClsH16 = parent_cluster >> 16;
      fe.ClsL16 = parent_cluster & 0xffff;
    } else {
      fe.ClsH16 = clusters[0] >> 16;
      fe.ClsL16 = clusters[0] & 0xffff;
    }
    fe.FileLen = 0;
    return {FileEntry{.a = fe}};
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
                                  QWORD parent_cluster) override {
    int clust = clusters[0], now = 0;
    for (auto &i : files) {
      string name = i.first.substr(0, i.first.find_last_of('.'));
      string ext = i.first.substr(i.first.find_last_of('.') + 1);
      if (i.first.find('.') == string::npos) {
        name = i.first;
        ext = "";
      }
      if (i.first == "." || i.first == "..") {
        name = i.first;
        ext = "";
      }
      auto fes = i.second->get_fe(name, ext, parent_cluster, clusters[0]);
      for (auto &fe : fes) {
        check();
        os.seekp(base + (clust - 2) * BytesPerSec * SecPerClus +
                     now * FileEntrySize,
                 ios::beg);
        ++now;
        write(os, fe);
        if (!(name == "." || name == ".."))
          i.second->generate_fileentry(os, base, parent_cluster);
      }
    }
  }
#undef check
};
vector<filename> split(const string &s) {
  vector<filename> result;
  regex r(R"(/[^/]+)");
  regex_iterator<string::const_iterator> it(s.begin(), s.end(), r);
  regex_iterator<string::const_iterator> end;
  for (; it != end; it++) {
    string str = it->str().substr(1);
    result.push_back(filename(str, (str == ".")    ? 64
                                   : (str == "..") ? 65
                                                   : 256));
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
  string realname;
  while (getline(f, line)) {
    string l = line.substr(0, line.find(":"));
    string r = line.substr(line.find(":") + 1);
    if (l == "vol") {
      root.files[filename(r, 128)] = new volume;
    }
    if (l == "dir") {
      auto d = &root;
      for (auto &i : split(r)) {
        if (d->files.count(i) == 0) {
          d->files[i] = new directory;
        }
        d = dynamic_cast<directory *>(d->files[i]);
      }
    }
    if (l == "real") {
      realname = r;
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
      dynamic_cast<file *>(d->files[x[x.size() - 2]])->real_file = realname;
    }
    if (l == "lname") {
      auto d = &root;
      auto x = split(r);
      for (int I = 0; I < x.size() - 2; ++I) {
        auto i = x[I];
        if (d->files.count(i) == 0) {
          d->files[i] = new directory;
        }
        d = dynamic_cast<directory *>(d->files[i]);
      }
      d->files[filename(x[x.size() - 1], 255)] =
          new LFN(x[x.size() - 2], ChkSum(x[x.size() - 1].x.c_str()));
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
  cout << "File entries finish!" << endl;
  root.fill_file(output, bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt +
                             bootsec.BPB_NumFATs * bootsec.BPB_FATSz32 *
                                 bootsec.BPB_BytsPerSec);
  output.close();
  cout << "Finish!" << endl;
  cout << "Sec Per Tracks (STP): " << dec << bootsec.BPB_SecPerTrk << endl;
  cout << "Head: " << bootsec.BPB_NumHeads << endl;
  cout << "cylinder: "
       << Totsecs / (bootsec.BPB_SecPerTrk * bootsec.BPB_NumHeads) << endl;
  return 0;
}
