//Con GL.
//#include <GL/gl.h>
//#include <GL/freeglut.h>
//#include <GL/glu.h>

//Con GLEW.
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <IL/il.h> 
#include <IL/ilu.h>

#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

/*g++ main.cpp -std=c++11 -lGL -lGLU -lglut -lIL -lILU -lGLEW*/

//TODO: Migrate to SDL2 so Glut and IL can be removed.

#include "texture.h"
#include "defs.h"
#include "quadcolor.h"
#include "quadtex.h"

GLfloat 	xcam=0.f,
		ycam=0.f;
const int 
	w_screen=640,
	h_screen=480;

Texture tex, tex2, tex3, tex4;
std::vector<QuadColor> quadcolors;
std::vector<QuadTex> quadtexs;

void init_open_gl()
{
	 //Initialize GLEW
	GLenum glewError=glewInit(); 
	if(glewError!=GLEW_OK)
	{
		auto err=reinterpret_cast<const char *>(glewGetErrorString(glewError));
		std::string msg="Error: "+std::string(err);
		throw std::runtime_error(msg);
	} 

//TODO... Hmmmm...
	//Make sure OpenGL 2.1 is supported 
//	if(!GLEW_VERSION_2_1)
//	{		
//		throw std::runtime_error("No Glew 2.1 supported.");
//	}

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

void setup_geometry()
{
	//TODO: Read this shit from files :D!.
	//TODO: Do occlusion test.

	QuadColor qc1;
	qc1.color(1.f, 0.f, 0.f, .1f)
	.posicion(0.f, 0.f, 0.f)
	.punto(0.f, 0.f)
	.punto(300.f, 0.f)
	.punto(300.f, 300.f)
	.punto(0.f, 300.f);

	QuadColor qc2;
	qc2.color(.5f, 0.1f, 0.f, .1f)
	.posicion(200.f, 0.f, 0.f)
	.punto(0.f, 0.f)
	.punto(100.f, 0.f)
	.punto(100.f, 100.f)
	.punto(0.f, 100.f);

	quadcolors.push_back(qc1);
	quadcolors.push_back(qc2);
	for(auto& q : quadcolors) q.prepare();

	QuadTex qt3(tex);
	qt3.posicion(0.f, 0.f, 0.f)
	.punto(0.f, 0.f, 0.f, 0.f)
	.punto(100.f, 0.f, 1.f, 0.f)
	.punto(100.f, 100.f, 1.f, 1.f)
	.punto(0.f, 100.f, 0.f, 1.f);

	QuadTex qt4(tex4);
	qt4.posicion(100.f, 0.f, 0.f)
	.transformar_textura(.5f, .5f, 0.f)
	.punto(0.f, 0.f, 0.f, 0.f)
	.punto(100.f, 0.f, 2.f, 0.f)
	.punto(100.f, 100.f, 2.f, 2.f)
	.punto(0.f, 100.f, 0.f, 2.f);

	//Dibujar con textura full...
	QuadTex qt2(tex2);
	qt2.posicion(0.f, 100.f, 0.f)
	.rotacion(45.0f, 0.f, 0.f, 1.f)
	.punto(0.f, 0.f, 0.f, 0.f)
	.punto(100.f, 0.f, 1.f, 0.f)
	.punto(100.f, 100.f, 1.f, 1.f)
	.punto(0.f, 100.f, 0.f, 1.f);

	//Dibujar con textura troceada...
	//TODO: Translate texture coordinates to pixels???.
	QuadTex qt(tex3);
	qt.posicion(100.f, 100.f, 0.f)
	.punto(0.f, 0.f, 0.f, 0.f)
	.punto(100.f, 0.f, 1.f / 8.f, 0.f)
	.punto(100.f, 100.f, 1.f / 8.f, 1.f / 8.f)
	.punto(0.f, 100.f, 0.f, 1.f / 8.f);

	quadtexs.push_back(qt);
	quadtexs.push_back(qt2);
	quadtexs.push_back(qt3);
	quadtexs.push_back(qt4);
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

	for(const auto& q : quadcolors) q.draw();
	for(const auto& q : quadtexs) q.draw();

	//Intercambiar buffers.
	glutSwapBuffers();
}

void do_main_loop(int v)
{
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

//std::cout<<glGetString(GL_EXTENSIONS)<<std::endl;

		tex.load_from_file("cosa.png");
		tex2.load_from_file("cosa.png");
		tex3.load_from_file("template_tiles.png");
		tex4.load_from_file("lens_flare.png");
		setup_geometry();

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
