#ifndef PREPROCESS_PARALLEL__
#define PREPROCESS_PARALLEL__

#include "util/fake_ofstream.hh"
#include "util/file_piece.hh"

#include <iostream>

#include <stdint.h>

template <class Pass> int FilterParallel(Pass &pass, bool pair, int argc, char **argv) {
  uint64_t input = 0, output = 0;
  if (argc == 1) {
    StringPiece line;
    util::FilePiece in(0, NULL, &std::cerr);
    util::FakeOFStream out(1);
    while (true) {
      try {
        line = in.ReadLine();
      } catch (const util::EndOfFileException &e) { break; }
      ++input;
      if (pass(line)) {
        out << line << '\n';
        ++output;
      }
    }
  } else if (argc == 5) {
    StringPiece line0, line1, combined;
    util::FilePiece in0(argv[1], &std::cerr), in1(argv[2]);
    util::FakeOFStream out0(util::CreateOrThrow(argv[3])), out1(util::CreateOrThrow(argv[4]));
    while (true) {
      try {
        line0 = in0.ReadLine();
      } catch (const util::EndOfFileException &e) { break; }
      line1 = in1.ReadLine();
      ++input;
      bool p = false;
      if (pair) {
        std::string str0(!line0.empty() ? line0.data() : "", line0.size());
        std::string str1(!line1.empty() ? line1.data() : "", line1.size());
        combined.set((str0 + "|||" + str1).c_str());
        p = pass(combined);
      } else {
        p = pass(line0) && pass(line1);
      }
      if (p) {
        out0 << line0 << '\n';
        out1 << line1 << '\n';
        ++output;
      }
    }
    try {
      line1 = in1.ReadLine();
      std::cerr << "Input is not balaced: " << argv[2] << " has " << line1 << std::endl;
      return 2;
    } catch (const util::EndOfFileException &e) {}
  } else if (argc == 7) {
    StringPiece line0, line1;
    util::FilePiece in0(argv[1], &std::cerr), in1(argv[2]);
    util::FakeOFStream out0(util::CreateOrThrow(argv[5])), out1(util::CreateOrThrow(argv[6]));

    while (true) {
      try {
        line0 = in0.ReadLine();
      } catch (const util::EndOfFileException &e) { break; }
      line1 = in1.ReadLine();
      pass.AddLine(line0);
      pass.AddLine(line1);
    }
    try {
      line1 = in1.ReadLine();
      std::cerr << "Input is not balaced: " << argv[2] << " has " << line1 << std::endl;
      return 2;
    } catch (const util::EndOfFileException &e) {}

    std::cerr << "Finished reading " << argv[1] << " and " << argv[2] << std::endl;    

    util::FilePiece in2(argv[3], &std::cerr), in3(argv[4]);

    // Do the actual filtering.
    while (true) {
      try {
        line0 = in2.ReadLine();
      } catch (const util::EndOfFileException &e) { break; }
      line1 = in3.ReadLine();
      ++input;
      if (pass(line0) && pass(line1)) {
        out0 << line0 << '\n';
        out1 << line1 << '\n';
        ++output;
      }
    }
    try {
      line1 = in3.ReadLine();
      std::cerr << "Input is not balaced: " << argv[4] << " has " << line1 << std::endl;
      return 2;
    } catch (const util::EndOfFileException &e) {}

  } else {
    std::cerr << 
      "To filter one file, run\n" << argv[0] << " <stdin >stdout\n"
      "To filter parallel files, run\n" << argv[0] << "in0 in1 out0 out1\n"
      "To remove all lines that are either in the source or the target of"
      " a small bitext from the bitext, run\n" << argv[0] <<  "small_in0 small_in1 in0 in1 out0 out1\n";
    return 1;
  }
  std::cerr << "Kept " << input << " / " << output << " = " << (static_cast<float>(output) / static_cast<float>(input)) << std::endl;
  return 0;
}

#endif
