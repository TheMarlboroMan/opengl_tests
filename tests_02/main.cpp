#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <iostream>
#include <stdexcept>

/*g++ main.cpp -std=c++11 -lGL -lGLU -lGLEW*/

const int 
	w_screen=640,
	h_screen=480;

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

	if(!GLEW_VERSION_2_1)
	{		
		throw std::runtime_error("No Glew 2.1 supported.");
	}

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
}

void render()
{
	//Limpiar la pantalla...
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint gVertexBuffer=0;
	glGenBuffers(1, &gVertexBuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);

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
	switch(k)
	{
		case 'q': throw new std::runtime_error("We're done here"); break;
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
		glutKeyboardFunc(do_input_handling);
		glutDisplayFunc(render);
		glutTimerFunc(1000 / 60, do_main_loop, 0);
		glutMainLoop();
	}
	catch(std::exception& e)
	{
		std::cout<<"Exception caught "<<e.what()<<std::endl;
		return 1;
	}

	return 0;
}
