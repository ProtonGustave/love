// I'm using classes here so this objects can be passed accross threads

#include "Spine.h"
#include "Graphics.h"

float worldVertices[1000];

void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
    love::graphics::Image* img = (love::graphics::Image*)self->atlas->rendererObject;
    self->rendererObject = img;
    self->width = img->getWidth();
    self->height = img->getHeight();
}

// void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
//     // TODO: ideally images should be loaded from Lua side, JSON and ATLAS TOO
//     using namespace love;
//
//     // if (self->magFilter == SP_ATLAS_LINEAR) texture->setSmooth(true);
//     // if (self->uWrap == SP_ATLAS_REPEAT && self->vWrap == SP_ATLAS_REPEAT) texture->setRepeated(true);
//
//     // self->rendererObject = texture;
//     // Vector2u size = texture->getSize();
//     // self->width = size.x;
//     // self->height = size.y;
//     // if (imagemodule == nullptr)
//     //     luaL_error(L, "Cannot load images without the love.image module.");
//
//     auto imagemodule = Module::getInstance<image::Image>(Module::M_IMAGE);
// 	auto fs = Module::getInstance<filesystem::Filesystem>(Module::M_FILESYSTEM);
//     auto gfx = Module::getInstance<graphics::Graphics>(Module::M_GRAPHICS);
//
//     StrongRef<image::ImageData> idata;
//     graphics::Image::Slices slices(graphics::TEXTURE_2D);
//     filesystem::File* file = fs->newFile(path);
//     Data* data = file->read();
//     file->release();
//     idata.set(imagemodule->newImageData(data), Acquire::NORETAIN);
//     slices.set(0, 0, idata);
//     graphics::Image::Settings settings;
//     graphics::Image* img = gfx->newImage(slices, settings);
// 	// img->setFilter(filter);
//     self->rendererObject = img;
//     self->width = img->getWidth();
//     self->height = img->getHeight();
//
//     printf("IMAGE LOADED %d %d\n", self->width, self->height);
// }

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
    love::graphics::Image* img = (love::graphics::Image*)self->rendererObject;
    img->release();
}

// not needed to read files here
char *_spUtil_readFile(const char *path, int *length) {
    // NO-OP
    return 0;
    // using namespace love;
    //
    // // TODO: make sure this stays alive
	// auto fs = Module::getInstance<filesystem::Filesystem>(Module::M_FILESYSTEM);
    // printf("OPNE ATLAS %s\n", path);
    // filesystem::File* file = fs->newFile(path);
    // Data* data = file->read();
    // *length = data->getSize();
    // return (char*)data->getData();
}

namespace love
{
namespace graphics
{

// SkeletonData
love::Type SkeletonData::type("SkeletonData", &Object::type);

SkeletonData::SkeletonData(love::filesystem::FileData* atlasData, love::filesystem::FileData* jsonData, Image* img)
{
    atlas = spAtlas_create((char *) atlasData->getData(), atlasData->getSize(), "", img);
    spSkeletonJson* json = spSkeletonJson_create(atlas);
	skeletonData = spSkeletonJson_readSkeletonData(json, (char *)jsonData->getData());

    // If loading failed, print the error
    if (!skeletonData) {
        spSkeletonJson_dispose(json);
        spAtlas_dispose(atlas);
        throw love::Exception("Couldn't read skeleton data file, error: %s", json->error);
    }

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
    auto gfx = Module::getInstance<Graphics>(Module::M_GRAPHICS);
    skeleton = spSkeleton_create(skeletonData->skeletonData);
    // skeleton->scaleX = 0.2;
    // skeleton->scaleY = -0.2;
    // skeleton->x = 500;
    // skeleton->y = 500;

    // TODO: vertex num should be bonesCount * 4
    mesh = gfx->newMesh(1000, PRIMITIVE_TRIANGLES, vertex::USAGE_STREAM);
}

Skeleton::~Skeleton()
{
    spSkeleton_dispose(skeleton);
}

void Skeleton::draw(Graphics *gfx, const Matrix4 &m)
{
    mesh->draw(gfx, m);
}

// draw skeleton
void Skeleton::updateMesh()
{
    unsigned short quadIndices[6] = {0, 1, 2, 2, 3, 0};

    // Early out if skeleton is invisible
    if (skeleton->color.a == 0) return;

    Vertex vertex;
    Image *texture = 0;
    int totalVerticesNum = 0;

    for (int i = 0; i < skeleton->slotsCount; ++i) {
        spSlot *slot = skeleton->drawOrder[i];
        spAttachment *attachment = slot->attachment;
        if (!attachment) continue;

        // Early out if slot is invisible
        if (slot->color.a == 0 || !slot->bone->active) {
            continue;
        }

        float *vertices = worldVertices;
        int verticesCount = 0;
        float *uvs = 0;
        unsigned short *indices = 0;
        int indicesCount = 0;
        spColor *attachmentColor;

        if (attachment->type == SP_ATTACHMENT_REGION) {
            spRegionAttachment *regionAttachment = (spRegionAttachment *) attachment;
            attachmentColor = &regionAttachment->color;

            // Early out if slot is invisible
            if (attachmentColor->a == 0) {
                continue;
            }

            spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, vertices, 0, 2);
            verticesCount = 4;
            uvs = regionAttachment->uvs;
            indices = quadIndices;
            indicesCount = 6;
            texture = (Image *) ((spAtlasRegion *) regionAttachment->rendererObject)->page->rendererObject;
        } else if (attachment->type == SP_ATTACHMENT_MESH) {
            spMeshAttachment *mesh = (spMeshAttachment *) attachment;
            attachmentColor = &mesh->color;

            // Early out if slot is invisible
            if (attachmentColor->a == 0) {
                continue;
            }

            texture = (Image *) ((spAtlasRegion *) mesh->rendererObject)->page->rendererObject;
            spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, worldVertices, 0, 2);
            verticesCount = mesh->super.worldVerticesLength >> 1;
            uvs = mesh->uvs;
            indices = mesh->triangles;
            indicesCount = mesh->trianglesCount;
        } else {
            continue;
        }

