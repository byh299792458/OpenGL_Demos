#ifndef __TEXTURE_2D_HPP__
#define __TEXTURE_2D_HPP__

#include <string>

class Texture2D{
public:
	Texture2D(const std::string& path, bool is_gramma_correct = false);
	unsigned int getTextureId(void)const noexcept { return id_; }
private:
	unsigned int id_;
	int width_, height_, num_channels_;
};

#endif // !__TEXTURE_HPP__
