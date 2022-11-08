//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: Main.cpp
// ========
// Main function for cg template.
//
// Author: Paulo Pagliosa
// Last revision: 07/11/2022

#include "graphics/Application.h"
#include "MainWindow.h"

int
main(int argc, char** argv)
{
  puts("Ds template by Paulo Pagliosa (ppagliosa@gmail.com)\n");
  puts("Camera controls keys:\n"
    "(w) move forward  (s) move backward\n"
    "(a) move left     (d) move right\n"
    "(q) move up       (z) move down\n");
  puts("Mouse controls:\n"
    "(scroll wheel)    zoom\n"
    "(middle-click)    pan\n"
    "(Alt+right-click) rotate");
  return cg::Application{new MainWindow{1280, 720}}.run(argc, argv);
}
