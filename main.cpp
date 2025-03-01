// Arnav Sinha https://github.com/arnav43
// https://www.linkedin.com/in/arnavsinha4334

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader_s.h"
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>

// Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// our position and velocity (vx, vy)
glm::vec2 position = glm::vec2(0.0f, 0.0f); // center initially 
glm::vec2 velocity = glm::vec2(0.0003f, 0.0001f);; //vx, vy

// colors
std::vector <glm::vec3> colors = {
    {1.0f, 0.0f, 0.0f}, // red
    {0.0f, 1.0f, 0.0f}, // green
    {0.0f, 0.0f, 1.0f}, // blue
    {1.0f, 1.0f, 0.0f}, // yellow
    {1.0f, 0.0f, 1.0f}, // magenta
    {0.0f, 1.0f, 1.0f}, // cyan
    {1.0f, 1.0f, 1.0f}  // white
};
glm::vec3 currentColor = glm::vec3(1.0f, 1.0f, 1.0f);

// we need a random color everytime it touches a corner
glm::vec3 getRandomColor() {
    glm::vec3 newColor;
    do {
        int index = rand() % colors.size();
        newColor = colors[index];
    } while (newColor == currentColor); // so we dont get same color :p
    return newColor;
}

int main()
{



    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("3.3.shader.vs", "3.3.shader.fs");

    float my_vertices[] = {
        // positions        // texture coords
        -0.20f, -0.20f, 0.0f,  0.0f, 0.0f, // Bottom Left
         0.20f, -0.20f, 0.0f,  1.0f, 0.0f, // Bottom Right
        -0.20f,  0.20f, 0.0f,  0.0f, 1.0f, // Top Left

        -0.20f,  0.20f, 0.0f,  0.0f, 1.0f, // Top Left
         0.20f, -0.20f, 0.0f,  1.0f, 0.0f, // Bottom Right
         0.20f,  0.20f, 0.0f,  1.0f, 1.0f  // Top Right
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // lets bind vao first then vbo
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(my_vertices), my_vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // ACTIVATE position

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // activate texture

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Adding the DVD logo Texture
    unsigned int dvd_texture;

    glGenTextures(1, &dvd_texture);
    glBindTexture(GL_TEXTURE_2D, dvd_texture);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // lets load our data, im using stb_image.h to load my image (has lots of types it supports)
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // so our image is not inverted

    // Add proper texture error checking
    // PUT IMAGE PATH ---- YOURS!
    unsigned char* data = stbi_load("my_image2.png", &width, &height, &nrChannels, 0);


    if (data) {
        // Use GL_RGBA only if the image has 4 channels, otherwise use GL_RGB
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "ERROR::FAILED_TO_LOAD_TEXTURE" << std::endl;

    }
    stbi_image_free(data);

    ourShader.use();
    // let us activate our shader before use
    glUniform1i(glGetUniformLocation(ourShader.ID, "dvd_texture"), 0);



    // Render loop
    while (!glfwWindowShouldClose(window))
    {

        bool touchX = false, touchY = false;

        position = position + velocity;

        // if our dvd hits left or right (x=-1 or x=1) -> we set vx = -vx
        if (position.x > 0.81f || position.x < -0.81f) {
            velocity.x = -velocity.x + ((rand() % 100 - 50) / 5000000.0f); // Small random tweak
            currentColor = getRandomColor();
        }
        // if it hits top or bottom (y=1 or y=-1) -> we set vy = -vy
        if (position.y > 0.9f || position.y < -0.9f) {
            velocity.y = -velocity.y - ((rand() % 100 - 50) / 5000000.0f); // Small random tweak
            currentColor = getRandomColor();
        }

        glm::mat4 transform_dvd = glm::mat4(1.0f); // identity matrix we initialze
        transform_dvd = glm::translate(transform_dvd, glm::vec3(position, 1.0f));

        // Send matrices to shader
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "transform_dvd"), 1, GL_FALSE, glm::value_ptr(transform_dvd));

        // sending color to shader
        glUniform3fv(glGetUniformLocation(ourShader.ID, "dvd_color"), 1, glm::value_ptr(currentColor));

        // Input handling
        processInput(window);

        // Rendering commands here
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // pitch black bg
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate texture unit first, then bind texture
        glActiveTexture(GL_TEXTURE0);  // This line was missing
        glBindTexture(GL_TEXTURE_2D, dvd_texture);

        // lets render our rectangle
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0); // Unbind after use (optional but good practice)


        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Cleanup and exit
    glfwTerminate();
    return 0;   
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
