extern "C" {
#include <TH.h>
#include <luaT.h>
}

#include "THpp.hpp"
#include "mongoose.h"
using namespace std;
using namespace TH;

static int InitMongoose(lua_State* L) {
  setLuaState(L);
  string tty = FromLuaStack<string>(1);
  Mongoose* mongoose = new Mongoose(tty);
  if (mongoose->file == NULL)
    lua_pushnil(L);
  else
    lua_pushlightuserdata(L, (void*)mongoose);
  return 1;
}

static int ReleaseMongoose(lua_State* L) {
  setLuaState(L);
  int lmongoose = FromLuaStack<int>(1);
  THassert(lua_islightuserdata(L, lmongoose));
  Mongoose* mongoose = (Mongoose*)lua_touserdata(L, lmongoose);
  delete mongoose;
  return 0;
}

//============================================================
// Register functions in LUA
//


#define torch_(NAME)        TH_CONCAT_3(torch_, Real, NAME)
#define torch_Tensor        TH_CONCAT_STRING_3(torch.,Real,Tensor)
#define libmongoose_(NAME)  TH_CONCAT_3(libmongoose_, Real, NAME)

static const luaL_reg libmongoose_init [] = {
  {"initMongoose", InitMongoose},
  {"releaseMongoose", ReleaseMongoose},
  {NULL, NULL}
};

#include "generic/mongoose.cpp"
#include "THGenerateFloatTypes.h"

LUA_EXTERNC DLL_EXPORT int luaopen_mongoose(lua_State *L) {
  luaL_register(L, "libmongoose", libmongoose_init);
  
  libmongoose_FloatMain_init(L);
  libmongoose_DoubleMain_init(L);
  
  return 1;
}
