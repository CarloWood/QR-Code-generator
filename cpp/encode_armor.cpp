// COMPILE AS:
// g++ -std=c++11 -Wall -fsanitize=undefined -O1 -o encode_armor encode_armor.cpp BitBuffer.cpp QrCode.cpp QrSegment.cpp
//
// USAGE:
// gpg --armor --export-secret-key BC27F53B0837 | ./encode_armor
//
// Writes part_x.svg files that you can open (and print) with a browser (for example).

#include "QrCode.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

int main()
{
  std::string ascii_text_with_newlines;
  for (std::string line; std::getline(std::cin, line);)
  {
    ascii_text_with_newlines += line;
    ascii_text_with_newlines += '\n';
  }
  size_t length = ascii_text_with_newlines.length();

  size_t end;
  int part_x = 0;
  for (size_t beg = 0; beg < length; beg = end, ++part_x)
  {
    size_t len = length - beg;
    for (;;)
    {
      end = beg + len;
      std::string part = ascii_text_with_newlines.substr(beg, len);
      std::vector<QrSegment> segs0 = QrSegment::makeSegments(part.c_str());
      try
      {
        QrCode const qr = QrCode::encodeSegments(segs0, QrCode::Ecc::HIGH, QrCode::MIN_VERSION, QrCode::MAX_VERSION, -1, true);
        std::stringstream filename;
        filename << "part_" << part_x << ".svg";
        std::cerr << "Writing to \"" << filename.str() << "\"." << std::endl;
        std::ofstream file(filename.str().c_str());
        file << qr.toSvgString(4) << std::endl;
        file.close();
      }
      catch (char const* msg)
      {
        if (strcmp(msg, "Data too long") == 0)
        {
          --len;
          continue;
        }
        std::cerr << "Error: " << msg << std::endl;
        return EXIT_FAILURE;
      }
      break;
    }
  }
  std::cout << "Success, wrote " << part_x << " parts." << std::endl;
}
