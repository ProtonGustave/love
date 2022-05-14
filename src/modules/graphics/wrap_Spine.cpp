#include "wrap_Spine.h"
#include "math/wrap_Transform.h"

// Apparently it's dangerous to use love.graphics in threads, think how not to
// TODO: create more API, add TrackEntry(not needed maybe as an object?)

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

int w_SkeletonData_getSetupSize(lua_State *L)
{
	SkeletonData* skeletonData = luax_checkskeletondata(L, 1);
    lua_pushnumber(L, skeletonData->skeletonData->width);
    lua_pushnumber(L, skeletonData->skeletonData->height);
    return 2;
}

int w_SkeletonData_findAnimation(lua_State *L)
{
	SkeletonData* skeletonData = luax_checkskeletondata(L, 1);
    const char* animName = luaL_checkstring(L, 2);
    spAnimation* anim = spSkeletonData_findAnimation(skeletonData->skeletonData, animName);

    if (anim == 0) {
        lua_pushnil(L);
    } else {
        luax_pushtype(L, new Animation(anim));
    }

    return 1;
}

static const luaL_Reg w_SkeletonData_functions[] =
{
	{ "findAnimation", w_SkeletonData_findAnimation },
	{ "getSetupSize", w_SkeletonData_getSetupSize },
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

int w_StateData_setDefaultMix(lua_State *L)
{
    StateData* stateData = luax_checkstatedata(L, 1);
    float defaultMix = luaL_checknumber(L, 2);
    stateData->stateData->defaultMix = defaultMix;
    return 0;
}

static const luaL_Reg w_StateData_functions[] =
{
	{ "createState", w_StateData_createState },
	{ "setDefaultMix", w_StateData_setDefaultMix },
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

// TODO: check if animations exists
int w_State_setAnimationByName(lua_State *L)
{
    State* state = luax_checkstate(L, 1);
	int track = (int) luaL_checkinteger(L, 2);
    const char* animName = luaL_checkstring(L, 3);
    bool loop = luax_checkboolean(L, 4);
    spTrackEntry* trackEntry = spAnimationState_setAnimationByName(state->state, track, animName, loop);
    luax_pushtype(L, new TrackEntry(trackEntry));
    return 1;
}

int w_State_addAnimationByName(lua_State *L)
{
    State* state = luax_checkstate(L, 1);
	int track = (int) luaL_checkinteger(L, 2);
    const char* animName = luaL_checkstring(L, 3);
    bool loop = luax_checkboolean(L, 4);
    float delay = luaL_checknumber(L, 5);
    spTrackEntry* trackEntry = spAnimationState_addAnimationByName(state->state, track, animName, loop, delay);
    luax_pushtype(L, new TrackEntry(trackEntry));
    return 1;
}

int w_State_setEmptyAnimation(lua_State *L)
{
    State* state = luax_checkstate(L, 1);
	int track = (int) luaL_checkinteger(L, 2);
    float mixDuration = luaL_checknumber(L, 3);
    spTrackEntry* trackEntry = spAnimationState_setEmptyAnimation(state->state, track, mixDuration);
    luax_pushtype(L, new TrackEntry(trackEntry));
    return 1;
}

int w_State_getCurrent(lua_State *L)
{
    State* state = luax_checkstate(L, 1);
	int track = (int) luaL_checkinteger(L, 2);
    spTrackEntry* trackEntry = spAnimationState_getCurrent(state->state, track);

    if (trackEntry == 0) {
        lua_pushnil(L);
    } else {
        luax_pushtype(L, new TrackEntry(trackEntry));
    }

    return 1;
}

int w_State_getTracksCount(lua_State *L)
{
    State* state = luax_checkstate(L, 1);
    lua_pushnumber(L, state->state->tracksCount);
    return 1;
}

static const luaL_Reg w_State_functions[] =
{
    { "getCurrent", w_State_getCurrent },
    { "setAnimationByName", w_State_setAnimationByName },
    { "addAnimationByName", w_State_addAnimationByName },
    { "setEmptyAnimation", w_State_setEmptyAnimation },
    { "getTracksCount", w_State_getTracksCount },
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

int w_Skeleton_findBone(lua_State *L)
{
    Skeleton* skeleton = luax_checkskeleton(L, 1);
    const char* boneName = luaL_checkstring(L, 2);
    spBone* bone = spSkeleton_findBone(skeleton->skeleton, boneName);

    if (bone == 0) {
        lua_pushnil(L);
    } else {
        luax_pushtype(L, new Bone(bone));
    }

    return 1;
}

int w_Skeleton_setPosition(lua_State *L)
{
    Skeleton* skeleton = luax_checkskeleton(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    skeleton->skeleton->x = x;
    skeleton->skeleton->y = y;
    return 0;
}

int w_Skeleton_getPosition(lua_State *L)
{
    Skeleton* skeleton = luax_checkskeleton(L, 1);
    lua_pushnumber(L, skeleton->skeleton->x);
    lua_pushnumber(L, skeleton->skeleton->y);
    return 2;
}

int w_Skeleton_setScale(lua_State *L)
{
    Skeleton* skeleton = luax_checkskeleton(L, 1);
    float sx = luaL_checknumber(L, 2);
    float sy = luaL_checknumber(L, 3);
    skeleton->skeleton->scaleX = sx;
    skeleton->skeleton->scaleY = sy;
    return 0;
}

int w_Skeleton_getScale(lua_State *L)
{
    Skeleton* skeleton = luax_checkskeleton(L, 1);
    lua_pushnumber(L, skeleton->skeleton->scaleX);
    lua_pushnumber(L, skeleton->skeleton->scaleY);
    return 2;
}

int w_Skeleton_setAttachment(lua_State *L)
{
    Skeleton* skeleton = luax_checkskeleton(L, 1);
    const char* slotName = luaL_checkstring(L, 2);
    const char* attachmentName = luaL_checkstring(L, 3);
    spSkeleton_setAttachment(skeleton->skeleton, slotName, attachmentName);
    return 0;
}

static const luaL_Reg w_Skeleton_functions[] =
{
    { "findBone", w_Skeleton_findBone },
    { "setAttachment", w_Skeleton_setAttachment },
    { "setPosition", w_Skeleton_setPosition },
    { "getPosition", w_Skeleton_getPosition },
    { "setScale", w_Skeleton_setScale },
    { "getScale", w_Skeleton_getScale },
	{ "updateWorldTransform", w_Skeleton_updateWorldTransform },
	{ "updateMesh", w_Skeleton_updateMesh },
	{ 0, 0 }
};

extern "C" int luaopen_skeleton(lua_State *L)
{
	return luax_register_type(L, &Skeleton::type, w_Skeleton_functions, nullptr);
}

// Bone
Bone* luax_checkbone(lua_State *L, int idx)
{
	return luax_checktype<Bone>(L, idx);
}

int w_Bone_getWorldTransform(lua_State *L)
{
    Bone* bone = luax_checkbone(L, 1);
    lua_pushnumber(L, bone->bone->a);
    lua_pushnumber(L, bone->bone->b);
    lua_pushnumber(L, bone->bone->c);
    lua_pushnumber(L, bone->bone->d);
    lua_pushnumber(L, bone->bone->worldX);
    lua_pushnumber(L, bone->bone->worldY);
    return 6;
}

int w_Bone_getTransform(lua_State *L)
{
    Bone* bone = luax_checkbone(L, 1);
    lua_pushnumber(L, bone->bone->x);
    lua_pushnumber(L, bone->bone->y);
    lua_pushnumber(L, bone->bone->rotation);
    lua_pushnumber(L, bone->bone->scaleX);
    lua_pushnumber(L, bone->bone->scaleY);
    lua_pushnumber(L, bone->bone->shearX);
    lua_pushnumber(L, bone->bone->shearY);
    return 7;
}

int w_Bone_getSetupTransform(lua_State *L)
{
    Bone* bone = luax_checkbone(L, 1);
    lua_pushnumber(L, bone->bone->data->x);
    lua_pushnumber(L, bone->bone->data->y);
    lua_pushnumber(L, bone->bone->data->rotation);
    lua_pushnumber(L, bone->bone->data->scaleX);
    lua_pushnumber(L, bone->bone->data->scaleY);
    lua_pushnumber(L, bone->bone->data->shearX);
    lua_pushnumber(L, bone->bone->data->shearY);
    return 7;
}

int w_Bone_getSetupPosition(lua_State *L)
{
    Bone* bone = luax_checkbone(L, 1);
    lua_pushnumber(L, bone->bone->data->x);
    lua_pushnumber(L, bone->bone->data->y);
    return 2;
}

int w_Bone_updateWorldTransformWith (lua_State* L)
{
    Bone* bone = luax_checkbone(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float rotation = luaL_checknumber(L, 4);
    float scaleX = luaL_checknumber(L, 5);
    float scaleY = luaL_checknumber(L, 6);
    float shearX = luaL_checknumber(L, 7);
    float shearY = luaL_checknumber(L, 8);
    spBone_updateWorldTransformWith(bone->bone, x, y, rotation, scaleX, scaleY, shearX, shearY);
    return 0;
}

int w_Bone_setPosition(lua_State *L)
{
    Bone* bone = luax_checkbone(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    bone->bone->x = x;
    bone->bone->y = y;
    return 0;
}

int w_Bone_getPosition(lua_State *L)
{
    Bone* bone = luax_checkbone(L, 1);
    lua_pushnumber(L, bone->bone->x);
    lua_pushnumber(L, bone->bone->y);
    return 2;
}

static const luaL_Reg w_Bone_functions[] =
{
    { "updateWorldTransformWith", w_Bone_updateWorldTransformWith },
    { "getWorldTransform", w_Bone_getWorldTransform },
    { "getTransform", w_Bone_getTransform },
    { "getPosition", w_Bone_getPosition },
    { "setPosition", w_Bone_setPosition },
    { "getSetupTransform", w_Bone_getSetupTransform },
    { "getSetupPosition", w_Bone_getSetupPosition },
	{ 0, 0 }
};

extern "C" int luaopen_bone(lua_State *L)
{
	return luax_register_type(L, &Bone::type, w_Bone_functions, nullptr);
}

// TrackEntry
TrackEntry* luax_checktrackentry(lua_State *L, int idx)
{
	return luax_checktype<TrackEntry>(L, idx);
}

int w_TrackEntry_getAnimationTime(lua_State* L)
{
    TrackEntry* trackEntry = luax_checktrackentry(L, 1);
    lua_pushnumber(L, spTrackEntry_getAnimationTime(trackEntry->trackEntry));
    return 1;
}

int w_TrackEntry_getAnimationEnd(lua_State* L)
{
    TrackEntry* trackEntry = luax_checktrackentry(L, 1);
    lua_pushnumber(L, trackEntry->trackEntry->animationEnd);
    return 1;
}

int w_TrackEntry_getAnimation(lua_State* L)
{
    TrackEntry* trackEntry = luax_checktrackentry(L, 1);
    luax_pushtype(L, new Animation(trackEntry->trackEntry->animation));
    return 1;
}

int w_TrackEntry_getTimeScale(lua_State* L)
{
    TrackEntry* trackEntry = luax_checktrackentry(L, 1);
    lua_pushnumber(L, trackEntry->trackEntry->timeScale);
    return 1;
}

int w_TrackEntry_setTimeScale(lua_State* L)
{
    TrackEntry* trackEntry = luax_checktrackentry(L, 1);
    float timeScale = luaL_checknumber(L, 2);
    trackEntry->trackEntry->timeScale = timeScale;
    return 0;
}

int w_TrackEntry_setMixDuration(lua_State* L)
{
    TrackEntry* trackEntry = luax_checktrackentry(L, 1);
    float mixDuration = luaL_checknumber(L, 2);
    trackEntry->trackEntry->mixDuration = mixDuration;
    return 0;
}

int w_TrackEntry_setTrackTime(lua_State* L)
{
    TrackEntry* trackEntry = luax_checktrackentry(L, 1);
    float trackTime = luaL_checknumber(L, 2);
    trackEntry->trackEntry->trackTime = trackTime;
    return 0;
}

static const luaL_Reg w_TrackEntry_functions[] =
{
    { "setTrackTime", w_TrackEntry_setTrackTime },
    { "getAnimationTime", w_TrackEntry_getAnimationTime },
    { "getAnimationEnd", w_TrackEntry_getAnimationEnd },
    { "getAnimation", w_TrackEntry_getAnimation },
    { "getTimeScale", w_TrackEntry_getTimeScale },
    { "setTimeScale", w_TrackEntry_setTimeScale },
    { "setMixDuration", w_TrackEntry_setMixDuration },

	{ 0, 0 }
};

extern "C" int luaopen_trackentry(lua_State *L)
{
	return luax_register_type(L, &TrackEntry::type, w_TrackEntry_functions, nullptr);
}

// Animation
Animation* luax_checkanimation(lua_State *L, int idx)
{
	return luax_checktype<Animation>(L, idx);
}

int w_Animation_getName(lua_State* L)
{
    Animation* anim = luax_checkanimation(L, 1);
    lua_pushstring(L, anim->anim->name);
    return 1;
}

int w_Animation_getDuration(lua_State* L)
{
    Animation* anim = luax_checkanimation(L, 1);
    lua_pushnumber(L, anim->anim->duration);
    return 1;
}

static const luaL_Reg w_Animation_functions[] =
{
    { "getName", w_Animation_getName },
    { "getDuration", w_Animation_getDuration },
	{ 0, 0 }
};

extern "C" int luaopen_animation(lua_State *L)
{
	return luax_register_type(L, &Animation::type, w_Animation_functions, nullptr);
}

} // graphics
} // love
