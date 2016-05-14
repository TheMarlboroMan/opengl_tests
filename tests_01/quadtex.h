#ifndef QUADTEX_H
#define QUADTEX_H

#include "texture.h"
#include "defs.h"

class QuadTex
{
	public:

	QuadTex(const Texture& t):texture(t), pos{0.f, 0.f, 0.f},
		textr{0.f, 0.f, 0.f},
		rot{0.f, 0.f, 0.f, 0.f}
	{
		
	}

	QuadTex&	posicion(float px, float py, float pz) 
	{
		pos={px, py, pz};
		return *this;
	}

	QuadTex&	transformar_textura(float px, float py, float pz) 
	{
		textr={px, py, pz};
		return *this;
	}

	QuadTex&	rotacion(float pa, float px, float py, float pz) 
	{
		rot={pa, px, py, pz};
		return *this;
	}

	QuadTex&	punto(float px, float py, float tx, float ty) 
	{
		puntos.push_back({px, py});
		puntostex.push_back({tx, ty});
		return *this;
	}
	
	void		draw() const
	{
		if(textr.x || textr.y || textr.z)
		{
			glMatrixMode(GL_TEXTURE);
			glTranslatef(textr.x, textr.y, textr.z);
		}

		glMatrixMode(GL_MODELVIEW);
		glTranslatef(pos.x, pos.y, pos.z); //Mover...
		if(rot.a) glRotatef(rot.a, rot.x, rot.y, rot.z);

		glColor3f(1.f, 1.f, 1.f); //Demodular color...
		glEnable(GL_TEXTURE_2D); //Activa
		glBindTexture(GL_TEXTURE_2D, texture.get_id()); //Cargar textura.
		glBegin(GL_QUADS);
		auto pit=std::begin(puntos), tit=std::begin(puntostex);
		while(pit < std::end(puntos))
		{
			glTexCoord2f(tit->x, tit->y);
			glVertex2f(pit->x, pit->y);
			++pit;
			++tit;
		}
		glEnd();

		//Restaurar traslación.
		if(rot.a) glRotatef(-rot.a, rot.x, rot.y, rot.z);
		glTranslatef(-pos.x, -pos.y, -pos.z);

		if(textr.x || textr.y || textr.z)
		{
			glMatrixMode(GL_TEXTURE);
			glTranslatef(-textr.x, -textr.y, -textr.z);
		}
	}

	private:

	const Texture&		texture;
	fPoint3d		pos, textr;
	fRot			rot;
	std::vector<fPoint>	puntos, puntostex;
};

#endif
