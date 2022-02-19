
#include "ObjectLoader.h"
#include <string>

#include "glm/glm.hpp"

namespace ObjectLoader {
	bool LoadObject(const char* path, Mesh& outMesh)
	{
		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;

		FILE* file = fopen(path, "r");

		if (file == NULL) {
			printf("Couldn't open file at %s", path);
			return false;
		}

		while (1) {
			char lineHeader[128] = { 0 };

			int res = fscanf(file, "%s", lineHeader);

			if (res == EOF) {
				printf("eof \n");
				break;
			}
				
			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vert1, vert2, vert3;
				unsigned int vertIdx[3], uvIdx[3], normalIdx[3];

				int matches = fscanf
				(
					file,
					"%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertIdx[0],
					&uvIdx[0],
					&normalIdx[0],

					&vertIdx[1],
					&uvIdx[1],
					&normalIdx[1],

					&vertIdx[2],
					&uvIdx[2],
					&normalIdx[2]
				);

				if (matches != 9) {
					//TODO: add triangulation
					printf("Failed to read file, did you forget to triangulate?");
					return false;
				}

				vertexIndices.push_back(vertIdx[0]);
				vertexIndices.push_back(vertIdx[1]);
				vertexIndices.push_back(vertIdx[2]);

				uvIndices.push_back(uvIdx[0]);
				uvIndices.push_back(uvIdx[1]);
				uvIndices.push_back(uvIdx[2]);

				normalIndices.push_back(normalIdx[0]);
				normalIndices.push_back(normalIdx[1]);
				normalIndices.push_back(normalIdx[2]);

				/* Index vertices */
				for (unsigned int i = 0; i < vertexIndices.size(); i++) {
					unsigned int vertexIdx = vertexIndices[i];
					glm::vec3 vertex = temp_vertices[vertexIdx - 1]; // - 1 because .obj index starts from 1
					
					outMesh.vertices.push_back(vertex);
				}

				/* Index UVs */
				for (unsigned int i = 0; i < uvIndices.size(); i++) {
					unsigned int uvIdx = uvIndices[i];
					glm::vec2 uv = temp_uvs[uvIdx - 1]; // - 1 because .obj index starts from 1
					outMesh.uvs.push_back(uv);
				}

				/* Index normals */
				for (unsigned int i = 0; i < normalIndices.size(); i++) {
					unsigned int normalIdx = normalIndices[i];
					glm::vec3 normal = temp_normals[normalIdx - 1]; // - 1 because .obj index starts from 1
					outMesh.normals.push_back(normal);
				}
			}
			printf("%d\n", outMesh.vertices.size());
		}
		return true;
	}

	GLuint LoadBMP(const char* imgPath)
	{
		unsigned char header[54];
		unsigned int dataPosition;
		unsigned int width, height;
		unsigned int imgSize;

		unsigned char* rgbData;

		FILE* imgFile = fopen(imgPath, "rb");

		if (!imgFile) {
			printf("Could not open image %s\n", imgPath);
			return 0;
		}

		if (fread(header, 1, 54, imgFile) != 54) {
			printf("Incorrect BMP file\n");
			return 0;
		}

		if (header[0] != 'B' || header[1] != 'M') {
			printf("Incorrect BMP file\n");
		}

		dataPosition	= *(int*)&(header[0x0A]);
		imgSize			= *(int*)&(header[0x22]);
		width			= *(int*)&(header[0x12]);
		height			= *(int*)&(header[0x16]);

		if (imgSize == 0)		imgSize = width * height * 3;
		if (dataPosition == 0)	dataPosition = 54;
		 
		rgbData = new unsigned char[imgSize];
		
		fread(rgbData, 1, imgSize, imgFile);
		fclose(imgFile);

		GLuint textureID;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, rgbData);

		delete[] rgbData;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
}