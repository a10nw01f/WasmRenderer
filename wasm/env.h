#pragma once

extern "C" {
  extern void Debugger();

  extern void TexImage2DHandle(unsigned int target, int level, unsigned int internalFormat, unsigned int format, 
                              unsigned int type, unsigned int handle);

  extern void TexStorage2D(
    unsigned int target, 
    unsigned int levels, 
    unsigned int internalFormat, 
    unsigned int width, 
    unsigned int height);
}