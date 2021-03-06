// Copyright (C) 2020 Carl Enlund
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "utilities.h"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

void SeedRandom() {
  srand(static_cast<unsigned int>(time(nullptr)));
}

bool LoadFile(const std::string &path, std::string *data) {
  std::ifstream file(path.c_str());
  if (!file) {
    return false;
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  *data = buffer.str();
  return true;
}
