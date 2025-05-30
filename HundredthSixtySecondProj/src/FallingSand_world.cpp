// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#include "FallingSand_world.hpp"

#include <algorithm>
#include <cstdio>
#include <future>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>
#include <typeinfo>
#include <vector>

#include "FallingSand_chunk.hpp"
#include "FallingSand_base_debug.hpp"
#include "FallingSand_base_memory.h"
#include "FallingSand_const.h"
#include "FallingSand_core.hpp"
#include "FallingSand_global.hpp"
#include "FallingSand_filesystem.h"
#include "FallingSand_macros.hpp"
#include "FallingSand_mathlib.hpp"
#include "FallingSand_engine.hpp"
#include "FallingSand_cells.h"
#include "FallingSand_jsonwarp.h"
#include "FallingSand_lua_wrapper.hpp"
#include "FallingSand_scripting.hpp"
#include "FallingSand_utility.hpp"
#include "FallingSand_game_datastruct.hpp"
#include "FallingSand_npc.hpp"
#include "FallingSand_reflectionflat.hpp"
#include "FallingSand_textures.hpp"
#include "FallingSand_world_generator.h"

namespace ME {

std::mutex g_mutex_loadchunk;
std::mutex g_mutex_updatechunkmesh;

void world::init(std::string worldPath, u16 w, u16 h, R_Target *target, Audio *audioEngine) { init(worldPath, w, h, target, audioEngine, new MaterialTestGenerator()); }

void world::init(std::string worldPath, u16 w, u16 h, R_Target *target, Audio *audioEngine, WorldGenerator *generator) {

    this->worldName = worldPath;

    if (!noSaveLoad) {
        std::filesystem::create_directories(worldPath);
        std::filesystem::create_directories(worldPath + "/chunks");
    }

    metadata = WorldMeta::loadWorldMeta(this->worldName, noSaveLoad);

    width = w;
    height = h;

    METADOT_INFO("World size: {0}x{1}={2}"_f(w, h, w * h).c_str());

    world_sys.tickPool = create_scope<thread_pool>(16);
    world_sys.tickVisitedPool = create_scope<thread_pool>(16);
    world_sys.updateRigidBodyHitboxPool = create_scope<thread_pool>(16);

    this->audioEngine = audioEngine;

    newTemps = new i32[width * height];

    gen = generator;

    populators = gen->getPopulators();

    hasPopulator = new bool[6];
    for (int i = 0; i < 6; i++) hasPopulator[i] = false;
    for (int i = 0; i < populators.size(); i++) {
        hasPopulator[populators[i]->getPhase()] = true;
        if (populators[i]->getPhase() > highestPopulator) highestPopulator = populators[i]->getPhase();
    }

    this->target = target;
    loadZone = {0, 0, (float)w, (float)h};

    noise.SetSeed(RNG_Next(global.game->RNG));
    noise.SetNoiseType(FastNoise::Perlin);

    auto ha = phmap::flat_hash_map<int, phmap::flat_hash_map<int, Chunk *>>();
    // ha.set_deleted_key(INT_MAX);
    // ha.set_empty_key(INT_MIN);
    chunkCache = ha;

    f32 distributedPointsDistance = 0.05f;
    for (int i = 0; i < (1 / distributedPointsDistance) * (1 / distributedPointsDistance); i++) {
        f32 x = rand() % 1000 / 1000.0;
        f32 y = rand() % 1000 / 1000.0;

        for (int j = 0; j < distributedPoints.size(); j++) {
            f32 dx = distributedPoints[j].x - x;
            f32 dy = distributedPoints[j].y - y;
            if (dx * dx + dy * dy < distributedPointsDistance * distributedPointsDistance) {
                goto tooClose;
            }
        }

        distributedPoints.emplace_back(x, y);
    tooClose : {}
    }

    rigidBodies.reserve(1);

    // 重置为世界大小

    dirty = new bool[width * height];
    layer2Dirty = new bool[width * height];
    backgroundDirty = new bool[width * height];
    lastActive = new bool[width * height];
    active = new bool[width * height];
    this->tickVisited1 = new bool[width * height];
    this->tickVisited2 = new bool[width * height];
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            dirty[x + y * width] = false;
            layer2Dirty[x + y * width] = false;
            lastActive[x + y * width] = true;
            active[x + y * width] = false;
            backgroundDirty[x + y * width] = false;
        }
    }

    real_tiles.resize(width * height);
    flowX = new f32[width * height];
    flowY = new f32[width * height];
    prevFlowX = new f32[width * height];
    prevFlowY = new f32[width * height];
    real_layer2.resize(width * height);
    background.resize(width * height);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            setTile(x, y, Tiles_NOTHING);
            real_layer2[x + y * width] = Tiles_NOTHING;
            background[x + y * width] = 0x00000000;
            flowX[x + y * width] = 0;
            flowY[x + y * width] = 0;
            prevFlowX[x + y * width] = 0;
            prevFlowY[x + y * width] = 0;
            // cells.push_back(new CellData(x, y, 0, 0, 0, 0.1, 0xffff00));
        }
    }

    gravity = b2Vec2(0, 20);
    b2world = ME::create_scope<b2World>(gravity);

    struct gameplay_feature {};
    registry.assign_feature<gameplay_feature>().add_system<ControableSystem>().add_system<NpcSystem>().add_system<WorldEntitySystem>();

    b2PolygonShape nothingShape;
    nothingShape.SetAsBox(0, 0);
    this->staticBody = makeRigidBody(b2_staticBody, 0, 0, 0, nothingShape, 0, 0, global.game->Iso.texturepack.cloud);

    updateWorldMesh();

    b2PolygonShape dynamicBox3;
    dynamicBox3.SetAsBox(10.0f, 2.0f, {10, -10}, 0);
    RigidBody *rb = makeRigidBody(b2_dynamicBody, 300, 300, 0, dynamicBox3, 1, .3, LoadTexture("data/assets/objects/testObject3.png"));

    rigidBodies.push_back(rb);
    updateRigidBodyHitbox(rb);

    // b2PolygonShape dynamicBox4;
    // dynamicBox4.SetAsBox(64.0f, 64.0f, {32, -32}, 0);
    // RigidBody *rb2 = makeRigidBody(b2_dynamicBody, 400, 0, 0, dynamicBox4, 1, .3, LoadAsepriteTexture("data/assets/textures/Sprite-0003.ase"));

    // rigidBodies.push_back(rb2);
    // updateRigidBodyHitbox(rb2);
}

RigidBody *world::makeRigidBody(b2BodyType type, f32 x, f32 y, f32 angle, b2PolygonShape shape, f32 density, f32 friction, TextureRef texture) {

    b2BodyDef bodyDef;
    bodyDef.type = type;
    bodyDef.position.Set(x, y);
    bodyDef.angle = angle * PI / 180;
    b2Body *body = b2world->CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;

    body->CreateFixture(&fixtureDef);

    RigidBody *rb = new RigidBody(body);
    rb->setTexture(texture);
    if (texture != NULL) {
        rb->matWidth = rb->get_surface()->w;
        rb->matHeight = rb->get_surface()->h;
        rb->tiles = new MaterialInstance[rb->matWidth * rb->matHeight];
        for (int xx = 0; xx < rb->matWidth; xx++) {
            for (int yy = 0; yy < rb->matHeight; yy++) {
                u32 pixel = ME_get_pixel(rb->get_surface(), xx, yy);
                if (((pixel >> 24) & 0xff) != 0x00) {
                    MaterialInstance inst = TilesCreate((rand() % 250 == -1) ? GAME()->materials_list.FIRE.id : GAME()->materials_list.OBSIDIAN.id, xx + (int)x, yy + (int)y);
                    inst.color = pixel;
                    rb->tiles[xx + yy * rb->matWidth] = inst;
                } else {
                    MaterialInstance inst = TilesCreate(GAME()->materials_list.GENERIC_AIR.id, xx + (int)x, yy + (int)y);
                    rb->tiles[xx + yy * rb->matWidth] = inst;
                }
            }
        }

        // for (int x = 0; x < rb->surface->w; x++) {
        //     for (int y = 0; y < rb->surface->h; y++) {
        //         MaterialInstance mat = rb->tiles[x + y * rb->surface->w];
        //         if (mat.mat->id == GAME()->materials_list.GENERIC_AIR.id) {
        //             ME_get_pixel(rb->surface, x, y) = 0x00000000;
        //         } else {
        //             ME_get_pixel(rb->surface, x, y) = (mat.mat->alpha << 24) + mat.color;
        //         }
        //     }
        // }

        // rb->set_texture(R_CopyImageFromSurface(rb->get_surface()));
        // R_SetImageFilter(rb->get_texture(), R_FILTER_NEAREST);
        rb->updateImage({});
    }
    // rigidBodies.push_back(rb);
    return rb;
}

RigidBody *world::makeRigidBodyMulti(b2BodyType type, f32 x, f32 y, f32 angle, std::vector<b2PolygonShape> shape, f32 density, f32 friction, TextureRef texture) {

#if 1

    b2BodyDef bodyDef;
    bodyDef.type = type;
    bodyDef.position.Set(x, y);
    bodyDef.angle = angle * PI / 180;
    b2Body *body = b2world->CreateBody(&bodyDef);

    for (int i = 0; i < shape.size(); i++) {
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape[i];
        fixtureDef.density = density;
        fixtureDef.friction = friction;

        body->CreateFixture(&fixtureDef);
    }

    RigidBody *rb = new RigidBody(body);
    rb->setTexture(texture);
    if (texture != NULL) {
        rb->matWidth = rb->get_surface()->w;
        rb->matHeight = rb->get_surface()->h;
        rb->tiles = new MaterialInstance[rb->matWidth * rb->matHeight];
        for (int xx = 0; xx < rb->matWidth; xx++) {
            for (int yy = 0; yy < rb->matHeight; yy++) {
                u32 pixel = ME_get_pixel(rb->get_surface(), xx, yy);
                if (((pixel >> 24) & 0xff) != 0x00) {
                    MaterialInstance inst = TilesCreate(rand() % 250 == -1 ? GAME()->materials_list.FIRE.id : GAME()->materials_list.OBSIDIAN.id, xx + (int)x, yy + (int)y);
                    inst.color = pixel;
                    rb->tiles[xx + yy * rb->matWidth] = inst;
                } else {
                    MaterialInstance inst = TilesCreate(GAME()->materials_list.GENERIC_AIR.id, xx + (int)x, yy + (int)y);
                    rb->tiles[xx + yy * rb->matWidth] = inst;
                }
            }
        }

        /*for (int x = 0; x < rb->surface->w; x++) {
            for (int y = 0; y < rb->surface->h; y++) {
                MaterialInstance mat = rb->tiles[x + y * rb->surface->w];
                if (mat.mat->id == Materials::GENERIC_AIR.id) {
                    ME_get_pixel(rb->surface, x, y) = 0x00000000;
                } else {
                    ME_get_pixel(rb->surface, x, y) = (mat.mat->alpha << 24) + mat.color;
                }
            }
        }*/

        // rb->set_texture(R_CopyImageFromSurface(rb->get_surface()));
        // R_SetImageFilter(rb->get_texture(), R_FILTER_NEAREST);

        rb->updateImage({});
    }
    // rigidBodies.push_back(rb);
    return rb;
#endif
}

