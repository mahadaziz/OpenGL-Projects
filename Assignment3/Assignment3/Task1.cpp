// COMPSCI 3GC3 Assignment 3 Task 1
// Mahad Aziz - azizm17 - 400250379
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"

#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

std::string obj_path = "Data/floor.obj";
std::string tex_path = "Data/floor.jpeg";
std::string obj_path1 = "Data/bucket.obj";
std::string tex_path1 = "Data/bucket.jpg";

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Use Texture for Mesh Color", NULL, NULL);
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

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    // tinyobj load obj
    std::string warn, err;
    bool bTriangulate = true;
    bool bSuc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_path.c_str(), nullptr, bTriangulate);
    if (!bSuc)
    {
        std::cout << "tinyobj error: " << err.c_str() << std::endl;
        return -1;
    }

    tinyobj::attrib_t attrib1;
    std::vector<tinyobj::shape_t> shapes1;
    std::vector<tinyobj::material_t> materials1;
    // tinyobj load obj
    std::string warn1, err1;
    bool bTriangulate1 = true;
    bool bSuc1 = tinyobj::LoadObj(&attrib1, &shapes1, &materials1, &warn1, &err1, obj_path1.c_str(), nullptr, bTriangulate1);
    if (!bSuc1)
    {
        std::cout << "tinyobj error: " << err1.c_str() << std::endl;
        return -1;
    }

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(tex_path.c_str(), &width, &height, &nrChannels, 0);

    int width1, height1, nrChannels1;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data1 = stbi_load(tex_path1.c_str(), &width1, &height1, &nrChannels1, 0);

    std::vector<float> vbuffer;
    std::vector<float> nbuffer;
    std::vector<float> tbuffer;
    for (const auto& shape : shapes) {
        for (auto face : shape.mesh.indices)
        {
            int vid = face.vertex_index;
            int nid = face.normal_index;
            int tid = face.texcoord_index;
            //fill in vertex buffer with vertex positions
            vbuffer.push_back(attrib.vertices[vid * 3]);//vertex vid’s x
            vbuffer.push_back(attrib.vertices[vid * 3 + 1]);//vertex vid’s y
            vbuffer.push_back(attrib.vertices[vid * 3 + 2]);//vertex vid’s z
            //fill in normal buffer with normal directions
            nbuffer.push_back(attrib.normals[nid * 3]);
            nbuffer.push_back(attrib.normals[nid * 3 + 1]);
            nbuffer.push_back(attrib.normals[nid * 3 + 2]);

            tbuffer.push_back(attrib.texcoords[tid * 2]);
            tbuffer.push_back(attrib.texcoords[tid * 2 + 1]);
        }
    }

    std::vector<float> vbuffer1;
    std::vector<float> nbuffer1;
    std::vector<float> tbuffer1;
    for (const auto& shape : shapes1) {
        for (auto face : shape.mesh.indices)
        {
            int vid = face.vertex_index;
            int nid = face.normal_index;
            int tid = face.texcoord_index;
            //fill in vertex buffer with vertex positions
            vbuffer1.push_back(attrib1.vertices[vid * 3]);//vertex vid’s x
            vbuffer1.push_back(attrib1.vertices[vid * 3 + 1]);//vertex vid’s y
            vbuffer1.push_back(attrib1.vertices[vid * 3 + 2]);//vertex vid’s z
            //fill in normal buffer with normal directions
            nbuffer1.push_back(attrib1.normals[nid * 3]);
            nbuffer1.push_back(attrib1.normals[nid * 3 + 1]);
            nbuffer1.push_back(attrib1.normals[nid * 3 + 2]);

            tbuffer1.push_back(attrib1.texcoords[tid * 2]);
            tbuffer1.push_back(attrib1.texcoords[tid * 2 + 1]);
        }
    }

    unsigned int vertexVBO, normalVBO, textureVBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &textureVBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(float), &vbuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, nbuffer.size() * sizeof(float), &nbuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, tbuffer.size() * sizeof(float), &tbuffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int vertexVBO1, normalVBO1, textureVBO1, VAO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &vertexVBO1);
    glGenBuffers(1, &normalVBO1);
    glGenBuffers(1, &textureVBO1);

    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO1);
    glBufferData(GL_ARRAY_BUFFER, vbuffer1.size() * sizeof(float), &vbuffer1[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO1);
    glBufferData(GL_ARRAY_BUFFER, nbuffer1.size() * sizeof(float), &nbuffer1[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO1);
    glBufferData(GL_ARRAY_BUFFER, tbuffer1.size() * sizeof(float), &tbuffer1[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int textureT;
    glGenTextures(1, &textureT);
    glBindTexture(GL_TEXTURE_2D, textureT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, data);//define the texture using image data
    stbi_image_free(data);//don’t forget to release the image data

    unsigned int textureT1;
    glGenTextures(1, &textureT1);
    glBindTexture(GL_TEXTURE_2D, textureT1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB,
        GL_UNSIGNED_BYTE, data1);//define the texture using image data
    stbi_image_free(data1);//don’t forget to release the image data

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------

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
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        ourShader.setMat4("model", model);

        view = glm::lookAt(glm::vec3(50.0f, 100.0f, 200.0f), glm::vec3(0.0f, 80.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

        ourShader.setMat4("MVP", projection * view * model);

        // render boxes
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, textureT);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer.size() / 3);

        /*glBindVertexArray(VAO1);
        glBindTexture(GL_TEXTURE_2D, textureT1);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer1.size() / 3);*/

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &vertexVBO);

    glDeleteBuffers(1, &normalVBO);

    glDeleteBuffers(1, &textureVBO);

    glDeleteTextures(1, &textureT);

    glDeleteVertexArrays(1, &VAO1);

    glDeleteBuffers(1, &vertexVBO1);

    glDeleteBuffers(1, &normalVBO1);

    glDeleteBuffers(1, &textureVBO1);

    glDeleteTextures(1, &textureT1);

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