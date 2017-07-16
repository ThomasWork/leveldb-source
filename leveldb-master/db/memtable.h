// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_MEMTABLE_H_
#define STORAGE_LEVELDB_DB_MEMTABLE_H_

#include <string>
#include "leveldb/db.h"
#include "db/dbformat.h"
#include "db/skiplist.h"
#include "util/arena.h"

namespace leveldb {

class InternalKeyComparator;
class Mutex;
class MemTableIterator;

class MemTable {
 public:
  // MemTables are reference counted.  The initial reference count
  // is zero and the caller must call Ref() at least once.
  //MemTable�������ü����ķ�������������ü���
  //Ϊ0�������߱������Ref() ����һ��
  explicit MemTable(const InternalKeyComparator& comparator);

  // Increase reference count.
  void Ref() { ++refs_; }

  // Drop reference count.  Delete if no more references exist.
  void Unref() {
    --refs_;
    assert(refs_ >= 0);
    if (refs_ <= 0) {
      delete this;//���ref���ٵ�0��ɾ���ñ�
    }
  }

    //����������ݽṹ���ʹ���˶����ֽڵ�����
  //��MemTable���޸�ʱ�����øú����ǰ�ȫ��
  size_t ApproximateMemoryUsage();//ʵ�ʷ��ص���arena �б�����ڴ�ʹ����


// The keys returned by this
  // iterator are internal keys encoded by AppendInternalKey in the
  // db/format.{h,cc} module.
  //����һ��������������memtable�е�����
  //�����߱���ȷ�����������ʱ��MemTableҲ�Ǵ���
  //������������ص�key ��internal key ��
  Iterator* NewIterator();

  // Add an entry into memtable that maps key to value at the
  // specified sequence number and with the specified type.
  // Typically value will be empty if type==kTypeDeletion.
  void Add(SequenceNumber seq, ValueType type,
           const Slice& key,
           const Slice& value);

  // If memtable contains a value for key, store it in *value and return true.
  // If memtable contains a deletion for key, store a NotFound() error
  // in *status and return true.
  // Else, return false.
  //���������key�������洢��*value�У�������
  //�����key�Ѿ�ɾ������*status�б���NotFound()
  //���򣬷���false���������������û���ҵ�
  //�Ż�ȥ�����level����
  bool Get(const LookupKey& key, std::string* value, Status* s);

 private:
  ~MemTable();  // Private since only Unref() should be used to delete it

  struct KeyComparator {
    const InternalKeyComparator comparator;
    explicit KeyComparator(const InternalKeyComparator& c) : comparator(c) { }
    int operator()(const char* a, const char* b) const;
  };
  friend class MemTableIterator;
  friend class MemTableBackwardIterator;

  typedef SkipList<const char*, KeyComparator> Table;

  KeyComparator comparator_;
  int refs_;
  Arena arena_;
  Table table_;//���Ｔ��һ������memtable�Ƕ�����ķ�װ

  // No copying allowed
  MemTable(const MemTable&);//��ֹ�������캯��
  void operator=(const MemTable&);//��ֹ��ֵ����
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_MEMTABLE_H_
