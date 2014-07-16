#include "preprocess/dedupe.hh"
#include "preprocess/parallel.hh"

class RemoveOverlaps {
  public:
    bool operator()(const StringPiece &line) {
      uint64_t key = util::MurmurHashNative(line.data(), line.size()) + 1;
      Table::MutableIterator it;
      return !table_.UnsafeMutableFind(key, it);
    }
    
    void AddLine(const StringPiece &line) {
      Entry entry;
      entry.key = util::MurmurHashNative(line.data(), line.size()) + 1;
      Table::MutableIterator it;
      table_.FindOrInsert(entry, it);
   }

  private:
    typedef util::AutoProbing<Entry, util::IdentityHash> Table;
    Table table_;
};

int main(int argc, char *argv[]) {
  RemoveOverlaps remove_overlaps;
  return FilterParallel(remove_overlaps, false, argc, argv);
}
