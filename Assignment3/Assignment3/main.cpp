// COMPSCI 3GC3 Assignment 3 Task 3
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
#include <cmath>

std::string obj_path = "Data/timmy.obj";
std::string tex_path = "Data/timmy.png";
std::string obj_path1 = "Data/bucket.obj";
std::string tex_path1 = "Data/bucket.jpg";
std::string obj_path2 = "Data/floor.obj";
std::string tex_path2 = "Data/floor.jpeg";

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shading with Spotlights", NULL, NULL);
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

    tinyobj::attrib_t attrib2;
    std::vector<tinyobj::shape_t> shapes2;
    std::vector<tinyobj::material_t> materials2;
    // tinyobj load obj
    std::string warn2, err2;
    bool bTriangulate2 = true;
    bool bSuc2 = tinyobj::LoadObj(&attrib2, &shapes2, &materials2, &warn2, &err2, obj_path2.c_str(), nullptr, bTriangulate2);
    if (!bSuc2)
    {
        std::cout << "tinyobj error: " << err2.c_str() << std::endl;
        return -1;
    }

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(tex_path.c_str(), &width, &height, &nrChannels, 0);

    int width1, height1, nrChannels1;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data1 = stbi_load(tex_path1.c_str(), &width1, &height1, &nrChannels1, 0);

    int width2, height2, nrChannels2;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load(tex_path2.c_str(), &width2, &height2, &nrChannels2, 0);

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

    std::vector<float> vbuffer2;
    std::vector<float> nbuffer2;
    std::vector<float> tbuffer2;
    for (const auto& shape : shapes2) {
        for (auto face : shape.mesh.indices)
        {
            int vid = face.vertex_index;
            int nid = face.normal_index;
            int tid = face.texcoord_index;
            //fill in vertex buffer with vertex positions
            vbuffer2.push_back(attrib2.vertices[vid * 3]);//vertex vid’s x
            vbuffer2.push_back(attrib2.vertices[vid * 3 + 1]);//vertex vid’s y
            vbuffer2.push_back(attrib2.vertices[vid * 3 + 2]);//vertex vid’s z
            //fill in normal buffer with normal directions
            nbuffer2.push_back(attrib2.normals[nid * 3]);
            nbuffer2.push_back(attrib2.normals[nid * 3 + 1]);
            nbuffer2.push_back(attrib2.normals[nid * 3 + 2]);

            tbuffer2.push_back(attrib2.texcoords[tid * 2]);
            tbuffer2.push_back(attrib2.texcoords[tid * 2 + 1]);
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

    unsigned int vertexVBO2, normalVBO2, textureVBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &vertexVBO2);
    glGenBuffers(1, &normalVBO2);
    glGenBuffers(1, &textureVBO2);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO2);
    glBufferData(GL_ARRAY_BUFFER, vbuffer2.size() * sizeof(float), &vbuffer2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO2);
    glBufferData(GL_ARRAY_BUFFER, nbuffer2.size() * sizeof(float), &nbuffer2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO2);
    glBufferData(GL_ARRAY_BUFFER, tbuffer2.size() * sizeof(float), &tbuffer2[0], GL_STATIC_DRAW);
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

    unsigned int textureT2;
    glGenTextures(1, &textureT2);
    glBindTexture(GL_TEXTURE_2D, textureT2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB,
        GL_UNSIGNED_BYTE, data2);//define the texture using image data
    stbi_image_free(data2);//don’t forget to release the image data

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------

    ourShader.use();

    ourShader.setVec3("lightPos", glm::vec3(0.0f, 200.0f, 0.0f));
    ourShader.setVec3("diffuseColor1", glm::vec3(1.0f, 0.0f, 0.0f));
    ourShader.setVec3("diffuseColor2", glm::vec3(0.0f, 1.0f, 0.0f));
    ourShader.setVec3("diffuseColor3", glm::vec3(0.0f, 0.0f, 1.0f));
    ourShader.setVec3("ambientColor", glm::vec3(0.2f, 0.2f, 0.2f));
    /*ourShader.setVec3("spotDirection1", glm::vec3(50.0f, -200.0f, -50.0f));
    ourShader.setVec3("spotDirection2", glm::vec3(-50.0f, -200.0f, -50.0f));
    ourShader.setVec3("spotDirection3", glm::vec3(0.0f, -200.0f, 50.0f));*/
    ourShader.setFloat("cutoff", glm::cos(glm::radians(30.0f)));

    ourShader.setFloat("kc", 1.0f);
    //0.8
    ourShader.setFloat("kl", 0.35 * 1e-4);
    //0.4
    ourShader.setFloat("kq", 0.44 * 1e-4);
    float theta = 0.0f;

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
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 spotDirection1(50.0f, -200.0f, -50.0f);
        glm::vec3 spotDirection2(-50.0f, -200.0f, -50.0f);
        glm::vec3 spotDirection3(0.0f, -200.0f, 50.0f);
        spotDirection1 = rotationMatrix * glm::vec4(spotDirection1, 0.0f);
        spotDirection2 = rotationMatrix * glm::vec4(spotDirection2, 0.0f);
        spotDirection3 = rotationMatrix * glm::vec4(spotDirection3, 0.0f);
        theta += 0.05f;
        ourShader.setVec3("spotDirection1", spotDirection1);
        ourShader.setVec3("spotDirection2", spotDirection2);
        ourShader.setVec3("spotDirection3", spotDirection3);

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

        glBindVertexArray(VAO1);
        glBindTexture(GL_TEXTURE_2D, textureT1);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer1.size() / 3);

        glBindVertexArray(VAO2);
        glBindTexture(GL_TEXTURE_2D, textureT2);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer2.size() / 3);

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

    glDeleteVertexArrays(1, &VAO2);

    glDeleteBuffers(1, &vertexVBO2);

    glDeleteBuffers(1, &normalVBO2);

    glDeleteBuffers(1, &textureVBO2);

    glDeleteTextures(1, &textureT2);



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