void world::updateRigidBodyHitbox(RigidBody *rb) {

    C_Surface *sfc = rb->get_surface();

    if (static_cast<bool>(sfc)) return;

    for (int x = 0; x < sfc->w; x++) {
        for (int y = 0; y < sfc->h; y++) {
            MaterialInstance mat = rb->tiles[x + y * sfc->w];
            if (mat.mat->id == GAME()->materials_list.GENERIC_AIR.id) {
                ME_get_pixel(sfc, x, y) = 0x00000000;
            } else {
                ME_get_pixel(sfc, x, y) = (mat.mat->alpha << 24) + (mat.color & 0x00ffffff);
            }
        }
    }

    int minX = sfc->w;
    int maxX = 0;
    int minY = sfc->h;
    int maxY = 0;
    for (int x = 0; x < sfc->w; x++) {
        for (int y = 0; y < sfc->h; y++) {
            if (((ME_get_pixel(sfc, x, y) >> 24) & 0xff) != 0x00) {
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
        }
    }
    maxX++;
    maxY++;

    if (static_cast<bool>(sfc)) {
        // TODO: 23/7/15 修复
        C_Surface *sf = SDL_CreateRGBSurfaceWithFormat(sfc->flags, maxX - minX, maxY - minY, sfc->format->BitsPerPixel, sfc->format->format);
        C_Rect src = {minX, minY, maxX - minX, maxY - minY};
        SDL_SetSurfaceBlendMode(sfc, SDL_BlendMode::SDL_BLENDMODE_NONE);
        SDL_BlitSurface(sfc, &src, sf, NULL);
        SDL_FreeSurface(sfc);

        if (static_cast<bool>(rb->get_surface())) {
            if (rb->get_surface()->w <= 1 || rb->get_surface()->h <= 1) {
                b2world->DestroyBody(rb->body);
                rigidBodies.erase(std::remove(rigidBodies.begin(), rigidBodies.end(), rb), rigidBodies.end());
                return;
            }

            // rb->set_surface(sf);

            auto tex = create_ref<Texture>(sf);
            rb->setTexture(tex);
            sfc = rb->get_surface();

        } else {
            ME_ASSERT(0);
        }
    } else {
        return;
    }

    f32 s = sin(rb->body->GetAngle());
    f32 c = cos(rb->body->GetAngle());

    // translate point back to origin:
    minX -= 0;
    minY -= 0;

    // rotate point
    f32 xnew = minX * c - minY * s;
    f32 ynew = minX * s + minY * c;

    // translate point back:
    rb->body->SetTransform(b2Vec2(rb->body->GetPosition().x + xnew, rb->body->GetPosition().y + ynew), rb->body->GetAngle());

    // If it is a single pixel rigid body, it will be deconstructed.
    if (maxX == 1 || maxY == 1) return;

    if (!static_cast<bool>(sfc) && !static_cast<bool>(rb->get_surface()) &&
        // !static_cast<bool>(xnew) &&
        // !static_cast<bool>(ynew) &&
        1)
        return;

    bool foundAnything = false;
    for (int x = 0; x < sfc->w; x++)
        for (int y = 0; y < sfc->h; y++) {
            bool f = ((ME_get_pixel(sfc, x, y) >> 24) & 0xff) == 0x00 ? 0 : 1;
            foundAnything = foundAnything || f;
        }

    if (!foundAnything) return;

    u8 *data = new u8[sfc->w * sfc->h];
    bool *edgeSeen = new bool[sfc->w * sfc->h];

    for (int y = 0; y < sfc->h; y++) {
        for (int x = 0; x < sfc->w; x++) {
            data[x + y * sfc->w] = ((ME_get_pixel(sfc, x, y) >> 24) & 0xff) == 0x00 ? 0 : 1;
            edgeSeen[x + y * sfc->w] = false;
        }
    }

    std::vector<std::vector<MEvec2>> meshes = {};

    std::list<TPPLPoly> shapes;
    std::list<MarchingSquares::Result> results;
    int inn = 0;
    int lookIndex = 0;

    while (true) {
        inn++;

        int lookX = lookIndex % sfc->w;
        int lookY = lookIndex / sfc->w;
        if (inn == 1) {
            lookX = sfc->w / 2;
            lookY = sfc->h / 2;
        }

        int edgeX = -1;
        int edgeY = -1;
        int size = sfc->w * sfc->h;
        for (int i = lookIndex; i < size; i++) {
            if (data[i] != 0) {

                int numBorders = 0;
                // if (i % texture->w - 1 >= 0) numBorders += data[(i % texture->w - 1) + i / texture->w * texture->w];
                // if (i / texture->w - 1 >= 0) numBorders += data[(i % texture->w)+(i / texture->w - 1) * texture->w];
                if (i % sfc->w + 1 < sfc->w) numBorders += data[(i % sfc->w + 1) + i / sfc->w * sfc->w];
                if (i / sfc->w + 1 < sfc->h) numBorders += data[(i % sfc->w) + (i / sfc->w + 1) * sfc->w];
                if (i / sfc->w + 1 < sfc->h && i % sfc->w + 1 < sfc->w) numBorders += data[(i % sfc->w + 1) + (i / sfc->w + 1) * sfc->w];

                // int val = value(i % texture->w, i / texture->w, texture->w, height, data);
                if (numBorders != 3) {
                    edgeX = i % sfc->w;
                    edgeY = i / sfc->w;
                    break;
                }
            }
        }

        if (edgeX == -1) {
            break;
        }

        // MarchingSquares::Direction edge = MarchingSquares::FindEdge(texture->w, texture->h, data, lookX, lookY);

        lookX = edgeX;
        lookY = edgeY;

        lookIndex = lookX + lookY * sfc->w + 1;

        if (edgeSeen[lookX + lookY * sfc->w]) {
            inn--;
            continue;
        }

        int val = MarchingSquares::value(lookX, lookY, sfc->w, sfc->h, data);
        if (val == 0 || val == 15) {
            inn--;
            continue;
        }

        MarchingSquares::Result r = MarchingSquares::FindPerimeter(lookX, lookY, sfc->w, sfc->h, data);
        results.push_back(r);

        std::vector<MEvec2> worldMesh;

        f32 lastX = (f32)r.initialX;
        f32 lastY = (f32)r.initialY;
        for (int i = 0; i < r.directions.size(); i++) {
            // if(r.directions[i].x != 0) r.directions[i].x = r.directions[i].x / abs(r.directions[i].x);
            // if(r.directions[i].y != 0) r.directions[i].y = r.directions[i].y / abs(r.directions[i].y);

            for (int ix = 0; ix < std::max(abs(r.directions[i].x), 1); ix++) {
                for (int iy = 0; iy < std::max(abs(r.directions[i].y), 1); iy++) {
                    int ilx = (int)(lastX + ix * (r.directions[i].x < 0 ? -1 : 1));
                    int ily = (int)(lastY - iy * (r.directions[i].y < 0 ? -1 : 1));

                    if (ilx < 0) ilx = 0;
                    if (ilx >= sfc->w) ilx = sfc->w - 1;

                    if (ily < 0) ily = 0;
                    if (ily >= sfc->h) ily = sfc->h - 1;

                    int ind = ilx + ily * sfc->w;
                    if (ind >= size) {
                        continue;
                    }
                    edgeSeen[ind] = true;
                }
            }

            lastX += (f32)r.directions[i].x;
            lastY -= (f32)r.directions[i].y;
            worldMesh.emplace_back(lastX, lastY);
        }

        worldMesh = simplify(worldMesh, 1);

        if (worldMesh.size() < 3) {
            // 1x1 that breaks everything
            continue;
        }

        meshes.push_back(worldMesh);

        TPPLPoly poly;
        poly.Init((long)worldMesh.size());

        for (int i = 0; i < worldMesh.size(); i++) {
            poly[(int)worldMesh.size() - i - 1] = {worldMesh[i].x, worldMesh[i].y};
        }

        if (poly.GetOrientation() == TPPL_CW) {
            poly.SetHole(true);
        }

        if (poly.GetNumPoints() > 2) shapes.push_back(poly);
    }

    delete[] edgeSeen;
    delete[] data;
    std::list<TPPLPoly> result2;

    TPPLPartition part;
    TPPLPartition part2;

    part.RemoveHoles(&shapes, &result2);

    polys2s.clear();

    std::vector<C_Surface *> polys2sSfcs = {};
    std::vector<bool> polys2sWeld = {};
    for (auto it = result2.begin(); it != result2.end(); it++) {
        std::list<TPPLPoly> result;

        std::list<TPPLPoly> l = {*it};
        part2.Triangulate_EC(&l, &result);

        /*bool* solid = new bool[10 * 10];
        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                solid[x + y * width] = rand() % 2 == 0;
            }
        }*/

        // Ps::MarchingSquares ms = Ps::MarchingSquares(solid, texture->w, texture->h);

        // Ps::MarchingSquares ms = Ps::MarchingSquares(texture);
        // worldMesh = ms.extract_simple(2);

        std::vector<b2PolygonShape> polys2;

        int n = 0;
        std::for_each(result.begin(), result.end(), [&](TPPLPoly cur) {
            if ((cur[0].x == cur[1].x && cur[1].x == cur[2].x) || (cur[0].y == cur[1].y && cur[1].y == cur[2].y)) return;

            b2Vec2 vec[3] = {{(f32)cur[0].x, (f32)cur[0].y}, {(f32)cur[1].x, (f32)cur[1].y}, {(f32)cur[2].x, (f32)cur[2].y}};
            b2PolygonShape sh;
            sh.Set(vec, 3);

            polys2.push_back(sh);
            n++;
        });

        if (polys2.size() > 0) {
            polys2s.push_back(polys2);

            polys2sSfcs.push_back(SDL_CreateRGBSurfaceWithFormat(sfc->flags, sfc->w, sfc->h, sfc->format->BitsPerPixel, sfc->format->format));

            polys2sWeld.push_back(false);
        }
    }

    if (polys2s.size() > 0) {
        std::vector<std::future<void>> poolResults = {};

        if (sfc->w > 10) {
            int nThreads = world_sys.updateRigidBodyHitboxPool->n_idle();
            int div = sfc->w / nThreads;
            int rem = sfc->w % nThreads;

            for (int thr = 0; thr < nThreads; thr++) {
                poolResults.push_back(world_sys.updateRigidBodyHitboxPool->push([&, thr, div, rem](int id) {
                    int stx = thr * div;
                    int enx = stx + div + (thr == nThreads - 1 ? rem : 0);

                    for (int x = stx; x < enx; x++) {
                        for (int y = 0; y < sfc->h; y++) {
                            if (((ME_get_pixel(sfc, x, y) >> 24) & 0xff) == 0x00) continue;

                            int nb = 0;

                            int nearestDist = 100000;

                            // for each body
                            for (int b = 0; b < polys2s.size(); b++) {
                                // for each triangle in the mesh
                                for (int i = 0; i < polys2s[b].size(); i++) {
#if 0
                                    // 确保是计算对象为三角形
                                    if (((phy::Polygon *)polys2s[b][i])->vertices().size() != 3) continue;

                                    // 动态质心计算 需要考虑优化方案
                                    auto centroid = phy::GeometryAlgorithm2D::triangleCentroid(((phy::Polygon *)polys2s[b][i])->vertices()[0], ((phy::Polygon *)polys2s[b][i])->vertices()[1],
                                                                                               ((phy::Polygon *)polys2s[b][i])->vertices()[2]);
                                    int dst = abs(x - centroid.x) + abs(y - centroid.y);
#else
                                    int dst = abs(x - polys2s[b][i].m_centroid.x) + abs(y - polys2s[b][i].m_centroid.y);
#endif

                                    if (dst < nearestDist) {
                                        nearestDist = dst;
                                        nb = b;
                                    }
                                }
                            }

                            ME_get_pixel(polys2sSfcs[nb], x, y) = ME_get_pixel(sfc, x, y);
                            if (x == rb->weldX && y == rb->weldY) polys2sWeld[nb] = true;
                        }
                    }
                }));
            }

        } else {
            for (int x = 0; x < sfc->w; x++) {
                for (int y = 0; y < sfc->h; y++) {
                    if (((ME_get_pixel(sfc, x, y) >> 24) & 0xff) == 0x00) continue;

                    int nb = 0;

                    int nearestDist = 100000;

                    // for each body
                    for (int b = 0; b < polys2s.size(); b++) {
                        // for each triangle in the mesh
                        for (int i = 0; i < polys2s[b].size(); i++) {
#if 0
                            // 确保是计算对象为三角形
                            if (((phy::Polygon *)polys2s[b][i])->vertices().size() != 3) continue;

                            // 动态质心计算 需要考虑优化方案
                            auto centroid = phy::GeometryAlgorithm2D::triangleCentroid(((phy::Polygon *)polys2s[b][i])->vertices()[0], ((phy::Polygon *)polys2s[b][i])->vertices()[1],
                                                                                       ((phy::Polygon *)polys2s[b][i])->vertices()[2]);
                            int dst = abs(x - centroid.x) + abs(y - centroid.y);
#else
                            int dst = abs(x - polys2s[b][i].m_centroid.x) + abs(y - polys2s[b][i].m_centroid.y);
#endif
                            if (dst < nearestDist) {
                                nearestDist = dst;
                                nb = b;
                            }
                        }
                    }

                    ME_get_pixel(polys2sSfcs[nb], x, y) = ME_get_pixel(sfc, x, y);
                    if (x == rb->weldX && y == rb->weldY) polys2sWeld[nb] = true;
                }
            }
        }

        for (int i = 0; i < poolResults.size(); i++) {
            poolResults[i].get();
        }

        for (int b = 0; b < polys2s.size(); b++) {
            std::vector<b2PolygonShape> polys2 = polys2s[b];

            // TODO: 23/7/18 修改 polys2sSfcs 为 Texture
            auto tex = create_ref<Texture>(polys2sSfcs[b]);

            RigidBody *rbn = makeRigidBodyMulti(b2_dynamicBody, 0, 0, rb->body->GetAngle(), polys2, rb->body->GetFixtureList()[0].GetDensity(), rb->body->GetFixtureList()[0].GetFriction(), tex);

            rbn->body->SetTransform(b2Vec2(rb->body->GetPosition().x, rb->body->GetPosition().y), rb->body->GetAngle());
            rbn->body->SetLinearVelocity(rb->body->GetLinearVelocity());
            rbn->body->SetAngularVelocity(rb->body->GetAngularVelocity());

            rbn->outline = shapes;
            rbn->texNeedsUpdate = true;
            rbn->hover = rb->hover;

            bool weld = polys2sWeld[b];
            rbn->back = weld;

            if (weld) {
                b2WeldJointDef weldJ;
                weldJ.bodyA = rbn->body;
                weldJ.bodyB = staticBody->body;
                weldJ.localAnchorA = -rbn->body->GetPosition();
                b2world->CreateJoint(&weldJ);
                rbn->weldX = rb->weldX;
                rbn->weldY = rb->weldY;
                b2Filter bf = {};
                bf.categoryBits = 0x0002;
                bf.maskBits = 0x0000;
                for (b2Fixture *f = rbn->body->GetFixtureList(); f; f = f->GetNext()) {
                    f->SetFilterData(bf);
                }
            } else {
                for (b2Fixture *f = rbn->body->GetFixtureList(); f; f = f->GetNext()) {
                    f->SetFilterData(rb->body->GetFixtureList()[0].GetFilterData());
                }
            }

            rbn->item = rb->item;
            rigidBodies.push_back(rbn);

            if (result2.size() > 1) {
                if (result2.size() == 2 && (result2.front().GetNumPoints() <= 3 || result2.back().GetNumPoints() <= 3)) {
                    // weird edge case that causes infinite recursion
                    // TODO: actually figure out why that happens
                } else {
                    updateRigidBodyHitbox(rbn);
                }
            }
        }
    }

    b2world->DestroyBody(rb->body);

    rigidBodies.erase(std::remove(rigidBodies.begin(), rigidBodies.end(), rb), rigidBodies.end());

    // delete[] rb->tiles;
    // R_FreeImage(rb->get_texture());
    // SDL_FreeSurface(rb->get_surface());
    // delete rb;
    rb->clean();
    delete rb;
}

void world::updateChunkMesh(Chunk *chunk) {

    std::lock_guard<std::mutex> locker(g_mutex_updatechunkmesh);

    /*
    if (chunk->rb != nullptr) b2world->DestroyBody(chunk->rb->body);
    */

    // 区块坐标转化为世界坐标
    int chTx = chunk->x * CHUNK_W + loadZone.x;
    int chTy = chunk->y * CHUNK_H + loadZone.y;

    if (chTx < 0 || chTy < 0 || chTx + CHUNK_W >= width || chTy + CHUNK_H >= height) {
        return;
    }

#pragma region

    bool foundAnything = false;
    for (int x = 0; x < CHUNK_W; x++) {
        for (int y = 0; y < CHUNK_H; y++) {

            Material *mat = real_tiles[(x + chTx) + (y + chTy) * width].mat;
            if (mat != nullptr && mat->physicsType == PhysicsType::SOLID) {
                foundAnything = true;
                goto found;
            }

            // bool f = tiles[(x + meshZone.x) + (y + meshZone.y) * width].mat->physicsType == PhysicsType::SOLID;
            // foundAnything = foundAnything || f;
        }
    }
found : {};

    if (!foundAnything) {
        return;
    }

    unsigned char *data = new unsigned char[CHUNK_W * CHUNK_H];

    bool *edgeSeen = new bool[CHUNK_W * CHUNK_H];

    for (int y = 0; y < CHUNK_H; y++) {
        for (int x = 0; x < CHUNK_W; x++) {
            data[x + y * CHUNK_W] = real_tiles[(x + chTx) + (y + chTy) * width].mat->physicsType == PhysicsType::SOLID;
            edgeSeen[x + y * CHUNK_W] = false;
        }
    }

    std::vector<std::vector<MEvec2>> worldMeshes = {};
    std::list<TPPLPoly> shapes;
    std::list<MarchingSquares::Result> results;
    int inn = 0;
    int lookIndex = 0;

    // don't profile children
    while (true) {
        // inn++;
        int lookX = lookIndex % CHUNK_W;
        int lookY = lookIndex / CHUNK_W;
        /*if (inn == 1) {
            lookX = CHUNK_W / 2;
            lookY = CHUNK_H / 2;
        }*/

        int edgeX = -1;
        int edgeY = -1;
        int size = CHUNK_W * CHUNK_H;

        for (int i = lookIndex; i < size; i++) {
            if (data[i] != 0) {

                int numBorders = 0;
                // if (i % CHUNK_W - 1 >= 0) numBorders += data[(i % CHUNK_W - 1) + i / CHUNK_W * CHUNK_W];
                // if (i / CHUNK_W - 1 >= 0) numBorders += data[(i % CHUNK_W)+(i / CHUNK_W - 1) * CHUNK_W];
                if (i % CHUNK_W + 1 < CHUNK_W) numBorders += data[(i % CHUNK_W + 1) + i / CHUNK_W * CHUNK_W];
                if (i / CHUNK_W + 1 < CHUNK_H) numBorders += data[(i % CHUNK_W) + (i / CHUNK_W + 1) * CHUNK_W];
                if (i / CHUNK_W + 1 < CHUNK_H && i % CHUNK_W + 1 < CHUNK_W) numBorders += data[(i % CHUNK_W + 1) + (i / CHUNK_W + 1) * CHUNK_W];

                // int val = value(i % CHUNK_W, i / CHUNK_W, CHUNK_W, height, data);
                if (numBorders != 3) {
                    edgeX = i % CHUNK_W;
                    edgeY = i / CHUNK_W;
                    break;
                }
            }
        }

        if (edgeX == -1) {
            break;
        }

        // MarchingSquares::Direction edge = MarchingSquares::FindEdge(CHUNK_W, CHUNK_H, data, lookX, lookY);

        lookX = edgeX;
        lookY = edgeY;

        lookIndex = lookX + lookY * CHUNK_W + 1;

        if (edgeSeen[lookX + lookY * CHUNK_W]) {
            inn--;
            continue;
        }

        int val = MarchingSquares::value(lookX, lookY, CHUNK_W, CHUNK_H, data);

        if (val == 0 || val == 15) {
            inn--;
            continue;
        }

        MarchingSquares::Result r = MarchingSquares::FindPerimeter(lookX, lookY, CHUNK_W, CHUNK_H, data);

        results.push_back(r);

        std::vector<MEvec2> worldMesh;

        f32 lastX = (f32)r.initialX;
        f32 lastY = (f32)r.initialY;

        for (int i = 0; i < r.directions.size(); i++) {
            // if(r.directions[i].x != 0) r.directions[i].x = r.directions[i].x / abs(r.directions[i].x);
            // if(r.directions[i].y != 0) r.directions[i].y = r.directions[i].y / abs(r.directions[i].y);

            for (int ix = 0; ix < std::max(abs(r.directions[i].x), 1); ix++) {
                for (int iy = 0; iy < std::max(abs(r.directions[i].y), 1); iy++) {
                    int ilx = (int)(lastX + ix * (r.directions[i].x < 0 ? -1 : 1));
                    int ily = (int)(lastY - iy * (r.directions[i].y < 0 ? -1 : 1));

                    if (ilx < 0) ilx = 0;
                    if (ilx >= CHUNK_W) ilx = CHUNK_W - 1;

                    if (ily < 0) ily = 0;
                    if (ily >= CHUNK_H) ily = CHUNK_H - 1;

                    int ind = ilx + ily * CHUNK_W;
                    if (ind >= size) {
                        continue;
                    }
                    edgeSeen[ind] = true;
                }
            }

            lastX += (f32)r.directions[i].x;
            lastY -= (f32)r.directions[i].y;
            worldMesh.emplace_back(lastX, lastY);
        }

        worldMesh = simplify(worldMesh, 1);

        if (worldMesh.size() < 3) {
            // 1x1 pixel that breaks everything
            continue;
        }

        worldMeshes.push_back(worldMesh);

        TPPLPoly poly;

        poly.Init((long)worldMesh.size());

        for (int i = 0; i < worldMesh.size(); i++) {
            poly[(int)worldMesh.size() - i - 1] = {worldMesh[i].x, worldMesh[i].y};
        }

        if (poly.GetOrientation() == TPPL_CW) {
            poly.SetHole(true);
        }

        shapes.push_back(poly);
    }

    delete[] edgeSeen;
    delete[] data;
    std::list<TPPLPoly> result;
    std::list<TPPLPoly> result2;

    TPPLPartition part;
    TPPLPartition part2;

    part.RemoveHoles(&shapes, &result2);

    part2.Triangulate_EC(&result2, &result);

    /*bool* solid = new bool[10 * 10];
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            solid[x + y * width] = rand() % 2 == 0;
        }
    }*/

    // Ps::MarchingSquares ms = Ps::MarchingSquares(solid, CHUNK_W, CHUNK_H);

    // Ps::MarchingSquares ms = Ps::MarchingSquares(texture);
    // worldMesh = ms.extract_simple(2);

    chunk->polys.clear();

    std::for_each(result.begin(), result.end(), [&](TPPLPoly cur) {
        if (cur[0].x == cur[1].x && cur[1].x == cur[2].x) {
            cur[0].x += 0.01f;
        }

        if (cur[0].y == cur[1].y && cur[1].y == cur[2].y) {
            cur[0].y += 0.01f;
        }

        std::vector<b2Vec2> vec = {{(f32)cur[0].x, (f32)cur[0].y}, {(f32)cur[1].x, (f32)cur[1].y}, {(f32)cur[2].x, (f32)cur[2].y}};
        // worldTris.push_back(vec);
        b2PolygonShape sh;
        sh.Set(&vec[0], 3);

        chunk->polys.push_back(sh);
    });

#pragma endregion

    // TODO: 世界区块的box2d碰撞

    auto texture = LoadTexture("data/assets/objects/testObject3.png");

    if (chunk->rb) {
        delete[] chunk->rb->tiles;

        chunk->rb->chunk_clean();

        delete chunk->rb;
    }
    chunk->rb = makeRigidBodyMulti(b2_staticBody, chunk->x * CHUNK_W + loadZone.x, chunk->y * CHUNK_H + loadZone.y, 0, chunk->polys, 1, 0.3, texture);

    for (b2Fixture *f = chunk->rb->body->GetFixtureList(); f; f = f->GetNext()) {
        b2Filter bf = {};
        bf.categoryBits = 0x0001;
        f->SetFilterData(bf);
    }

    worldRigidBodies.push_back(chunk->rb);
}

void world::updateWorldMesh() {

    if (lastMeshZone.x == meshZone.x && lastMeshZone.y == meshZone.y && lastMeshZone.w == meshZone.w && lastMeshZone.h == meshZone.h) {
        if (lastMeshLoadZone.x == loadZone.x && lastMeshLoadZone.y == loadZone.y && lastMeshLoadZone.w == loadZone.w && lastMeshLoadZone.h == loadZone.h) {
            return;
        }
    }

    int prevMinChX = (int)std::floor((lastMeshZone.x - lastMeshLoadZone.x) / CHUNK_W);
    int prevMinChY = (int)std::floor((lastMeshZone.y - lastMeshLoadZone.y) / CHUNK_H);
    int prevMaxChX = (int)std::ceil((lastMeshZone.x + lastMeshZone.w - lastMeshLoadZone.x) / CHUNK_W);
    int prevMaxChY = (int)std::ceil((lastMeshZone.y + lastMeshZone.h + lastMeshLoadZone.y) / CHUNK_H);

    int minChX = (int)std::floor((meshZone.x - loadZone.x) / CHUNK_W);
    int minChY = (int)std::floor((meshZone.y - loadZone.y) / CHUNK_H);
    int maxChX = (int)std::ceil((meshZone.x + meshZone.w - loadZone.x) / CHUNK_W);
    int maxChY = (int)std::ceil((meshZone.y + meshZone.h - loadZone.y) / CHUNK_H);

    for (int i = 0; i < worldRigidBodies.size(); i++) {
        b2world->DestroyBody(worldRigidBodies[i]->body);
    }
    worldRigidBodies.clear();

    if (meshZone.w == 0 || meshZone.h == 0) return;

    // METADOT_BUG(std::format("updateWorldMesh {0}/{1} {2}/{3}", minChX, maxChX, minChY, maxChY).c_str());

    for (int cx = minChX; cx <= maxChX; cx++) {
        for (int cy = minChY; cy <= maxChY; cy++) {
            updateChunkMesh(getChunk(cx, cy));
        }
    }

    // 更新区块mesh后同步
    lastMeshZone = meshZone;
    lastMeshLoadZone = loadZone;
}

MaterialInstance world::getTile(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return Tiles_TEST_SOLID;
    return real_tiles[x + y * width];
}

void world::setTile(int x, int y, MaterialInstance type) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    real_tiles[x + y * width] = type;
    dirty[x + y * width] = true;
}

