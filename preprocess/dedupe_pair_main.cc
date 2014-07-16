#include "preprocess/dedupe.hh"
#include "preprocess/parallel.hh"


class DedupePair {
  public:
    bool operator()(const StringPiece &line) {
      Entry entry;
      entry.key = util::MurmurHashNative(line.data(), line.size()) + 1;
      Table::MutableIterator it;
      return !table_.FindOrInsert(entry, it);
    }

    void AddLine(const StringPiece &line) {
      this->operator()(line);
    }

  private:
    typedef util::AutoProbing<Entry, util::IdentityHash> Table;
    Table table_;
};



int main(int argc, char *argv[]) {
  DedupePair dedupe;
  return FilterParallel(dedupe, true, argc, argv);
}
