#include "Mesh.h"

Mesh::Mesh()
{
	
}

void Mesh::setMeshData(const MeshData& inMeshData)
{
	name = inMeshData.meshName;

	vertices = inMeshData.vertices;
	uvs = inMeshData.uvs;
	normals = inMeshData.normals;

	initialized();
}

void Mesh::initialized()
{
}
