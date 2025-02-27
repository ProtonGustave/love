#pragma once

// LOVE
#include "common/config.h"
#include "common/Object.h"
#include "common/Module.h"
#include "Drawable.h"
#include "Image.h"
#include "Mesh.h"
#include "filesystem/Filesystem.h"
#include "filesystem/wrap_Filesystem.h"
#include <spine/extension.h>
#include <spine/spine.h>

namespace love
{
namespace graphics
{

class Graphics;

class SkeletonData : public Object
{
public:
	static love::Type type;

    spSkeletonData* skeletonData;
    spAtlas* atlas;

    SkeletonData(love::filesystem::FileData* atlasData, love::filesystem::FileData* jsonData, Image* img);
	virtual ~SkeletonData();
}; // SkeletonData

class StateData : public Object
{
public:
	static love::Type type;

    SkeletonData* skeletonData;
    spAnimationStateData* stateData;

	StateData(SkeletonData* skeletonData);
	virtual ~StateData();
}; // StateData

class State : public Object
{
public:
	static love::Type type;

    spAnimationState* state;
    StateData* stateData;

	State(StateData* stateData);
	virtual ~State();
}; // State

class Skeleton : public Drawable
{
public:
	static love::Type type;

    SkeletonData* skeletonData;
    spSkeleton* skeleton;
    Mesh* mesh;

	Skeleton(SkeletonData* skeletonData);
	virtual ~Skeleton();

    void updateMesh();

	// Implements Drawable.
	void draw(love::graphics::Graphics *gfx, const Matrix4 &m) override;
}; // Skeleton

class Bone : public Object
{
public:
	static love::Type type;
    spBone* bone;
	Bone(spBone* bone): bone(bone) {};
}; // Bone

class TrackEntry : public Object
{
public:
	static love::Type type;
    spTrackEntry* trackEntry;
	TrackEntry(spTrackEntry* trackEntry): trackEntry(trackEntry) {};
}; // TrackEntry

class Animation : public Object
{
public:
	static love::Type type;
    spAnimation* anim;
	Animation(spAnimation* anim): anim(anim) {};
}; // Animation

class Slot : public Object
{
public:
	static love::Type type;
    spSlot* slot;
	Slot(spSlot* slot): slot(slot) {};
}; // Animation

} // graphics
} // love
