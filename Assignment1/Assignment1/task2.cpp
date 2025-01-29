#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"

#include <iostream>
#include <fstream>
#include <string>

static unsigned int ss_id = 0;
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Static Solar System", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("default.vert", "default.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,

        -1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f,

         1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,

    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(24.0f,  0.0f, 0.0f),
        glm::vec3(36.0f, 0.0f, 0.0f),
    };

    glm::vec3 cubeScales[] = {
        glm::vec3(6.0f,  6.0f,  6.0f),
        glm::vec3(3.0f,  3.0f, 3.0f),
        glm::vec3(1.5f, 1.5f, 1.5f),
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // activate shader
        ourShader.use();

        // create transformations
        glm::mat4 view = glm::lookAt(glm::vec3(100.0f, 50.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

        // render box
        glBindVertexArray(VAO);
        float tiltAngle = -23.4f;

        glm::mat4 sunModel = glm::mat4(1.0f);
        glm::mat4 earthModel = glm::mat4(1.0f);
        glm::mat4 moonModel = glm::mat4(1.0f);

        //Sun
        sunModel = glm::translate(sunModel, cubePositions[0]);

        //Earth
        earthModel = glm::translate(earthModel, cubePositions[1]);
        earthModel = glm::rotate(earthModel, glm::radians(tiltAngle), glm::vec3(0.0f, 0.0f, 1.0f));

        //Moon
        moonModel = glm::translate(moonModel, cubePositions[2]);

        sunModel = glm::scale(sunModel, cubeScales[0]);
        earthModel = glm::scale(earthModel, cubeScales[1]);
        moonModel = glm::scale(moonModel, cubeScales[2]);

        glm::mat4 sunMVP = projection * view * sunModel;
        glm::mat4 earthMVP = projection * view * earthModel;
        glm::mat4 moonMVP = projection * view * moonModel;

        ourShader.setMat4("MVP", sunMVP);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        ourShader.setMat4("MVP", earthMVP);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        ourShader.setMat4("MVP", moonMVP);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte* pixels = new GLubyte[totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id++;
    delete[] pixels;
    fout.flush();
    fout.close();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //press p to capture screen
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("Assignment0-ss", buffer_width, buffer_height);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}