// COMPSCI 3GC3 Assignment 2 Task 2
// Mahad Aziz - azizm17 - 400250379
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"

//added lines for tinyobjloader
#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"

#include <fstream>
#include <iostream>
#include <string>

static unsigned int ss_id = 0;
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

std::string obj_path = "Data/base.obj";
std::string weight_path = "Data/11.weights";
std::ifstream file(weight_path);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Render Blendshapes for Facial Expressions", NULL, NULL);
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
    glEnable(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BACK);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("default.vert", "default.frag");

    tinyobj::attrib_t attrib_base;
    std::vector<tinyobj::shape_t> shapes_base;
    std::vector<tinyobj::material_t> materials_base;
    // tinyobj load obj
    std::string warn, err;
    bool bTriangulate = true;
    bool bSuc = tinyobj::LoadObj(&attrib_base, &shapes_base, &materials_base, &warn, &err, obj_path.c_str(), nullptr, bTriangulate);
    if (!bSuc)
    {
        std::cout << "tinyobj error: " << err.c_str() << std::endl;
        return -1;
    }

    std::string line;
    tinyobj::attrib_t attrib_new;
    std::vector<tinyobj::shape_t> shapes_new;
    std::vector<tinyobj::material_t> materials_new;
    if (file.is_open()) {
        int face = 0;
        while (std::getline(file, line)) {
            obj_path = "Data/" + std::to_string(face) + ".obj";
            face++;
            bool bTriangulate = true;
            bool bSuc = tinyobj::LoadObj(&attrib_new, &shapes_new, &materials_new, &warn, &err, obj_path.c_str(), nullptr, bTriangulate);
            if (!bSuc)
            {
                std::cout << "tinyobj error: " << err.c_str() << std::endl;
                return -1;
            }

            assert(attrib_base.vertices.size() == attrib_new.vertices.size());
            for (unsigned i = 0; i < attrib_base.vertices.size(); ++i)
            {
                attrib_base.vertices[i] += std::stof(line) * (attrib_new.vertices[i] - attrib_base.vertices[i]);
            }
        }
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<float> vbuffer;  // This will hold vertex data
    std::vector<float> nbuffer;  // This will hold normal data
    for (const auto& shape : shapes_base) {
        for (auto face : shape.mesh.indices)
        {
            int vid = face.vertex_index;
            int nid = face.normal_index;
            //fill in vertex buffer with vertex positions
            vbuffer.push_back(attrib_base.vertices[vid * 3]);
            vbuffer.push_back(attrib_base.vertices[vid * 3 + 1]);
            vbuffer.push_back(attrib_base.vertices[vid * 3 + 2]);
            //fill in normal buffer with normal directions
            nbuffer.push_back(attrib_base.normals[nid * 3]);
            nbuffer.push_back(attrib_base.normals[nid * 3 + 1]);
            nbuffer.push_back(attrib_base.normals[nid * 3 + 2]);
        }
    }

    unsigned int vertexVBO, normalVBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &normalVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(float), &vbuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, nbuffer.size() * sizeof(float), &nbuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);  // Unbind current buffer
    glBindVertexArray(0);  // Unbind the VAO

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        // render
        // ------
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // activate shader
        ourShader.use();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        ourShader.setMat4("model", model);

        view = glm::lookAt(glm::vec3(20.0f, 50.0f, 200.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

        ourShader.setMat4("MVP", projection * view * model);

        // render boxes
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer.size() / 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteBuffers(1, &vertexVBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteVertexArrays(1, &VAO);

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