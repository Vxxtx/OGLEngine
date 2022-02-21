
#include "Mesh.h"

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vertexBuf);
	glDeleteBuffers(1, &uvBuf);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shaderProgId);

	glDeleteTextures(1, &objTexture);
}

void Mesh::setMeshData(const MeshData& inMeshData)
{
	//name = inMeshData.meshName;

	vertices = inMeshData.vertices;
	uvs = inMeshData.uvs;
	normals = inMeshData.normals;

	initialized();
}

void Mesh::draw(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgId);

    glm::mat4 mvp = projectionMatrix * viewMatrix * glm::mat4(1.0);

    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, &glm::mat4(1.0)[0][0]);
    glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);

	GLuint lightID = glGetUniformLocation(shaderProgId, "LightPosition_worldspace");

    glm::vec3 lightPos = glm::vec3(4, 4, 4);
    glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, objTexture);

    glUniform1i(textureId, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuf);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuf);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // change to this when vertex indexing done
    //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	printf("%d\n", vertices.size());
}

void Mesh::initialized()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shaderProgId = LoadShaders("engine/vertex.txt", "engine/frag.txt");

	matrixId = glGetUniformLocation(shaderProgId, "MVP");
	viewMatrixId = glGetUniformLocation(shaderProgId, "V");
	modelMatrixId = glGetUniformLocation(shaderProgId, "M");

	objTexture = LoadBMP("engine/tex.bmp");
	textureId = glGetUniformLocation(shaderProgId, "myTextureSampler");

	glGenBuffers(1, &vertexBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvBuf);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuf);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuf);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuf);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glUseProgram(shaderProgId);

}

GLuint Mesh::LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
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

GLuint Mesh::LoadBMP(const char* imgPath)
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

	dataPosition = *(int*)&(header[0x0A]);
	imgSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

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

bool Mesh::LoadObject(const char* path, MeshData& outMesh)
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

Mesh Mesh::SpawnObject(glm::vec3 world_position)
{
	MeshData newMeshData;
	bool result = Mesh::LoadObject("engine/cube.obj", newMeshData);

	Mesh newMesh{};
	newMesh.vertices = newMeshData.vertices;
	newMesh.uvs = newMeshData.uvs;
	newMesh.normals = newMeshData.normals;

	return newMesh;
}