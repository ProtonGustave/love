// I'm using classes here so this objects can be passed accross threads

#include "Spine.h"
#include "Graphics.h"

void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
    using namespace love;

    // if (self->magFilter == SP_ATLAS_LINEAR) texture->setSmooth(true);
    // if (self->uWrap == SP_ATLAS_REPEAT && self->vWrap == SP_ATLAS_REPEAT) texture->setRepeated(true);

    // self->rendererObject = texture;
    // Vector2u size = texture->getSize();
    // self->width = size.x;
    // self->height = size.y;
    // if (imagemodule == nullptr)
    //     luaL_error(L, "Cannot load images without the love.image module.");

    auto imagemodule = Module::getInstance<image::Image>(Module::M_IMAGE);
	auto fs = Module::getInstance<filesystem::Filesystem>(Module::M_FILESYSTEM);
    auto gfx = Module::getInstance<graphics::Graphics>(Module::M_GRAPHICS);

    StrongRef<image::ImageData> idata;
    graphics::Image::Slices slices(graphics::TEXTURE_2D);
    filesystem::File* file = fs->newFile(path);
    Data* data = file->read();
    file->release();
    idata.set(imagemodule->newImageData(data), Acquire::NORETAIN);
    slices.set(0, 0, idata);
    graphics::Image::Settings settings;
    graphics::Image* img = gfx->newImage(slices, settings);
	// img->setFilter(filter);
    self->rendererObject = img;
    self->width = img->getWidth();
    self->height = img->getHeight();
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
    // TODO: delete this
    // (love::graphics::Image) self->rendererObject;
}

// TODO: maybe use local function for file reading
char *_spUtil_readFile(const char *path, int *length) {
    return _spReadFile(path, length);
}

namespace love
{
namespace graphics
{

// SkeletonData
love::Type SkeletonData::type("SkeletonData", &Object::type);

SkeletonData::SkeletonData(const char* atlasPath, const char* jsonPath)
{
    // Load the atlas from a file. The last argument is a void* that will be 
    // stored in atlas->rendererObject.
    spAtlas* atlas = spAtlas_createFromFile(atlasPath, 0);

    // load json from file
    spSkeletonJson* json = spSkeletonJson_create(atlas);
    skeletonData = spSkeletonJson_readSkeletonDataFile(json, jsonPath);

    // If loading failed, print the error
    if (!skeletonData) {
        printf("%s\n", json->error);
        spSkeletonJson_dispose(json);
        // TODO: how to throw error to lua?
    }

    // Dispose the spSkeletonJson as we no longer need it after loading.
    spSkeletonJson_dispose(json);
}

SkeletonData::~SkeletonData()
{
    spSkeletonData_dispose(skeletonData);
    spAtlas_dispose(atlas);
}

// StateData
love::Type StateData::type("StateData", &Object::type);

StateData::StateData(const SkeletonData* skeletonData)
{
    stateData = spAnimationStateData_create(skeletonData->skeletonData);
}

StateData::~StateData()
{
    spAnimationStateData_dispose(stateData);
}

// State
love::Type State::type("State", &Object::type);

State::State(const StateData* stateData)
{
    state = spAnimationState_create(stateData->stateData);
}

State::~State()
{
    spAnimationState_dispose(state);
}

// Skeleton
love::Type Skeleton::type("Skeleton", &Drawable::type);

Skeleton::Skeleton(const SkeletonData* skeletonData)
{
    skeleton = spSkeleton_create(skeletonData->skeletonData);
}

Skeleton::~Skeleton()
{
    spSkeleton_dispose(skeleton);
}

// TODO DRAW SKELETON
// void Text::draw(Graphics *gfx, const Matrix4 &m)
// {
// 	if (vertex_buffer == nullptr || draw_commands.empty())
// 		return;
//
// 	gfx->flushStreamDraws();
//
// 	if (Shader::isDefaultActive())
// 		Shader::attachDefault(Shader::STANDARD_DEFAULT);
//
// 	if (Shader::current)
// 		Shader::current->checkMainTextureType(TEXTURE_2D, false);
//
// 	// Re-generate the text if the Font's texture cache was invalidated.
// 	if (font->getTextureCacheID() != texture_cache_id)
// 		regenerateVertices();
//
// 	int totalverts = 0;
// 	for (const Font::DrawCommand &cmd : draw_commands)
// 		totalverts = std::max(cmd.startvertex + cmd.vertexcount, totalverts);
//
// 	vertex_buffer->unmap(); // Make sure all pending data is flushed to the GPU.
//
// 	Graphics::TempTransform transform(gfx, m);
//
// 	for (const Font::DrawCommand &cmd : draw_commands)
// 		gfx->drawQuads(cmd.startvertex / 4, cmd.vertexcount / 4, vertexAttributes, vertexBuffers, cmd.texture);
// }

} // graphics
} // love
