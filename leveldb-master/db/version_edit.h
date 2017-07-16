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

//��Sstable�ķ�װ�����������Сkey��
struct FileMetaData {
  int refs;
  int allowed_seeks;        //��compact֮ǰ����seek ���ٴ�    // Seeks allowed until compaction
  uint64_t number;
  uint64_t file_size;         //�ļ�ռ�ж����ֽ�
  InternalKey smallest;      //Table ��С��internal key
  InternalKey largest;        // Table ����internal key

  FileMetaData() : refs(0), allowed_seeks(1 << 30), file_size(0) { }//�����ʼ��������̫���ˣ�������е���
};

class VersionEdit {//����Ӧ���Ƕ�Version���޸Ĳ����ķ�װ?
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
  //�����ָ�������ָ���ļ�
  //Ҫ��:��version��û�б�����
  //Ҫ��:"smallest" ��"largest" ���ļ��е���С������key
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

 private://���ݳ�Ա����˽�е�
  friend class VersionSet;//��Ԫ��

  typedef std::set< std::pair<int, uint64_t> > DeletedFileSet;

  std::string comparator_;//DB һ��������������߼��ͱ��뱣�ּ��ݣ���comparator����������Ϊƾ֤
  uint64_t log_number_;//log ��FileNumber
  uint64_t prev_log_number_;//����log ��FileNumber
  uint64_t next_file_number_;//��һ�����õ�FileNumber
  SequenceNumber last_sequence_;
  //��ʶ�Ƿ���ڣ�����ʹ��
  bool has_comparator_;
  bool has_log_number_;
  bool has_prev_log_number_;
  bool has_next_file_number_;
  bool has_last_sequence_;

//Ҫ���µ�level 
  std::vector< std::pair<int, InternalKey> > compact_pointers_;
  DeletedFileSet deleted_files_;//Ҫɾ����sstable�ļ�����compact������
  std::vector< std::pair<int, FileMetaData> > new_files_;//�µ��ļ�
};//ÿ��compact֮�󶼻Ὣ��Ӧ��VersionEdit encode ����manifest�ļ�

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_VERSION_EDIT_H_
