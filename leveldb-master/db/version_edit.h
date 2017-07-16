// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_VERSION_EDIT_H_
#define STORAGE_LEVELDB_DB_VERSION_EDIT_H_

#include <set>
#include <utility>
#include <vector>
#include "db/dbformat.h"

namespace leveldb {

class VersionSet;

//对Sstable的封装，保存最大最小key等
struct FileMetaData {
  int refs;
  int allowed_seeks;        //在compact之前允许seek 多少次    // Seeks allowed until compaction
  uint64_t number;
  uint64_t file_size;         //文件占有多少字节
  InternalKey smallest;      //Table 最小的internal key
  InternalKey largest;        // Table 最大的internal key

  FileMetaData() : refs(0), allowed_seeks(1 << 30), file_size(0) { }//这里初始化的数字太大了，后面会有调整
};

class VersionEdit {//这里应该是对Version的修改操作的封装?
 public:
  VersionEdit() { Clear(); }
  ~VersionEdit() { }

  void Clear();

  void SetComparatorName(const Slice& name) {
    has_comparator_ = true;
    comparator_ = name.ToString();
  }
  void SetLogNumber(uint64_t num) {
    has_log_number_ = true;
    log_number_ = num;
  }
  void SetPrevLogNumber(uint64_t num) {
    has_prev_log_number_ = true;
    prev_log_number_ = num;
  }
  void SetNextFile(uint64_t num) {
    has_next_file_number_ = true;
    next_file_number_ = num;
  }
  void SetLastSequence(SequenceNumber seq) {
    has_last_sequence_ = true;
    last_sequence_ = seq;
  }
  void SetCompactPointer(int level, const InternalKey& key) {
    compact_pointers_.push_back(std::make_pair(level, key));
  }

  // Add the specified file at the specified number.
  // REQUIRES: This version has not been saved (see VersionSet::SaveTo)
  // REQUIRES: "smallest" and "largest" are smallest and largest keys in file
  //添加在指定号码的指定文件
  //要求:该version还没有被保存
  //要求:"smallest" 和"largest" 是文件中的最小和最大的key
  void AddFile(int level, uint64_t file,
               uint64_t file_size,
               const InternalKey& smallest,
               const InternalKey& largest) {
    FileMetaData f;
    f.number = file;
    f.file_size = file_size;
    f.smallest = smallest;
    f.largest = largest;
    new_files_.push_back(std::make_pair(level, f));
  }

  // Delete the specified "file" from the specified "level".
  void DeleteFile(int level, uint64_t file) {
    deleted_files_.insert(std::make_pair(level, file));
  }

  void EncodeTo(std::string* dst) const;
  Status DecodeFrom(const Slice& src);

  std::string DebugString() const;

 private://数据成员都是私有的
  friend class VersionSet;//友元类

  typedef std::set< std::pair<int, uint64_t> > DeletedFileSet;

  std::string comparator_;//DB 一旦创建，排序的逻辑就必须保持兼容，用comparator的名字来作为凭证
  uint64_t log_number_;//log 的FileNumber
  uint64_t prev_log_number_;//辅助log 的FileNumber
  uint64_t next_file_number_;//下一个可用的FileNumber
  SequenceNumber last_sequence_;
  //标识是否存在，方便使用
  bool has_comparator_;
  bool has_log_number_;
  bool has_prev_log_number_;
  bool has_next_file_number_;
  bool has_last_sequence_;

//要更新的level 
  std::vector< std::pair<int, InternalKey> > compact_pointers_;
  DeletedFileSet deleted_files_;//要删除的sstable文件，即compact的输入
  std::vector< std::pair<int, FileMetaData> > new_files_;//新的文件
};//每次compact之后都会将对应的VersionEdit encode 进入manifest文件

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_VERSION_EDIT_H_
