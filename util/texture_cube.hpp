#ifndef __TEXTURE_CUBE_HPP__
#define __TEXTURE_CUBE_HPP__

#include <string>
#include <vector>

class TextureCube{
public:
	TextureCube(const std::vector<std::string>& faces, bool is_gramma_correct = false);
	unsigned int getTextureId(void)const noexcept { return id_; }
private:
	unsigned int id_;
	int width_, height_, num_channels_;
};

#endif // !__TEXTURE_CUBE_HPP__
