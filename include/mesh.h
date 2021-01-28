#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

struct Vertex {
	// position
	glm::vec3 Position;

	// normal
	glm::vec3 Normal;

	// texCoords
	glm::vec2 TexCoords;

	// tangent
	glm::vec3 Tangent;

	// bitangent
	glm::vec3 Bitangent;

	//Constructors
	Vertex() {
	}

	Vertex(glm::vec3 _Position) {
		this->Position = _Position;
	}

};

struct TextureMesh {
	unsigned int id;
	std::string type;
	aiString path;

	unsigned int width = 0;
	unsigned int height = 0;
};

class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TextureMesh> textures;
	unsigned int VAO;

	glm::vec3 min;
	glm::vec3 max;

	/*  Functions  */
	// constructor
	Mesh() {
	}

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureMesh> textures, glm::vec3 min, glm::vec3 max)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->min = min;
		this->max = max;

		setupMesh();
	}

	// render the mesh
	void Draw(Shader shader)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
											  // retrieve texture number (the N in diffuse_textureN)
			std::stringstream ss;
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
				ss << diffuseNr++; // transfer unsigned int to stream
			else if (name == "texture_specular")
				ss << specularNr++; // transfer unsigned int to stream
			else if (name == "texture_normal")
				ss << normalNr++; // transfer unsigned int to stream
			else if (name == "texture_height")
				ss << heightNr++; // transfer unsigned int to stream
			number = ss.str();
			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader.getProgramId(), (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

/* *********************************************************************************************************
Very Simple Meshes
********************************************************************************************************* */
void createScreenQuad() {
	//this->vertices = vector<glm::vec3>(4);

	this->vertices = { Vertex(glm::vec3(-1, -1, 0)),
		Vertex(glm::vec3(1, -1, 0)),
		Vertex(glm::vec3(1,  1, 0)),
		Vertex(glm::vec3(-1,  1, 0)) };

	this->indices = { 0, 1, 3, 1, 2, 3 };

	setupMesh();
}

void createBox() {
		this->vertices = {
			Vertex(glm::vec3(0.0,  0.0,  0.0)),
			Vertex(glm::vec3(0.0,  0.0,  1.0)),
			Vertex(glm::vec3(0.0,  1.0,  0.0)),
			Vertex(glm::vec3(0.0,  1.0,  1.0)),
			Vertex(glm::vec3(1.0,  0.0,  0.0)),
			Vertex(glm::vec3(1.0,  0.0,  1.0)),
			Vertex(glm::vec3(1.0,  1.0,  0.0)),
			Vertex(glm::vec3(1.0,  1.0,  1.0))
		};
		this->indices = { 1, 7, 5,
							1, 3, 7,
							1, 4, 3,
							1, 2, 4,
							3, 8, 7,
							3, 4, 8,
							5, 7, 8,
							5, 8, 6,
							1, 5, 6,
							1, 6, 2,
							2, 6, 8,
							2, 8, 4 };
		for (int i = 0; i < indices.size(); i++) {
			this->indices[i] -= 1;
		}

		setupMesh();
}


/* *********************************************************************************************************
Draw Wireframe
********************************************************************************************************* */
void enableWireframe() {
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
}

void disableWireframe() {
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
}

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
};
#endif