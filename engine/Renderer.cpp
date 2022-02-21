#define GLEW_STATIC

#include "Renderer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include "ObjectLoader.h"

Renderer::Renderer(int wWidth, int wHeight)
    : width(wWidth), height(wHeight)
{
    d_width = (double)width;
    d_height = (double)height;
}

Renderer::~Renderer()
{
}

int Renderer::Begin()
{
    /* Needed for GLEW to work */
    glewExperimental = true;

    /* If glfw fails to init, close */
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    /* Set 4x Anti-Aliasing */
    glfwWindowHint(GLFW_SAMPLES, 4);

    /* OpenGL version 3.3 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    /* Something about MacOS */
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    thisWindow = glfwCreateWindow(width, height, "OGL", NULL, NULL);
    if (thisWindow == NULL) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(thisWindow);
    glewExperimental = GL_TRUE; // Needed for core profile

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glfwSetInputMode(thisWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(thisWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(thisWindow, width / 2, height / 2);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);

    /* Enable face culling for unseen faces */
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders("engine/vertex.txt", "engine/frag.txt");

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    GLuint objTexture = ObjectLoader::LoadBMP("engine/tex.bmp");
    GLuint textureID = glGetUniformLocation(programID, "myTextureSampler");

    MeshData newMesh;
    bool result = ObjectLoader::LoadObject("engine/cube.obj", newMesh);

    std::vector<glm::vec3>& vertices = newMesh.vertices;
    std::vector<glm::vec2>& uvs = newMesh.uvs;
    std::vector<glm::vec3>& normals = newMesh.normals;

    if (!result) {
        printf("\nFailed to read object, exiting");
        return -1;
    }

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
   
   // std::vector<unsigned int> indices;

   // GLuint elementBuffer;
   // glGenBuffers(1, &elementBuffer);
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
   // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glUseProgram(programID);
    GLuint lightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    double lastFrameCounterTime = glfwGetTime();
    int nbFrames = 0;
    
    while (!glfwWindowShouldClose(thisWindow)) {
        if (glfwGetKey(thisWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(thisWindow, GL_TRUE);

            glDeleteBuffers(1, &vertexBuffer);
            glDeleteBuffers(1, &uvBuffer);
            glDeleteVertexArrays(1, &VertexArrayID);
            glDeleteProgram(programID);
            glDeleteTextures(1, &objTexture);

            glfwTerminate();
        }

        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastFrameCounterTime >= 1.0) {
            std::string frameTimeString = "Frametime: ";
            
            frameTimeString.append(std::to_string(1000.0 / double(nbFrames)));
            glfwSetWindowTitle(thisWindow, frameTimeString.c_str());
            nbFrames = 0;
            lastFrameCounterTime += 1.0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);

        calcInputs(thisWindow);
        glm::mat4 projectionMatrix = getProjectionMatrix();
        glm::mat4 viewMatrix = getViewMatrix();
        glm::mat4 Model = glm::mat4(10.0);
        glm::mat4 mvp = projectionMatrix * viewMatrix * Model;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

        glm::vec3 lightPos = glm::vec3(4, 4, 4);
        glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, objTexture);

        glUniform1i(textureID, 0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // change to this when vertex indexing done
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(thisWindow);
        glfwPollEvents();
    }
    return 0;
}

glm::mat4 Renderer::getMatricesFromInput()
{
    return glm::mat4();
}

GLuint Renderer::LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Cannot open %s\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader: %s\n", vertex_file_path);
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader: %s\n", fragment_file_path);
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void Renderer::calcInputs(GLFWwindow* window)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    glfwSetCursorPos(thisWindow, d_width / 2.0, d_height / 2.0);

    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;

    hAngle += mSpeed * deltaTime * float(d_width / 2.0 - x);
    vAngle += mSpeed * deltaTime * float(d_height / 2.0 - y);

    if (hAngle > 6.28f) {
        hAngle = -6.28f;
    }

    if (hAngle < -6.28f) {
        hAngle = 6.28f;
    }

    /* Limit vertical angle so that camera can't go upside down */
    if (vAngle > 1.8f) {
        vAngle = 1.8f;
    }

    if (vAngle < -1.8f) {
        vAngle = -1.8f;
    }

    // printf("X: %6.2lf Y: %6.2lf", hAngle, vAngle);
    // std::cout << "\n";

    glm::vec3 direction(
        cos(vAngle) * sin(hAngle),
        sin(vAngle),
        cos(vAngle) * cos(hAngle)
    );

    glm::vec3 right = glm::vec3(
        sin(hAngle - 3.14f / 2.0f),
        0,
        cos(hAngle - 3.14f / 2.0f)
    );

    glm::vec3 up = glm::cross(right, direction);

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_position -= right * deltaTime * speed;
    }

    // Go up
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera_position += up * deltaTime * speed;
    }

    // Go down
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera_position -= up * deltaTime * speed;
    }

    // float FOV = initialFOV - 5 * glfwGetMouseWheel
    projMatrix = glm::perspective(glm::radians(FOV), 4.f / 3.f, 0.1f, 100.f);
    viewMatrix = glm::lookAt(camera_position, camera_position + direction, up);
}
