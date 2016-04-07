#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <string>
#include <stdexcept>

/*g++ main.cpp -std=c++11 -lGL -lGLU -lglut*/

enum colores{COLOR_MODE_CYAN, COLOR_MODE_MULTI};
int color_mode=COLOR_MODE_CYAN;
GLfloat projection_scale=1.f;
const int 
	w_screen=640,
	h_screen=480;

void init_open_gl()
{
	//Establecer el modo a matriz de proyección y cargarla a 1.
	glMatrixMode(GL_PROJECTION); 	
	glLoadIdentity();

	//Izquierda, derecha, abajo, arriba, cerca y lejos...
	//Ahora el punto 0.0 es arriba a la izquierda.
	glOrtho(0.0, w_screen, h_screen, 0.0, 1.0, -1.0);

	//Establecer el modo a matriz de proyección y cargarla a 1.
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();

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
}

void update()
{

}

void render()
{
	//Limpiar la pantalla...
	glClear(GL_COLOR_BUFFER_BIT);

	//Cambiamos a la matriz de modelo.
	glMatrixMode(GL_MODELVIEW);

	//Cargamos la identidad para que los cambios inferiores no se acumulen.
	glLoadIdentity();

	//Traslación de lo que dibujemos en x, y y z.
	glTranslatef(w_screen/2.f, h_screen/2.f, 0.f);

/*
	//Esto es para el sistema de coordenadas -1 : 1...
	glBegin( GL_QUADS );
		glColor3f(0.f, 1.f, 1.f);
	       	glVertex2f(-0.5f, -0.5f);
	        glVertex2f(-0.5f, 0.5f);
	        glVertex2f(0.5f, 0.5f);
	        glVertex2f(0.5f, -0.5f);
	glEnd();
*/
	
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
	glBegin( GL_QUADS );
		do_color(color_mode, i++);
        	glVertex2f(-50.f, -50.f);
		do_color(color_mode, i++);
	        glVertex2f(-50.f, 50.f);
		do_color(color_mode, i++);
	        glVertex2f(50.f, 50.f);
		do_color(color_mode, i++);
	        glVertex2f(50.f, -50.f);
	glEnd();

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
	float np=projection_scale;

	switch(k)
	{
		case 'q':
			np-=0.1;
		break;
		case 'e':
			np=1.0f;
		break;
		case 'w':
			np+=0.1;
		break;
		case 'a':
			color_mode=color_mode==COLOR_MODE_CYAN ? COLOR_MODE_MULTI : COLOR_MODE_CYAN;
		break;
		case 'z': 
			throw new std::runtime_error("We're done here");
		break;
	}

	if(np!=projection_scale)
	{
		projection_scale=np;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity(); 
		glOrtho( 0.0, w_screen * projection_scale, h_screen * projection_scale, 0.0, 1.0, -1.0 );
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
