#include <iostream>
#include <cstring>
#include <SFML_Graphics.hpp>
//#include <SFML_OpenGL.hpp>
#include <glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <math.h>
#include <unordered_set>

#include "war_window.h"
#include "war_shader.h"
#include "war_transform.h"
#include "war_camera.h"
#include "war_vao.h"
#include "tinyobjloader.h"
#include "war_content.h"
#include "war_world.h"

using namespace std;

#undef main
int main(int argc, char *argv[])
{
    //int casda = getchar();
    InitWindow();

    // InitWorld() here?
    srand(time(0));

    gladLoadGL();

    projection = glm::perspective(glm::radians(90.0f), (float)window.getSize().x / window.getSize().y, 0.1f, 1000.0f);
    view = glm::mat4(1.0f);
    model = glm::mat4(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    if (!sf::Shader::isAvailable()) {
        throw std::runtime_error("Shaders are not available");
    }

    Content content;
    content.initContent("test_save");

    camera.position.z += 16;
    camera.position.x += 16;
    camera.position.y += 32;

    InitWorld();

    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized){
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
                projection = glm::perspective(glm::radians(90.0f), (float)event.size.width / event.size.height, 0.1f, 1000.0f);
                window_center = sf::Vector2i(window.getSize() / 2u);
            }
            else if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::W){
                    moveForward = true;
                }
                else if (event.key.code == sf::Keyboard::S){
                    moveBack = true;
                }
                else if (event.key.code == sf::Keyboard::A){
                    moveLeft = true;
                }
                else if (event.key.code == sf::Keyboard::D){
                    moveRight = true;
                }
                else if (event.key.code == sf::Keyboard::Space){
                    moveUp = true;
                }
                else if (event.key.code == sf::Keyboard::LShift){
                    moveDown = true;
                }
                else if (event.key.code == sf::Keyboard::Escape){
                    if (cursor){
                        cursor = false;
                        window.setMouseCursorVisible(false);
                        sf::Mouse::setPosition(window_center, window); // For keeping camera angle after returning from ESC mode
                    }
                    else {
                        cursor = true;
                        window.setMouseCursorVisible(true);
                    }
                }
            }
            else if (event.type == sf::Event::KeyReleased){
                if (event.key.code == sf::Keyboard::W){
                    moveForward = false;
                }
                else if (event.key.code == sf::Keyboard::S){
                    moveBack = false;
                }
                else if (event.key.code == sf::Keyboard::A){
                    moveLeft = false;
                }
                else if (event.key.code == sf::Keyboard::D){
                    moveRight = false;
                }
                else if (event.key.code == sf::Keyboard::Space){
                    moveUp = false;
                }
                else if (event.key.code == sf::Keyboard::LShift){
                    moveDown = false;
                }
            }
        }

        /* Render here */
        glClearColor(0.6,0.8,1,0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MoveCamera();

        model = glm::mat4(1.0);
        //shader.setUniform("mvp", sf::Glsl::Mat4(glm::value_ptr(projection * view)));
        DrawWorld(&camera);
        //sf::Shader::bind(NULL);
        window.display();
    }

    return 0;
}
