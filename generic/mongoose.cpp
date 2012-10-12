#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/mongoose.cpp"
#else

//======================================================================
// File: mongoose.cpp
//
// Description: Mongoose 9 DoF 
//
// Created: Sept 24th, 2012
//
// Author: Michael Mathieu // mathieu@cs.nyu.edu
//======================================================================

#include "THpp.hpp"
#include<iostream>
#include "mongoose.h"

using namespace TH;
using namespace std;

#ifndef __READSHORT_DEF_240912__
#define __READSHORT_DEF_240912__
inline int readShort(unsigned char* b) {
  return (((int)b[0] - 1) << 8) + b[1] - 32768;
}

inline long readLong(unsigned char* b) {
  return ((b[0] & 63) << 28) + ((b[1] & 127) << 21) + ((b[2] & 127) << 14) +
    ((b[3] & 127) << 7) + (b[4] & 127);
}

template<typename realT>
inline void readV3D(unsigned char* b, realT* dest) {
  dest[0] = (realT)(readShort(b  )) * (realT)0.1;
  dest[1] = (realT)(readShort(b+2)) * (realT)0.1;
  dest[2] = (realT)(readShort(b+4)) * (realT)0.1;
}
#endif


bool libmongoose_(FetchMongooseElem)(Mongoose & mongoose, real* data) {
  size_t n = fread(mongoose.buffer+mongoose.i_buffer, sizeof(unsigned char), LINE_SIZE-mongoose.i_buffer, mongoose.file);
  if (n < 0)
    cout << "ARGHHHHHHHHHHHZZZZZZZZ" << endl;

  /* DEBUG
  int ct = 0;
  cout << mongoose.i_buffer << " " << n << " | ";
  for (int i = 0; i < mongoose.i_buffer+n; ++i) {
    cout << (int)mongoose.buffer[i] << " ";
    if (++ct % 10 == 0) cout << "* ";
  }
  cout << endl;
  */

  for (int i = mongoose.i_buffer+n-1; i >= max(mongoose.i_buffer, 1); --i)
    if (mongoose.buffer[i] == 0) {
      // beginning of line at the wrong place. drop what was before
      memmove(mongoose.buffer, mongoose.buffer+i, (mongoose.i_buffer+n-i)*sizeof(char));
      mongoose.i_buffer += n-i;
      cout << "zero" << endl;
      return true;
    }
  mongoose.i_buffer += n;
  if (mongoose.buffer[0] != 0) {
    // corrupted line. drop it.
    cout << "corrupted line" << endl;
    mongoose.i_buffer = 0;
    return true;
  }
  if (mongoose.i_buffer < LINE_SIZE)
    return false;
  if (mongoose.i_buffer > LINE_SIZE)
    cout << "ERROR !!!! " << mongoose.i_buffer << endl;
  mongoose.i_buffer = 0;
  unsigned char checksum = 0;
  for (int i = 0; i < LINE_SIZE-1; ++i)
    checksum += mongoose.buffer[i];
  if (checksum == 0)
    checksum = 1;
  if (checksum != mongoose.buffer[LINE_SIZE-1]) {
    cout << "corrupted line (chksum)" << endl;
    // checksum error. drop the line.
    return true;
  }
  
  data[18] = readLong(mongoose.buffer+1);
  if (mongoose.buffer[1] & 64) {
    //readV3D<real>(mongoose.buffer+6, mongoose.data);
    //readV3D<real>(mongoose.buffer+12, mongoose.data+3);
    //readV3D<real>(mongoose.buffer+18, mongoose.data+6);  
    //readV3D<real>(mongoose.buffer+24, mongoose.data+9);
  } else {
    readV3D<real>(mongoose.buffer+6, data);
    readV3D<real>(mongoose.buffer+12, data+3);
    readV3D<real>(mongoose.buffer+18, data+6);  
    for (int i = 8; i >= 0; --i)
      data[i+9] = (real)(readShort(mongoose.buffer+24+2*i)) * (real)0.001;
  }
  return true;
}

static int libmongoose_(FetchMongooseData)(lua_State *L) {
  setLuaState(L);
  Mongoose & mongoose = *((Mongoose*)lua_touserdata(L, 1));
  Tensor<real> dataL = FromLuaStack<Tensor<real> >(2);
  real* data = dataL.data();
  
  while (libmongoose_(FetchMongooseElem)(mongoose, data));

  return 0;
}

//============================================================
// Register functions in LUA
//

static const luaL_reg libmongoose_(Main__) [] = {
  {"fetchMongooseData",     libmongoose_(FetchMongooseData)},
  {NULL, NULL}  /* sentinel */
};

LUA_EXTERNC DLL_EXPORT int libmongoose_(Main_init) (lua_State *L) {
  luaT_pushmetatable(L, torch_Tensor);
  luaT_registeratname(L, libmongoose_(Main__), "libmongoose");
  lua_pop(L,1);
  return 1;
}

#endif
