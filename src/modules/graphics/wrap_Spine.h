#pragma once

#include "Spine.h"
#include "common/runtime.h"

namespace love
{
namespace graphics
{
    SkeletonData* luax_checkskeletondata(lua_State *L, int idx);
    StateData* luax_checkstatedata(lua_State *L, int idx);
    State* luax_checkstate(lua_State *L, int idx);
    Skeleton* luax_checkskeleton(lua_State *L, int idx);
    Bone* luax_checkbone(lua_State *L, int idx);
    TrackEntry* luax_checktrackentry(lua_State *L, int idx);
    Animation* luax_checkanimation(lua_State *L, int idx);

    extern "C" int luaopen_skeletondata(lua_State *L);
    extern "C" int luaopen_statedata(lua_State *L);
    extern "C" int luaopen_state(lua_State *L);
    extern "C" int luaopen_skeleton(lua_State *L);
    extern "C" int luaopen_bone(lua_State *L);
    extern "C" int luaopen_trackentry(lua_State *L);
    extern "C" int luaopen_animation(lua_State *L);
} // graphics
} // love

