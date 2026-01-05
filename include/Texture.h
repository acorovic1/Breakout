#pragma once
#define TEXTURE_H

#include <glad/glad.h>

class Texture2D
{
public:
	unsigned int ID;
	unsigned int Width, Height; // width and height of the image in pixels
	unsigned int InternalFormat; // format of texture object
	unsigned int ImageFormat; // format of loaded image

	unsigned int Wrap_S;	// wrapping mode on S axis
	unsigned int Wrap_T;	// wrapping mode on T axis
	unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
	unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels	

	Texture2D(); // sets default texture modes 

	void Generate(unsigned int width, unsigned int height, unsigned char* data); // generates texture from image data
	void Bind() const; // binds the texture as the current active GL_TEXTURE_2D texture object
};