#pragma once

// LOVE
#include "common/config.h"
#include "common/Object.h"
#include "Drawable.h"

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

	SkeletonData(std::string& atlasPath, std::string& jsonPath);
	virtual ~SkeletonData();
}; // SkeletonData

class StateData : public Object
{
public:
	static love::Type type;

    spAnimationStateData* stateData;

	StateData(const SkeletonData* skeletonData);
	virtual ~StateData();
}; // StateData

class State : public Object
{
public:
	static love::Type type;

    spAnimationState* state;

	State(const StateData* stateData);
	virtual ~State();
}; // State

class Skeleton : public Drawable
{
public:
	static love::Type type;
    spSkeleton* skeleton;

	Skeleton(const SkeletonData* skeletonData);
	virtual ~Skeleton();

	// Implements Drawable.
	void draw(love::graphics::Graphics *gfx, const Matrix4 &m) override;
}; // Skeleton

} // graphics
} // love
