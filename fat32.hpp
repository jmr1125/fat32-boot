#include <iostream>
#include <iterator>
using std::ostream;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long long QWORD;
struct bootsector {            // OFF=0x0
  BYTE jmpcmd[3];              // off=0x0
  BYTE BS_OEMNAME[8];          // off=0x3
  WORD BPB_BytsPerSec = 512;   // off=0xb
  BYTE BPB_SecPerClus = 4;     // off=0xd
  WORD BPB_RsvdSecCnt;         // off=0xe
  BYTE BPB_NumFATs = 2;        // off=0x10
  WORD BPB_RootEntCnt = 0;     // off=0x11
  WORD BPB_TotSec16 = 0;       // off=0x13
  BYTE BPB_Media = 0xf8;       // off=0x15
  WORD BPB_FATSz16 = 0;        // off=0x16
  WORD BPB_SecPerTrk = 0x3f;   // off=0x18
  WORD BPB_NumHeads = 0xff;    // off=0x1a
  DWORD BPB_HiddSec = 0;       // off=0x1c
  DWORD BPB_TotSec32;          // off=0x20
  DWORD BPB_FATSz32;           // off=0x24
  WORD BPB_ExtFlags = 0;       // off=0x28
  WORD BPB_FSVer = 0;          // off=0x2a
  DWORD BPB_RootClus = 2;      // off=0x2c
  WORD BPB_FSInfo = 1;         // off=0x30
  WORD BPB_BkBootSec = 6;      // off=0x32
  BYTE BPB_Reserved[12] = {0}; // off=0x34
  BYTE BS_DrvNum = 0x80;       // off=0x40 0x00 for floppy
  BYTE BS_Reserved = 0;        // off=0x41
  BYTE BS_BootSig = 0x29;      // off=0x42
  DWORD BS_VolID;              // off=0x43
  BYTE BS_VolLab[11];          // off=0x47
  BYTE BS_FilSysType[8] = {'F', 'A', 'T', '3', '2', 0, 0, 0}; // off=0x52
  BYTE BS_BootCode32[420];                                    // off=0x5a
  WORD BS_BootSign = 0xaa55;                                  // off=0x1fe
} __attribute__((packed));
struct FSInfo {                    // OFF=0x200
  DWORD FSI_LeadSig = 0x41615252;  // off=0x0
  BYTE FSI_Reserved1[480] = {0};   // off=0x4
  DWORD FSI_StrucSig = 0x61417272; // off=0x1e4
  DWORD FSI_Free_Count;            // off=0x1e8
  DWORD FSI_Nxt_Free;              // off=0x1ec
  BYTE FSI_Reserved2[12] = {0};    // off=0x1f0
  DWORD FSI_TrailSig = 0xaa550000; // off=0x1fc
} __attribute__((packed));
static_assert(sizeof(bootsector) == 512);
static_assert(sizeof(FSInfo) == 512);
// #define C (*(it++))
// #define C (readByte(it),++it;)
template <typename InpIt> BYTE c(InpIt &it) {
  auto c = *it;
  ++it;
  return c;
}
#define C c(it)
template <typename InpIt> BYTE readByte(InpIt &it) { return *it; }
template <typename InpIt> WORD readWORD(InpIt &it) {
  return (C | ((WORD)C << 8));
}
template <typename InpIt> DWORD readDWORD(InpIt &it) {
  return (readWORD(it) | ((DWORD)readWORD(it) << 16));
}
template <typename InpIt> QWORD readQWORD(InpIt &it) {
  return (readDWORD(it) | ((QWORD)readDWORD(it) << 32));
}
inline void write(ostream &os, BYTE v) { os.put(v); }
inline void write(ostream &os, WORD v) {
  write(os, (BYTE)v);
  write(os, (BYTE)(v >> 8));
}
inline void write(ostream &os, DWORD v) {
  write(os, (WORD)v);
  write(os, (WORD)(v >> 16));
}
inline void write(ostream &os, QWORD v) {
  write(os, (DWORD)v);
  write(os, (DWORD)(v >> 32));
}

