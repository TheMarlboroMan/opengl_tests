#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
	public:
	
		Texture():id(0), width(0), height(0) {}
		~Texture()
	{
		free();
	}

	void	load(GLuint* pixels, GLuint width, GLuint height)
	{
		free();
		//Genera un id de textura.
		glGenTextures(1, &id); 

		//Y la bindea...
		glBindTexture(GL_TEXTURE_2D, id); 

		//Reservar memoria.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		//Establecer comportamiento.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Desmontar la textura para que no salga en geometria...
		glBindTexture(GL_TEXTURE_2D, 0); 

		if(glGetError() != GL_NO_ERROR)
		{
			auto err=reinterpret_cast<const char *>(gluErrorString(glGetError()));
			std::string msg="Error: "+std::string(err);
			throw std::runtime_error(msg);
		}

		glDisable(GL_TEXTURE_2D); //Activa
	}

	void load_from_file(const std::string& filename)
	{
		ILuint id=0; 
		ilGenImages(1, &id); 
		ilBindImage(id);

		if(ilLoadImage(filename.c_str())!= IL_TRUE) throw std::runtime_error("Error: unable to load "+filename);
		if(ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) != IL_TRUE) throw std::runtime_error("Error: unable to convert "+filename);
		load((GLuint*)ilGetData(), (GLuint)ilGetInteger(IL_IMAGE_WIDTH), (GLuint)ilGetInteger(IL_IMAGE_HEIGHT));
		ilDeleteImages(1, &id);		
	}

	void	free()
	{
		if(id)
		{
			glDeleteTextures(1, &id); 
			id=0;
			width=0;
			height=0;
		}
	}

	GLuint	get_id() const {return id;}
	GLuint	get_width() const {return width;}
	GLuint	get_height() const {return height;}

	private:

	GLuint		id,
			width,
			height;
};

#endif
