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
inline int readShort(char* b) {
  int ret = (b[0] << 8) + b[1];
  if (ret == 32511) {
    ret = 0xffff;
  } else if ((ret > 32512) and (ret < 32768)) {
    ret = 32767 - ret;
    ret = (ret << 8) + 255;
  } else {
    ret = -257;
  }
  return (ret < 32768) ? ret : (ret-65535);
}

inline long readLong(char* b) {
  return ((b[0] & 63) << 28) + ((b[1] & 127) << 21) + ((b[2] & 127) << 14) +
    ((b[3] & 127) << 7) + (b[4] & 127);
}

template<typename realT>
inline void readV3D(char* b, realT* dest) {
  dest[0] = (realT)(readLong(b  )) * (realT)0.1;
  dest[1] = (realT)(readLong(b+1)) * (realT)0.1;
  dest[2] = (realT)(readLong(b+2)) * (realT)0.1;
}
#endif




static int libmongoose_(InitMongoose)(lua_State* L) {
  setLuaState(L);
  string tty = FromLuaStack<string>(1);
  Tensor<real> data = FromLuaStack<Tensor<real> >(2);
  THassert(data.size(0) == 19);
  Mongoose<real>* mongoose = new Mongoose<real>(tty, data.data());
  if (mongoose->file == NULL)
    lua_pushnil(L);
  else
    lua_pushlightuserdata(L, (void*)mongoose);
  return 1;
}

static int libmongoose_(ReleaseMongoose)(lua_State* L) {
  setLuaState(L);
  Mongoose<real>* mongoose = (Mongoose<real>*)lua_touserdata(L, 1);
  delete mongoose;
  return 0;
}

static int libmongoose_(FetchMongooseData)(lua_State *L) {
  setLuaState(L);
  Mongoose<real> & mongoose = *((Mongoose<real>*)lua_touserdata(L, 1));
  
  size_t n = fread(mongoose.buffer+mongoose.i_buffer,
		   sizeof(char), LINE_SIZE-mongoose.i_buffer, mongoose.file);
  mongoose.i_buffer += n;
  if (mongoose.buffer[0] != 0) {
    // corrupted line. drop it.
    mongoose.i_buffer = 0;
    return 0;
  }
  if (mongoose.i_buffer < LINE_SIZE)
    return 0;
  mongoose.i_buffer = 0;
  char checksum = 0;
  for (int i = 0; i < LINE_SIZE-1; ++i)
    checksum += mongoose.buffer[i];
  if (checksum == 0)
    checksum = 1;
  if (checksum != mongoose.buffer[LINE_SIZE-1])
    // checksum error. drop the line.
    return 0;
  
  mongoose.data[18] = readLong(mongoose.buffer+1);
  if (mongoose.buffer[0] & 64) {
    //readV3D<real>(mongoose.buffer+6, mongoose.data);
    //readV3D<real>(mongoose.buffer+12, mongoose.data+3);
    //readV3D<real>(mongoose.buffer+18, mongoose.data+6);  
    //readV3D<real>(mongoose.buffer+24, mongoose.data+9);
  } else {
    readV3D<real>(mongoose.buffer+6, mongoose.data);
    readV3D<real>(mongoose.buffer+12, mongoose.data+3);
    readV3D<real>(mongoose.buffer+18, mongoose.data+6);  
    for (int i = 8; i >= 0; --i)
      mongoose.data[i+9] = (real)(readShort(mongoose.buffer+24+2*i)) * (real)0.01;
  }

  return 0;
}

//============================================================
// Register functions in LUA
//

static const luaL_reg libmongoose_(Main__) [] = {
  {"initMongoose", libmongoose_(InitMongoose)},
  {"releaseMongoose", libmongoose_(ReleaseMongoose)},
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
