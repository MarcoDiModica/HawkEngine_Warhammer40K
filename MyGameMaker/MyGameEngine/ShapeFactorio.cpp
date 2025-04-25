#include "Mesh.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

std::shared_ptr<Mesh> Mesh::CreateCube()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	const vec3 v000(-1.0f, -1.0f, -1.0f);
	const vec3 v001(-1.0f, -1.0f, 1.0f);
	const vec3 v010(-1.0f, 1.0f, -1.0f);
	const vec3 v011(-1.0f, 1.0f, 1.0f);
	const vec3 v100(1.0f, -1.0f, -1.0f);
	const vec3 v101(1.0f, -1.0f, 1.0f);
	const vec3 v110(1.0f, 1.0f, -1.0f);
	const vec3 v111(1.0f, 1.0f, 1.0f);

	std::vector<Vertex> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	std::vector<unsigned int> indices;

	// Cara frontal (+Z)
	vertices.push_back(Vertex{ v001 }); // 0
	vertices.push_back(Vertex{ v101 }); // 1
	vertices.push_back(Vertex{ v111 }); // 2
	vertices.push_back(Vertex{ v011 }); // 3

	// Cara trasera (-Z)
	vertices.push_back(Vertex{ v100 }); // 4
	vertices.push_back(Vertex{ v000 }); // 5
	vertices.push_back(Vertex{ v010 }); // 6
	vertices.push_back(Vertex{ v110 }); // 7

	// Cara derecha (+X)
	vertices.push_back(Vertex{ v101 }); // 8
	vertices.push_back(Vertex{ v100 }); // 9
	vertices.push_back(Vertex{ v110 }); // 10
	vertices.push_back(Vertex{ v111 }); // 11

	// Cara izquierda (-X)
	vertices.push_back(Vertex{ v000 }); // 12
	vertices.push_back(Vertex{ v001 }); // 13
	vertices.push_back(Vertex{ v011 }); // 14
	vertices.push_back(Vertex{ v010 }); // 15

	// Cara superior (+Y)
	vertices.push_back(Vertex{ v011 }); // 16
	vertices.push_back(Vertex{ v111 }); // 17
	vertices.push_back(Vertex{ v110 }); // 18
	vertices.push_back(Vertex{ v010 }); // 19

	// Cara inferior (-Y)
	vertices.push_back(Vertex{ v000 }); // 20
	vertices.push_back(Vertex{ v100 }); // 21
	vertices.push_back(Vertex{ v101 }); // 22
	vertices.push_back(Vertex{ v001 }); // 23

	// Normals para cada vértice
	for (int i = 0; i < 4; i++) normals.push_back(vec3(0.0f, 0.0f, 1.0f));  // Cara frontal
	for (int i = 0; i < 4; i++) normals.push_back(vec3(0.0f, 0.0f, -1.0f)); // Cara trasera
	for (int i = 0; i < 4; i++) normals.push_back(vec3(1.0f, 0.0f, 0.0f));  // Cara derecha
	for (int i = 0; i < 4; i++) normals.push_back(vec3(-1.0f, 0.0f, 0.0f)); // Cara izquierda
	for (int i = 0; i < 4; i++) normals.push_back(vec3(0.0f, 1.0f, 0.0f));  // Cara superior
	for (int i = 0; i < 4; i++) normals.push_back(vec3(0.0f, -1.0f, 0.0f)); // Cara inferior

	// Asignar las normales a los vértices
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].normal = normals[i];
	}

	// Texcoords para cada cara (usando el mismo patrón para todas)
	for (int i = 0; i < 6; i++) {
		texCoords.push_back(vec2(0.0f, 0.0f)); // Esquina inferior izquierda
		texCoords.push_back(vec2(1.0f, 0.0f)); // Esquina inferior derecha
		texCoords.push_back(vec2(1.0f, 1.0f)); // Esquina superior derecha
		texCoords.push_back(vec2(0.0f, 1.0f)); // Esquina superior izquierda
	}

	// Asignar las coordenadas de textura a los vértices
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].texCoord = texCoords[i];
	}

	// Colores para cada cara
	vec3 faceColors[6] = {
		vec3(1.0f, 0.5f, 0.5f), // Frontal (Rosa claro)
		vec3(0.5f, 0.5f, 1.0f), // Trasera (Azul claro)
		vec3(1.0f, 0.5f, 0.0f), // Derecha (Naranja)
		vec3(0.0f, 0.5f, 1.0f), // Izquierda (Azul marino)
		vec3(0.5f, 1.0f, 0.5f), // Superior (Verde claro)
		vec3(1.0f, 1.0f, 0.5f)  // Inferior (Amarillo claro)
	};

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			colors.push_back(faceColors[i]);
		}
	}

	// Tangentes y bitangentes para cada cara
	std::vector<vec3> faceTangents = {
		vec3(1.0f, 0.0f, 0.0f),  // Frontal
		vec3(-1.0f, 0.0f, 0.0f), // Trasera
		vec3(0.0f, 0.0f, -1.0f), // Derecha
		vec3(0.0f, 0.0f, 1.0f),  // Izquierda
		vec3(1.0f, 0.0f, 0.0f),  // Superior
		vec3(1.0f, 0.0f, 0.0f)   // Inferior
	};

	std::vector<vec3> faceBitangents = {
		vec3(0.0f, 1.0f, 0.0f),  // Frontal
		vec3(0.0f, 1.0f, 0.0f),  // Trasera
		vec3(0.0f, 1.0f, 0.0f),  // Derecha
		vec3(0.0f, 1.0f, 0.0f),  // Izquierda
		vec3(0.0f, 0.0f, -1.0f), // Superior
		vec3(0.0f, 0.0f, 1.0f)   // Inferior
	};

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			tangents.push_back(faceTangents[i]);
			bitangents.push_back(faceBitangents[i]);

			// Asignar a los vértices
			int index = i * 4 + j;
			vertices[index].tangent = faceTangents[i];
			vertices[index].bitangent = faceBitangents[i];
		}
	}

	// Índices para cada cara (2 triángulos por cara)
	for (int i = 0; i < 6; i++) {
		int base = i * 4;
		indices.push_back(base);     // 0
		indices.push_back(base + 1); // 1
		indices.push_back(base + 2); // 2

		indices.push_back(base);     // 0
		indices.push_back(base + 2); // 2
		indices.push_back(base + 3); // 3
	}

	// Inicializar bone data como -1 para cada vértice
	for (auto& vertex : vertices) {
		model->SetVertexBoneDataToDefault(vertex);
	}

	// Asignar los datos al modelo
	model->GetModelData().vertexData = vertices;
	model->GetModelData().indexData = indices;
	model->GetModelData().vertex_normals = normals;
	model->GetModelData().vertex_texCoords = texCoords;
	model->GetModelData().vertex_colors = colors;
	model->GetModelData().vertex_tangents = tangents;
	model->GetModelData().vertex_bitangents = bitangents;

	model->SetMeshName("Cube");

	// Calcular bounding box
	BoundingBox meshBBox;
	meshBBox.min = vec3(-1.0f, -1.0f, -1.0f);
	meshBBox.max = vec3(1.0f, 1.0f, 1.0f);
	mesh->setBoundingBox(meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Cube");
	mesh->loadToOpenGL();

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreatePlane()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	// Crear vértices del plano
	std::vector<Vertex> vertices(4);
	vertices[0].position = vec3(-1.0f, 0.0f, 1.0f);   // Esquina inferior izquierda
	vertices[1].position = vec3(1.0f, 0.0f, 1.0f);    // Esquina inferior derecha
	vertices[2].position = vec3(1.0f, 0.0f, -1.0f);   // Esquina superior derecha
	vertices[3].position = vec3(-1.0f, 0.0f, -1.0f);  // Esquina superior izquierda

	// Normales (todas apuntando hacia arriba)
	std::vector<glm::vec3> normals(4, vec3(0.0f, 1.0f, 0.0f));

	// Asignar normales a vértices
	for (int i = 0; i < 4; i++) {
		vertices[i].normal = normals[i];
	}

	// Coordenadas de textura
	std::vector<glm::vec2> texCoords = {
		vec2(0.0f, 0.0f),  // 0: Esquina inferior izquierda
		vec2(1.0f, 0.0f),  // 1: Esquina inferior derecha
		vec2(1.0f, 1.0f),  // 2: Esquina superior derecha
		vec2(0.0f, 1.0f)   // 3: Esquina superior izquierda
	};

	// Asignar coordenadas de textura a vértices
	for (int i = 0; i < 4; i++) {
		vertices[i].texCoord = texCoords[i];
	}

	// Índices para los triángulos
	std::vector<unsigned int> indices = {
		0, 1, 2,  // Primer triángulo
		0, 2, 3   // Segundo triángulo
	};

	// Colores para los vértices (degradado de blanco a azul)
	std::vector<glm::vec3> colors = {
		vec3(1.0f, 1.0f, 1.0f),  // Blanco
		vec3(0.8f, 0.8f, 1.0f),  // Casi blanco
		vec3(0.4f, 0.4f, 1.0f),  // Azul claro
		vec3(0.0f, 0.0f, 1.0f)   // Azul
	};

	// Tangentes (a lo largo del eje X)
	std::vector<glm::vec3> tangents(4, vec3(1.0f, 0.0f, 0.0f));

	// Asignar tangentes a vértices
	for (int i = 0; i < 4; i++) {
		vertices[i].tangent = tangents[i];
	}

	// Bitangentes (a lo largo del eje Z negativo)
	std::vector<glm::vec3> bitangents(4, vec3(0.0f, 0.0f, -1.0f));

	// Asignar bitangentes a vértices
	for (int i = 0; i < 4; i++) {
		vertices[i].bitangent = bitangents[i];
	}

	// Inicializar bone data como -1 para cada vértice
	for (auto& vertex : vertices) {
		model->SetVertexBoneDataToDefault(vertex);
	}

	// Asignar datos al modelo
	model->GetModelData().vertexData = vertices;
	model->GetModelData().indexData = indices;
	model->GetModelData().vertex_normals = normals;
	model->GetModelData().vertex_texCoords = texCoords;
	model->GetModelData().vertex_colors = colors;
	model->GetModelData().vertex_tangents = tangents;
	model->GetModelData().vertex_bitangents = bitangents;

	model->SetMeshName("Plane");

	// Calcular bounding box
	BoundingBox meshBBox;
	meshBBox.min = vec3(-1.0f, 0.0f, -1.0f);
	meshBBox.max = vec3(1.0f, 0.0f, 1.0f);
	mesh->setBoundingBox(meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Plane");
	mesh->loadToOpenGL();

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreateSphere()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	const int stacks = 20;
	const int slices = 20;
	const float radius = 1.0f;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	// Generar vértices para la esfera
	for (int i = 0; i <= stacks; ++i) {
		float phi = i * glm::pi<float>() / stacks;
		float sinPhi = sin(phi);
		float cosPhi = cos(phi);

		for (int j = 0; j <= slices; ++j) {
			float theta = j * 2.0f * glm::pi<float>() / slices;
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);

			// Posición del vértice
			float x = cosTheta * sinPhi;
			float y = cosPhi;
			float z = sinTheta * sinPhi;

			// Crear vértice
			Vertex vertex;
			vertex.position = glm::vec3(x, y, z) * radius;

			// Normal (misma dirección que la posición para una esfera)
			vec3 normal = vec3(x, y, z);
			vertex.normal = normal;
			normals.push_back(normal);

			// Coordenadas de textura
			float u = static_cast<float>(j) / slices;
			float v = static_cast<float>(i) / stacks;
			vertex.texCoord = vec2(u, v);
			texCoords.push_back(vec2(u, v));

			// Color (degradado basado en posición)
			vec3 color = vec3(
				(x + 1.0f) * 0.5f,  // R: -1 a 1 mapeado a 0 a 1
				(y + 1.0f) * 0.5f,  // G: -1 a 1 mapeado a 0 a 1
				(z + 1.0f) * 0.5f   // B: -1 a 1 mapeado a 0 a 1
			);
			colors.push_back(color);

			// Calcular tangente
			// Para una esfera, podemos derivar la tangente con respecto a theta
			vec3 tangent = vec3(
				-sinTheta * sinPhi,  // dx/dtheta
				0.0f,                // dy/dtheta
				cosTheta * sinPhi    // dz/dtheta
			);

			// Normalizar la tangente
			if (glm::length(tangent) > 0.0001f) {
				tangent = glm::normalize(tangent);
			}
			else {
				// Evitar vectores muy pequeños
				tangent = vec3(1.0f, 0.0f, 0.0f);
			}
			vertex.tangent = tangent;
			tangents.push_back(tangent);

			// Calcular bitangente (cross product de normal y tangente)
			vec3 bitangent = glm::cross(normal, tangent);
			vertex.bitangent = bitangent;
			bitangents.push_back(bitangent);

			// Inicializar bone data
			model->SetVertexBoneDataToDefault(vertex);

			// Agregar vértice
			vertices.push_back(vertex);
		}
	}

	// Generar índices
	for (int i = 0; i < stacks; ++i) {
		for (int j = 0; j < slices; ++j) {
			int first = i * (slices + 1) + j;
			int second = first + slices + 1;

			// Triángulo 1
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			// Triángulo 2
			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}

	// Asignar datos al modelo
	model->GetModelData().vertexData = vertices;
	model->GetModelData().indexData = indices;
	model->GetModelData().vertex_normals = normals;
	model->GetModelData().vertex_texCoords = texCoords;
	model->GetModelData().vertex_colors = colors;
	model->GetModelData().vertex_tangents = tangents;
	model->GetModelData().vertex_bitangents = bitangents;

	model->SetMeshName("Sphere");

	// Calcular bounding box
	BoundingBox meshBBox;
	meshBBox.min = vec3(-radius, -radius, -radius);
	meshBBox.max = vec3(radius, radius, radius);
	mesh->setBoundingBox(meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Sphere");
	mesh->loadToOpenGL();

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreateCylinder() {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	std::shared_ptr<Model> model = std::make_shared<Model>();

	// Parámetros del cilindro
	const float radius = 1.0f;
	const float height = 2.0f;
	const int slices = 20;
	const float halfHeight = height / 2.0f;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	// Vértice central de la base inferior
	Vertex bottomCenter;
	bottomCenter.position = vec3(0.0f, -halfHeight, 0.0f);
	bottomCenter.texCoord = vec2(0.5f, 0.5f);
	bottomCenter.normal = vec3(0.0f, -1.0f, 0.0f);
	bottomCenter.tangent = vec3(1.0f, 0.0f, 0.0f);
	bottomCenter.bitangent = vec3(0.0f, 0.0f, -1.0f);
	model->SetVertexBoneDataToDefault(bottomCenter);

	vertices.push_back(bottomCenter);
	normals.push_back(bottomCenter.normal);
	texCoords.push_back(bottomCenter.texCoord);
	colors.push_back(vec3(0.2f, 0.2f, 0.8f)); // Azul oscuro
	tangents.push_back(bottomCenter.tangent);
	bitangents.push_back(bottomCenter.bitangent);

	// Vértices de la base inferior
	for (int i = 0; i < slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		Vertex vertex;
		vertex.position = vec3(x, -halfHeight, z);

		// Coordenada de textura
		float u = (cos(angle) + 1.0f) / 2.0f;
		float v = (sin(angle) + 1.0f) / 2.0f;
		vertex.texCoord = vec2(u, v);

		// Normal apuntando hacia abajo
		vertex.normal = vec3(0.0f, -1.0f, 0.0f);

		// Tangente - perpendicular a la normal y apuntando hacia fuera
		vec3 tangent = glm::normalize(vec3(z, 0.0f, -x));
		vertex.tangent = tangent;

		// Bitangente - producto vectorial de normal y tangente
		vec3 bitangent = glm::cross(vertex.normal, tangent);
		vertex.bitangent = bitangent;

		// Inicializar bone data
		model->SetVertexBoneDataToDefault(vertex);

		vertices.push_back(vertex);
		normals.push_back(vertex.normal);
		texCoords.push_back(vertex.texCoord);
		colors.push_back(vec3(0.2f, 0.2f, 0.8f)); // Azul oscuro
		tangents.push_back(tangent);
		bitangents.push_back(bitangent);
	}

	// Vértice central de la base superior
	Vertex topCenter;
	topCenter.position = vec3(0.0f, halfHeight, 0.0f);
	topCenter.texCoord = vec2(0.5f, 0.5f);
	topCenter.normal = vec3(0.0f, 1.0f, 0.0f);
	topCenter.tangent = vec3(1.0f, 0.0f, 0.0f);
	topCenter.bitangent = vec3(0.0f, 0.0f, -1.0f);
	model->SetVertexBoneDataToDefault(topCenter);

	vertices.push_back(topCenter);
	normals.push_back(topCenter.normal);
	texCoords.push_back(topCenter.texCoord);
	colors.push_back(vec3(0.8f, 0.2f, 0.2f)); // Rojo oscuro
	tangents.push_back(topCenter.tangent);
	bitangents.push_back(topCenter.bitangent);

	// Vértices de la base superior
	for (int i = 0; i < slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		Vertex vertex;
		vertex.position = vec3(x, halfHeight, z);

		// Coordenada de textura
		float u = (cos(angle) + 1.0f) / 2.0f;
		float v = (sin(angle) + 1.0f) / 2.0f;
		vertex.texCoord = vec2(u, v);

		// Normal apuntando hacia arriba
		vertex.normal = vec3(0.0f, 1.0f, 0.0f);

		// Tangente - perpendicular a la normal y apuntando hacia fuera
		vec3 tangent = glm::normalize(vec3(z, 0.0f, -x));
		vertex.tangent = tangent;

		// Bitangente - producto vectorial de normal y tangente
		vec3 bitangent = glm::cross(vertex.normal, tangent);
		vertex.bitangent = bitangent;

		// Inicializar bone data
		model->SetVertexBoneDataToDefault(vertex);

		vertices.push_back(vertex);
		normals.push_back(vertex.normal);
		texCoords.push_back(vertex.texCoord);
		colors.push_back(vec3(0.8f, 0.2f, 0.2f)); // Rojo oscuro
		tangents.push_back(tangent);
		bitangents.push_back(bitangent);
	}

	// Vértices para el cuerpo del cilindro
	for (int i = 0; i < slices; ++i) {
		float angle = 2.0f * glm::pi<float>() * i / slices;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		// Vector normal radial (apunta hacia afuera)
		vec3 normal = glm::normalize(vec3(x, 0.0f, z));

		// Vector tangente (alrededor del cilindro)
		vec3 tangent = glm::normalize(vec3(-z, 0.0f, x));

		// Bitangente (a lo largo del eje Y)
		vec3 bitangent = vec3(0.0f, 1.0f, 0.0f);

		// Vértice inferior del cuerpo
		Vertex bottomVertex;
		bottomVertex.position = vec3(x, -halfHeight, z);
		bottomVertex.texCoord = vec2(static_cast<float>(i) / slices, 0.0f);
		bottomVertex.normal = normal;
		bottomVertex.tangent = tangent;
		bottomVertex.bitangent = bitangent;
		model->SetVertexBoneDataToDefault(bottomVertex);

		vertices.push_back(bottomVertex);
		normals.push_back(normal);
		texCoords.push_back(bottomVertex.texCoord);
		colors.push_back(vec3(0.2f, 0.5f, 0.7f)); // Gradiente azul a verde
		tangents.push_back(tangent);
		bitangents.push_back(bitangent);

		// Vértice superior del cuerpo
		Vertex topVertex;
		topVertex.position = vec3(x, halfHeight, z);
		topVertex.texCoord = vec2(static_cast<float>(i) / slices, 1.0f);
		topVertex.normal = normal;
		topVertex.tangent = tangent;
		topVertex.bitangent = bitangent;
		model->SetVertexBoneDataToDefault(topVertex);

		vertices.push_back(topVertex);
		normals.push_back(normal);
		texCoords.push_back(topVertex.texCoord);
		colors.push_back(vec3(0.7f, 0.5f, 0.2f)); // Gradiente verde a rojo
		tangents.push_back(tangent);
		bitangents.push_back(bitangent);
	}

	// Índices para la base inferior
	unsigned int bottomCenterIndex = 0;
	for (int i = 0; i < slices; ++i) {
		unsigned int current = i + 1;
		unsigned int next = (i + 1) % slices + 1;

		indices.push_back(bottomCenterIndex);
		indices.push_back(current);
		indices.push_back(next);
	}

	// Índices para la base superior
	unsigned int topCenterIndex = slices + 1;
	for (int i = 0; i < slices; ++i) {
		unsigned int current = topCenterIndex + i + 1;
		unsigned int next = topCenterIndex + (i + 1) % slices + 1;

		indices.push_back(topCenterIndex);
		indices.push_back(next);
		indices.push_back(current);
	}

	// Índices para el cuerpo del cilindro
	unsigned int bodyStartIndex = topCenterIndex + slices + 1;
	for (int i = 0; i < slices; ++i) {
		unsigned int bottomLeft = bodyStartIndex + i * 2;
		unsigned int bottomRight = bodyStartIndex + ((i + 1) % slices) * 2;
		unsigned int topLeft = bottomLeft + 1;
		unsigned int topRight = bottomRight + 1;

		// Triangulación del cuerpo (winding order correcto)
		indices.push_back(bottomLeft);
		indices.push_back(topRight);
		indices.push_back(bottomRight);

		indices.push_back(bottomLeft);
		indices.push_back(topLeft);
		indices.push_back(topRight);
	}

	// Asignar datos al modelo
	model->GetModelData().vertexData = vertices;
	model->GetModelData().indexData = indices;
	model->GetModelData().vertex_normals = normals;
	model->GetModelData().vertex_texCoords = texCoords;
	model->GetModelData().vertex_colors = colors;
	model->GetModelData().vertex_tangents = tangents;
	model->GetModelData().vertex_bitangents = bitangents;

	model->SetMeshName("Cylinder");

	// Calcular bounding box
	BoundingBox meshBBox;
	meshBBox.min = vec3(-radius, -halfHeight, -radius);
	meshBBox.max = vec3(radius, halfHeight, radius);
	mesh->setBoundingBox(meshBBox);

	mesh->setModel(model);
	mesh->filePath = std::string("Shapes/Cylinder");
	mesh->loadToOpenGL();

	return mesh;
}