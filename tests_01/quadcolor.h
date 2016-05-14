#ifndef QUADCOLOR_H
#define QUADCOLOR_H

#include "defs.h"

class QuadColor
{
	public:

	QuadColor(): r(0.f), g(0.f), b(0.f), a(0.f), 
		pos{0.f, 0.f, 0.f},
		rot{0.f, 0.f, 0.f, 0.f},
		indices{0,1,2,3},
		vertexbuffer_id{0},
		prepared(false)
	{
		
	}

	QuadColor&	color(float pr, float pg, float pb, float pa) 
	{
		r=pr; g=pg; b=pb; a=pa;
		return *this;
	}

	QuadColor&	posicion(float px, float py, float pz) 
	{
		pos={px, py, pz};
		return *this;
	}

	QuadColor&	rotacion(float pa, float px, float py, float pz) 
	{
		rot={pa, px, py, pz};
		return *this;
	}

	QuadColor&	punto(float px, float py) 
	{
		puntos.push_back({px, py});
		return *this;
	}

	void		prepare()
	{
//		glGenBuffers(1, &vertexbuffer_id); 
//		glBindBufferARB(GL_ARRAY_BUFFER, vertexbuffer_id); 
//		glBufferDataARB(GL_ARRAY_BUFFER, 4 * sizeof(fPoint), puntos.data(), GL_STATIC_DRAW);

//		glGenBuffers(1, &indexbuffer_id);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer_id);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		prepared=true;
	}
	
	void		draw() const
	{
		if(!prepared) 
		{
			std::cout<<"Colorquad not prepared"<<std::endl;
			throw std::runtime_error("ColorQuad not prepared");		
		}

		glMatrixMode(GL_MODELVIEW);
		glTranslatef(pos.x, pos.y, pos.z); //Mover...
		if(rot.a) glRotatef(rot.a, rot.x, rot.y, rot.z);

		//TODO: Alpha no funciona.
		glColor4f(r, g, b, a); //Demodular color...
		glDisable(GL_TEXTURE_2D);

		//TODO: http://lazyfoo.net/tutorials/OpenGL/17_vertex_buffer_objects/index.php
		//TODO: Instead of draw we would go "prepare" first.
		//TODO: Check by making this thing global, or something.
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, puntos.data());
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);

		//Restaurar traslación.
		if(rot.a) glRotatef(-rot.a, rot.x, rot.y, rot.z);
		glTranslatef(-pos.x, -pos.y, -pos.z);
	}

	private:

	float			r,g,b,a;
	fPoint3d		pos;
	fRot			rot;
	std::vector<fPoint>	puntos;
	GLuint 			vertexbuffer_id, indexbuffer_id;
	std::vector<GLuint> 	indices;
	bool 			prepared;
};

#endif