template <typename InpIt> bootsector read_bootsector(InpIt &it) {
  bootsector res;
  for (int i = 0; i < 3; ++i) {
    res.jmpcmd[i] = C;
  }
  for (int i = 0; i < 8; ++i) {
    res.BS_OEMNAME[i] = C;
  }
  res.BPB_BytsPerSec = readWORD(it);
  res.BPB_SecPerClus = C;
  res.BPB_RsvdSecCnt = readWORD(it);
  res.BPB_NumFATs = C;
  res.BPB_RootEntCnt = readWORD(it);
  res.BPB_TotSec16 = readWORD(it);
  res.BPB_Media = C;
  res.BPB_FATSz16 = readWORD(it);
  res.BPB_SecPerTrk = readWORD(it);
  res.BPB_NumHeads = readWORD(it);
  res.BPB_HiddSec = readDWORD(it);
  res.BPB_TotSec32 = readDWORD(it);
  res.BPB_FATSz32 = readDWORD(it);
  res.BPB_ExtFlags = readWORD(it);
  res.BPB_FSVer = readWORD(it);
  res.BPB_RootClus = readDWORD(it);
  res.BPB_FSInfo = readWORD(it);
  res.BPB_BkBootSec = readWORD(it);
  for (int i = 0; i < 12; ++i)
    res.BPB_Reserved[i] = C;
  res.BS_DrvNum = C;
  res.BS_Reserved = C;
  res.BS_BootSig = C;
  res.BS_VolID = readDWORD(it);
  for (int i = 0; i < 11; ++i)
    res.BS_VolLab[i] = C;
  for (int i = 0; i < 8; ++i)
    res.BS_FilSysType[i] = C;
  for (int i = 0; i < 420; ++i)
    res.BS_BootCode32[i] = C;
  res.BS_BootSign = readWORD(it);
  return res;
}
void write(ostream &os, const bootsector &bs) {
  for (int i = 0; i < 3; ++i)
    os.write((char *)&bs.jmpcmd[i], 1);
  for (int i = 0; i < 8; ++i)
    os.write((char *)&bs.BS_OEMNAME[i], 1);
  write(os, bs.BPB_BytsPerSec);
  write(os, bs.BPB_SecPerClus);
  write(os, bs.BPB_RsvdSecCnt);
  write(os, bs.BPB_NumFATs);
  write(os, bs.BPB_RootEntCnt);
  write(os, bs.BPB_TotSec16);
  write(os, bs.BPB_Media);
  write(os, bs.BPB_FATSz16);
  write(os, bs.BPB_SecPerTrk);
  write(os, bs.BPB_NumHeads);
  write(os, bs.BPB_HiddSec);
  write(os, bs.BPB_TotSec32);
  write(os, bs.BPB_FATSz32);
  write(os, bs.BPB_ExtFlags);
  write(os, bs.BPB_FSVer);
  write(os, bs.BPB_RootClus);
  write(os, bs.BPB_FSInfo);
  write(os, bs.BPB_BkBootSec);
  for (int i = 0; i < 12; ++i)
    os.write((char *)&bs.BPB_Reserved[i], 1);
  write(os, bs.BS_DrvNum);
  write(os, bs.BS_Reserved);
  write(os, bs.BS_BootSig);
  write(os, bs.BS_VolID);
  for (int i = 0; i < 11; ++i)
    os.write((char *)&bs.BS_VolLab[i], 1);
  for (int i = 0; i < 8; ++i)
    os.write((char *)&bs.BS_FilSysType[i], 1);
  for (int i = 0; i < 420; ++i)
    os.write((char *)&bs.BS_BootCode32[i], 1);
  write(os, bs.BS_BootSign);
}
template <typename InpIt> FSInfo read_fsinfo(InpIt &it) {
  FSInfo res;
  res.FSI_LeadSig = readDWORD(it);
  for (int i = 0; i < 480; ++i) {
    res.FSI_Reserved1[i] = C;
  }
  res.FSI_StrucSig = readDWORD(it);
  res.FSI_Free_Count = readDWORD(it);
  res.FSI_Nxt_Free = readDWORD(it);
  for (int i = 0; i < 12; ++i) {
    res.FSI_Reserved2[i] = C;
  }
  res.FSI_TrailSig = readDWORD(it);
  return res;
}
void write(ostream &os, const FSInfo &fsi) {
  write(os, fsi.FSI_LeadSig);
  for (int i = 0; i < 480; ++i)
    write(os, fsi.FSI_Reserved1[i]);
  write(os, fsi.FSI_StrucSig);
  write(os, fsi.FSI_Free_Count);
  write(os, fsi.FSI_Nxt_Free);
  for (int i = 0; i < 12; ++i)
    write(os, fsi.FSI_Reserved2[i]);
  write(os, fsi.FSI_TrailSig);
}
struct EmptySector {
  BYTE zeros[510]{0};
  WORD Sign = 0xaa55;
} __attribute__((packed));
struct ShortFileName {
  BYTE FileName[8];  // off=0x0
  BYTE Extention[3]; // off=0x08
  BYTE Attr;         // off=0x0b
  BYTE Reserved;     // off=0x0c
  BYTE Time10ms;     // off=0xd
  WORD TimeCreate;   // off=0x0e
  WORD DateCreate;   // off=0x10
  WORD DateVst;      // off=0x12
  WORD ClsH16;       // off=0x14
  WORD TimeModify;   // off=0x16
  WORD DateModify;   // off=0x18
  WORD ClsL16;       // off=0x1a
  DWORD FileLen;     // off=0x1c
};
struct LongFileName {
  BYTE Num;              // off=0x0 76543210: [rsv][last][rsv][n][u][m][b][er]
  BYTE UnicodeName[10];  // off=0x1
  BYTE Flag;             // off=0xb =0x0f
  BYTE Rsv;              // off=0xc
  BYTE Check;            // off=0xd
  BYTE UnicodeName1[12]; // off=0xe
  WORD Sect;             // off=0x1a
  BYTE UnicodeName2[4];  // off=0x1c
};
union FileEntry {
  ShortFileName a;
  LongFileName b;
};
inline void write(ostream &os, const ShortFileName &f) {
  for (int i = 0; i < 8; ++i)
    write(os, f.FileName[i]);
  for (int i = 0; i < 3; ++i)
    write(os, f.Extention[i]);
  write(os, f.Attr);
  write(os, f.Reserved);
  write(os, f.Time10ms);
  write(os, f.TimeCreate);
  write(os, f.DateCreate);
  write(os, f.DateVst);
  write(os, f.ClsH16);
  write(os, f.TimeModify);
  write(os, f.DateModify);
  write(os, f.ClsL16);
  write(os, f.FileLen);
}
inline void write(ostream &os, const LongFileName &f) {
  write(os, f.Num);
  for (int i = 0; i < 10; ++i)
    write(os, f.UnicodeName[i]);
  write(os, f.Flag);
  write(os, f.Rsv);
  write(os, f.Check);
  for (int i = 0; i < 12; ++i)
    write(os, f.UnicodeName1[i]);
  write(os, f.Sect);
  for (int i = 0; i < 4; ++i)
    write(os, f.UnicodeName2[i]);
}
#define FileEntrySize sizeof(FileEntry)
#define is_long(f) ((f).b.Flag == 0x0f)
static_assert(sizeof(EmptySector) == 512);
static_assert(sizeof(ShortFileName) == 32);
static_assert(sizeof(LongFileName) == 32);
static_assert(sizeof(FileEntry) == 32);
template <typename InpIt> FileEntry read_fileentry(InpIt &it, bool Long) {
  FileEntry res;
  if (Long) {
    res.b.Num = C;
    for (int i = 0; i < 10; ++i) {
      res.b.UnicodeName[i] = C;
    }
    res.b.Flag = C;
    res.b.Rsv = C;
    res.b.Check = C;
    for (int i = 0; i < 12; ++i) {
      res.b.UnicodeName1[i] = C;
    }
    res.b.Sect = readWORD(it);
    for (int i = 0; i < 4; ++i) {
      res.b.UnicodeName2[i] = C;
    }
  } else {
    for (int i = 0; i < 8; ++i) {
      res.a.FileName[i] = C;
    }
    for (int i = 0; i < 3; ++i) {
      res.a.Extention[i] = C;
    }
    res.a.Attr = C;
    res.a.Reserved = C;
    res.a.Time10ms = C;
    res.a.TimeCreate = readWORD(it);
    res.a.DateCreate = readWORD(it);
    res.a.DateVst = readWORD(it);
    res.a.ClsH16 = readWORD(it);
    res.a.TimeModify = readWORD(it);
    res.a.DateModify = readWORD(it);
    res.a.ClsL16 = readWORD(it);
    res.a.FileLen = readDWORD(it);
  }
  return res;
}
#include <fstream>
FileEntry read_fileentry(std::ifstream &fin) {
  std::istreambuf_iterator<char> it(fin);
  fin.seekg(0x0b, std::ios::cur);
  bool isLong = (*it == 0x0f);
  fin.seekg(-0x0b, std::ios::cur);
  it = std::istreambuf_iterator<char>(fin);
  return read_fileentry(it, isLong);
}
#undef C

