#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <IL/il.h> 
#include <IL/ilu.h>

#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

/*g++ main.cpp -std=c++11 -lGL -lGLU -lglut -lIL -lILu*/

//TODO: Migrate to SDL2 so Glut and IL can be removed.

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

struct fPoint
{
	float x, y;
};

struct fPoint3d
{
	float x, y, z;
};

struct fRot
{
	float a, x, y, z;
};

class QuadTex
{
	public:

	QuadTex(const Texture& t):texture(t), pos{0.f, 0.f, 0.f},
		rot{0.f, 0.f, 0.f, 0.f}
	{
		
	}

	QuadTex&	posicion(float px, float py, float pz) 
	{
		pos={px, py, pz};
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
	
	void		draw()
	{
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

	}

	private:

	const Texture&		texture;
	fPoint3d		pos;
	fRot			rot;
	std::vector<fPoint>	puntos, puntostex;
};

enum colors{COLOR_MODE_CYAN, COLOR_MODE_MULTI};
enum viewports{FULL=0, HALF_CENTER, HALF_TOP, VIEWPORT_MAX};
int 		color_mode=COLOR_MODE_CYAN,
		viewport_mode=FULL;
GLfloat 	xcam=0.f,
		ycam=0.f;
const int 
	w_screen=640,
	h_screen=480;
Texture tex, tex2, tex3;

//This shall dissapear.
void load_texture(Texture& t)
{
	const int w=128, h=128, tot=w*h;
	GLuint px[tot];
	for(int i=0; i < tot; ++i) 
	{ 
		GLubyte* colors = (GLubyte*)&px[i];

		if(i / 128 & 16 ^ i % 128 & 16) 
		{
			for(int j=0; j<4; ++j) colors[j]=0xFF;
		} 
		else 
		{ 
			for(int j=0; j<4; ++j) colors[j]=0x88;
		}
	}

	t.load(px, w, h);
}

void init_open_gl()
{
 	//Establecer viewport...
	glViewport(0.f, 0.f, w_screen, h_screen);

	//Establecer el modo a matriz de proyección y cargarla a 1.
	glMatrixMode(GL_PROJECTION); 	
	glLoadIdentity();

	//Izquierda, derecha, abajo, arriba, cerca y lejos...
	//Ahora el punto 0.0 es arriba a la izquierda.
	glOrtho(0.0, w_screen, h_screen, 0.0, 1.0, -1.0);

	//Establecer el modo a matriz de proyección y cargarla a 1.
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();

	//Habilitar texturas...
	glEnable(GL_TEXTURE_2D);

	//Guardar la matriz de modelo...
	glPushMatrix();

	//Establecer a que los vértices vayan en el sentido de las agujas del reloj.
	//glFrontFace(GL_CW);

	//Establecer el color de fondo.
	glClearColor(0.f, 0.f, 0.f, 1.f);

	if(glGetError() != GL_NO_ERROR ) 
	{
		auto err=reinterpret_cast<const char *>(gluErrorString(glGetError()));
		std::string msg="Error: "+std::string(err);
		throw std::runtime_error(msg);
	}

	 //DevIL 
	ilInit(); 
	ilClearColour(255, 255, 255, 000); 
	if(ilGetError() != IL_NO_ERROR) 
	{
//		auto err=reinterpret_cast<const char *>(gluErrorString(glGetError()));
		std::string msg="Error: "+std::string(iluErrorString(ilGetError()));
		throw std::runtime_error(msg);
	}	
}

void cycle_viewport()
{
	++viewport_mode;
	if(viewport_mode==VIEWPORT_MAX) viewport_mode=FULL;

	//x, y, w, h

	float x=0.f, y=0.f, w=w_screen, h=h_screen;

	switch(viewport_mode)
	{
		case FULL:break;
		case HALF_CENTER:	
			x=w_screen/4.f;
			y=h_screen/4.f;
			w=w_screen/2.f;
			h=h_screen/2.f;
		break;
		case HALF_TOP:		
			x=w_screen/4.f;
			y=h_screen/2.0;
			w=w_screen/2.f;
			h=h_screen/2.f;
		break;
	}	

	glViewport(x, y, w, h); 
}

void update()
{

}

void render()
{
	//Limpiar la pantalla...
	glClear(GL_COLOR_BUFFER_BIT);

	//Cambiamos a la matriz de modelo, sacando la que ya ha cambiado con
	//la cámara. Además, la guardamos de nuevo...
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPushMatrix();

	//Cargamos la identidad para que los cambios inferiores no se acumulen.
	//glLoadIdentity();

	//Traslación de lo que dibujemos en x, y y z. Centrar en pantalla, básicamente
	//glTranslatef(w_screen/2.f, h_screen/2.f, 0.f);

	//TODO: Read this shit from files :D!.

	//TODO: Do occlusion test.

	QuadTex qt3(tex);
	qt3.posicion(0.f, 0.f, 0.f)
	.punto(0.f, 0.f, 0.f, 0.f)
	.punto(100.f, 0.f, 1.f, 0.f)
	.punto(100.f, 100.f, 1.f, 1.f)
	.punto(0.f, 100.f, 0.f, 1.f)
	.draw();

	QuadTex qt4(tex);
	qt4.posicion(100.f, 0.f, 0.f)
	.punto(0.f, 0.f, 0.f, 0.f)
	.punto(100.f, 0.f, 1.f, 0.f)
	.punto(100.f, 100.f, 1.f, 1.f)
	.punto(0.f, 100.f, 0.f, 1.f)
	.draw();

	//Dibujar con textura full...
	QuadTex qt2(tex2);
	qt2.posicion(0.f, 100.f, 0.f)
	.rotacion(45.0f, 0.f, 0.f, 1.f)
	.punto(0.f, 0.f, 0.f, 0.f)
	.punto(100.f, 0.f, 1.f, 0.f)
	.punto(100.f, 100.f, 1.f, 1.f)
	.punto(0.f, 100.f, 0.f, 1.f)
	.draw();

	//Dibujar con textura troceada...
	//TODO: Translate texture coordinates to pixels???.
	QuadTex qt(tex3);
	qt.posicion(100.f, 100.f, 0.f)
	.punto(0.f, 0.f, 0.f, 0.f)
	.punto(100.f, 0.f, 1.f / 8.f, 0.f)
	.punto(100.f, 100.f, 1.f / 8.f, 1.f / 8.f)
	.punto(0.f, 100.f, 0.f, 1.f / 8.f)
	.draw();
	
	//Intercambiar buffers.
	glutSwapBuffers();
}

void do_main_loop(int v)
{
	update(); 
	render(); 
	glutTimerFunc(1000 / 60, do_main_loop, 0);
}

void do_input_handling(unsigned char k, int x, int y)
{
	float cx=xcam, cy=ycam;

	switch(k)
	{
		case 'w': ycam+=8.f; break;
		case 'a': xcam+=8.f; break;
		case 's': ycam-=8.f; break;
		case 'd': xcam-=8.f; break;
		case 'q': throw new std::runtime_error("We're done here"); break;
		case 'z': color_mode=color_mode==COLOR_MODE_CYAN ? COLOR_MODE_MULTI : COLOR_MODE_CYAN; break;
		case 'x': cycle_viewport(); break;
	}

	//Actualizar matriz de modelo: sacamos la que hay, la cargamos a cero
	//y la trasladamos según la cámara. Luego la volvemos a guardar.
	if(cx!=xcam || cy!=ycam)
	{
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
		glLoadIdentity();
		glTranslatef(-xcam, -ycam, 0.f);
		glPushMatrix();
	}
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
	//Arrancar glut, que toma el control ¬¬...
	glutInit(&argc, argv);
	
	//Iniciar con versión 2.1.
	glutInitContextVersion(2,1);

	//Establecer a doble buffer, crear pantalla.
	glutInitDisplayMode(GLUT_DOUBLE); 
	glutInitWindowSize(w_screen, h_screen); 
	glutCreateWindow("Test");

	try
	{
		init_open_gl();
		load_texture(tex);
		tex2.load_from_file("cosa.png");
		tex3.load_from_file("template_tiles.png");

		//Indicar función de input...
		glutKeyboardFunc(do_input_handling);

		//Indicar dirección de función de render.
		glutDisplayFunc(render);

		//Indicar dirección de función de loop principal, que la llame 60 veces por segundo.
		glutTimerFunc(1000 / 60, do_main_loop, 0);

		//Hacer loop.
		glutMainLoop();
	}
	catch(std::exception& e)
	{
		std::cout<<"Exception caught "<<e.what()<<std::endl;
		return 1;
	}

	return 0;
}
	//TODO: Take this to a class...
/*
	auto do_color=[](int color_mode, int i)
	{
		if(color_mode==COLOR_MODE_CYAN) glColor3f(0.f, 1.f, 1.f);
		else switch(i)
		{
			case 0: glColor3f(1.f, 0.f, 0.f); break;
			case 1: glColor3f(1.f, 1.f, 0.f); break;
			case 2: glColor3f(0.f, 1.f, 0.f); break;
			case 3: glColor3f(0.f, 0.f, 1.f); break;
		}
	};

	int i=0;
	//Iniciar dibujo de polígono... en este caso un quad.

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glColor3f(.2f, .2f, .2f);
        	glVertex2f(-w_screen / 3.f, -h_screen / 3.f);
	        glVertex2f(-w_screen / 3.f, h_screen / 3.f);
	        glVertex2f(w_screen / 3.f, h_screen / 3.f);
	        glVertex2f(w_screen / 3.f, -h_screen / 3.f);
	glEnd();

	glBegin(GL_QUADS);
		do_color(color_mode, i++);
        	glVertex2f(-150.f, -150.f);
		do_color(color_mode, i++);
	        glVertex2f(-150.f, 150.f);
		do_color(color_mode, i++);
	        glVertex2f(150.f, 150.f);
		do_color(color_mode, i++);
	        glVertex2f(150.f, -150.f);
	glEnd();
*/
