CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
CMAKE_POLICY(VERSION 2.6)
FIND_PACKAGE(Torch REQUIRED)

SET(src THpp.cpp mongoose.cpp)
SET(luasrc init.lua)

ADD_TORCH_PACKAGE(mongoose "${src}" "${luasrc}" "Mongoose")
TARGET_LINK_LIBRARIES(mongoose luaT TH)
