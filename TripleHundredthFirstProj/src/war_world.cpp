#include "war_world.h"
#include "war_shader.h"
#include "war_camera.h"

std::unordered_map<long long, Chunk> world;

sf::Shader world_shader;
sf::Texture atlas_texture;

void InitWorld()
{
    if (!world_shader.loadFromFile("shaders/world.vert", "shaders/world.frag")) {
        throw std::runtime_error("World shader(s) file(s) not found");
    }
    atlas_texture.loadFromImage(atlas);
    world_shader.setUniform("atlas", atlas_texture);
}

void AddObject(int x, int y, Object & obj)
{
    world[coordsToKey(x, y)].objects.push_back(obj);
}

void UpdateWorld(const float & dt)
{
    #warning unused function UpdateWorld
}

void DrawWorld(Transform * camera)
{
    sf::Shader::bind(&world_shader);
    world_shader.setUniform("projection", sf::Glsl::Mat4(glm::value_ptr(projection)));
    world_shader.setUniform("view", sf::Glsl::Mat4(glm::value_ptr(view)));
    bool need_recalc = false;
    int view_dist = 10;
    for (int z = floor(camera->position.z / CHUNK_SIZE) - view_dist; z <= floor(camera->position.z / CHUNK_SIZE) + view_dist; ++z) {
        for (int x = floor(camera->position.x / CHUNK_SIZE) - view_dist; x <= floor(camera->position.x / CHUNK_SIZE) + view_dist; ++x) {
            long long key = coordsToKey(x, z);
            if (world.find(key) == world.end()) {
                world.emplace(std::piecewise_construct,
                    std::forward_as_tuple(key),
                    std::forward_as_tuple(x, z));
            }
            if (!world[key].generated) {
                need_recalc = true;
                world[key].Generate();
            }
        }
    }

    if (need_recalc) {
        for (int z = floor(camera->position.z / CHUNK_SIZE) - view_dist; z <= floor(camera->position.z / CHUNK_SIZE) + view_dist; ++z) {
            for (int x = floor(camera->position.x / CHUNK_SIZE) - view_dist; x <= floor(camera->position.x / CHUNK_SIZE) + view_dist; ++x) {
                long long key = coordsToKey(x, z);
                world[key].Recalculate();
            }
        }
    }

    for (int z = floor(camera->position.z / CHUNK_SIZE) - view_dist; z <= floor(camera->position.z / CHUNK_SIZE) + view_dist; ++z) {
        for (int x = floor(camera->position.x / CHUNK_SIZE) - view_dist; x <= floor(camera->position.x / CHUNK_SIZE) + view_dist; ++x) {
            long long key = coordsToKey(x, z);
            world_shader.setUniform("model", sf::Glsl::Mat4(
                glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(x * (int)CHUNK_SIZE, 0, z * (int)CHUNK_SIZE)))
            ));

            std::cout << "Drawing " << x << "; " << z << std::endl;
            world_shader.setUniform("textures", world[key].textures);
            world[key].Draw();
            //sf::sleep(sf::milliseconds(20));

            //for ()//auto & obj : world[key].objects)
            //{
                //obj.render(target, content);
            //}
        }
    }
}