#include <iomanip>
#include <ostream>
inline auto _print(std::ostream &ost, const BYTE *arr, size_t n) {
  static const auto print = [&ost](const BYTE *arr, size_t n) {
    ost << (int)(*arr & 0xff) << " ";
    ++arr;
    for (int i = 0; i < n - 1; ++i, ++arr) {
      ost << (int)(*arr & 0xff) << " ";
    }
    return "";
  };
  return print(arr, n);
}
inline auto _print1(std::ostream &ost, const BYTE *arr, size_t n) {
  for (int i = 0; i < n; ++i) {
    ost << "[" << arr[i] << "]";
  }
  return "";
}
#define print(a, b) _print(ost, a, b)
#define print1(a, b) _print1(ost, a, b)
inline std::ostream &operator<<(std::ostream &ost, const bootsector v) {
  ost << std::hex; //<< std::setfill('0') << std::setw(10);
  ost << "jmp: " << (int)(v.jmpcmd[0] & 0xff) << " "
      << (int)(v.jmpcmd[1] & 0xff) << " " << (int)(v.jmpcmd[2] & 0xff)
      << std::endl;
  ost << "OEMNAME:    ==" << v.BS_OEMNAME[0] << v.BS_OEMNAME[1]
      << v.BS_OEMNAME[2] << v.BS_OEMNAME[3] << v.BS_OEMNAME[4]
      << v.BS_OEMNAME[5] << v.BS_OEMNAME[6] << v.BS_OEMNAME[7] << std::endl;
  ost << "BytsPerSec: 0x" << v.BPB_BytsPerSec << std::endl;
  ost << "SecPerClus: 0x" << (int)(v.BPB_SecPerClus & 0xff) << std::endl;
  ost << "RsvdSecCnt: 0x" << v.BPB_RsvdSecCnt << std::endl;
  ost << "NumFATs:    0x" << (int)(v.BPB_NumFATs & 0xff) << std::endl;
  ost << "RootEntCnt: 0x" << v.BPB_RootEntCnt << std::endl;
  ost << "TotSec16:   0x" << v.BPB_TotSec16 << std::endl;
  ost << "Media:      0x" << (int)(v.BPB_Media & 0xff) << std::endl;
  ost << "FATSz16:    0x" << v.BPB_FATSz16 << std::endl;
  ost << "SecPerTrk:  0x" << v.BPB_SecPerTrk << std::endl;
  ost << "NumHeads:   0x" << v.BPB_NumHeads << std::endl;
  ost << "HiddSec:    0x" << v.BPB_HiddSec << std::endl;
  ost << "TotSec32:   0x" << v.BPB_TotSec32 << std::endl;
  ost << "FATSz32:    0x" << v.BPB_FATSz32 << std::endl;
  ost << "ExtFlags:   0x" << v.BPB_ExtFlags << std::endl;
  ost << "FSVer:      0x" << v.BPB_FSVer << std::endl;
  ost << "RootClus:   0x" << v.BPB_RootClus << std::endl;
  ost << "FSInfo:     0x" << v.BPB_FSInfo << std::endl;
  ost << "BkBootSec:  0x" << v.BPB_BkBootSec << std::endl;
  ost << "Reserved:   0x" << print(v.BPB_Reserved, 12) << std::endl;
  ost << "DrvNum:     0x" << (int)(v.BS_DrvNum & 0xff) << std::endl;
  ost << "Reserved:   0x" << (int)(v.BS_Reserved & 0xff) << std::endl;
  ost << "BootSig:    0x" << (int)(v.BS_BootSig & 0xff) << std::endl;
  ost << "VolID:      0x" << v.BS_VolID << std::endl;
  ost << "VolLab:     0x" << print(v.BS_VolLab, 11) << std::endl;
  ost << "FilSysType: 0x" << print(v.BS_FilSysType, 8) << std::endl;
  ost << "BootCode32: 0x" << print(v.BS_BootCode32, 420) << std::endl;
  ost << "BootSign:   0x" << v.BS_BootSign << std::endl;
  return ost;
}

