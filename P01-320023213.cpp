#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>
#include <windows.h>
#include <random>

//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;
float red = 1.0f, green = 0.0f, blue = 0.0f;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
}";

//Fragment Shader
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(0.4f,0.4f,0.4f,0.4f);	 			\n\
}";

void CrearTriangulo() {
	GLfloat vertices[] = {

		//  Para S (Ubicada en la parte inferior izquierda para la diagonal)
		-0.8f, -0.4f, 0.0f,  -0.4f, -0.4f, 0.0f,  -0.8f, -0.5f, 0.0f, // Barra superior
		-0.4f, -0.4f, 0.0f,  -0.4f, -0.5f, 0.0f,  -0.8f, -0.5f, 0.0f,
		-0.8f, -0.5f, 0.0f,  -0.7f, -0.5f, 0.0f,  -0.8f, -0.7f, 0.0f, // Barra lateral izq arriba
		-0.7f, -0.5f, 0.0f,  -0.7f, -0.7f, 0.0f,  -0.8f, -0.7f, 0.0f,
		-0.8f, -0.6f, 0.0f,  -0.4f, -0.6f, 0.0f,  -0.8f, -0.7f, 0.0f, // Barra medio
		-0.4f, -0.6f, 0.0f,  -0.4f, -0.7f, 0.0f,  -0.8f, -0.7f, 0.0f,
		-0.5f, -0.7f, 0.0f,  -0.4f, -0.7f, 0.0f,  -0.5f, -0.9f, 0.0f, // Barra lateral der abajo
		-0.4f, -0.7f, 0.0f,  -0.4f, -0.9f, 0.0f,  -0.5f, -0.9f, 0.0f,
		-0.8f, -0.8f, 0.0f,  -0.4f, -0.8f, 0.0f,  -0.8f, -0.9f, 0.0f, // Barra inferior
		-0.4f, -0.8f, 0.0f,  -0.4f, -0.9f, 0.0f,  -0.8f, -0.9f, 0.0f,

		//  Para N (Ubicada en el centro para la diagonal)
		-0.2f, -0.2f, 0.0f,  -0.1f, -0.2f, 0.0f,  -0.2f,  0.2f, 0.0f, // Poste izquierdo
		-0.1f, -0.2f, 0.0f,  -0.1f,  0.2f, 0.0f,  -0.2f,  0.2f, 0.0f,
		-0.1f,  0.2f, 0.0f,   0.1f, -0.2f, 0.0f,  -0.2f,  0.2f, 0.0f, // Diagonal
		 0.1f, -0.2f, 0.0f,   0.2f, -0.2f, 0.0f,  -0.1f,  0.2f, 0.0f,
		 0.1f, -0.2f, 0.0f,   0.2f, -0.2f, 0.0f,   0.1f,  0.2f, 0.0f, // Poste derecho
		 0.2f, -0.2f, 0.0f,   0.2f,  0.2f, 0.0f,   0.1f,  0.2f, 0.0f,

		 // Para T (Ubicada en la parte superior derecha para completar la diagonal)
		  0.4f,  0.8f, 0.0f,   0.8f,  0.8f, 0.0f,   0.4f,  0.7f, 0.0f, // Barra horizontal
		  0.8f,  0.8f, 0.0f,   0.8f,  0.7f, 0.0f,   0.4f,  0.7f, 0.0f,
		  0.55f, 0.7f, 0.0f,   0.65f, 0.7f, 0.0f,   0.55f, 0.4f, 0.0f, // Poste vertical
		  0.65f, 0.7f, 0.0f,   0.65f, 0.4f, 0.0f,   0.55f, 0.4f, 0.0f,

	};

	glGenVertexArrays(1, &VAO); //generar 1 VAO
	glBindVertexArray(VAO);//asignar VAO

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es esttico pues no se modificarn los valores

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
	glEnableVertexAttribArray(0);
	//agregar valores a vrtices y luego declarar un nuevo vertexAttribPointer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Funcin para agregar los shaders a la tarjeta grfica//the Program recibe los datos de theShader
{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader, 1, theCode, codeLength);//Se le asigna al shader el cdigo
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevencin de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el cdigo a la tarjeta grfica
}

void CompileShaders() {
	shader = glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//se linkean los shaders a la tarjeta grfica
	//verificaciones y prevencin de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}
}

int main() {
	//Inicializacin de GLFW
	if (!glfwInit())
	{
		printf("Fall inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//**** LAS SIGUIENTES 4 LNEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSIN DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamao de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fall inicializacin de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

	//Llamada a las funciones creadas antes del main
	CrearTriangulo();
	CompileShaders();

	// Random (Se usa random_device para asegurar que cada ejecucion sea distinta)
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		//Recibir eventos del usuario
		glfwPollEvents();

		// 1.- Cambiar colores fondo de forma random cada 2 segundos
		red = dist(gen);
		green = dist(gen);
		blue = dist(gen);

		//Limpiar la ventana con el color aleatorio generado
		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		// 2.- Dibujar las 3 iniciales SNT en diagonal
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 60); // S=30 vrtices, N=18 vrtices, T=12 vrtices
		glBindVertexArray(0);

		glUseProgram(0);

		// Pausar el cambio de color por 2 segundos (Periodicidad)
		Sleep(2000);

		glfwSwapBuffers(mainWindow);

		//NO ESCRIBIR NINGUNA LNEA DESPUS DE glfwSwapBuffers(mainWindow); 
	}

	return 0;
}