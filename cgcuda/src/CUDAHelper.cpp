//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2025 Paulo Pagliosa.                        |
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
// OVERVIEW: CUDAHelper.cpp
// ========
// Source file for CUDA utilities.
//
// Author: Paulo Pagliosa
// Last revision: 05/12/2025

#include <stdarg.h>
#include <stdlib.h>

#include "CUDAHelper.h"

namespace cg::cuda
{ // begin namespace cg::cuda

void
error(const char* fmt, ...)
{
  constexpr auto maxLen = 1023;
  va_list args;
  char msg[maxLen + 1];

  va_start(args, fmt);
  vsnprintf(msg, maxLen, fmt, args);
  fprintf(stderr, "CUDA error: %s.\n", msg);
  fputs("Press any key to exit...", stderr);
  (void)getchar();
  exit(EXIT_FAILURE);
}

void
checkError(cudaError_t code, const char* file, int line)
{
  if (cudaSuccess != code)
  {
    reset();
    error("%s(%i): (%d) %s",
      file,
      line,
      code,
      cudaGetErrorString(code));
  }
}

void
checkLastError(const char* errorMessage, const char* file, int line)
{
  cudaError_t code = cudaGetLastError();

  if (cudaSuccess == code)
    code = cudaDeviceSynchronize();
  if (cudaSuccess != code)
  {
    reset();
    error("%s(%i): %s: (%d) %s",
      file,
      line,
      errorMessage,
      code,
      cudaGetErrorString(code));
  }
}

} // end namespace cg::cuda
