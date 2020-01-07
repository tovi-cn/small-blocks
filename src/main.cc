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

#include <iostream>

#include "game.h"
#include "input_system.h"
#include "renderer.h"
#include "window.h"

int main() {
  Window window;
  if (!window.Initialize("Small Blocks")) {
    return 1;
  }

  Renderer renderer(&window);
  if (!renderer.Initialize()) {
    return 1;
  }

  InputSystem input(&window);
  input.Initialize();

  Game game(&window, &renderer, &input);
  if (!game.Initialize()) {
    return 1;
  }

  game.Run();
  return 0;
}