MaterialInstance world::getTileLayer2(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return Tiles_TEST_SOLID;
    return real_layer2[x + y * width];
}

void world::setTileLayer2(int x, int y, MaterialInstance type) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    real_layer2[x + y * width] = type;
    layer2Dirty[x + y * width] = true;
}

f32 CalculateVerticalFlowValue(f32 remainingLiquid, f32 destLiquid) {
    f32 sum = remainingLiquid + destLiquid;
    f32 value = 0;

    if (sum <= FLUID_MaxValue) {
        value = FLUID_MaxValue;
    } else if (sum < 2 * FLUID_MaxValue + FLUID_MaxCompression) {
        value = (FLUID_MaxValue * FLUID_MaxValue + sum * FLUID_MaxCompression) / (FLUID_MaxValue + FLUID_MaxCompression);
    } else {
        value = (sum + FLUID_MaxCompression) / 2.0f;
    }

    return value;
}

void world::tick() {

// TODO: 如果我们只检查最后标记为dirty的tiles会怎么样

// #define DEBUG_FRICTION
// #define DO_REVERSE
#define DO_MULTITHREADING 1

#if DO_MULTITHREADING
    bool whichTickVisited = false;
    memset(tickVisited1, false, (size_t)width * height);
#endif

    // TODO: 尝试找到一种方法来优化这个循环，因为液体需要高迭代次数
    for (int iter = 0; iter < global.game->Iso.globaldef.cell_iter; iter++) {

#ifdef DO_REVERSE
        bool reverseX = (tickCt + iter) % 2 == 0;
#else
        bool reverseX = false;
#endif
        for (int tk = 0; tk < 4; tk++) {

            int chOfsX = tk % 2;              // 0 1 0 1
            int chOfsY = 1 - ((tk % 4) / 2);  // 1 1 0 0

#if DO_MULTITHREADING
            std::vector<std::future<std::vector<CellData *>>> results = {};
#endif
#if DO_MULTITHREADING
            bool *tickVisited = whichTickVisited ? tickVisited2 : tickVisited1;
            std::future<void> tickVisitedDone = world_sys.tickVisitedPool->push([&](int id) { memset(whichTickVisited ? tickVisited1 : tickVisited2, false, (size_t)width * height); });
#else
            bool *tickVisited = tickVisited1;
            memset(tickVisited1, false, width * height);
#endif

            for (int cx = tickZone.x + chOfsX * CHUNK_W; cx < (tickZone.x + tickZone.w); cx += CHUNK_W * 2) {
                for (int cy = tickZone.y + chOfsY * CHUNK_H; cy < (tickZone.y + tickZone.h); cy += CHUNK_H * 2) {

#if DO_MULTITHREADING
                    results.push_back(world_sys.tickPool->push([&, cx, cy](int id) {
                        std::vector<CellData *> parts = {};

#else

#endif

                        for (int dy = CHUNK_H - 1; dy >= 0; dy--) {
                            int y = cy + dy;
                            for (int dxf = 0; dxf < CHUNK_W; dxf++) {
                                int dx = reverseX ? (CHUNK_W - 1) - dxf : dxf;
                                int x = cx + dx;
                                int index = x + y * width;

                                if (tickVisited[index]) continue;

                                if (iter >= real_tiles[index].mat->iterations) {
                                    tickVisited[index] = true;
                                    continue;
                                }
                                MaterialInstance tile = real_tiles[index];

                                int type = tile.mat->physicsType;

                                if (tile.mat->id == GAME()->materials_list.FIRE.id) {
                                    if (rand() % 10 == 0) {
                                        u32 rgb = 255;
                                        rgb = (rgb << 8) + 100 + rand() % 50;
                                        rgb = (rgb << 8) + 50;
                                        tile.color = rgb;
                                    }

                                    if (rand() % 10 == 0) {
                                        CellData *p = new CellData(tile, x, y - 1, (rand() % 10 - 5) / 20.0f, -((rand() % 10) / 10.0f) / 3.0f + -0.5f, 0, 0.01f);
                                        p->temporary = true;
                                        p->lifetime = 30;
                                        p->fadeTime = 10;
#if DO_MULTITHREADING
                                        parts.push_back(p);
#else
                                    cells.push_back(p);
#endif
                                    }

                                    if (rand() % 150 == 0) {
                                        // tiles[index] = TilesCreateSteam();
                                        real_tiles[index] = Tiles_NOTHING;
                                        dirty[index] = true;
                                        tickVisited[index] = true;
                                    } else {
                                        bool foundAny = false;
                                        for (int xx = -2; xx <= 2; xx++) {
                                            for (int yy = -2; yy <= 2; yy++) {
                                                if (real_tiles[(x + xx) + (y + yy) * width].mat->physicsType == PhysicsType::SOLID) {
                                                    foundAny = true;
                                                    if (rand() % 500 == 0) {
                                                        real_tiles[(x + xx) + (y + yy) * width] = TilesCreateFire();
                                                        dirty[(x + xx) + (y + yy) * width] = true;
                                                        tickVisited[(x + xx) + (y + yy) * width] = true;
                                                    }
                                                }
                                            }
                                        }
                                        if (!foundAny && rand() % 120 == 0) {
                                            real_tiles[index] = Tiles_NOTHING;
                                            dirty[index] = true;
                                            tickVisited[index] = true;
                                        }
                                    }
                                }

                                if (type == PhysicsType::SAND) {
                                    // active[index] = true;
                                    MaterialInstance belowTile = real_tiles[x + (y + 1) * width];
                                    int below = belowTile.mat->physicsType;

                                    if (tile.mat->interact && belowTile.mat->id >= 0 && belowTile.mat->id < GAME()->materials_count && tile.mat->nInteractions[belowTile.mat->id] > 0) {
                                        for (int i = 0; i < tile.mat->nInteractions[belowTile.mat->id]; i++) {
                                            MaterialInteraction in = tile.mat->interactions[belowTile.mat->id][i];
                                            if (in.type == INTERACT_TRANSFORM_MATERIAL) {
                                                for (int xx = in.ofsX - in.data2; xx <= in.ofsX + in.data2; xx++) {
                                                    for (int yy = in.ofsY - in.data2; yy <= in.ofsY + in.data2; yy++) {
                                                        if (real_tiles[(x + xx) + (y + yy) * width].mat->id == belowTile.mat->id) {
                                                            real_tiles[(x + xx) + (y + yy) * width] = TilesCreate(GAME()->materials_container[in.data1]->id, x + xx, y + yy);
                                                            dirty[(x + xx) + (y + yy) * width] = true;
                                                            tickVisited[(x + xx) + (y + yy) * width] = true;
                                                        }
                                                    }
                                                }
                                            } else if (in.type == INTERACT_SPAWN_MATERIAL) {
                                                for (int xx = in.ofsX - in.data2; xx <= in.ofsX + in.data2; xx++) {
                                                    for (int yy = in.ofsY - in.data2; yy <= in.ofsY + in.data2; yy++) {
                                                        if ((xx == 0 && yy == 0) || real_tiles[(x + xx) + (y + yy) * width].mat->id == Tiles_NOTHING.mat->id) {
                                                            real_tiles[(x + xx) + (y + yy) * width] = TilesCreate(GAME()->materials_container[in.data1]->id, x + xx, y + yy);
                                                            dirty[(x + xx) + (y + yy) * width] = true;
                                                            tickVisited[(x + xx) + (y + yy) * width] = true;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        continue;
                                    }

                                    if (tile.mat->react && tile.mat->nReactions > 0) {
                                        bool react = false;
                                        for (int i = 0; i < tile.mat->nReactions; i++) {
                                            MaterialInteraction in = tile.mat->reactions[i];
                                            if (in.type == REACT_TEMPERATURE_BELOW) {
                                                if (tile.temperature < in.data1) {
                                                    real_tiles[index] = TilesCreate(GAME()->materials_container[in.data2]->id, x, y);
                                                    real_tiles[index].temperature = tile.temperature;
                                                    dirty[index] = true;
                                                    tickVisited[index] = true;
                                                    react = true;
                                                }
                                            } else if (in.type == REACT_TEMPERATURE_ABOVE) {
                                                if (tile.temperature > in.data1) {
                                                    real_tiles[index] = TilesCreate(GAME()->materials_container[in.data2]->id, x, y);
                                                    real_tiles[index].temperature = tile.temperature;
                                                    dirty[index] = true;
                                                    tickVisited[index] = true;
                                                    react = true;
                                                }
                                            }
                                        }
                                        if (react) continue;
                                    }

                                    bool canMoveBelow = (below == PhysicsType::AIR || (below != PhysicsType::SOLID && belowTile.mat->density < tile.mat->density));
                                    if (!canMoveBelow) continue;

                                    MaterialInstance belowLTile = real_tiles[(x - 1) + (y + 1) * width];
                                    int belowL = belowLTile.mat->physicsType;
                                    MaterialInstance belowRTile = real_tiles[(x + 1) + (y + 1) * width];
                                    int belowR = belowRTile.mat->physicsType;

                                    bool canMoveBelowL = (belowL == PhysicsType::AIR || (belowL != PhysicsType::SOLID && belowLTile.mat->density < tile.mat->density));
                                    bool canMoveBelowR = (belowR == PhysicsType::AIR || (belowR != PhysicsType::SOLID && belowRTile.mat->density < tile.mat->density));

                                    if (canMoveBelow && !((canMoveBelowL || canMoveBelowR) && rand() % 20 == 0)) {
                                        if (belowTile.mat->physicsType == PhysicsType::AIR && getTile(x, y + 2).mat->physicsType == PhysicsType::AIR &&
                                            getTile(x, y + 3).mat->physicsType == PhysicsType::AIR && getTile(x, y + 4).mat->physicsType == PhysicsType::AIR) {
                                            setTile(x, y, belowTile);
#if DO_MULTITHREADING
                                            parts.push_back(new CellData(tile, x, y + 1, (rand() % 10 - 5) / 20.0f, -((rand() % 2) + 3) / 10.0f + 1.5f, 0, 0.1f));
#else
                                        cells.push_back(new CellData(tile, x, y + 1, (rand() % 10 - 5) / 20.0f, -((rand() % 2) + 3) / 10.0f + 1.5f, 0, 0.1f));
#endif
                                        } else {
                                            real_tiles[index] = belowTile;
                                            dirty[index] = true;
                                            // setTile(x, y, belowTile);
                                            // setTile(x, y + 1, tile);
                                            if (rand() % 2 == 0) {
                                                tile.moved = true;
#ifdef DEBUG_FRICTION
                                                tile.color = 0xffffffff;
#endif
                                            }
                                            real_tiles[(x) + (y + 1) * width] = tile;
                                            dirty[(x) + (y + 1) * width] = true;
                                            tickVisited[x + (y + 1) * width] = true;
                                        }

                                        int selfTrasmitMovementChance = 2;

                                        if (rand() % selfTrasmitMovementChance == 0) {
                                            if (x > 0 && real_tiles[(x - 1) + (y + 1) * width].mat->physicsType == PhysicsType::SAND) {
                                                int otherTransmitMovementChance = 2;
                                                if (rand() % otherTransmitMovementChance == 0) {
                                                    real_tiles[(x - 1) + (y + 1) * width].moved = true;
#ifdef DEBUG_FRICTION
                                                    real_tiles[(x - 1) + (y + 1) * width].color = 0xff00ffff;
                                                    dirty[(x - 1) + (y + 1) * width] = true;
#endif
                                                }
                                            }

                                            if (x < width - 1 && real_tiles[(x + 1) + (y + 1) * width].mat->physicsType == PhysicsType::SAND) {
                                                int otherTransmitMovementChance = 2;
                                                if (rand() % otherTransmitMovementChance == 0) {
                                                    real_tiles[(x + 1) + (y + 1) * width].moved = true;
#ifdef DEBUG_FRICTION
                                                    real_tiles[(x + 1) + (y + 1) * width].color = 0xff00ffff;
                                                    dirty[(x + 1) + (y + 1) * width] = true;
#endif
                                                }
                                            }
                                        }
                                    }

                                } else if (type == PhysicsType::SOUP) {

                                    // based on https://github.com/jongallant/LiquidSimulator (MIT License)

                                    // NOTE: for liquids, tile.moved is tile.settled in the original algorithm

                                    if (tile.fluidAmount == 0.0f) continue;

                                    if (tile.fluidAmount < FLUID_MinValue) {
                                        tile.fluidAmount = 0.0f;
                                        real_tiles[index] = tile;
                                        continue;
                                    }

                                    if (tile.fluidAmount > 0.005 && getTile(x, y + 1).mat->physicsType == PhysicsType::AIR && getTile(x, y + 2).mat->physicsType == PhysicsType::AIR &&
                                        getTile(x, y + 3).mat->physicsType == PhysicsType::AIR && getTile(x, y + 4).mat->physicsType == PhysicsType::AIR) {
                                        setTile(x, y, Tiles_NOTHING);

                                        int n = tile.fluidAmount / 4;
                                        if (n < 1) n = 1;

                                        for (int i = 0; i < n; i++) {
                                            f32 amt = tile.fluidAmount / n;

                                            MaterialInstance nt = MaterialInstance(tile.mat, tile.color, tile.temperature);
                                            nt.fluidAmount = amt;
                                            nt.fluidAmountDiff = 0;
                                            nt.moved = false;
#if DO_MULTITHREADING
                                            parts.push_back(new CellData(nt, x, y + 1, (rand() % 10 - 5) / 30.0f, -((rand() % 2) + 3) / 10.0f + 1.0f, 0, 0.1f));
#else
                                        cells.push_back(new CellData(nt, x, y + 1, (rand() % 10 - 5) / 20.0f, -((rand() % 2) + 3) / 10.0f + 1.5f, 0, 0.1f));
#endif
                                        }

                                        continue;
                                    }

                                    if (tile.moved) continue;

                                    f32 startValue = tile.fluidAmount;
                                    f32 remainingValue = tile.fluidAmount;

                                    MaterialInstance bottom = real_tiles[(x) + (y + 1) * width];

                                    bool airBelow = bottom.mat->physicsType == PhysicsType::AIR;
                                    if ((airBelow && iter <= 2) || (bottom.mat->id == tile.mat->id)) {
                                        f32 dstFl = bottom.mat->physicsType == PhysicsType::SOUP ? bottom.fluidAmount : 0.0f;

                                        f32 flow = CalculateVerticalFlowValue(startValue, dstFl) - dstFl;
                                        if (bottom.fluidAmount > 0 && flow > FLUID_MinFlow) flow *= FLUID_FlowSpeed;

                                        flow = std::max(flow, 0.0f);
                                        if (flow > std::min(FLUID_MaxFlow, startValue)) flow = std::min(FLUID_MaxFlow, startValue);

                                        if (flow != 0) {
                                            remainingValue -= flow;
                                            tile.fluidAmountDiff -= flow;
                                            if (bottom.mat->physicsType == PhysicsType::AIR) {
                                                real_tiles[(x) + (y + 1) * width] = MaterialInstance(tile.mat, tile.color, tile.temperature);
                                                real_tiles[(x) + (y + 1) * width].fluidAmount = 0.0f;
                                            }
                                            real_tiles[(x) + (y + 1) * width].fluidAmountDiff += flow;
                                            // tiles[(x)+(y + 1) * width].moved = true;
                                        }
                                        flowY[index] += flow;
                                    } else if (iter == 0 && bottom.mat->physicsType == PhysicsType::SOUP && (bottom.mat->id != tile.mat->id)) {
                                        if (rand() % 10 == 0) {
                                            real_tiles[index] = bottom;
                                            real_tiles[(x) + (y + 1) * width] = tile;
                                            continue;
                                        }
                                    }

                                    if (remainingValue < FLUID_MinValue) {
                                        tile.fluidAmountDiff -= remainingValue;
                                        real_tiles[index] = tile;
                                        continue;
                                    }

                                    MaterialInstance left = real_tiles[(x - 1) + (y)*width];
                                    bool canMoveLeft = (left.mat->physicsType == PhysicsType::AIR || (left.mat->id == tile.mat->id)) && !airBelow;

                                    MaterialInstance right = real_tiles[(x + 1) + (y)*width];
                                    bool canMoveRight = (right.mat->physicsType == PhysicsType::AIR || (right.mat->id == tile.mat->id)) && !airBelow;

                                    if (canMoveLeft) {
                                        f32 dstFl = left.mat->physicsType == PhysicsType::SOUP ? left.fluidAmount : 0.0f;

                                        f32 flow = (remainingValue - dstFl) / (canMoveRight ? 3.0f : 2.0f);
                                        if (flow > FLUID_MinFlow) flow *= FLUID_FlowSpeed;

                                        flow = std::max(flow, 0.0f);
                                        if (flow > std::min(FLUID_MaxFlow, remainingValue)) flow = std::min(FLUID_MaxFlow, remainingValue);

                                        if (flow != 0) {
                                            remainingValue -= flow;
                                            tile.fluidAmountDiff -= flow;
                                            if (left.mat->physicsType == PhysicsType::AIR) {
                                                real_tiles[(x - 1) + (y)*width] = MaterialInstance(tile.mat, tile.color, tile.temperature);
                                                real_tiles[(x - 1) + (y)*width].fluidAmount = 0.0f;
                                            }
                                            real_tiles[(x - 1) + (y)*width].fluidAmountDiff += flow;
                                            // tiles[(x - 1) + (y)*width].moved = true;
                                        }
                                        flowX[index] -= flow;
                                    }

                                    if (remainingValue < FLUID_MinValue) {
                                        tile.fluidAmountDiff -= remainingValue;
                                        real_tiles[index] = tile;
                                        continue;
                                    }

                                    if (canMoveRight) {
                                        f32 dstFl = right.mat->physicsType == PhysicsType::SOUP ? right.fluidAmount : 0.0f;

                                        f32 flow = (remainingValue - dstFl) / (canMoveLeft ? 2.0f : 2.0f);
                                        if (flow > FLUID_MinFlow) flow *= FLUID_FlowSpeed;

                                        flow = std::max(flow, 0.0f);
                                        if (flow > std::min(FLUID_MaxFlow, remainingValue)) flow = std::min(FLUID_MaxFlow, remainingValue);

                                        if (flow != 0) {
                                            remainingValue -= flow;
                                            tile.fluidAmountDiff -= flow;
                                            if (right.mat->physicsType == PhysicsType::AIR) {
                                                real_tiles[(x + 1) + (y)*width] = MaterialInstance(tile.mat, tile.color, tile.temperature);
                                                real_tiles[(x + 1) + (y)*width].fluidAmount = 0.0f;
                                            }
                                            real_tiles[(x + 1) + (y)*width].fluidAmountDiff += flow;
                                            // tiles[(x + 1) + (y)*width].moved = true;
                                        }
                                        flowX[index] += flow;
                                    }

                                    if (remainingValue < FLUID_MinValue) {
                                        tile.fluidAmountDiff -= remainingValue;
                                        real_tiles[index] = tile;
                                        continue;
                                    }

                                    MaterialInstance top = real_tiles[(x) + (y - 1) * width];

                                    if (top.mat->physicsType == PhysicsType::AIR || (top.mat->id == tile.mat->id)) {
                                        f32 dstFl = top.mat->physicsType == PhysicsType::SOUP ? top.fluidAmount : 0.0f;

                                        f32 flow = remainingValue - CalculateVerticalFlowValue(remainingValue, dstFl);
                                        if (flow > FLUID_MinFlow) flow *= FLUID_FlowSpeed;

                                        flow = std::max(flow, 0.0f);
                                        if (flow > std::min(FLUID_MaxFlow, remainingValue)) flow = std::min(FLUID_MaxFlow, remainingValue);

                                        if (flow != 0) {
                                            remainingValue -= flow;
                                            tile.fluidAmountDiff -= flow;
                                            if (top.mat->physicsType == PhysicsType::AIR) {
                                                real_tiles[(x) + (y - 1) * width] = MaterialInstance(tile.mat, tile.color, tile.temperature);
                                                real_tiles[(x) + (y - 1) * width].fluidAmount = 0.0f;
                                            }
                                            real_tiles[(x) + (y - 1) * width].fluidAmountDiff += flow;
                                            // tiles[(x)+(y - 1) * width].moved = true;
                                        }
                                        flowY[index] -= flow;
                                    } else if (iter == 0 && top.mat->physicsType == PhysicsType::SOUP && (top.mat->id != tile.mat->id)) {
                                        if (rand() % 10 == 0) {
                                            real_tiles[index] = top;
                                            real_tiles[(x) + (y - 1) * width] = tile;
                                            continue;
                                        }
                                    }

                                    if (remainingValue < FLUID_MinValue) {
                                        tile.fluidAmountDiff -= remainingValue;
                                        real_tiles[index] = tile;
                                        continue;
                                    }

                                    if (startValue == remainingValue) {
                                        tile.settleCount++;
                                        if (tile.settleCount >= 10) {
                                            tile.moved = true;
                                        }
                                    } else {
                                        dirty[index] = true;
                                        if (top.mat->physicsType == PhysicsType::SOUP) real_tiles[(x) + (y - 1) * width].moved = false;
                                        if (bottom.mat->physicsType == PhysicsType::SOUP) real_tiles[(x) + (y + 1) * width].moved = false;
                                        if (left.mat->physicsType == PhysicsType::SOUP) real_tiles[(x - 1) + (y)*width].moved = false;
                                        if (right.mat->physicsType == PhysicsType::SOUP) real_tiles[(x + 1) + (y)*width].moved = false;
                                    }

                                    real_tiles[index] = tile;

                                    // active[index] = true;
                                    MaterialInstance belowTile = real_tiles[(x) + (y + 1) * width];
                                    int below = belowTile.mat->physicsType;

                                    // if(tile.mat->interact && belowTile.mat->id >= 0 && belowTile.mat->id < GAME()->materials_count && tile.mat->nInteractions[belowTile.mat->id] > 0) {
                                    //     for(int i = 0; i < tile.mat->nInteractions[belowTile.mat->id]; i++) {
                                    //         MaterialInteraction in = tile.mat->interactions[belowTile.mat->id][i];
                                    //         if(in.type == INTERACT_TRANSFORM_MATERIAL) {
                                    //             for(int xx = in.ofsX - in.data2; xx <= in.ofsX + in.data2; xx++) {
                                    //                 for(int yy = in.ofsY - in.data2; yy <= in.ofsY + in.data2; yy++) {
                                    //                     if(tiles[(x + xx) + (y + yy) * width].mat->id == belowTile.mat->id) {
                                    //                         tiles[(x + xx) + (y + yy) * width] = TilesCreate(GAME()->materials_container[in.data1], x + xx, y + yy);
                                    //                         dirty[(x + xx) + (y + yy) * width] = true;
                                    //                         tickVisited[(x + xx) + (y + yy) * width] = true;
                                    //                     }
                                    //                 }
                                    //             }
                                    //         } else if(in.type == INTERACT_SPAWN_MATERIAL) {
                                    //             for(int xx = in.ofsX - in.data2; xx <= in.ofsX + in.data2; xx++) {
                                    //                 for(int yy = in.ofsY - in.data2; yy <= in.ofsY + in.data2; yy++) {
                                    //                     if((xx == 0 && yy == 0) || tiles[(x + xx) + (y + yy) * width].mat->id == Tiles_NOTHING.mat->id) {
                                    //                         tiles[(x + xx) + (y + yy) * width] = TilesCreate(GAME()->materials_container[in.data1], x + xx, y + yy);
                                    //                         dirty[(x + xx) + (y + yy) * width] = true;
                                    //                         tickVisited[(x + xx) + (y + yy) * width] = true;
                                    //                     }
                                    //                 }
                                    //             }
                                    //         }
                                    //     }
                                    //     continue;
                                    // }

                                    // if(tile.mat->react && tile.mat->nReactions > 0) {
                                    //     bool react = false;
                                    //     for(int i = 0; i < tile.mat->nReactions; i++) {
                                    //         MaterialInteraction in = tile.mat->reactions[i];
                                    //         if(in.type == REACT_TEMPERATURE_BELOW) {
                                    //             if(tile.temperature < in.data1) {
                                    //                 tiles[index] = TilesCreate(GAME()->materials_container[in.data2], x, y);
                                    //                 tiles[index].temperature = tile.temperature;
                                    //                 dirty[index] = true;
                                    //                 tickVisited[index] = true;
                                    //                 react = true;
                                    //             }
                                    //         } else if(in.type == REACT_TEMPERATURE_ABOVE) {
                                    //             if(tile.temperature > in.data1) {
                                    //                 tiles[index] = TilesCreate(GAME()->materials_container[in.data2], x, y);
                                    //                 tiles[index].temperature = tile.temperature;
                                    //                 dirty[index] = true;
                                    //                 tickVisited[index] = true;
                                    //                 react = true;
                                    //             }
                                    //         }
                                    //     }
                                    //     if(react) continue;
                                    // }

                                    if (tile.mat->id == GAME()->materials_list.WATER.id && belowTile.mat->id == GAME()->materials_list.LAVA.id) {
                                        real_tiles[index] = TilesCreateSteam();
                                        dirty[index] = true;
                                        real_tiles[(x) + (y + 1) * width] = TilesCreateObsidian(x, y + 1);
                                        dirty[(x) + (y + 1) * width] = true;
                                        tickVisited[(x) + (y + 1) * width] = true;

                                        for (int xx = -1; xx <= 1; xx++) {
                                            for (int yy = 0; yy <= 2; yy++) {
                                                if (real_tiles[(x + xx) + (y + yy) * width].mat->id == GAME()->materials_list.LAVA.id) {
                                                    real_tiles[(x + xx) + (y + yy) * width] = TilesCreateObsidian(x + xx, y + yy);
                                                    dirty[(x + xx) + (y + yy) * width] = true;
                                                    tickVisited[(x + xx) + (y + yy) * width] = true;
                                                }
                                            }
                                        }

                                        continue;
                                    }

                                    // bool canMoveBelow = (below == PhysicsType::AIR || (below != PhysicsType::SOLID && belowTile.mat->density < tile.mat->density));
                                    // if(!canMoveBelow) continue;

                                    // MaterialInstance belowLTile = tiles[(x - 1) + (y + 1) * width];
                                    // int belowL = belowLTile.mat->physicsType;
                                    // MaterialInstance belowRTile = tiles[(x + 1) + (y + 1) * width];
                                    // int belowR = belowRTile.mat->physicsType;

                                    // bool canMoveBelowL = (belowL == PhysicsType::AIR || (belowL != PhysicsType::SOLID && belowLTile.mat->density < tile.mat->density));
                                    // bool canMoveBelowR = (belowR == PhysicsType::AIR || (belowR != PhysicsType::SOLID && belowRTile.mat->density < tile.mat->density));

                                    // if(canMoveBelow && !((canMoveBelowL || canMoveBelowR) && rand() % 10 == 0)) {
                                    //     if(belowTile.mat->physicsType == PhysicsType::AIR && getTile(x, y + 2).mat->physicsType == PhysicsType::AIR && getTile(x, y + 3).mat->physicsType ==
                                    //     PhysicsType::AIR && getTile(x, y + 4).mat->physicsType == PhysicsType::AIR) {
                                    //         setTile(x, y, belowTile);
                                    //         #if DO_MULTITHREADING
                                    //         parts.push_back(new CellData(tile, x, y + 1, (rand() % 10 - 5) / 20.0f, -((rand() % 2) + 3) / 10.0f + 1.5f, 0, 0.1f));
                                    //         #else
                                    //         cells.push_back(new CellData(tile, x, y + 1, (rand() % 10 - 5) / 20.0f, -((rand() % 2) + 3) / 10.0f + 1.5f, 0, 0.1f));
                                    //         #endif
                                    //     } else {
                                    //         tiles[index] = belowTile;
                                    //         dirty[index] = true;
                                    //         //setTile(x, y, belowTile);
                                    //         //setTile(x, y + 1, tile);
                                    //         tiles[(x)+(y + 1) * width] = tile;
                                    //         dirty[(x)+(y + 1) * width] = true;
                                    //         tickVisited[x + (y + 1) * width] = true;
                                    //     }
                                    // }
                                } else if (type == PhysicsType::GAS) {
                                    // active[index] = true;
                                    int above = real_tiles[(x) + (y - 1) * width].mat->physicsType;

                                    int aboveL = real_tiles[(x - 1) + (y - 1) * width].mat->physicsType;
                                    int aboveR = real_tiles[(x + 1) + (y - 1) * width].mat->physicsType;

                                    if (above == 0 && !((aboveL == 0 || aboveR == 0) && rand() % 2 == 0)) {
                                        real_tiles[index] = getTile(x, y - 1);
                                        dirty[index] = true;

                                        real_tiles[(x) + (y - 1) * width] = tile;
                                        dirty[(x) + (y - 1) * width] = true;

                                        tickVisited[(x) + (y - 1) * width] = true;
                                    }
                                }
                            }
                        }

                        for (int dy = CHUNK_H - 1; dy >= 0; dy--) {
                            int y = cy + dy;
                            for (int dxf = 0; dxf < CHUNK_W; dxf++) {
                                int dx = reverseX ? (CHUNK_W - 1) - dxf : dxf;
                                int x = cx + dx;
                                int index = x + y * width;

                                if (tickVisited[index]) continue;

                                MaterialInstance tile = real_tiles[index];

                                int type = tile.mat->physicsType;

                                if (type == PhysicsType::SAND) {
                                    // active[index] = true;
                                    MaterialInstance belowLTile = real_tiles[(x - 1) + (y + 1) * width];
                                    int belowL = belowLTile.mat->physicsType;
                                    MaterialInstance belowRTile = real_tiles[(x + 1) + (y + 1) * width];
                                    int belowR = belowRTile.mat->physicsType;

                                    bool canMoveBelowL = (belowL == PhysicsType::AIR || (belowL != PhysicsType::SOLID && belowLTile.mat->density < tile.mat->density));
                                    bool canMoveBelowR = (belowR == PhysicsType::AIR || (belowR != PhysicsType::SOLID && belowRTile.mat->density < tile.mat->density));

                                    bool stoppedByFriction = !tile.moved;

                                    // 1 to ~127
                                    int slipperyness = tile.mat->slipperyness;

                                    if (stoppedByFriction) {
                                        int drop = 0;

                                        for (int pil = 0; pil < 10; pil++) {
                                            int pilChL = real_tiles[(x - 1) + (y + 1 + pil) * width].mat->physicsType;
                                            int pilChR = real_tiles[(x + 1) + (y + 1 + pil) * width].mat->physicsType;

                                            if (pilChL == PhysicsType::AIR || pilChR == PhysicsType::AIR) {
                                                drop++;
                                            }
                                        }

                                        // max number of pixels tall a pillar can be before being unstable
                                        int maxStability = 8 / sqrt(slipperyness) + 1;

                                        if (drop + 1 - maxStability > 0) {
                                            int chance = 1000 / (drop + 1 - maxStability);
                                            if (chance < 1000) {
                                                if (rand() % chance == 0) {
                                                    stoppedByFriction = false;
                                                    real_tiles[(x) + (y)*width].moved = true;
#ifdef DEBUG_FRICTION
                                                    real_tiles[(x) + (y)*width].color = 0xff0000ff;
                                                    dirty[(x) + (y)*width] = true;
#endif
                                                }
                                            }
                                        }
                                    }

                                    if (stoppedByFriction || !(canMoveBelowL || canMoveBelowR)) {
                                        real_tiles[(x) + (y)*width].moved = false;
#ifdef DEBUG_FRICTION
                                        real_tiles[(x) + (y)*width].color = 0xff000000;
                                        dirty[(x) + (y)*width] = true;
#endif
                                        continue;
                                    }

                                    bool shouldMove = rand() % (2 * slipperyness) != 0;

                                    if (shouldMove && (canMoveBelowL || canMoveBelowR)) {
                                        int selfTrasmitMovementChance = 2;

                                        if (rand() % selfTrasmitMovementChance == 0) {
                                            if (real_tiles[(x) + (y + 1) * width].mat->physicsType == PhysicsType::SAND) {
                                                int otherTransmitMovementChance = 2;
                                                if (rand() % otherTransmitMovementChance == 0) {
                                                    real_tiles[(x) + (y + 1) * width].moved = true;
#ifdef DEBUG_FRICTION
                                                    real_tiles[(x) + (y + 1) * width].color = 0xffff00ff;
                                                    dirty[(x) + (y + 1) * width] = true;
#endif
                                                }
                                            }
                                        }
                                    }

                                    if (shouldMove && canMoveBelowL && (!canMoveBelowR || rand() % 2 == 0)) {
                                        if (real_tiles[(x - 1) + y * width].mat->physicsType == PhysicsType::AIR) {
                                            real_tiles[(x - 1) + y * width] = belowLTile;
                                            dirty[(x - 1) + y * width] = true;
                                            tickVisited[(x - 1) + (y)*width] = true;
                                            real_tiles[index] = Tiles_NOTHING;
                                            dirty[index] = true;
                                        } else {
                                            real_tiles[index] = belowLTile;
                                            dirty[index] = true;
                                            tickVisited[index] = true;
                                        }

                                        if (rand() % (20 * slipperyness) == 0) {
                                            tile.moved = false;
#ifdef DEBUG_FRICTION
                                            tile.color = 0xff000000;
#endif
                                        }
                                        real_tiles[(x - 1) + (y + 1) * width] = tile;
                                        dirty[(x - 1) + (y + 1) * width] = true;
                                        tickVisited[(x - 1) + (y + 1) * width] = true;

                                    } else if (shouldMove && canMoveBelowR) {

                                        if (real_tiles[(x + 1) + y * width].mat->physicsType == PhysicsType::AIR) {
                                            real_tiles[(x + 1) + y * width] = belowRTile;
                                            dirty[(x + 1) + y * width] = true;
                                            real_tiles[index] = Tiles_NOTHING;
                                            dirty[index] = true;
                                        } else {
                                            real_tiles[index] = belowRTile;
                                            dirty[index] = true;
                                            tickVisited[index] = true;
                                        }

                                        if (rand() % (20 * slipperyness) == 0) {
                                            tile.moved = false;
#ifdef DEBUG_FRICTION
                                            tile.color = 0xff000000;
#endif
                                        }
                                        real_tiles[(x + 1) + (y + 1) * width] = tile;
                                        dirty[(x + 1) + (y + 1) * width] = true;
                                        tickVisited[(x + 1) + (y + 1) * width] = true;

                                    } else {
                                        real_tiles[(x) + (y)*width].moved = false;
#ifdef DEBUG_FRICTION
                                        real_tiles[(x) + (y)*width].color = 0xff000000;
                                        dirty[(x) + (y)*width] = true;
#endif
                                    }
                                } else if (type == PhysicsType::SOUP) {

                                    tile.fluidAmount += tile.fluidAmountDiff;
                                    tile.fluidAmountDiff = 0.0f;
                                    if (tile.fluidAmount < FLUID_MinValue) {
                                        real_tiles[index] = Tiles_NOTHING;
                                        dirty[index] = true;
                                        tickVisited[index] = true;
                                    } else {
                                        real_tiles[index] = tile;
                                        /*uint8_t c = (1.0f - tile.fluidAmount / 8.0f) * 255;
                                    int rgb = c;
                                    rgb = (rgb << 8) + c;
                                    rgb = (rgb << 8) + c;
                                    tiles[index].color = rgb;*/
                                        dirty[index] = true;
                                        tickVisited[index] = true;
                                    }

                                    // OLD:

                                    // active[index] = true;
                                    /*MaterialInstance belowLTile = tiles[(x - 1) + (y + 1) * width];
                                int belowL = belowLTile.mat->physicsType;
                                MaterialInstance belowRTile = tiles[(x + 1) + (y + 1) * width];
                                int belowR = belowRTile.mat->physicsType;

                                bool canMoveBelowL = (belowL == PhysicsType::AIR || (belowL != PhysicsType::SOLID && belowLTile.mat->density < tile.mat->density));
                                bool canMoveBelowR = (belowR == PhysicsType::AIR || (belowR != PhysicsType::SOLID && belowRTile.mat->density < tile.mat->density));

                                if(!(canMoveBelowL || canMoveBelowR)) continue;

                                MaterialInstance lTile = tiles[(x - 1) + (y)* width];
                                int l = lTile.mat->physicsType;
                                MaterialInstance rTile = tiles[(x + 1) + (y)* width];
                                int r = rTile.mat->physicsType;

                                bool canMoveL = (l == PhysicsType::AIR || (l != PhysicsType::SOLID && lTile.mat->density < tile.mat->density));
                                bool canMoveR = (r == PhysicsType::AIR || (r != PhysicsType::SOLID && rTile.mat->density < tile.mat->density));

                                if(!((canMoveL || canMoveR) && rand() % 10 == 0)) {
                                    if(canMoveBelowL && !(canMoveBelowR && rand() % 2 == 0)) {
                                        if(tiles[(x - 1) + y * width].mat->physicsType == PhysicsType::AIR) {
                                            tiles[(x - 1) + y * width] = belowLTile;
                                            dirty[(x - 1) + y * width] = true;
                                            tiles[index] = Tiles_NOTHING;
                                            dirty[index] = true;
                                        } else {
                                            tiles[index] = belowLTile;
                                            dirty[index] = true;
                                        }

                                        tiles[(x - 1) + (y + 1) * width] = tile;
                                        dirty[(x - 1) + (y + 1) * width] = true;
                                        tickVisited[(x - 1) + (y + 1) * width] = true;
                                    } else if(canMoveBelowR) {
                                        if(tiles[(x + 1) + y * width].mat->physicsType == PhysicsType::AIR) {
                                            tiles[(x + 1) + y * width] = belowRTile;
                                            dirty[(x + 1) + y * width] = true;
                                            tiles[index] = Tiles_NOTHING;
                                            dirty[index] = true;
                                        } else {
                                            tiles[index] = belowRTile;
                                            dirty[index] = true;
                                        }

                                        tiles[(x + 1) + (y + 1) * width] = tile;
                                        dirty[(x + 1) + (y + 1) * width] = true;
                                        tickVisited[(x + 1) + (y + 1) * width] = true;
                                    }
                                }*/
                                } else if (type == PhysicsType::GAS) {
                                    // active[index] = true;
                                    int aboveL = real_tiles[(x - 1) + (y - 1) * width].mat->physicsType;
                                    int aboveR = real_tiles[(x + 1) + (y - 1) * width].mat->physicsType;

                                    if (aboveL == 0 && !(aboveR == 0 && rand() % 2 == 0)) {
                                        real_tiles[index] = real_tiles[(x - 1) + (y - 1) * width];
                                        dirty[index] = true;

                                        real_tiles[(x - 1) + (y - 1) * width] = tile;
                                        dirty[(x - 1) + (y - 1) * width] = true;
                                        tickVisited[(x - 1) + (y - 1) * width] = true;
                                    } else if (aboveR == 0) {
                                        real_tiles[index] = real_tiles[(x + 1) + (y - 1) * width];
                                        dirty[index] = true;

                                        real_tiles[(x + 1) + (y - 1) * width] = tile;
                                        dirty[(x + 1) + (y - 1) * width] = true;
                                        tickVisited[(x + 1) + (y - 1) * width] = true;
                                    }
                                }
                            }
                        }

                        for (int dy = CHUNK_H - 1; dy >= 0; dy--) {
                            int y = cy + dy;
                            for (int dxf = 0; dxf < CHUNK_W; dxf++) {
                                int dx = reverseX ? (CHUNK_W - 1) - dxf : dxf;
                                int x = cx + dx;
                                int index = x + y * width;

                                if (tickVisited[index]) continue;

                                MaterialInstance tile = real_tiles[index];

                                int type = tile.mat->physicsType;

                                if (type == PhysicsType::SOUP) {
                                    // active[index] = true;

                                    /*MaterialInstance lTile = tiles[(x - 1) + (y)* width];
                                int l = lTile.mat->physicsType;
                                MaterialInstance rTile = tiles[(x + 1) + (y)* width];
                                int r = rTile.mat->physicsType;

                                bool canMoveL = (l == PhysicsType::AIR || (l != PhysicsType::SOLID && lTile.mat->density < tile.mat->density));
                                bool canMoveR = (r == PhysicsType::AIR || (r != PhysicsType::SOLID && rTile.mat->density < tile.mat->density));

                                if(canMoveL && !(canMoveR && rand() % 2 == 5)) {
                                    tiles[index] = lTile;
                                    dirty[index] = true;

                                    tiles[(x - 1) + (y)* width] = tile;
                                    dirty[(x - 1) + (y)* width] = true;
                                    tickVisited[(x - 1) + (y)* width] = true;
                                } else if(canMoveR) {
                                    tiles[index] = rTile;
                                    dirty[index] = true;

                                    tiles[(x + 1) + (y)* width] = tile;
                                    dirty[(x + 1) + (y)* width] = true;
                                    tickVisited[(x + 1) + (y)* width] = true;
                                }*/
                                } else if (type == PhysicsType::GAS) {
                                    // active[index] = true;

                                    int l = real_tiles[(x - 1) + (y)*width].mat->physicsType;
                                    int r = real_tiles[(x + 1) + (y)*width].mat->physicsType;

                                    if (l == 0 && !(r == 0 && rand() % 2 == 0)) {
                                        real_tiles[index] = getTile(x - 1, y);
                                        dirty[index] = true;

                                        real_tiles[(x - 1) + (y)*width] = tile;
                                        dirty[(x - 1) + (y)*width] = true;
                                        tickVisited[(x - 1) + (y)*width] = true;
                                    } else if (r == 0) {
                                        real_tiles[index] = getTile(x + 1, y);
                                        dirty[index] = true;

                                        real_tiles[(x + 1) + (y)*width] = tile;
                                        dirty[(x + 1) + (y)*width] = true;
                                        tickVisited[(x + 1) + (y)*width] = true;
                                    } else {
                                        if (tile.mat->id == GAME()->materials_list.STEAM.id) {
                                            if (rand() % 10 == 0) {
                                                real_tiles[index] = TilesCreateWater();
                                                dirty[index] = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }

#if DO_MULTITHREADING
                        return parts;
                    }));
#endif
                }
            }

#if DO_MULTITHREADING

            for (int i = 0; i < results.size(); i++) {

                std::vector<CellData *> pts = results[i].get();

                cells.insert(cells.end(), pts.begin(), pts.end());
            }
            tickVisitedDone.get();

            whichTickVisited = !whichTickVisited;

#endif

            // while (tickPool->n_idle() != tickPool->size()) {
            //  //printf("%d / %d", tickPool->n_idle(), tickPool->size());
            //  auto n = tickPool->n_idle();
            //  auto i = 0;
            // }
        }
    }

#undef DEBUG_FRICTION
#undef DO_MULTITHREADING
#undef DO_REVERSE

    tickCt++;

    for (int i = 0; i < 1; i++) {
        int randX = rand() % (int)tickZone.w;
        int randY = rand() % (int)tickZone.h;
        // setTile(tickZone.x + randX, tickZone.y + randY, MaterialInstance(&Materials::GENERIC_SOLID, 0x00ff00ff));
        physicsCheck(tickZone.x + randX, tickZone.y + randY);
    }

    /*delete lastActive;
lastActive = active;
active = new bool[width * height];*/

    /*std::fill(light, light + width * height, 0);
for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
        if (tiles[x + y * width].mat->physicsType == PhysicsType::AIR) {
            applyLightRec(x, y, 1);
        }
    }
}*/
}

void world::tickTemperature() {
    // TODO: multithread

    for (int y = (tickZone.y + tickZone.h) - 1; y >= tickZone.y; y--) {
        for (int x = tickZone.x; x < (tickZone.x + tickZone.w); x++) {
            f32 n = 0.01;
            f32 v = 0;
            // for (int xx = -1; xx <= 1; xx++) {
            //  for (int yy = -1; yy <= 1; yy++) {
            //      f32 factor = abs(tiles[(x + xx) + (y + yy) * width].temperature) / 64 * tiles[(x + xx) + (y + yy) * width].mat->conductionOther;
            //      //factor = fmax(-1, fmin(factor, 1));

            //      v += tiles[(x + xx) + (y + yy) * width].temperature * factor;
            //      n += factor;

            //      // ((v1 * f1) + (v2 * f2)) / (f1 + f2)
            //      //=(v1 * f1) + (v2 * f2)
            //  }
            //}
            f32 factor = 0;
#define FN(xa, ya)                                                                                                                             \
    if (real_tiles[(x + xa) + (y + ya) * width].temperature != 0) {                                                                            \
        factor = abs(real_tiles[(x + xa) + (y + ya) * width].temperature) / 64 * real_tiles[(x + xa) + (y + ya) * width].mat->conductionOther; \
        v += real_tiles[(x + xa) + (y + ya) * width].temperature * factor;                                                                     \
        n += factor;                                                                                                                           \
    }
            FN(-1, -1);
            FN(-1, 0);
            FN(-1, 1);
            FN(0, -1);
            FN(0, 0);
            FN(0, 1);
            FN(1, -1);
            FN(1, 0);
            FN(1, 1);

#undef FN

            if (v != 0) {
                newTemps[x + y * width] = real_tiles[x + y * width].mat->addTemp + (v / n * real_tiles[x + y * width].mat->conductionSelf) +
                                          (real_tiles[x + y * width].temperature * (1 - real_tiles[x + y * width].mat->conductionSelf));
            } else {
                newTemps[x + y * width] = real_tiles[x + y * width].mat->addTemp + real_tiles[x + y * width].temperature;
            }
        }
    }
    // iterate

    for (int y = (tickZone.y + tickZone.h) - 1; y >= tickZone.y; y--) {
        for (int x = tickZone.x; x < (tickZone.x + tickZone.w); x++) {
            real_tiles[x + y * width].temperature = newTemps[x + y * width];
        }
    }
    // copy
}

void world::renderCells(unsigned char **texture) {

    for (auto &cur : cells) {
        if (cur->x < 0 || cur->x >= width || cur->y < 0 || cur->y >= height) continue;

        f32 alphaMod = 1;
        if (cur->temporary) {
            if (cur->lifetime < cur->fadeTime) {
                alphaMod = (cur->lifetime / (f32)cur->fadeTime);
            }
        }
        // f32 alphaMod = 1;

        const unsigned int offset = (width * 4 * (int)cur->y) + (int)cur->x * 4;
        u32 color = cur->tile.color;
        (*texture)[offset + 2] = (color >> 0) & 0xff;                    // b
        (*texture)[offset + 1] = (color >> 8) & 0xff;                    // g
        (*texture)[offset + 0] = (color >> 16) & 0xff;                   // r
        (*texture)[offset + 3] = (u8)(cur->tile.mat->alpha * alphaMod);  // a
        // SDL_SetRenderDrawColor(renderer, (cur->tile.color >> 16) & 0xff, (cur->tile.color >> 8) & 0xff, (cur->tile.color >> 0) & 0xff, (u8)(cur->tile.mat->alpha * alphaMod));
        // SDL_RenderDrawPoint(renderer, (int)cur->x, (int)cur->y);
    }
}

void world::tickCells() {

    auto func = [&](CellData *cur) {
        if (cur->temporary && cur->lifetime <= 0) {
            cur->killCallback();
            delete cur;
            return true;
        }

        if (cur->targetForce != 0) {
            f32 tdx = cur->targetX - cur->x;
            f32 tdy = cur->targetY - cur->y;
            f32 normFac = sqrtf(tdx * tdx + tdy * tdy);

            cur->vx += tdx / normFac * cur->targetForce;
            cur->vy += tdy / normFac * cur->targetForce;

            if (normFac < 100) {
                cur->vx *= 0.95f;
                cur->vy *= 0.95f;
            }
        }

        int lx = cur->x;
        int ly = cur->y;

        if ((cur->x < 0 || (int)(cur->x) >= width || cur->y < 0 || (int)(cur->y) >= height)) {
            cur->killCallback();
            return true;
        }

        if (!(lx >= tickZone.x && ly >= tickZone.y && lx < tickZone.x + tickZone.w && ly < tickZone.y + tickZone.h)) return false;

        cur->vx += cur->ax;
        cur->vy += cur->ay;

        int div = (int)((abs(cur->vx) + abs(cur->vy)) + 1);

        f32 dvx = cur->vx / div;
        f32 dvy = cur->vy / div;

        for (int i = 0; i < div; i++) {
            cur->x += dvx;
            cur->y += dvy;

            if ((cur->x < 0 || (int)(cur->x) >= width || cur->y < 0 || (int)(cur->y) >= height)) {
                cur->killCallback();
                return true;
            }

            if (!cur->phase && real_tiles[(int)(cur->x) + (int)(cur->y) * width].mat->physicsType != PhysicsType::AIR) {
                bool allowCollision = true;
                bool isObject = real_tiles[(int)(cur->x) + (int)(cur->y) * width].mat->physicsType == PhysicsType::OBJECT;

                switch (cur->inObjectState) {
                    case 0:  // first frame of particle's life
                        if (isObject) {
                            cur->inObjectState = 1;
                        } else {
                            cur->inObjectState = 2;
                        }
                        break;
                    case 1:  // particle spawned in object and was in object last tick
                        if (!isObject) cur->inObjectState = 2;
                        break;
                }

                if (!isObject || cur->inObjectState == 2) {
                    if (cur->temporary) {
                        cur->killCallback();
                        delete cur;
                        return true;
                    }

                    if (real_tiles[(int)(lx) + (int)(ly)*width].mat->physicsType != PhysicsType::AIR) {
                        /*for (int y = 0; y < 40; y++) {
                            if (tiles[(int)(cur->x) + (int)(cur->y - y) * width].mat->physicsType == PhysicsType::AIR) {
                                tiles[(int)(cur->x) + (int)(cur->y - y) * width] = cur->tile;
                                dirty[(int)(cur->x) + (int)(cur->y - y) * width] = true;
                                break;
                            }
                        }*/

                        bool succeeded = false;
                        {
                            // printf("=========");
                            int X = 32;
                            int Y = 32;
                            int x = 0, y = 0, dx = 0, dy = -1;
                            int t = std::max(X, Y);
                            int maxI = t * t;

                            for (int j = 0; j < maxI; j++) {
                                if ((-X / 2 <= x) && (x <= X / 2) && (-Y / 2 <= y) && (y <= Y / 2)) {
                                    // printf("%d, %d", x, y);
                                    // DO STUFF
                                    if (real_tiles[(int)(cur->x + x) + (int)(cur->y + y) * width].mat->physicsType == PhysicsType::AIR) {
                                        real_tiles[(int)(cur->x + x) + (int)(cur->y + y) * width] = cur->tile;
                                        dirty[(int)(cur->x + x) + (int)(cur->y + y) * width] = true;
                                        succeeded = true;
                                        break;
                                    } else if (cur->tile.mat->physicsType == PhysicsType::SOUP && cur->tile.mat == real_tiles[(int)(cur->x + x) + (int)(cur->y + y) * width].mat) {

                                        real_tiles[(int)(cur->x + x) + (int)(cur->y + y) * width].fluidAmount += cur->tile.fluidAmount;
                                        dirty[(int)(cur->x + x) + (int)(cur->y + y) * width] = true;
                                        succeeded = true;
                                        break;
                                    }
                                }

                                if ((x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))) {
                                    t = dx;
                                    dx = -dy;
                                    dy = t;
                                }
                                x += dx;
                                y += dy;
                            }
                        }

                        if (succeeded) {
                            cur->killCallback();
                            delete cur;
                            return true;
                        } else {
                            cur->vy = -4;
                            cur->y -= 16;
                            return false;
                        }
                    } else {
                        real_tiles[(int)(lx) + (int)(ly)*width] = cur->tile;
                        dirty[(int)(lx) + (int)(ly)*width] = true;
                        cur->killCallback();
                        delete cur;
                        return true;
                    }
                }
            }
        }

        if (cur->lifetime > 0) {
            cur->lifetime--;
        }

        return false;
    };

    // cells.erase(std::remove_if(cells.begin(), cells.end(), func), cells.end());

    std::erase_if(cells, func);

    // // Better cells removal effects
    // std::for_each(cells.begin(), cells.end(), [](CellData *cur) {
    //     cur->vx += cur->ax;
    //     cur->vy += cur->ay;
    //     cur->x += cur->vx;
    //     cur->y += cur->vy;
    //     // return cur->y > height;
    // });

    std::erase_if(cells, [&](auto c) { return c->y > height; });

    // std::remove_if(cells.begin(), cells.end(), [&](CellData* cur) {
    //  return cur->y > height;
    // });
}

void world::tickObjectsMesh() {

    std::vector<RigidBody *> *rbs = &rigidBodies;
    for (int i = 0; i < rbs->size(); i++) {
        RigidBody *cur = (*rbs)[i];
        if (!static_cast<bool>(cur->get_surface())) {
            // rigidBodies.erase(std::remove(rigidBodies.begin(), rigidBodies.end(), cur), rigidBodies.end());
            std::erase_if(rigidBodies, [&](auto c) { return cur == c; });
            continue;
        };
        if (cur->needsUpdate && cur->body->IsEnabled()) {
            updateRigidBodyHitbox(cur);
        }
    }
}

void world::tickObjectBounds() {

    std::vector<RigidBody *> rbs = rigidBodies;

    // 确定那些物体需要物理运算
    // 这一部分在物理引擎部分计算了 到时候只需要确定chunk就行了

    for (int i = 0; i < rbs.size(); i++) {
        RigidBody *cur = rbs[i];

        if (cur->is_cleaned) continue;

        f32 x = cur->body->GetWorldCenter().x;
        f32 y = cur->body->GetWorldCenter().y;
        cur->body->SetEnabled(x >= tickZone.x && y >= tickZone.y && x < tickZone.x + tickZone.w && y < tickZone.y + tickZone.h);
    }
}

void world::tickObjects() {

    int minX = width;
    int minY = height;
    int maxX = 0;
    int maxY = 0;

    std::vector<RigidBody *> rbs = rigidBodies;
    for (int i = 0; i < rbs.size(); i++) {
        RigidBody *cur = rbs[i];

        f32 x = cur->body->GetWorldCenter().x;
        f32 y = cur->body->GetWorldCenter().y;

        if (cur->body->IsEnabled()) {
            if (x - 100 < minX) minX = (int)x - 100;
            if (y - 100 < minY) minY = (int)y - 100;
            if (x + 100 > maxX) maxX = (int)x + 100;
            if (y + 100 > maxY) maxY = (int)y + 100;
        }

        if (cur->needsUpdate) {
            // updateRigidBodyHitbox(cur); // 在tickObjectsMesh中复写
            // continue;
        }
    }

    int meshZoneSnap = 16;
    int mzx = std::max((int)((minX - loadZone.x) / meshZoneSnap) * meshZoneSnap + (int)loadZone.x, 0);
    int mzy = std::max((int)((minY - loadZone.y) / meshZoneSnap) * meshZoneSnap + (int)loadZone.y, 0);
    meshZone = {(float)mzx, (float)mzy, (float)std::min((int)ceil(((f64)maxX - mzx) / (f64)meshZoneSnap) * meshZoneSnap, width - mzx - 1),
                (float)std::min((int)ceil(((f64)maxY - mzy) / (f64)meshZoneSnap) * meshZoneSnap, height - mzy - 1)};

    f32 timeStep = 33.0 / 1000.0;

    i32 velocityIterations = 5;
    i32 positionIterations = 2;

#if 1

    registry.for_each_component<WorldEntity>([this](ME::ecs::entity, WorldEntity &we) {
        we.rb->body->SetTransform(b2Vec2(we.x + loadZone.x + we.hw / 2 - 0.5, we.y + loadZone.y + we.hh / 2 - 1.5), 0);
        we.rb->body->SetLinearVelocity({(f32)(we.vx * 1.0), (f32)(we.vy * 1.0)});
    });

    b2world->Step(timeStep, velocityIterations, positionIterations);

    registry.for_each_component<WorldEntity>([this](ME::ecs::entity, WorldEntity &we) {
        /*cur->x = cur->rb->body->GetPosition().x + 0.5 - cur->hw / 2 - loadZone.x;
        cur->y = cur->rb->body->GetPosition().y + 1.5 - cur->hh / 2 - loadZone.y;*/
        // cur->rb->body->SetTransform(b2Vec2(cur->x + loadZone.x + cur->hw / 2 - 0.5, cur->y + loadZone.y + cur->hh / 2 - 1.5), 0);
        // cur->rb->body->SetLinearVelocity({ cur->vx * 25, cur->vy * (cur->vy > 0 ? 0 : 25) });

        // 将物理引擎的速度参数同步到对象
        we.vx = we.rb->body->GetLinearVelocity().x / 1.0;
        we.vy = we.rb->body->GetLinearVelocity().y / 1.0;
    });

#endif
}

void world::addCell(CellData *cell) { cells.push_back(cell); }

void world::explosion(int cx, int cy, int radius) {
    audioEngine->PlayEvent("event:/Explode");

    int outerRadius = radius * 2;
    for (int x = cx - outerRadius; x < cx + outerRadius; x++) {
        for (int y = cy - outerRadius; y < cy + outerRadius; y++) {
            MaterialInstance tile = getTile(x, y);
            if (tile.mat->physicsType == PhysicsType::AIR) continue;

            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy < radius * radius) {
                if (tile.mat->physicsType == PhysicsType::SOLID || rand() % 10 < 6) {
                    setTile(x, y, Tiles_NOTHING);
                } else {

                    int r = (tile.color >> 16) & 0xFF;
                    int g = (tile.color >> 8) & 0xFF;
                    int b = (tile.color >> 0) & 0xFF;

                    u32 rgb = r / 4;
                    rgb = (rgb << 8) + g / 4;
                    rgb = (rgb << 8) + b / 4;

                    tile.color = rgb;

                    cells.push_back(new CellData(tile, x, y + 1, dx / 10.0f + (rand() % 10 - 5) / 10.0f, dy / 6.0f + (rand() % 10 - 5) / 10.0f, 0, 0.1f));
                    setTile(x, y, Tiles_NOTHING);
                }
            } else if (dx * dx + dy * dy < outerRadius * outerRadius && tile.mat->physicsType != PhysicsType::SOLID) {
                cells.push_back(new CellData(tile, x, y, dx / 10.0f + (rand() % 10 - 5) / 10.0f, dy / 6.0f + (rand() % 10 - 5) / 10.0f, 0, 0.1f));
                setTile(x, y, Tiles_NOTHING);
            }
        }
    }

    // 预计在下一个世界tick重新计算mesh
    lastMeshZone = {};
}

void world::frame() {

    while (toLoad.size() > 0) {
        LoadChunkParams para = toLoad[0];
        toLoadAsyncList.push_back(std::async(static_cast<Chunk *(world::*)(LoadChunkParams)>(&world::loadChunk), this, para));
        toLoad.erase(toLoad.begin());
    }

    for (int i = 0; i < toLoadAsyncList.size(); i++) {

        // 判断区块是否已经生成或加载好了
        if (future_is_ready(toLoadAsyncList[i])) {
            Chunk *merge = toLoadAsyncList[i].get();

            // 保障合并列表的唯一性
            std::erase(readyToMerge, merge);

            readyToMerge.push_back(merge);

            // 初始化 chunkCache 的列
            if (!chunkCache.count(merge->x)) {
                auto h = phmap::flat_hash_map<int, Chunk *>();
                // h.set_deleted_key(INT_MAX);
                // h.set_empty_key(INT_MIN);
                chunkCache[merge->x] = h;
            }

            // 将区块合并对象复制到 chunkCache
            chunkCache[merge->x][merge->y] = merge;

            needToTickGeneration = true;

            // 清除区块生成加载异步对象
            toLoadAsyncList.erase(toLoadAsyncList.begin() + i);
            i--;
        }
    }

    int n = 0;

    while (readyToMerge.size() > 0 && n++ < 16) {
        Chunk *merge = readyToMerge[0];
        readyToMerge.pop_front();

        for (int x = 0; x < CHUNK_W; x++) {
            for (int y = 0; y < CHUNK_H; y++) {
                int tx = merge->x * CHUNK_W + loadZone.x + x;
                int ty = merge->y * CHUNK_H + loadZone.y + y;
                if (tx < 0 || tx >= width || ty < 0 || ty >= height) continue;

                real_tiles[tx + ty * width] = merge->tiles[x + y * CHUNK_W];
                dirty[tx + ty * width] = true;
                real_layer2[tx + ty * width] = merge->layer2[x + y * CHUNK_W];
                layer2Dirty[tx + ty * width] = true;
                background[tx + ty * width] = merge->background[x + y * CHUNK_W];
                backgroundDirty[tx + ty * width] = true;
            }
        }

        // delete prop;
    }
}

void world::tickChunkGeneration() {

    int n = 0;
    int cenX = (-loadZone.x + loadZone.w / 2) / CHUNK_W;
    int cenY = (-loadZone.y + loadZone.h / 2) / CHUNK_H;

    std::vector<std::future<void>> gen_results;

    for (auto &p : chunkCache) {
        if (p.first == INT_MIN) continue;  // Should be change when using phmap::flat_hash_map
        for (auto &p2 : p.second) {
            if (p2.first == INT_MIN) continue;  // Should be change when using phmap::flat_hash_map
            Chunk *m = p2.second;

            // Check should we unload chunk
            if (std::abs(m->x - cenX) >= CHUNK_UNLOAD_DIST || std::abs(m->y - cenY) >= CHUNK_UNLOAD_DIST) {
                unloadChunk(m);
                continue;
            }

            if (m->generationPhase < 0) continue;
            if (m->generationPhase >= std::min(highestPopulator, 5)) continue;

            for (int xx = -1; xx <= 1; xx++) {
                for (int yy = -1; yy <= 1; yy++) {
                    if (xx == 0 && yy == 0) continue;
                    Chunk *ch = getChunk(p.first + xx, p2.first + yy);

                    if (ch->generationPhase < p2.second->generationPhase) {
                        if (ch->pleaseDelete) {
                            delete ch;
                        }
                        goto nextChunk;
                    }
                    if (ch->pleaseDelete) {
                        delete ch;
                    }
                }
            }
            m->generationPhase++;
            populateChunk(m, m->generationPhase, true);

            gen_results.push_back(std::async(std::launch::async, [&]() noexcept { m->ChunkWrite(m->tiles, m->layer2, m->background); }));

            if (n++ > 4) {
                return;
            }

        nextChunk : {}
        }
    }

    for (auto &async : gen_results) async.get();

    needToTickGeneration = false;
}

void world::tickChunks() {

    if (lastLoadZone.x == loadZone.x && lastLoadZone.y == loadZone.y && lastLoadZone.w == loadZone.w && lastLoadZone.h == loadZone.h) {
        // camera didnt move
    } else {
        int changeX = loadZone.x - lastLoadZone.x;
        int changeY = loadZone.y - lastLoadZone.y;

        if (changeX != 0 || changeY != 0) {

            bool revX = changeX > 0;
            bool revY = changeY > 0;

            for (int y = 0; y < height; y++) {
                int oldY = (revY ? (height - y - 1) : y);
                int newY = oldY + changeY;
                if (newY < 0 || newY >= height) continue;
                for (int x = 0; x < width; x++) {
                    int oldX = (revX ? (width - x - 1) : x);
                    int newX = oldX + changeX;
                    if (newX >= 0 && newX < width) {
                        real_tiles[newX + newY * width] = real_tiles[oldX + oldY * width];
                        background[newX + newY * width] = background[oldX + oldY * width];
                        real_layer2[newX + newY * width] = real_layer2[oldX + oldY * width];
                    }
                }
            }

            if (changeX < 0) {
                for (int i = 0; i < abs(changeX); i++) {
                    if ((((int)loadZone.x - changeX - i) + (int)loadZone.w) % CHUNK_W == 0) {
                        for (int y = -loadZone.y + tickZone.y - CHUNK_W * 4; y <= -loadZone.y + tickZone.h + CHUNK_H * 9; y += CHUNK_H) {
                            int cy = floor(y / (f32)CHUNK_H);
                            int cx = floor((-(loadZone.x - changeX - i) + tickZone.w) / (f32)CHUNK_W) + 1;
                            for (int xx = 0; xx <= 4; xx++) {
                                queueLoadChunk(cx + xx, cy, true, xx == 0 && y >= -loadZone.y + tickZone.y && y <= -loadZone.y + tickZone.h + CHUNK_H);
                            }
                        }
                    }
                }

                for (int i = 0; i < abs(changeX); i++) {
                    if ((((int)loadZone.x - changeX - i - 1) + (int)loadZone.w) % CHUNK_W == 0) {
                        for (int y = -loadZone.y + tickZone.y; y <= -loadZone.y + tickZone.h + CHUNK_H; y += CHUNK_H) {
                            int cy = floor(y / (f32)CHUNK_H);
                            int cx = ceil((-(loadZone.x - changeX + i)) / (f32)CHUNK_W);
                            // unloadChunk(cx, cy);
                            chunkSaveCache(getChunk(cx, cy));
                        }
                    }
                }
            } else if (changeX > 0) {
                for (int i = 0; i < abs(changeX); i++) {
                    if ((((int)loadZone.x - changeX + i) + (int)loadZone.w) % CHUNK_W == 0) {
                        for (int y = -loadZone.y + tickZone.y - CHUNK_W * 4; y <= -loadZone.y + tickZone.h + CHUNK_H * 9; y += CHUNK_H) {
                            int cy = floor(y / (f32)CHUNK_H);
                            int cx = ceil((-(loadZone.x - changeX + i)) / (f32)CHUNK_W);
                            for (int xx = 0; xx <= 4; xx++) {
                                queueLoadChunk(cx - xx, cy, true, xx == 0 && y >= -loadZone.y + tickZone.y && y <= -loadZone.y + tickZone.h + CHUNK_H);
                            }
                        }
                    }
                }

                for (int i = 0; i < abs(changeX); i++) {
                    if ((((int)loadZone.x - changeX + i + 1) + (int)loadZone.w) % CHUNK_W == 0) {
                        for (int y = -loadZone.y + tickZone.y; y <= -loadZone.y + tickZone.h + CHUNK_H; y += CHUNK_H) {
                            int cy = floor(y / (f32)CHUNK_H);
                            int cx = floor((-(loadZone.x - changeX - i) + tickZone.w) / (f32)CHUNK_W) + 1;
                            // unloadChunk(cx, cy);
                            chunkSaveCache(getChunk(cx, cy));
                        }
                    }
                }
            }

            if (changeY < 0) {
                for (int i = 0; i < abs(changeY); i++) {
                    if ((((int)loadZone.y - changeY - i) + (int)loadZone.h) % CHUNK_H == 0) {
                        for (int x = -loadZone.x + tickZone.x - CHUNK_W * 4; x <= -loadZone.x + tickZone.w + CHUNK_W * 9; x += CHUNK_W) {
                            int cx = floor(x / (f32)CHUNK_W);
                            int cy = floor((-(loadZone.y - changeY - i) + tickZone.h) / (f32)CHUNK_H) + 1;
                            for (int yy = 0; yy <= 4; yy++) {
                                queueLoadChunk(cx, cy + yy, true, yy == 0 && x >= -loadZone.x + tickZone.x && x <= -loadZone.x + tickZone.w + CHUNK_W);
                            }
                        }
                    }
                }

                for (int i = 0; i < abs(changeY); i++) {
                    if ((((int)loadZone.y - changeY - i - 1) + (int)loadZone.h) % CHUNK_H == 0) {
                        for (int x = -loadZone.x + tickZone.x; x <= -loadZone.x + tickZone.w + CHUNK_W; x += CHUNK_W) {
                            int cx = floor(x / (f32)CHUNK_W);
                            int cy = ceil((-(loadZone.y - changeY + i)) / (f32)CHUNK_H);
                            // unloadChunk(cx, cy);
                            chunkSaveCache(getChunk(cx, cy));
                        }
                    }
                }
            } else if (changeY > 0) {
                for (int i = 0; i < abs(changeY); i++) {
                    if ((((int)loadZone.y - changeY + i) + (int)loadZone.h) % CHUNK_H == 0) {
                        for (int x = -loadZone.x + tickZone.x - CHUNK_W * 4; x <= -loadZone.x + tickZone.w + CHUNK_W * 9; x += CHUNK_W) {
                            int cx = floor(x / (f32)CHUNK_W);
                            int cy = ceil((-(loadZone.y - changeY + i)) / (f32)CHUNK_H);
                            for (int yy = 0; yy <= 4; yy++) {
                                queueLoadChunk(cx, cy - yy, true, yy == 0 && x >= -loadZone.x + tickZone.x && x <= -loadZone.x + tickZone.w + CHUNK_W);
                            }
                        }
                    }
                }

                for (int i = 0; i < abs(changeY); i++) {
                    if ((((int)loadZone.y - changeY + i + 1) + (int)loadZone.h) % CHUNK_H == 0) {
                        for (int x = -loadZone.x + tickZone.x; x <= -loadZone.x + tickZone.w + CHUNK_W; x += CHUNK_W) {
                            int cx = floor(x / (f32)CHUNK_W);
                            int cy = floor((-(loadZone.y - changeY - i) + tickZone.h) / (f32)CHUNK_H) + 1;
                            // unloadChunk(cx, cy);
                            chunkSaveCache(getChunk(cx, cy));
                        }
                    }
                }
            }

            for (int i = 0; i < cells.size(); i++) {
                cells[i]->x += changeX;
                cells[i]->y += changeY;
            }

            for (int i = 0; i < rigidBodies.size(); i++) {
                RigidBody cur = *rigidBodies[i];
                cur.body->SetTransform(b2Vec2(cur.body->GetPosition().x + changeX, cur.body->GetPosition().y + changeY), cur.body->GetAngle());
            }
        }

        lastLoadZone = loadZone;
    }
}

void world::queueLoadChunk(int cx, int cy, bool populate, bool render) {

    // toLoad.push_back(LoadChunkParams(cx, cy, populate, 0));
    Chunk *ch = getChunk(cx, cy);
    if (ch->hasTileCache) {

        if (render) {

            readyToMerge.erase(std::remove(readyToMerge.begin(), readyToMerge.end(), ch), readyToMerge.end());
            readyToMerge.push_back(ch);
        }

        if (!chunkCache.count(ch->x)) {
            auto h = phmap::flat_hash_map<int, Chunk *>();
            // h.set_deleted_key(INT_MAX);
            // h.set_empty_key(INT_MIN);
            chunkCache[ch->x] = h;
        }

        chunkCache[ch->x][ch->y] = ch;
        needToTickGeneration = true;

    } else {

        // TODO: 23/7/22 不再进行队列预加载

        // for (int x = -1; x <= 1; x++) {
        //     for (int y = 0; y <= 0; y++) {
        //         Chunk *chb = getChunk(cx + x, y);  // load chunk at ~x y
        //         if (chb->pleaseDelete) {
        //             if (!chunkCache.count(chb->x)) {
        //                 auto h = phmap::flat_hash_map<int, Chunk *>();
        //                 // h.set_deleted_key(INT_MAX);
        //                 // h.set_empty_key(INT_MIN);
        //                 chunkCache[chb->x] = h;
        //             }
        //             auto a = &chunkCache[chb->x];
        //             chunkCache[chb->x][chb->y] = chb;
        //             chb->pleaseDelete = false;
        //             chb->generationPhase = -1;
        //         }
        //     }
        // }

        // loadChunk(ch, populate, render);

        /*
        readyToMerge.push_back(ch);
        if(!chunkCache.count(ch->x)) {
            chunkCache[ch->x] = phmap::flat_hash_map<int, Chunk*>();
            chunkCache[ch->x].set_deleted_key(INT_MAX);
            chunkCache[ch->x].set_empty_key(INT_MIN);
        }
        chunkCache[ch->x][ch->y] = ch;
        needToTickGeneration = true;
        */

        toLoadAsyncList.push_back(std::async(static_cast<Chunk *(world::*)(Chunk *, bool, bool)>(&world::loadChunk), this, ch, populate, render));
    }

    for (int x = 0; x < CHUNK_W; x++) {
        int tx = cx * CHUNK_W + loadZone.x + x;
        if (tx < 0) continue;
        if (tx >= width) break;
        for (int y = 0; y < CHUNK_H; y++) {
            int ty = cy * CHUNK_H + loadZone.y + y;
            if (ty < 0) continue;
            if (ty >= height) break;

            real_tiles[tx + ty * width] = Tiles_TEST_SOLID;
            // dirty[tx + ty * width] = true;
        }
    }

    // loadChunk(cx, cy, populate);
}

Chunk *world::loadChunk(LoadChunkParams para) { return loadChunk(getChunk(para.x, para.y), para.populate, true); }

Chunk *world::loadChunk(Chunk *ch, bool populate, bool render) {

    std::lock_guard<std::mutex> locker(g_mutex_loadchunk);

    ch->pleaseDelete = false;

    auto generate_chunk_func = [this](Chunk *chunk) {
        this->generateChunk(chunk);
        chunk->generationPhase = 0;
        chunk->hasTileCache = true;
        this->populateChunk(chunk, 0, false);
        if (!noSaveLoad) chunk->ChunkWrite(chunk->tiles, chunk->layer2, chunk->background);
    };

    if (ch->hasTileCache) {
        // prop = ch->tiles;
    } else if (ch->ChunkHasFile() && !noSaveLoad) {
        try {
            ch->ChunkRead();
        } catch (...) {
            METADOT_BUG(std::format("Failed to read chunk {0} {1} so regenerate it", ch->x, ch->y).c_str());
            generate_chunk_func(ch);
        }
    } else {
        // METADOT_BUG(std::format("generate_chunk_func {0} {1}", ch->x, ch->y).c_str());
        generate_chunk_func(ch);
    }

    // if (populate) {
    //  if (!ch.populated) {
    //      Populator pop;
    //      std::vector<PlacedStructure> structs = pop.apply(prop, ch, *this);
    //      ch.populated = true;

    //      /*for (int i = 0; i < structs.size(); i++) {
    //          addStructure(structs[i]);
    //      }*/

    //      for (int i = 0; i < structures.size(); i++) {
    //          PlacedStructure st = structures[i];
    //          int sx = std::max(st.x + loadZone.x, ch.x*CHUNK_W + loadZone.x);
    //          int sy = std::max(st.y + loadZone.y, ch.y*CHUNK_H + loadZone.y);
    //          int ex = std::min(st.x + loadZone.x + st.base.w, ch.x*CHUNK_W + loadZone.x + CHUNK_W);
    //          int ey = std::min(st.y + loadZone.y + st.base.h, ch.y*CHUNK_H + loadZone.y + CHUNK_H);

    //          for (int x = sx; x < ex; x++) {
    //              for (int y = sy; y < ey; y++) {
    //                  int tx = x - (st.x + loadZone.x);
    //                  int ty = y - (st.y + loadZone.y);
    //
    //                  int chx = x - loadZone.x - ch.x*CHUNK_W;
    //                  int chy = y - loadZone.y - ch.y*CHUNK_H;

    //                  if (tx >= 0 && ty >= 0 && chx >= 0 && chy >= 0 && tx < st.base.w && ty < st.base.h && chx < CHUNK_W && chy < CHUNK_H) {
    //                      if (st.base.tiles[tx + ty * st.base.w].mat->id != Materials::GENERIC_AIR.id) {
    //                          prop[chx + chy * CHUNK_W] = st.base.tiles[tx + ty * st.base.w];
    //                      }
    //                  }
    //              }
    //          }
    //
    //      }

    //  }
    //}

    return ch;
}

void world::unloadChunk(Chunk *ch) {
    // MaterialInstance* data = new MaterialInstance[CHUNK_W * CHUNK_H];
    // for (int x = 0; x < CHUNK_W; x++) {
    //  for (int y = 0; y < CHUNK_H; y++) {
    //      int tx = ch->x * CHUNK_W + loadZone.x + x;
    //      int ty = ch->y * CHUNK_H + loadZone.y + y;
    //      if (tx < 0 || tx >= width || ty < 0 || ty >= height) continue;
    //      data[x + y * CHUNK_W] = tiles[tx + ty * width];
    //      tiles[tx + ty * width] = Tiles_NOTHING;
    //      //dirty[tx + ty * width] = true;
    //  }
    // }
    // MaterialInstance* layer2 = new MaterialInstance[CHUNK_W * CHUNK_H];
    // for (int x = 0; x < CHUNK_W; x++) {
    //  for (int y = 0; y < CHUNK_H; y++) {
    //      int tx = ch->x * CHUNK_W + loadZone.x + x;
    //      int ty = ch->y * CHUNK_H + loadZone.y + y;
    //      if (tx < 0 || tx >= width || ty < 0 || ty >= height) continue;
    //      layer2[x + y * CHUNK_W] = this->layer2[tx + ty * width];
    //      this->layer2[tx + ty * width] = Tiles_NOTHING;
    //      //dirty[tx + ty * width] = true;
    //  }
    // }
    // ch->write(data, layer2);

    chunkSaveCache(ch);
    if (!noSaveLoad) writeChunkToDisk(ch);

    if (chunkCache[ch->x].contains(ch->y)) {
        chunkCache[ch->x].erase(ch->y);
    }
    ch->ChunkDelete();
    /*delete data;
    delete layer2;*/
    // delete data;
}

void world::writeChunkToDisk(Chunk *ch) { ch->ChunkWrite(ch->tiles, ch->layer2, ch->background); }

void world::chunkSaveCache(Chunk *ch) {
    for (int x = 0; x < CHUNK_W; x++) {
        for (int y = 0; y < CHUNK_H; y++) {
            int tx = ch->x * CHUNK_W + loadZone.x + x;
            int ty = ch->y * CHUNK_H + loadZone.y + y;
            if (tx < 0 || tx >= width || ty < 0 || ty >= height) continue;
            if (real_tiles[tx + ty * width].id == Tiles_TEST_SOLID.id) continue;
            ch->tiles[x + y * CHUNK_W] = real_tiles[tx + ty * width];
            ch->layer2[x + y * CHUNK_W] = real_layer2[tx + ty * width];
            ch->background[x + y * CHUNK_W] = background[tx + ty * width];
        }
    }
}

void world::generateChunk(Chunk *ch) { gen->generateChunk(this, ch); }

int world::getBiomeAt(Chunk *ch, int x, int y) {

    try {
        if (ch->biomes_id.at((x - ch->x * CHUNK_W) + (y - ch->y * CHUNK_H) * CHUNK_W) != Biome::biomeGet("DEFAULT").id) {
            int biome_id = ch->biomes_id[(x - ch->x * CHUNK_W) + (y - ch->y * CHUNK_H) * CHUNK_W];
            if (ch->pleaseDelete) delete ch;
            return biome_id;
        }
    } catch (const std::out_of_range &ex) {
        METADOT_ERROR(std::format("[Exception] ch->biomes_id[{0}] {1}", (x - ch->x * CHUNK_W) + (y - ch->y * CHUNK_H) * CHUNK_W, ex.what()).c_str());
        return Biome::biomeGet("DEFAULT").id;
    }

    int ret = getBiomeAt(x, y);
    ch->biomes_id[(x - ch->x * CHUNK_W) + (y - ch->y * CHUNK_H) * CHUNK_W] = ret;
    return ret;
}

int world::getBiomeAt(int x, int y) {

    int ret = -1;

    // ret = Biome::biomeGet("DEFAULT");
    // return ret;

    if (abs(CHUNK_H * 3 - y) < CHUNK_H * 10) {
        f32 v = noise.GetCellular(x / 20.0, 0, 8592) / 2 + 0.5;
        int biomeCatNum = 3;
        int biomeCat = (int)(v * biomeCatNum);
        if (biomeCat == 0) {
            ret = Biome::biomeGetID("PLAINS");
        } else if (biomeCat == 1) {
            ret = Biome::biomeGetID("MOUNTAINS");
        } else if (biomeCat == 2) {
            ret = Biome::biomeGetID("FOREST");
        }
    } else {
        noise.SetCellularDistanceFunction(FastNoise::CellularDistanceFunction::Natural);
        noise.SetCellularJitter(0.3);
        noise.SetCellularReturnType(FastNoise::CellularReturnType::CellValue);
        f32 v = noise.GetCellular(x / 20.0, y / 20.0, 2039) / 2 + 0.5;
        f32 v2 = noise.GetCellular(x / 3.0, y / 3.0, 3890) / 2 + 0.5;
        int biomeCatNum = 4;
        int biomeCat = (int)(v * biomeCatNum);

        if (biomeCat == 0) {
            ret = v2 >= 0.5 ? Biome::biomeGetID("TEST_1_2") : Biome::biomeGetID("TEST_1");
        } else if (biomeCat == 1) {
            ret = v2 >= 0.5 ? Biome::biomeGetID("TEST_2_2") : Biome::biomeGetID("TEST_2");
        } else if (biomeCat == 2) {
            ret = v2 >= 0.5 ? Biome::biomeGetID("TEST_3_2") : Biome::biomeGetID("TEST_3");
        } else if (biomeCat == 3) {
            ret = v2 >= 0.5 ? Biome::biomeGetID("TEST_4_2") : Biome::biomeGetID("TEST_4");
        }
    }

    // 查找失败 返回默认群系
    if (ret == -1) ret = Biome::biomeGetID("DEFAULT");

    return ret;
}

void world::addStructure(PlacedStructure str) {
    structures.push_back(str);

    for (int x = 0; x < str.base.w; x++) {
        for (int y = 0; y < str.base.h; y++) {
            int dx = x + loadZone.x + str.x;
            int dy = y + loadZone.y + str.y;
            Chunk *ch = new Chunk;
            ch->ChunkInit(floor(dx / CHUNK_W), floor(dy / CHUNK_H), worldName);
            // if(ch.e)
            if (dx >= 0 && dy >= 0 && dx < width && dy < height) {
                real_tiles[dx + dy * width] = str.base.tiles[x + y * str.base.w];
                dirty[dx + dy * width] = true;
            }
        }
    }
}

MEvec2 world::getNearestPoint(f32 x, f32 y) {
    f32 xm = fmod(1 + fmod(x, 1), 1);
    f32 ym = fmod(1 + fmod(y, 1), 1);
    f32 closestDist = 100;
    MEvec2 closest;
    for (int i = 0; i < distributedPoints.size(); i++) {
        f32 dx = distributedPoints[i].x - xm;
        f32 dy = distributedPoints[i].y - ym;
        f32 d = dx * dx + dy * dy;
        if (d < closestDist) {
            closestDist = d;
            closest = distributedPoints[i];
        }
    }
    return {closest.x + (x - xm), closest.y + (y - ym)};
}

std::vector<MEvec2> world::getPointsWithin(f32 x, f32 y, f32 w, f32 h) {
    f32 xm = fmod(1 + fmod(x, 1), 1);
    f32 ym = fmod(1 + fmod(y, 1), 1);

    std::vector<MEvec2> pts;
    for (f32 xo = floor(x) - 1; xo < ceil(x + w); xo++) {
        for (f32 yo = floor(y) - 1; yo < ceil(y + h); yo++) {
            for (int i = 0; i < distributedPoints.size(); i++) {
                if (distributedPoints[i].x + xo > x && distributedPoints[i].y + yo > y && distributedPoints[i].x + xo < x + w && distributedPoints[i].y + yo < y + h) {
                    pts.push_back({distributedPoints[i].x + xo, distributedPoints[i].y + yo});
                }
            }
        }
    }

    return pts;
}

Chunk *world::getChunk(int cx, int cy) {

    auto xx = chunkCache.find(cx);
    if (xx != chunkCache.end()) {
        auto yy = xx->second.find(cy);
        if (yy != xx->second.end()) {
            return yy->second;
        }
    }

    // METADOT_WARN(std::format("failed to load chunk {0}_{1} in chunkCache, so generate it", cx, cy).c_str());

    /*for (int i = 0; i < chunkCache.size(); i++) {
        if (chunkCache[i]->x == cx && chunkCache[i]->y == cy) return chunkCache[i];
    }*/
    Chunk *c = new Chunk;
    c->ChunkInit(cx, cy, worldName);
    c->generationPhase = -1;
    c->pleaseDelete = true;
    int a = Biome::biomeGetID("DEFAULT");
    if (c->biomes_id.size() != CHUNK_W * CHUNK_H) {
        // c->biomes_id.clear();
        // c->biomes_id.resize(CHUNK_W * CHUNK_H);
    }
    std::fill(c->biomes_id.begin(), c->biomes_id.end(), a);
    return c;
}

void world::populateChunk(Chunk *ch, int phase, bool render) {

    bool has = hasPopulator[phase];
    if (!hasPopulator[phase]) return;

    int ax = (ch->x - phase);
    int ay = (ch->y - phase);
    int aw = 1 + (phase * 2);
    int ah = 1 + (phase * 2);

    // TODO: 23/7/31 优化
    Chunk **chs = new Chunk *[aw * ah];

    bool *dirtyChunk = new bool[aw * ah]();

    if (phase == 1) {
        int a = 0;
    }

    for (int cx = ax; cx < ax + aw; cx++) {
        for (int cy = ay; cy < ay + ah; cy++) {
            chs[(cx - ax) + (cy - ay) * aw] = getChunk(cx, cy);
            dirtyChunk[(cx - ax) + (cy - ay) * aw] = false;
        }
    }

    for (int i = 0; i < populators.size(); i++) {
        if (populators[i]->getPhase() == phase) {
            std::vector<PlacedStructure> strs = populators[i]->apply(ch->tiles, ch->layer2, chs, dirtyChunk, ax * CHUNK_W, ay * CHUNK_H, aw * CHUNK_W, ah * CHUNK_H, ch, this);
            for (int j = 0; j < strs.size(); j++) {
                for (int tx = 0; tx < strs[j].base.w; tx++) {
                    for (int ty = 0; ty < strs[j].base.h; ty++) {
                        int chx = (int)floor((tx + strs[j].x) / (f32)CHUNK_W) + 1 - ch->x;
                        int chy = (int)floor((ty + strs[j].y) / (f32)CHUNK_H) + 1 - ch->y;
                        int dxx = (CHUNK_W + ((tx + strs[j].x) % CHUNK_W)) % CHUNK_W;
                        int dyy = (CHUNK_H + ((ty + strs[j].y) % CHUNK_H)) % CHUNK_H;
                        if (strs[j].base.tiles[tx + ty * strs[j].base.w].mat->physicsType != PhysicsType::AIR) {
                            chs[chx + chy * aw]->tiles[dxx + dyy * CHUNK_W] = strs[j].base.tiles[tx + ty * strs[j].base.w];
                            dirtyChunk[chx + chy * aw] = true;
                        }
                    }
                }
            }
        }
    }

    for (int x = 0; x < aw; x++) {
        for (int y = 0; y < ah; y++) {
            if (dirtyChunk[x + y * aw]) {
                if (x != aw / 2 && y != ah / 2) {
                    chs[x + y * aw]->ChunkWrite(chs[x + y * aw]->tiles, chs[x + y * aw]->layer2, chs[x + y * aw]->background);
                    if (render) {
                        // 保证 chs[x + y * aw] 的唯一性
                        std::erase(readyToMerge, chs[x + y * aw]);

                        // 加入合并列表
                        readyToMerge.push_back(chs[x + y * aw]);
                    }
                }
            }
        }
    }

    if (render) {
        // 保证 ch 的唯一性
        std::erase(readyToMerge, ch);
        // 加入合并列表
        readyToMerge.push_back(ch);
    }
}

void world::tickEntities(R_Target *t) {

    auto func = [&](WorldEntity *cur) {
        int nIntersect = 0;
        int avInX = 0;
        int avInY = 0;
        for (int xx = 0; xx < cur->hw; xx++) {
            for (int yy = 0; yy < cur->hh; yy++) {
                int sx = (cur->x + xx) + loadZone.x;
                int sy = (cur->y + yy) + loadZone.y;
                if (sx < 0 || sy < 0 || sx >= width || sy >= height) continue;

                if (real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SOLID || real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SAND ||
                    real_tiles[sx + sy * width].mat->physicsType == PhysicsType::OBJECT) {
                    nIntersect++;
                    avInX += (xx - cur->hw / 2);
                    avInY += (yy - cur->hh / 2);
                }
            }
        }
        if (nIntersect > 0) {
            cur->x += avInX > 0 ? -1 : (avInX < 0 ? 1 : 0);
            cur->y += avInY > 0 ? -1 : (avInY < 0 ? 1 : 0);
        }

        cur->vy += 0.25;

        if (cur->vx > 0.001) {
            f32 stx = cur->x;
            for (f32 dx = 0; dx < cur->vx; dx += cur->vx / 8.0) {
                f32 nx = stx + dx;
                f32 ny = cur->y;

                bool collide = false;
                for (int xx = 0; xx < cur->hw; xx++) {
                    for (int yy = 0; yy < cur->hh; yy++) {
                        int sx = (nx + xx) + loadZone.x;
                        int sy = (ny + yy) + loadZone.y;
                        if (sx >= 0 && sy >= 0 && sx < width && sy < height) {
                            if (real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SOLID || real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SAND ||
                                real_tiles[sx + sy * width].mat->physicsType == PhysicsType::OBJECT) {
                                if (yy == cur->hh - 1) {
                                    for (int xx1 = 0; xx1 < cur->hw; xx1++) {
                                        for (int yy1 = 0; yy1 < cur->hh; yy1++) {
                                            int sx1 = (nx + xx1) + loadZone.x;
                                            int sy1 = (ny + yy1) + loadZone.y - 1;
                                            if (sx1 >= 0 && sy1 >= 0 && sx1 < width && sy1 < height) {
                                                if (real_tiles[sx1 + sy1 * width].mat->physicsType == PhysicsType::SOLID || real_tiles[sx1 + sy1 * width].mat->physicsType == PhysicsType::SAND ||
                                                    real_tiles[sx1 + sy1 * width].mat->physicsType == PhysicsType::OBJECT) {
                                                    collide = true;
                                                }
                                            }
                                        }
                                    }
                                    if (!collide) {
                                        ny--;
                                    }
                                } else {
                                    MaterialInstance tp = real_tiles[sx + sy * width];
                                    if (tp.mat->physicsType == PhysicsType::SAND) {
                                        addCell(new CellData(tp, sx, sy, (rand() % 10 - 5) / 10.0f + 0.5f, (rand() % 10 - 5) / 10.0f, 0, 0.1f));
                                        real_tiles[sx + sy * width] = Tiles_NOTHING;
                                        dirty[sx + sy * width] = true;

                                        cur->vx *= 0.99;
                                    } else {
                                        collide = true;
                                    }
                                }
                            }
                        }
                    }
                }

                if (!collide) {
                    cur->x = nx;
                    cur->y = ny;
                } else {
                    cur->vx /= 2;
                    break;
                }
            }
        } else if (cur->vx < -0.001) {
            f32 stx = cur->x;
            for (f32 dx = 0; dx > cur->vx; dx += cur->vx / 8.0) {
                f32 nx = stx + dx;
                f32 ny = cur->y;

                bool collide = false;
                for (int xx = 0; xx < cur->hw; xx++) {
                    for (int yy = 0; yy < cur->hh; yy++) {
                        int sx = (nx + xx) + loadZone.x;
                        int sy = (ny + yy) + loadZone.y;
                        if (sx >= 0 && sy >= 0 && sx < width && sy < height) {
                            if (real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SOLID || real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SAND ||
                                real_tiles[sx + sy * width].mat->physicsType == PhysicsType::OBJECT) {
                                if (yy == cur->hh - 1) {
                                    for (int xx1 = 0; xx1 < cur->hw; xx1++) {
                                        for (int yy1 = 0; yy1 < cur->hh; yy1++) {
                                            int sx1 = (nx + xx1) + loadZone.x;
                                            int sy1 = (ny + yy1) + loadZone.y - 1;
                                            if (sx1 >= 0 && sy1 >= 0 && sx1 < width && sy1 < height) {
                                                if (real_tiles[sx1 + sy1 * width].mat->physicsType == PhysicsType::SOLID || real_tiles[sx1 + sy1 * width].mat->physicsType == PhysicsType::SAND ||
                                                    real_tiles[sx1 + sy1 * width].mat->physicsType == PhysicsType::OBJECT) {
                                                    collide = true;
                                                }
                                            }
                                        }
                                    }
                                    if (!collide) {
                                        ny--;
                                    }
                                } else {
                                    MaterialInstance tp = real_tiles[sx + sy * width];
                                    if (tp.mat->physicsType == PhysicsType::SAND) {
                                        addCell(new CellData(tp, sx, sy, (rand() % 10 - 5) / 10.0f - 0.5f, (rand() % 10 - 5) / 10.0f, 0, 0.1f));
                                        real_tiles[sx + sy * width] = Tiles_NOTHING;
                                        dirty[sx + sy * width] = true;

                                        cur->vx *= 0.99;
                                    } else {
                                        collide = true;
                                    }
                                }
                            }
                        }
                    }
                }

                if (!collide) {
                    cur->x = nx;
                    cur->y = ny;
                } else {
                    cur->vx /= 2;
                    break;
                }
            }
        }

        cur->ground = false;

        if (cur->vy > 0.001) {
            f32 sty = cur->y;
            for (f32 dy = 0; dy < cur->vy; dy += cur->vy / 8.0) {
                f32 ny = sty + dy;
                f32 nx = cur->x;

                bool collide = false;
                for (int xx = 0; xx < cur->hw; xx++) {
                    for (int yy = 0; yy < cur->hh; yy++) {
                        int sx = (nx + xx) + loadZone.x;
                        int sy = (ny + yy) + loadZone.y;
                        if (sx >= 0 && sy >= 0 && sx < width && sy < height) {
                            if (real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SOLID || real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SAND ||
                                real_tiles[sx + sy * width].mat->physicsType == PhysicsType::OBJECT) {
                                collide = true;
                            }
                        }
                    }
                }

                if (!collide) {
                    cur->y = ny;
                } else {
                    cur->vy /= 2;
                    cur->ground = true;
                    break;
                }
            }
        } else if (cur->vy < -0.001) {
            f32 sty = cur->y;
            for (f32 dy = 0; dy > cur->vy; dy += cur->vy / 8.0) {
                f32 ny = sty + dy;
                f32 nx = cur->x;

                bool collide = false;
                for (int xx = 0; xx < cur->hw; xx++) {
                    for (int yy = 0; yy < cur->hh; yy++) {
                        int sx = (nx + xx) + loadZone.x;
                        int sy = (ny + yy) + loadZone.y;
                        if (sx >= 0 && sy >= 0 && sx < width && sy < height) {
                            if (real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SOLID || real_tiles[sx + sy * width].mat->physicsType == PhysicsType::SAND ||
                                real_tiles[sx + sy * width].mat->physicsType == PhysicsType::OBJECT) {
                                MaterialInstance tp = real_tiles[sx + sy * width];
                                if (tp.mat->physicsType == PhysicsType::SAND) {
                                    addCell(new CellData(tp, sx, sy, (rand() % 10 - 5) / 10.0f, (rand() % 10 - 5) / 10.0f - 0.5f, 0, 0.1f));
                                    real_tiles[sx + sy * width] = Tiles_NOTHING;
                                    dirty[sx + sy * width] = true;

                                    cur->vy *= 0.99;
                                } else {
                                    collide = true;
                                }
                            }
                        }
                    }
                }

                if (!collide) {
                    cur->y = ny;
                } else {
                    cur->vy /= 2;
                    cur->ground = true;
                    break;
                }
            }
        }

        // 速度过快
        if (std::fabs(cur->vx) >= 1024.0f || std::fabs(cur->vy) >= 1024.0f) {
            return true;
        }

        // 速度衰减
        cur->vx *= 0.99;
        cur->vy *= 0.99;

        // cur->render(t, loadZone.x, loadZone.y);

        cur->rb->body->SetTransform(b2Vec2(cur->x + loadZone.x + cur->hw / 2 - 0.5, cur->y + loadZone.y + cur->hh / 2 - 1.5), 0);
        cur->rb->body->SetLinearVelocity({cur->vx * 25, cur->vy * 25});

        return false;
    };

    // worldEntities.erase(std::remove_if(worldEntities.begin(), worldEntities.end(), func), worldEntities.end());
    registry.for_each_component<WorldEntity>([&](ecs::entity e, WorldEntity &we) {
        if (global.game->Iso.globaldef.debug_entities_test) {
            R_Rectangle(this->target, we.x, we.y, we.x + we.hw, we.y + we.hh, {0xff, 0xff, 0xff, 0xff});
        }

        bool destroy = func(&we);
        if (destroy) {
            if ((ecs::exists<Player>{})(e)) this->player = 0;
            registry.destroy_entity(e);
        }
    });
}

// Adapted from https://stackoverflow.com/a/52859805/8267529
void world::forLine(int x0, int y0, int x1, int y1, std::function<bool(int)> fn) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    int dLong = abs(dx);
    int dShort = abs(dy);

    int offsetLong = dx > 0 ? 1 : -1;
    int offsetShort = dy > 0 ? width : -width;

    if (dLong < dShort) {
        std::swap(dShort, dLong);
        std::swap(offsetShort, offsetLong);
    }

    int error = dLong / 2;
    int index = y0 * width + x0;
    const int offset[] = {offsetLong, offsetLong + offsetShort};
    const int abs_d[] = {dShort, dShort - dLong};
    for (int i = 0; i <= dLong; ++i) {
        if (fn(index)) return;
        const int errorIsTooBig = error >= dLong;
        index += offset[errorIsTooBig];
        error += abs_d[errorIsTooBig];
    }
}

// Adapted from https://gamedev.stackexchange.com/a/182143
void world::forLineCornered(int x0, int y0, int x1, int y1, std::function<bool(int)> fn) {

    f32 sx = x0;
    f32 sy = y0;
    f32 ex = x1;
    f32 ey = y1;

    f32 x = floor(sx);
    f32 y = floor(sy);
    f32 diffX = ex - sx;
    f32 diffY = ey - sy;
    f32 stepX = (diffX > 0) ? 1 : ((diffX < 0) ? -1 : 0);
    f32 stepY = (diffY > 0) ? 1 : ((diffY < 0) ? -1 : 0);

    f32 xOffset = ex > sx ? (ceil(sx) - sx) : (sx - floor(sx));
    f32 yOffset = ey > sy ? (ceil(sy) - sy) : (sy - floor(sy));
    f32 angle = atan2(-diffY, diffX);
    f32 tMaxX = xOffset / cos(angle);
    f32 tMaxY = yOffset / sin(angle);
    f32 tDeltaX = 1.0 / cos(angle);
    f32 tDeltaY = 1.0 / sin(angle);

    f32 manhattanDistance = abs(floor(ex) - floor(sx)) + abs(floor(ey) - floor(sy));
    std::vector<int> visited = {};
    for (int t = 0; t <= manhattanDistance; ++t) {
        if (std::find(visited.begin(), visited.end(), x + y * width) == visited.end() && fn(x + y * width)) return;
        visited.push_back(x + y * width);
        if (abs(tMaxX) < abs(tMaxY) || isnan(tMaxY)) {
            tMaxX += tDeltaX;
            x += stepX;
        } else {
            tMaxY += tDeltaY;
            y += stepY;
        }
    }
}

bool world::isPlayerInWorld() { return player != 0; }

std::tuple<WorldEntity *, Player *> world::getHostPlayer() {
    Player *pl = nullptr;
    WorldEntity *pl_we = nullptr;
    if (player == 0) return std::make_tuple(pl_we, pl);
    try {
        pl = registry.find_component<Player>(player);
        pl_we = registry.find_component<WorldEntity>(player);
    } catch (...) {
        METADOT_ERROR("[Exception] world::getHostPlayer() failed");
    }
    return std::make_tuple(pl_we, pl);
}

RigidBody *world::physicsCheck(int x, int y) {

    if (getTile(x, y).mat->physicsType != PhysicsType::SOLID) return nullptr;

    bool *visited = new bool[width * height];

    memset(visited, false, (size_t)width * height);

    u32 *cols = new u32[width * height];

    memset(cols, 0x00, (size_t)width * height * sizeof(u32));  // init to all 0s

    int count = 0;
    int minX = width;
    int maxX = 0;
    int minY = height;
    int maxY = 0;

    physicsCheck_flood(x, y, visited, &count, cols, &minX, &maxX, &minY, &maxY);

    if (count > 0 && count <= 1000) {
        if (count > 10) {

            C_Surface *sfc = SDL_CreateRGBSurfaceWithFormat(0, maxX - minX + 1, maxY - minY + 1, 32, SDL_PIXELFORMAT_ARGB8888);

            for (int yy = minY; yy <= maxY; yy++) {
                for (int xx = minX; xx <= maxX; xx++) {
                    if (visited[xx + yy * width]) {
                        ME_get_pixel(sfc, (unsigned long long)(xx)-minX, yy - minY) = cols[xx + yy * width];
                        real_tiles[xx + yy * width] = Tiles_NOTHING;
                        dirty[xx + yy * width] = true;
                    }
                }
            }

            if (static_cast<bool>(visited)) delete[] visited;
            if (static_cast<bool>(cols)) delete[] cols;

            // audioEngine.PlayEvent("event:/Player/Impact");

            auto tex = create_ref<Texture>(sfc);

            b2PolygonShape s;
            s.SetAsBox(1, 1);
            RigidBody *rb = makeRigidBody(b2_dynamicBody, (f32)minX, (f32)minY, 0, s, 1, (f32)0.3, tex);
            b2Filter bf = {};
            bf.categoryBits = 0x0001;
            bf.maskBits = 0xffff;
            rb->body->GetFixtureList()[0].SetFilterData(bf);
            rb->body->SetLinearVelocity({(f32)((rand() % 100) / 100.0 - 0.5), (f32)((rand() % 100) / 100.0 - 0.5)});

            rigidBodies.push_back(rb);
            updateRigidBodyHitbox(rb);

            lastMeshLoadZone.x--;
            updateWorldMesh();

            return rb;
        } else {

            for (int yy = minY; yy <= maxY; yy++) {
                for (int xx = minX; xx <= maxX; xx++) {
                    if (visited[xx + yy * width]) {
                        real_tiles[xx + yy * width] = Tiles_NOTHING;
                        dirty[xx + yy * width] = true;
                    }
                }
            }

            return nullptr;
        }
    } else {
        delete[] visited;
        delete[] cols;
    }

    return nullptr;
}

// Helper for World::physicsCheck that does the 4-way recursive flood fill
void world::physicsCheck_flood(int x, int y, bool *visited, int *count, u32 *cols, int *minX, int *maxX, int *minY, int *maxY) {
    if (*count > 1000 || x < 0 || x >= width || y < 0 || y >= height) return;
    if (!visited[x + y * width] && getTile(x, y).mat->physicsType == PhysicsType::SOLID) {
        if (x < *minX) *minX = x;
        if (x > *maxX) *maxX = x;
        if (y < *minY) *minY = y;
        if (y > *maxY) *maxY = y;

        visited[x + y * width] = true;
        (*count)++;
        // setTile(x, y, MaterialInstance(&Materials::GENERIC_SOLID, 0xff00ffff));

        cols[x + y * width] = real_tiles[x + y * width].color;

        physicsCheck_flood(x + 1, y, visited, count, cols, minX, maxX, minY, maxY);
        physicsCheck_flood(x, y + 1, visited, count, cols, minX, maxX, minY, maxY);
        physicsCheck_flood(x - 1, y, visited, count, cols, minX, maxX, minY, maxY);
        physicsCheck_flood(x, y - 1, visited, count, cols, minX, maxX, minY, maxY);
    }
}

void world::saveWorld() {

    this->metadata.save(this->worldName);

    // std::vector<std::future<void>> results = {};

    // TODO: 23/7/31 修复 phmap 相关
    for (auto &p : this->chunkCache) {
        if (p.first == INT_MIN) continue;
        for (auto &p2 : p.second) {
            if (p2.first == INT_MIN) continue;

            // results.push_back(global.game->GameIsolate_.updateDirtyPool->push([&](int id) {
            Chunk *m = p2.second;
            this->unloadChunk(m);
            //}));
        }
    }

    // for (int i = 0; i < results.size(); i++) {
    //     results[i].get();
    // }
}

WorldMeta WorldMeta::loadWorldMeta(std::string worldFileName, bool noSaveLoad) {

    WorldMeta meta = WorldMeta();

    using json = Json::Json;

    if (!noSaveLoad) {

        char *metaFilePath = new char[255];
        snprintf(metaFilePath, 255, "%s/world.json", worldFileName.c_str());

        if (!std::filesystem::exists(metaFilePath)) {
            METADOT_INFO(std::format("New world meta @ {0}", metaFilePath).c_str());
            meta.save(worldFileName);
        }

        auto c_metafile = ME_fs_readfile(metaFilePath);
        json metafile = json::parse(c_metafile.c_str());

        if (!metafile.empty()) {

            json root = metafile["metadata"];

            meta.worldName = root["worldName"].to<std::string>();
            meta.lastOpenedVersion = root["lastOpenedVersion"].to<std::string>();
            meta.lastOpenedTime = root["lastOpenedTime"].to<int>();

            METADOT_INFO(std::format("Load World ({0} {1} {2})", meta.worldName.c_str(), meta.lastOpenedVersion.c_str(), meta.lastOpenedTime).c_str());
        } else {
            METADOT_BUG("FP WAS NULL");
        }

        delete[] metaFilePath;
    } else {
        json root;
        root = root["metadata"];

        meta.worldName = root["worldName"].to<std::string>();
        meta.lastOpenedVersion = root["lastOpenedVersion"].to<std::string>();
        meta.lastOpenedTime = root["lastOpenedTime"].to<int>();
    }

    return meta;
}

bool WorldMeta::save(std::string worldFileName) {

    std::string metaFilePath = std::format("{0}/world.json", worldFileName);

    if (this->worldName.empty()) this->worldName = "WorldName";
    if (this->lastOpenedVersion.empty()) this->lastOpenedVersion = std::to_string(ME_buildnum());

    using json = Json::Json;

    json metafile = json::object();
    json root = json::object();

    // meta::dostruct::for_each(*this, [&](const char *name, const auto &value) { root.add(name, value); });

    // for (auto it = root.begin(); it != root.end(); ++it) {
    //     std::cout << it.key() << ":" << (*it).dump() << std::endl;
    // }
    metafile.add("metadata", root);
    metafile.add("root_seed", global.game->RNG->root_seed);

    // std::cout << metafile.print() << std::endl;
    // std::string worldMetaData = "LoadWorldMeta = function()\nsettings_data = {}\n";
    // SaveLuaConfig(*this, "settings_data", worldMetaData);
    // worldMetaData += "return settings_data\nend";

    METADOT_INFO(std::format("Saving world ({0})", metafile["metadata"]["worldName"].to<std::string>().c_str()).c_str());
    std::ofstream o(metaFilePath);
    o << metafile.print();

    return true;
}

world::~world() {

    real_tiles.clear();
    delete[] flowX;
    delete[] flowY;
    delete[] prevFlowX;
    delete[] prevFlowY;
    real_layer2.clear();
    background.clear();

    for (auto &v : cells) {
        delete v;
    }
    cells.clear();

    world_sys.tickPool->clear_queue();
    world_sys.tickVisitedPool->clear_queue();
    world_sys.updateRigidBodyHitboxPool->clear_queue();

    // tickPool->stop(false);
    // delete tickPool;
    // tickVisitedPool->stop(false);
    // delete tickVisitedPool;
    // updateRigidBodyHitboxPool->stop(false);
    // delete updateRigidBodyHitboxPool;

    delete[] newTemps;

    delete[] dirty;
    delete[] layer2Dirty;
    delete[] backgroundDirty;
    delete[] lastActive;
    delete[] active;
    delete[] tickVisited1;
    delete[] tickVisited2;

    auto b2world_ptr = b2world.release();
    delete b2world_ptr;

    for (auto &v : rigidBodies) {
        delete v;
    }
    rigidBodies.clear();

    staticBody->clean();
    delete staticBody;

    for (auto &v : polys2s) {
        for (auto &v1 : v) {
            // if (static_cast<bool>(v1)) delete v1;
        }
        v.clear();
    }
    polys2s.clear();

    worldMeshes.clear();
    worldTris.clear();

    for (auto &v : worldRigidBodies) {
        delete v;
    }
    worldRigidBodies.clear();

    toLoad.clear();

    for (auto &ch : toLoadAsyncList) {
        // delete v;
    }
    toLoadAsyncList.clear();

    for (auto &v : readyToMerge) {
        v->ChunkDelete();
    }
    readyToMerge.clear();

    delete gen;

    structures.clear();

    distributedPoints.clear();

    for (auto &v : chunkCache) {
        for (auto &v2 : v.second) {
            if (abs(v2.first) >= 128) {
                METADOT_ERROR("Abnormal chunk delete %d", v2.first);
                continue;
            }
            v2.second->ChunkDelete();
            delete v2.second;
        }
        v.second.clear();
    }
    chunkCache.clear();

    for (auto &v : populators) {
        delete v;
    }
    populators.clear();

    delete[] hasPopulator;

    registry.for_each_component<WorldEntity>([&](const ecs::entity e, WorldEntity &we) {
        if (static_cast<bool>(we.rb)) delete we.rb;
    });
}

}  // namespace ME