#ifndef __SGE_SCENE_MANAGER_HPP
#define __SGE_SCENE_MANAGER_HPP

#include <sge_engine-forward.hpp>
#include <sge_node.hpp>

#include <unordered_map>
#include <memory>
#include <string>

namespace sge
{
    class Scene
    {
        public:
            Scene();

            virtual void load(Engine &engine) = 0;
            virtual void unload(Engine &engine) = 0;

            std::shared_ptr<Node> get_root_node() const;

        protected:
            std::shared_ptr<Node> root_node;
    };

    class SceneManager
    {
        public:
            SceneManager(Engine &engine);
            ~SceneManager();

            void add_scene(const std::string &name, std::shared_ptr<Scene> scene);
            void switch_to_scene(const std::string &name);

            std::shared_ptr<Node> get_scene_node();

            bool event_handler(SDL_Event *event);
            void process_handler(Uint32 delta);
            void draw_handler();

        private:
            Engine &engine;
            std::shared_ptr<Scene> current_scene;

            std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
    };
}

#endif /* __SGE_SCENE_MANAGER_HPP */
