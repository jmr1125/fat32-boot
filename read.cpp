#include "fat32.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
using namespace std;
int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <file>" << endl;
    return 1;
  }
  ifstream fin(argv[1], ios::binary);
  if (!fin) {
    cerr << "Can not open file: " << argv[1] << endl;
    return 1;
  }
  istreambuf_iterator<char> it(fin);
  auto bootsec = read_bootsector(it);
  cout << bootsec << endl;
  auto fsinfo = read_fsinfo(it);
  cout << fsinfo << endl;
  cout << "bk boot sector locate at : 0x"
       << bootsec.BPB_BytsPerSec * bootsec.BPB_BkBootSec << endl;
  fin.seekg(bootsec.BPB_BytsPerSec * bootsec.BPB_BkBootSec, ios::beg);
  it = istreambuf_iterator<char>(fin);
  auto bkbootsec = read_bootsector(it);
  auto bkfsinfo = read_fsinfo(it);
  cout << bkbootsec << endl << bkfsinfo << endl;
  cout << "FAT locate at : 0x"
       << bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt << endl;
  // fin.seekg(bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt, ios::beg);
  // it = istreambuf_iterator<char>(fin);
  // cout << "FAT: 0x" << readDWORD(it) << " 0x" << readDWORD(it) << " 0x"
  //      << readDWORD(it) << " 0x" << readDWORD(it) << " 0x" << readDWORD(it)
  //      << " 0x" << readDWORD(it) << endl;
  fin.seekg(bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt, ios::cur);
  it = istreambuf_iterator<char>(fin);
  cout << "bk FAT: 0x" << readDWORD(it) << " 0x" << readDWORD(it) << " 0x"
       << readDWORD(it) << " 0x" << readDWORD(it) << " 0x" << readDWORD(it)
       << " 0x" << readDWORD(it) << endl;
  cout << "data locate at : 0x"
       << bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt +
              bootsec.BPB_NumFATs * bootsec.BPB_FATSz32 * bootsec.BPB_BytsPerSec
       << endl;
  fin.seekg(bootsec.BPB_BytsPerSec * bootsec.BPB_RsvdSecCnt +
                bootsec.BPB_NumFATs * bootsec.BPB_FATSz32 *
                    bootsec.BPB_BytsPerSec,
            ios::beg);
  QWORD total_sectors =
      (bootsec.BPB_TotSec16 == 0) ? bootsec.BPB_TotSec32 : bootsec.BPB_TotSec16;
  QWORD fat_size =
      (bootsec.BPB_FATSz16 == 0) ? bootsec.BPB_FATSz32 : bootsec.BPB_FATSz16;
  QWORD root_dir_sectors =
      ((bootsec.BPB_RootEntCnt * 32) + (bootsec.BPB_BytsPerSec - 1)) /
      bootsec.BPB_BytsPerSec;
  QWORD first_data_sector = bootsec.BPB_RsvdSecCnt +
                            (bootsec.BPB_NumFATs * fat_size) + root_dir_sectors;
  QWORD first_fat_sector = bootsec.BPB_RsvdSecCnt;
  QWORD data_sectors =
      total_sectors - (bootsec.BPB_RsvdSecCnt +
                       (bootsec.BPB_NumFATs * fat_size) + root_dir_sectors);
  QWORD total_clusters = data_sectors / bootsec.BPB_SecPerClus;
  cout << dec << "total_sectors = " << total_sectors << endl;
  cout << dec << "sectors_per_fat = " << fat_size << " sectors" << endl;
  cout << dec << "root_dir_sectors = " << root_dir_sectors << endl;
  cout << dec << "first_data_sector = " << first_data_sector << " sectors ( 0x"
       << hex << first_data_sector * bootsec.BPB_BytsPerSec << ")" << endl;
  cout << dec << "first_fat_sector = " << first_fat_sector << " sectors ( 0x"
       << hex << first_fat_sector * bootsec.BPB_BytsPerSec << ")" << endl;
  cout << dec << "data_sectors = " << data_sectors << " sectors" << endl;
  cout << dec << "total_clusters = " << total_clusters << " clusters" << endl;
  QWORD first_root_dir_sector = first_data_sector - root_dir_sectors;
  QWORD root_cluster_32 = bootsec.BPB_RootClus;
  cout << "first_root_dir_sector = " << first_root_dir_sector << " sectors "
       << endl;
  cout << "root_cluster_32 = " << root_cluster_32 << endl;
  auto first_sector_of_cluster = [&](QWORD cluster) -> QWORD {
    return ((cluster - 2) * bootsec.BPB_SecPerClus) + first_data_sector;
  };
  vector<DWORD> fat(fat_size / 4);
  vector<bool> clsHasRead(total_clusters);
  fin.seekg(first_fat_sector * bootsec.BPB_BytsPerSec, ios::beg); // read FAT
  it = istreambuf_iterator<char>(fin);
  for (QWORD i = 0; i < fat_size / 4; i++) {
    fat[i] = readDWORD(it) & 0x0fffffff;
  }
  fin.seekg(first_data_sector * bootsec.BPB_BytsPerSec, ios::beg); // read data
  vector<int> clsIdtoRead;
  clsIdtoRead.push_back(root_cluster_32);
  clsHasRead[root_cluster_32] = true;
  while (!clsIdtoRead.empty()) {
    QWORD cluster = clsIdtoRead.back();
    bool end = false;
    clsIdtoRead.pop_back();
    cout << "==========" << endl;
    cout << "cluster " << dec << cluster << " : ";
    for (QWORD i = 0; i < bootsec.BPB_SecPerClus * bootsec.BPB_BytsPerSec;
         i += FileEntrySize) {
      QWORD offset =
          first_sector_of_cluster(cluster) * bootsec.BPB_BytsPerSec + i;
      cout << hex << "  offset: 0x" << offset << endl;
      fin.seekg(offset, ios::beg);
      if (fin.get() == 0x00) {
        cout << "  end of this directory" << endl;
        end = true;
        break;
      }
      fin.seekg(-1, ios::cur);
      const FileEntry entry = read_fileentry(fin);
      if (is_long(entry))
        cout << "long name" << endl;
#if 0
      cout << "  entry: " << entry << endl;
#else
      if (is_long(entry))
        cout << "  UnicodeName:"
             << string((const char *)&entry.b.UnicodeName[0], 10) << endl
             << "  UnicodeName1:"
             << string((const char *)&entry.b.UnicodeName1[0], 12) << endl
             << "  UnicodeName2:"
             << string((const char *)&entry.b.UnicodeName2[0], 4) << endl;
      else
        cout << "  name: " << string((const char *)&entry.a.FileName[0], 8)
             << endl
             << "  Extension: "
             << string((const char *)&entry.a.Extention[0], 3) << endl
             << "  off: 0x" << hex
             << first_sector_of_cluster((entry.a.ClsH16 << 16) |
                                        entry.a.ClsL16) *
                    bootsec.BPB_BytsPerSec
             << endl
             << "  size: " << dec << entry.a.FileLen << endl;
#endif
      if (entry.a.Attr & 0x10) {
        cout << "directory to cluster: H:" << entry.a.ClsH16
             << " L:" << entry.a.ClsL16 << endl;
        if (!clsHasRead.at((entry.a.ClsH16 << 16) | entry.a.ClsL16)) {
          clsIdtoRead.push_back((entry.a.ClsH16 << 16) | entry.a.ClsL16);
          clsHasRead.at((entry.a.ClsH16 << 16) | entry.a.ClsL16) = true;
        }
      }
      cout << "---" << endl;
    }
    if (!end) {
      cout << "fat = 0x" << hex << fat.at(cluster) << endl;
      if (fat.at(cluster) < 0x0FFFFFF7 && !clsHasRead.at(fat.at(cluster))) {
        clsIdtoRead.push_back(fat.at(cluster));
        clsHasRead.at(fat.at(cluster)) = true;
        cout << "next: " << fat.at(cluster) << endl;
      }
      if (fat.at(cluster) == 0x0FFFFFF7) {
        cout << "Bad cluster" << endl;
        break;
      }
    }
  }
  return 0;
}
