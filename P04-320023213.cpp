/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(30, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();
	CrearCilindro(30, 1.0f);//índice 2 en MeshList



	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.5f, 0.5f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		/*
		//Inicio Grua--------------------------------------------------------------------------------------------------------------------
		
		//CREANDO LA CABINA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, -4.0f));
		//aqui está la jerarquía
		modelaux = model;
		model = glm::scale(model, glm::vec3(7.0f, 4.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 1.0f, 0.5f); // GRIS
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//Base
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(6.0f, 3.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMeshGeometry();

		//Llanta 1
		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-1.3f, 3.65f, 2.6f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelRin1 = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//Centro de la llanta
		model = modelRin1; 
		model = glm::scale(model, glm::vec3(0.6f, 1.05f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//Llanta 2
		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-1.3f, -3.65f, 2.6f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelRin2 = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//Centro de llanta
		model = modelRin2;
		model = glm::scale(model, glm::vec3(0.6f, 1.1f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//LLanta 3
		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-1.3f, -3.65f, -2.6f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelRin3 = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//Centro de llanta
		model = modelRin3;
		model = glm::scale(model, glm::vec3(0.6f, 1.1f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//Llanta 4
		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-1.3f, 3.65f, -2.6f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelRin4 = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//Centro de llanta
		model = modelRin4;
		model = glm::scale(model, glm::vec3(0.6f, 1.1f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMeshGeometry();

		//articulación 1
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Primer brazo
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		meshList[0]->RenderMesh(); 

		//articulacion 2
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		//segundo brazo
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		meshList[0]->RenderMesh(); 

		model = modelaux;

		//Articulación 3
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f); 
		color = glm::vec3(0.0f, 0.0f, 0.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		sp.render();

		//Tercer Brazo-------------------------------------------------------------------------------------------------------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		color = glm::vec3(1.0f, 1.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//Articulación 4-----------------------------------------------------------------------------------------------------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, -1.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		//Canasta-----------------------------------------------------------------------------------------------------------------
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.25f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();
		*/
		//Fin Grua------------------------------------------------------------------------------------------------------------------------

		
		//Torso---------------------------------------------------------------------------------------------------------------------------
		//Pecho
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, -4.0f));
		//aqui está la jerarquía
		modelaux = model;
		model = glm::scale(model, glm::vec3(3.5f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(0.85f, 0.45f, 0.1f); // Naranja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();


		//Primera esfera continuación de torso----------------------------------------------------------------------------------------------------
		model = modelaux; 
		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -1.8f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.7f, 1.7f, 1.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		//Cadera con rectangulo-------------------------------------------------------------------------------------------------------------------
		model = modelaux; 
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
		glm::mat4 modelauxCadera = model;
		model = glm::scale(model, glm::vec3(1.5f, 3.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 

		//Segunda esfera continuación del torso---------------------------------------------------------------------------------------------------
		model = modelauxCadera; 
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.7f, 1.7f, 1.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		//Cubo final del torso--------------------------------------------------------------------------------------------------------------------
		model = modelaux; 
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		glm::mat4 modelauxFinal = model;
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 

		//Capa trasera----------------------------------------------------------------------------------------------------------------------------
		model = modelauxFinal;
		model = glm::translate(model, glm::vec3(0.0f, 1.9f, 4.1f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		color = glm::vec3(0.4f, 0.2f, 0.1f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(2.5f, 0.1f, 8.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[4]->RenderMeshGeometry();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.2f, 5.1f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelauxCuello = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.6f, 0.8f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();

		//Capa delantera--------------------------------------------------------------------------------------------------------------------------
		model = modelauxCuello;
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.7f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.4f, 0.2f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(2.5f, 0.1f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		//Espada-----------------------------------------------------------------------------------------------------------------------------------
		model = modelauxCadera;
		model = glm::translate(model, glm::vec3(2.2f, 0.5f, -3.5f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 modelauxEspada = model;

		//Mango de espada--------------------------------------------
		color = glm::vec3(0.4f, 0.2f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelauxEspada;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.6f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 

		//Guarda de espada-------------------------------------------
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelauxEspada;
		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.1f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 

		//Hoja de espada---------------------------------------------
		color = glm::vec3(0.85f, 0.85f, 0.9f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelauxEspada;
		model = glm::translate(model, glm::vec3(0.0f, -3.5f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 10.0f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry(); 

		//Pomo de espada---------------------------------------------
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelauxEspada;
		model = glm::translate(model, glm::vec3(0.0f, 1.1f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 

		//Cabeza----------------------------------------------------------------------------------------------------------------------------------
		model = modelauxCuello; 
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		glm::mat4 modelauxCabeza = model; 

		color = glm::vec3(0.85f, 0.45f, 0.1f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.7f, 1.7f, 1.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render(); // Render de la esfera base

		//Ojos de cabeza-------------------------------------
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Ojo Izquierdo--------------------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(0.4f, 0.4f, 1.4f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render(); //

		//Ojo Derecho----------------------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(-0.4f, 0.4f, 1.4f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render(); 

		//Orejas---------------------------------------------
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Oreja Izquierda------------------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(0.7f, 1.4f, 0.0f));
		model = glm::scale(model, glm::vec3(2.2f, 2.5f, 1.9f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry(); 

		//Oreja Derecha--------------------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(-0.7f, 1.4f, 0.0f));
		model = glm::scale(model, glm::vec3(2.2f, 2.5f, 1.9f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry(); 

		//Sombrero-------------------------------------------
		color = glm::vec3(0.1f, 0.4f, 0.1f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Ala del sombrero-----------------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelauxAla = model;
		model = glm::scale(model, glm::vec3(2.6f, 0.1f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 

		//Copa del sombrero----------------------------------
		model = modelauxAla;
		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(1.3f, 0.7f, 1.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 

		//Pluma del sombrero---------------------------------
		color = glm::vec3(0.9f, 0.8f, 0.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelauxAla;
		model = glm::translate(model, glm::vec3(-1.0f, 1.3f, 0.0f));
		model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.15f, 1.4f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry(); 

		//Bigotes--------------------------------------------
		color = glm::vec3(0.6f, 0.6f, 0.6f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Bigote izquierdo-----------------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(0.8f, -0.5f, 1.1f));
		model = glm::rotate(model, glm::radians(80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMeshGeometry(); // Cono

		//Bigote derecho-------------------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(-0.8f, -0.5f, 1.1f));
		model = glm::rotate(model, glm::radians(-80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMeshGeometry(); // Cono

		//Boca-----------------------------------------------
		color = glm::vec3(0.9f, 0.7f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Parte izquierda de la boca-------------------------
		model = modelauxCabeza; 
		model = glm::translate(model, glm::vec3(0.35f, -0.65f, 1.45f));
		model = glm::scale(model, glm::vec3(0.6f, 0.5f, 0.4f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render(); 

		//Parte derecha de la boca---------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(-0.35f, -0.65f, 1.45f));
		model = glm::scale(model, glm::vec3(0.6f, 0.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render(); 

		//Nariz----------------------------------------------
		model = modelauxCabeza;
		model = glm::translate(model, glm::vec3(0.0f, -0.15f, 1.6f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.6f, 0.3f, 0.3f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMeshGeometry(); 

		//Pata delantera izquierda-----------------------------------------------------------------------------------------------------------------------------

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.1f, 7.4f, -4.0f)); 
		model = glm::translate(model, glm::vec3(1.2f, -1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArt1 = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.8));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();
		model = modelauxArt1;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
		glm::mat4 modelauxBrazoSup = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxBrazoSup;
		model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArt2 = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.7f, 1.2f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 
		model = modelauxArt2;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
		glm::mat4 modelauxBrazoInf = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.0f, 1.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxBrazoInf;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArt3 = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 

		//Pie---------------------------------------------------
		model = modelauxArt3; 
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.2f));
		glm::mat4 modelauxMaderaPie = model; 
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.0f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxMaderaPie;
		model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.6f));
		glm::mat4 modelauxSoporteMetal = model; 
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.2f, 0.3f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxSoporteMetal; 
		model = glm::translate(model, glm::vec3(-0.4f, -0.1f, 0.4f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.6f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxSoporteMetal; 
		model = glm::translate(model, glm::vec3(0.4f, -0.1f, 0.4f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.6f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 

		//Pata delantera derecha-----------------------------------------------------------------------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.1f, 7.4f, -4.0f)); 
		model = glm::translate(model, glm::vec3(-1.2f, -1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArt1Der = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.8));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 
		model = modelauxArt1Der;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
		glm::mat4 modelauxBrazoSupDer = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxBrazoSupDer;
		model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArt2Der = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.7f, 1.2f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 
		model = modelauxArt2Der;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
		glm::mat4 modelauxBrazoInfDer = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.0f, 1.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxBrazoInfDer;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArt3Der = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 

		//Pie---------------------------------------------------
		model = modelauxArt3Der;
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.2f));
		glm::mat4 modelauxMaderaPieDer = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.0f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxMaderaPieDer;
		model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.6f)); 
		glm::mat4 modelauxSoporteMetalDer = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.2f, 0.3f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxSoporteMetalDer;
		model = glm::translate(model, glm::vec3(0.4f, -0.1f, 0.4f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxSoporteMetalDer;
		model = glm::translate(model, glm::vec3(-0.4f, -0.1f, 0.4f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 

		//Pata trasera izquierda-----------------------------------------------------------------------------------------------------------------------------
		model = modelauxCadera; 
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, -4.0f)); 
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArtTras1 = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.8));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 
		model = modelauxArtTras1;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
		glm::mat4 modelauxMuslo = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.2f, 1.5f, 1.2f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxMuslo;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArtTras2 = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.7f, 1.2f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 
		model = modelauxArtTras2;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
		glm::mat4 modelauxPiernaTras = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.0f, 1.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxPiernaTras;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion10()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArtTras3 = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 

		//Pie
		model = modelauxArtTras3;
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.2f));
		glm::mat4 modelauxMaderaPieTras = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.0f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxMaderaPieTras;
		model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.6f));
		glm::mat4 modelauxSoporteMetalTras = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.2f, 0.3f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxSoporteMetalTras;
		model = glm::translate(model, glm::vec3(0.4f, -0.1f, 0.4f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxSoporteMetalTras;
		model = glm::translate(model, glm::vec3(-0.4f, -0.1f, 0.4f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); // Cubo

		//Pata trasera derecha-----------------------------------------------------------------------------------------------------------------------------
		model = modelauxCadera; 
		model = glm::translate(model, glm::vec3(-2.1f, 0.0f, -4.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion11()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArtTras1Der = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.8));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 
		model = modelauxArtTras1Der;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
		glm::mat4 modelauxMusloDer = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.2f, 1.5f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxMusloDer;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion12()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArtTras2Der = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.7f, 1.2f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();
		model = modelauxArtTras2Der;
		model = glm::translate(model, glm::vec3(0.0f, -1.4f, 0.0f));
		glm::mat4 modelauxPiernaTrasDer = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.0f, 1.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxPiernaTrasDer;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion13()), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelauxArtTras3Der = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 

		//Pie
		model = modelauxArtTras3Der;
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.2f));
		glm::mat4 modelauxMaderaPieTrasDer = model;
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.0f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxMaderaPieTrasDer;
		model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.6f));
		glm::mat4 modelauxSoporteMetalTrasDer = model;
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::scale(model, glm::vec3(1.2f, 0.3f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxSoporteMetalTrasDer;
		model = glm::translate(model, glm::vec3(0.4f, -0.1f, 0.4f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 
		model = modelauxSoporteMetalTrasDer;
		model = glm::translate(model, glm::vec3(-0.4f, -0.1f, 0.4f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh(); 

		//Cola-------------------------------------------------------------------------------------------------------------------------------------------
		glm::mat4 modelTemp;
		model = modelauxFinal;
		model = glm::translate(model, glm::vec3(0.0f, 1.3f, -1.5f));

		//Parte 1 de la cola----------------------------------------------------------
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion14() + 20.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[2]->RenderMeshGeometry();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.7f)); 
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.7f, 0.7f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[0]->RenderMesh();

		//Parte 2 de la cola----------------------------------------------------------
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15() + 15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[2]->RenderMeshGeometry();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.7f));
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.7f, 0.7f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[0]->RenderMesh();

		//Parte 3 de la cola----------------------------------------------------------
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion16() + 15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[2]->RenderMeshGeometry();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.7f));
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.9f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[0]->RenderMesh();

		//Parte 4 de la cola----------------------------------------------------------
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion17() + 15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[2]->RenderMeshGeometry();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.7f));
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.65f, 0.65f, 0.9f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[0]->RenderMesh();

		//Parte 5 de la cola----------------------------------------------------------
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion18() + 10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[2]->RenderMeshGeometry();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.7f));
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.9f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[0]->RenderMesh();

		//Parte 6 de la cola----------------------------------------------------------
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion19() + 10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.85f, 0.45f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelTemp = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f)); // Punta más grande
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelTemp));
		meshList[0]->RenderMesh();
		
		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}