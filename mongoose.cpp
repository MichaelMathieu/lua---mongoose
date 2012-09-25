extern "C" {
#include <TH.h>
#include <luaT.h>
}

#include "THpp.hpp"
using namespace std;
using namespace TH;

//============================================================
// Register functions in LUA
//


#define torch_(NAME)        TH_CONCAT_3(torch_, Real, NAME)
#define torch_Tensor        TH_CONCAT_STRING_3(torch.,Real,Tensor)
#define libmongoose_(NAME)  TH_CONCAT_3(libmongoose_, Real, NAME)

static const luaL_reg libmongoose_init [] = {
  {NULL, NULL}
};

#include "generic/mongoose.cpp"
#include "THGenerateFloatTypes.h"

LUA_EXTERNC DLL_EXPORT int luaopen_libmongoose(lua_State *L) {
  luaL_register(L, "libmongoose", libmongoose_init);
  
  libmongoose_FloatMain_init(L);
  libmongoose_DoubleMain_init(L);
  
  return 1;
}
