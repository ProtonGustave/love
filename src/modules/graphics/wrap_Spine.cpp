#include "wrap_Spine.h"
#include "math/wrap_Transform.h"

// TODO: create more API, add TrackEntry(not needed maybe as an object?)
// API:
// State:
//  :update(dt)
//  :apply(skeleton)
// -------------------------------------------------------------------------------------
// Lua API:
// stateData, skeletonData = love.graphics.newSpine(atlasPath, jsonPath)
// local skeleton = skeletonData:createSkeleton()
// local state = stateData:createState()
// state:update()
// state:apply(skeleton)
// skeleton:updateWorldTransform()
// skeleton:updateMesh()
// love.graphics.draw(skeleton)
//
// -------------------------------------------------------------------------------------

namespace love
{
namespace graphics
{

// SkeletonData
SkeletonData* luax_checkskeletondata(lua_State *L, int idx)
{
	return luax_checktype<SkeletonData>(L, idx);
}

int w_SkeletonData_createSkeleton(lua_State *L)
{
	SkeletonData* skeletonData = luax_checkskeletondata(L, 1);
    Skeleton* skeleton = new Skeleton(skeletonData);

    luax_pushtype(L, skeleton);
    return 1;
}

static const luaL_Reg w_SkeletonData_functions[] =
{
	{ "createSkeleton", w_SkeletonData_createSkeleton },
	{ 0, 0 }
};

extern "C" int luaopen_skeletondata(lua_State *L)
{
	return luax_register_type(L, &SkeletonData::type, w_SkeletonData_functions, nullptr);
}

// StateData
StateData* luax_checkstatedata(lua_State *L, int idx)
{
	return luax_checktype<StateData>(L, idx);
}

int w_StateData_createState(lua_State *L)
{
    StateData* stateData = luax_checkstatedata(L, 1);
    State* state = new State(stateData);

    luax_pushtype(L, state);
    return 1;
}

static const luaL_Reg w_StateData_functions[] =
{
	{ "createState", w_StateData_createState },
	{ 0, 0 }
};

extern "C" int luaopen_statedata(lua_State *L)
{
	return luax_register_type(L, &StateData::type, w_StateData_functions, nullptr);
}

// State
State* luax_checkstate(lua_State *L, int idx)
{
	return luax_checktype<State>(L, idx);
}

int w_State_apply(lua_State *L)
{
    State* state = luax_checkstate(L, 1);
    Skeleton* skeleton = luax_checkskeleton(L, 2);
    spAnimationState_apply(state->state, skeleton->skeleton);
    return 0;
}

int w_State_update(lua_State *L)
{
    State* state = luax_checkstate(L, 1);
	float dt = (float) luaL_checknumber(L, 2);
    spAnimationState_update(state->state, dt);
    return 0;
}

int w_State_setAnimationByName(lua_State *L)
{
    State* state = luax_checkstate(L, 1);
	int track = (int) luaL_checkinteger(L, 2);
    const char* animName = luaL_checkstring(L, 3);
    bool loop = luax_checkboolean(L, 4);
    // TODO: return created track entry
    spAnimationState_setAnimationByName(state->state, track, animName, loop);
    return 0;
}

static const luaL_Reg w_State_functions[] =
{
    // TODO: addAnimationByName, addEmptyAnimation
    // setEmptyAnimation, API for TrackEntry(mixScale etc)
    { "setAnimationByName", w_State_setAnimationByName },
	{ "update", w_State_update },
	{ "apply", w_State_apply },
	{ 0, 0 }
};

extern "C" int luaopen_state(lua_State *L)
{
	return luax_register_type(L, &State::type, w_State_functions, nullptr);
}

// Skeleton
Skeleton* luax_checkskeleton(lua_State *L, int idx)
{
	return luax_checktype<Skeleton>(L, idx);
}

int w_Skeleton_updateWorldTransform(lua_State *L)
{
    Skeleton* skeleton = luax_checkskeleton(L, 1);
    spSkeleton_updateWorldTransform(skeleton->skeleton);
    return 0;
}

int w_Skeleton_updateMesh(lua_State *L)
{
    Skeleton* skeleton = luax_checkskeleton(L, 1);
    skeleton->updateMesh();
    return 0;
}

static const luaL_Reg w_Skeleton_functions[] =
{
	{ "updateWorldTransform", w_Skeleton_updateWorldTransform },
	{ "updateMesh", w_Skeleton_updateMesh },
	{ 0, 0 }
};

extern "C" int luaopen_skeleton(lua_State *L)
{
	return luax_register_type(L, &Skeleton::type, w_Skeleton_functions, nullptr);
}

} // graphics
} // love
