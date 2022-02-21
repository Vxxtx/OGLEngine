#define GLEW_STATIC

#include "Mesh.h"

#include "ObjectLoader.h"

Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertex_buf);
    glDeleteBuffers(1, &uv_buf);
    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteProgram(shader_program_id);
    glDeleteTextures(1, &obj_texture);
}

void Mesh::setMeshData(const MeshData& inMeshData)
{
    //name = inMeshData.meshName;

    //vertices = inMeshData.vertices;
    //uvs = inMeshData.uvs;
    //normals = inMeshData.normals;

    initialized();
}

void Mesh::initialized()
{
    GLenum error;

    while (error = glGetError() != GL_NO_ERROR) {
        printf("%s\n", glewGetErrorString(error));

    }
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    shader_program_id = ObjectLoader::LoadShaders("engine/vertex.txt", "engine/frag.txt");

   

    matrix_id = glGetUniformLocation(shader_program_id, "MVP");
    view_matrix_id = glGetUniformLocation(shader_program_id, "V");
    model_matrix_id = glGetUniformLocation(shader_program_id, "M");

    obj_texture = ObjectLoader::LoadBMP("engine/tex.bmp");
    texture_id = glGetUniformLocation(shader_program_id, "myTextureSampler");

    MeshData newMesh;
    bool result = ObjectLoader::LoadObject("engine/cube.obj", newMesh);

    vertices = newMesh.vertices;
    uvs = newMesh.uvs;
    normals = newMesh.normals;

    if (!result) {
        printf("\nFailed to read object, exiting");
        return;
    }

    glGenBuffers(1, &vertex_buf);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uv_buf);

    glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normal_buf);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buf);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glUseProgram(shader_program_id);

    light_id = glGetUniformLocation(shader_program_id, "LightPosition_worldspace");


}

void Mesh::draw(glm::mat4 projection_matrix, glm::mat4 view_matrix)
{
    glUseProgram(shader_program_id);

    glm::mat4 mvp = projection_matrix * view_matrix * w_pos;

    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &w_pos[0][0]);
    glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);

    glm::vec3 lightPos = glm::vec3(4, 4, 4);
    glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj_texture);

    glUniform1i(texture_id, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buf);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buf);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // change to this when vertex indexing done
    //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