        uint8 r = static_cast<uint8>(skeleton->color.r * slot->color.r * attachmentColor->r * 255);
        uint8 g = static_cast<uint8>(skeleton->color.g * slot->color.g * attachmentColor->g * 255);
        uint8 b = static_cast<uint8>(skeleton->color.b * slot->color.b * attachmentColor->b * 255);
        uint8 a = static_cast<uint8>(skeleton->color.a * slot->color.a * attachmentColor->a * 255);
        vertex.color.r = r;
        vertex.color.g = g;
        vertex.color.b = b;
        vertex.color.a = a;

        // spColor light;
        // light.r = r / 255.0f;
        // light.g = g / 255.0f;
        // light.b = b / 255.0f;
        // light.a = a / 255.0f;

        // TODO: blend
        // sf::BlendMode blend;
        // if (!usePremultipliedAlpha) {
        //     switch (slot->data->blendMode) {
        //         case SP_BLEND_MODE_NORMAL:
        //             blend = normal;
        //             break;
        //         case SP_BLEND_MODE_ADDITIVE:
        //             blend = additive;
        //             break;
        //         case SP_BLEND_MODE_MULTIPLY:
        //             blend = multiply;
        //             break;
        //         case SP_BLEND_MODE_SCREEN:
        //             blend = screen;
        //             break;
        //         default:
        //             blend = normal;
        //     }
        // } else {
        //     switch (slot->data->blendMode) {
        //         case SP_BLEND_MODE_NORMAL:
        //             blend = normalPma;
        //             break;
        //         case SP_BLEND_MODE_ADDITIVE:
        //             blend = additivePma;
        //             break;
        //         case SP_BLEND_MODE_MULTIPLY:
        //             blend = multiplyPma;
        //             break;
        //         case SP_BLEND_MODE_SCREEN:
        //             blend = screenPma;
        //             break;
        //         default:
        //             blend = normalPma;
        //     }
        // }

        // TODO: different blend mode or texture render
        // if (states.texture == 0) states.texture = texture;
        //
        // if (states.blendMode != blend || states.texture != texture) {
        //     target.draw(*vertexArray, states);
        //     vertexArray->clear();
        //     states.blendMode = blend;
        //     states.texture = texture;
        // }

        int width = texture->getWidth();
        int height = texture->getHeight();

        for (int j = 0; j < indicesCount; ++j) {
            int index = indices[j] << 1;
            vertex.x = vertices[index];
            vertex.y = vertices[index + 1];
            vertex.s = uvs[index];
            vertex.t = uvs[index + 1];
            mesh->setVertex(totalVerticesNum, &vertex, sizeof(vertex));
            totalVerticesNum += 1;
        }
    }

    // TODO: move "mesh->" operations to draw?

    mesh->setDrawRange(0, totalVerticesNum);
    mesh->setTexture(texture);
}

// Bone
love::Type Bone::type("Bone", &Object::type);

// TrackEntry
love::Type TrackEntry::type("TrackEntry", &Object::type);

// Animation
love::Type Animation::type("Animation", &Object::type);

} // graphics
} // love
