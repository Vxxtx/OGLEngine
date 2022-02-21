#define GLEW_STATIC

#include "Mesh.h"
#include "Renderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

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

    double lastFrameCounterTime = glfwGetTime();
    int nbFrames = 0;
    
    Mesh newMesh1 = Mesh::SpawnObject(glm::vec3(0, 0, 0));
    meshes.push_back(newMesh1);

    while (!glfwWindowShouldClose(thisWindow)) {
        if (glfwGetKey(thisWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(thisWindow, GL_TRUE);


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

        calcInputs(thisWindow);
        glm::mat4 projectionMatrix = getProjectionMatrix();
        glm::mat4 viewMatrix = getViewMatrix();

        for (unsigned int i = 0; i < meshes.size(); ++i) {
            meshes[i].draw(projectionMatrix, viewMatrix);
        }

        glfwSwapBuffers(thisWindow);
        glfwPollEvents();
    }
    return 0;
}

glm::mat4 Renderer::getMatricesFromInput()
{
    return glm::mat4();
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
