#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../contrib/stbimage/stb_image.h"

class Texture {
public:
	enum class WrapMode {
		Clamp,
		Repeat
	};

	enum class FilterMode {
		Nearest,
		Bilinear,
		Trilinear
	};

	enum class Format {
		R,
		RG,
		RGB,
		RGBA
	};

	enum class Channels {
		Bits_8,
		Bits_16,
		Bits_32
	};


	int w, h;
	GLuint index, formatSize;
	GLenum internal_format, format;

public:

	Texture() {
	}

	//~Texture() {
	//	glDeleteTextures(1, &index);
	//}

	Texture(int test){
		int tex_width = 256;
		int tex_height = 256;
		int tex_depth = 256;
		int tex_size = tex_width * tex_height * tex_depth;

		char* data = new char[tex_size];
		
		for(unsigned int w = 0; w < tex_width; w++){
			for(unsigned int h = 0; h < tex_height; h++){
				for(unsigned int d = 0; d < tex_depth; d++){

				}
			}
		}

		//create and bind texture
		glGenTextures(1, &index);
		glBindTexture(GL_TEXTURE_3D, index);

		//set filtering and wrapping
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, tex_width, tex_height, tex_depth, 0, GL_RED, GL_FLOAT, &data);
	}

	Texture(int w, int h, GLenum internal_format, GLenum format, int type){

		this->w = w;
		this->h = h;
		this->format = format;
		this->internal_format = internal_format;

		switch (format) {
			case GL_RED: this->formatSize = 1; break;
			case GL_RG: this->formatSize = 2; break;
			case GL_RGB: this->formatSize = 3; break;
			case GL_RGBA: this->formatSize = 4; break;
			default: this->formatSize = -1; break;
		}

		glGenTextures(1, &index);
		glBindTexture(GL_TEXTURE_2D, index);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, type, 0);
	}

	Texture(int w, int h, GLenum internal_format, GLenum format, int type, unsigned char *data) {

		this->w = w;
		this->h = h;
		this->format = format;
		this->internal_format = internal_format;

		switch (format) {
		case GL_RGB: this->formatSize = 3; break;
		case GL_RGBA: this->formatSize = 4; break;
		default: this->formatSize = -1; break;
		}

		glGenTextures(1, &index);
		glBindTexture(GL_TEXTURE_2D, index);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, type, data);
	}

	Texture(std::string path, WrapMode wrapMode, FilterMode filterMode) {
		int width, height, nrComponents;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 3);

		std::cout << "Texture w: " << width << std::endl;
		std::cout << "Texture h: " << height << std::endl;
		std::cout << "Texture nrComponents: " << nrComponents << std::endl;

		this->w = width;
		this->h = height;

		std::vector<unsigned char> dataVector;
		for (unsigned int h = 0; h < height; h++) {
			for (unsigned int w = 0; w < width; w++) {
				for(unsigned int c = 0; c < nrComponents; c++){
					unsigned int k = (nrComponents) * (w + h * width) + c;
					dataVector.push_back(data[k]);
				}
				dataVector.push_back(255);
			}
		}

		glGenTextures(1, &index);
		glBindTexture(GL_TEXTURE_2D, index);
		setWrapMode(wrapMode);
		setFilterMode(filterMode);

		glTexImage2D(GL_TEXTURE_2D, 0, setInternalFormat(getFormatFromNumChannels(nrComponents), Channels::Bits_8), 
			width, height, 0, setFormat(getFormatFromNumChannels(nrComponents)), GL_UNSIGNED_BYTE, static_cast<void*>(dataVector.data()));

		free(data);
	}

	void Bind() {
		glBindTexture(GL_TEXTURE_2D, index);
	}

	int Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
		return index;
	}

	GLuint Index() const { 
		return index; 
	}
	
	unsigned char* transferToCPU() {
		glBindTexture(GL_TEXTURE_2D, index);
		
		GLint numBytes = 0;
		numBytes = this->w * this->h * formatSize;

		if (numBytes > 0) {
			unsigned char *pixels = (unsigned char*)malloc(numBytes); // allocate image data into RAM

			//glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_UNSIGNED_BYTE, pixels);
			glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, pixels);

			glBindTexture(GL_TEXTURE_2D, 0);
			return pixels;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		return nullptr;
	}

	void setWrapMode(WrapMode wrapMode) {
		switch (wrapMode) {
			case WrapMode::Repeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case WrapMode::Clamp:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				break;
		}
	}

	void setFilterMode(FilterMode filterMode) {
		switch (filterMode) {
			case FilterMode::Nearest: 		
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case FilterMode::Bilinear: 				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::Trilinear: 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			default:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
		}

	}

	GLuint setFormat(Format format) {
		switch (format) {
		case Format::R:
			return GL_RED;
			break;
		case Format::RG:
			return GL_RG;
			break;
		case Format::RGB:
			return GL_RGB;
		case Format::RGBA:
			return GL_RGBA;
		default:
			return GL_RGBA;
		}
	}
	
	Format getFormatFromNumChannels(int channels) {
		switch (format) {
		case 1:
			return Format::R;
			break;
		case 2:
			return Format::RG;
			break;
		case 3:
			return Format::RGB;
		case 4:
			return Format::RGBA;
		default:
			return Format::RGBA;
		}
	}

	GLuint setInternalFormat(Format format, Channels channel) {
		switch (format) {
		case Format::R:
			switch (channel) {
				case  Channels::Bits_8: return GL_R8; break;
				case  Channels::Bits_16: return GL_R16; break;
				case  Channels::Bits_32: return GL_R32F; break;
				default: return GL_R32F; break;
			}
		case Format::RG:
			switch (channel) {
				case  Channels::Bits_8: return GL_RG8; break;
				case  Channels::Bits_16: return GL_RG16; break;
				case  Channels::Bits_32: return GL_RG32F; break;
				default: return GL_RG32F; break;
			}
		case Format::RGB:
			switch (channel) {
				case  Channels::Bits_8: return GL_RGB8; break;
				case  Channels::Bits_16: return GL_RGB16; break;
				case  Channels::Bits_32: return GL_RGB32F; break;
				default: return GL_RGB32F; break;
			}
		case Format::RGBA:
			switch (channel) {
				case  Channels::Bits_8: return GL_RGBA8; break;
				case  Channels::Bits_16: return GL_RGBA16; break;
				case  Channels::Bits_32: return GL_RGBA32F; break;
				default: return GL_RGBA32F; break;
			}
		default:
			return GL_RGBA;
		}
	}
};

#endif
