#ifndef PREPROCESS_DEDUPE__
#define PREPROCESS_DEDUPE__

#include "util/murmur_hash.hh"
#include "util/probing_hash_table.hh"
#include "util/scoped.hh"

#include <boost/lexical_cast.hpp>

#include <iostream>

#include <stdint.h>

struct Entry {
  typedef uint64_t Key;
  uint64_t key;
  uint64_t GetKey() const { return key; }
  void SetKey(uint64_t to) { key = to; }
};


#endif