inline std::ostream &operator<<(std::ostream &ost, const FSInfo v) {
  ost << std::hex;
  ost << "LeadSig: 0x" << v.FSI_LeadSig << std::endl;
  ost << "Reserved1: 0x" << print(v.FSI_Reserved1, 480) << std::endl;
  ost << "StrucSig: 0x" << v.FSI_StrucSig << std::endl;
  ost << "Free Count: 0x" << v.FSI_Free_Count << std::endl;
  ost << "Nxt Free: 0x" << v.FSI_Nxt_Free << std::endl;
  ost << "Reserved2: 0x" << print(v.FSI_Reserved2, 12) << std::endl;
  ost << "TrailSig: 0x" << v.FSI_TrailSig << std::endl;
  return ost;
}
inline std::ostream &operator<<(std::ostream &ost, const FileEntry v) {
  ost << std::hex;
  if (is_long(v)) {
    ost << "Long File Name" << std::endl;
    ost << "Num: " << (int)v.b.Num << std::endl;
    ost << "UnicodeName: " << print(v.b.UnicodeName, 10) << std::endl;
    ost << "UnicodeName: " << print1(v.b.UnicodeName, 10) << std::endl;
    ost << "Flag: " << (int)v.b.Flag << std::endl;
    ost << "Rsv: " << v.b.Rsv << std::endl;
    ost << "Check:" << (int)v.b.Check << std::endl;
    ost << "UnicodeName1: " << print(v.b.UnicodeName1, 12) << std::endl;
    ost << "UnicodeName1: " << print1(v.b.UnicodeName1, 12) << std::endl;
    ost << "Sect: " << v.b.Sect << std::endl;
    ost << "UnicodeName2: " << print(v.b.UnicodeName2, 4) << std::endl;
    ost << "UnicodeName2: " << print1(v.b.UnicodeName2, 4) << std::endl;
  } else {
    ost << "Short File Name" << std::endl;
    ost << "Filename: " << print(v.a.FileName, 8) << std::endl;
    ost << "Extention: " << print(v.a.Extention, 3) << std::endl;
    ost << "Filename: " << print1(v.a.FileName, 8) << std::endl;
    ost << "Extention: " << print1(v.a.Extention, 3) << std::endl;
    ost << "Attr: " << (int)v.a.Attr << std::endl;
    ost << "Reserved: " << (int)v.a.Reserved << std::endl;
    ost << "Time10ms: " << (int)v.a.Time10ms << std::endl;
    ost << "TimeCreate: " << v.a.TimeCreate << std::endl;
    ost << "DateCreate: " << v.a.DateCreate << std::endl;
    ost << "DateVst: " << v.a.DateVst << std::endl;
    ost << "ClsH16: " << v.a.ClsH16 << std::endl;
    ost << "TimeModify: " << v.a.TimeModify << std::endl;
    ost << "DateModify: " << v.a.DateModify << std::endl;
    ost << "ClsL16: " << v.a.ClsL16 << std::endl;
    ost << "FileLen: " << v.a.FileLen << std::endl;
  }
  return ost;
}
#undef print
#undef print1
