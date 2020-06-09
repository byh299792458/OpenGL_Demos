#include "texture2d.hpp"
#include "stb_image/stb_image.h"
#include <iostream>
#include <exception>
#include <GL/gl3w.h>

Texture2D::Texture2D(const std::string& path, bool is_gramma_correct)
{
	glGenTextures(1, &id_);
    unsigned char* data = stbi_load(path.c_str(), &width_, &height_, &num_channels_, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (num_channels_ == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (num_channels_ == 3)
        {
            internalFormat = is_gramma_correct ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (num_channels_ == 4)
        {
            internalFormat = is_gramma_correct ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, id_);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        stbi_image_free(data);
        throw std::exception(("Texture failed to load at path: " + path).c_str());
    }
}

