#ifndef OPENGL_ENGINE_TEXTURE_H
#define OPENGL_ENGINE_TEXTURE_H

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>

class Texture {
public:
    Texture() = default;
    Texture(const std::string& path) : path_(path) {
        // texture 1
        // ---------
        glGenTextures(1, &textureId_);
        glBindTexture(GL_TEXTURE_2D, textureId_); 
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void loadData() {
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); 
        unsigned char *data = stbi_load(path_.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {

        }
        stbi_image_free(data);
    }

private:
    unsigned int textureId_;
    std::string path_ = "";
};

#endif // OPENGL_ENGINE_TEXTURE_H