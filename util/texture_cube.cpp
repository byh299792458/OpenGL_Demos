#include "texture_cube.hpp"
#include "stb_image/stb_image.h"
#include <iostream>
#include <exception>
#include <vector>
#include <GL/gl3w.h>

TextureCube::TextureCube(const std::vector<std::string>& faces, bool is_gramma_correct)
{
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width_, &height_, &num_channels_, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            stbi_image_free(data);
            throw std::exception(("Cubemap texture failed to load at path: " + faces[i]).c_str());
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

