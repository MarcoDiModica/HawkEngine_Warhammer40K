#include "ForwardPlus.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include "../MyGameEngine/Shaders.h"
#include "../MyGameEngine/ShaderManager.h"

ForwardPlusLighting& ForwardPlusLighting::GetInstance() {
	static ForwardPlusLighting instance;
	return instance;
}

bool ForwardPlusLighting::Initialize(int windowWidth, int windowHeight) {
	screenWidth = windowWidth;
	screenHeight = windowHeight;

	if (!GLEW_ARB_compute_shader) {
		LOG(LogType::LOG_INFO, "Compute Shaders no soportados por esta GPU!");
		useClusteredLighting = false;
	}

	RecalculateTiles();

	// Crear buffer para luces puntuales
	glCreateBuffers(1, &pointLightBuffer);
	glNamedBufferStorage(pointLightBuffer,
		MAX_POINT_LIGHTS * sizeof(GPUPointLight),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	// Crear buffer para luz direccional
	glCreateBuffers(1, &directionalLightBuffer);
	glNamedBufferStorage(directionalLightBuffer,
		sizeof(GPUDirectionalLight),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	// Crear buffers para tile grid y light indices
	int numTiles = tilesX * tilesY;
	glCreateBuffers(1, &lightGridBuffer);
	glNamedBufferStorage(lightGridBuffer,
		numTiles * sizeof(glm::uvec2),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &lightIndicesBuffer);
	glNamedBufferStorage(lightIndicesBuffer,
		numTiles * maxLightsPerTile * sizeof(uint32_t),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	// Inicializar luz direccional por defecto
	directionalLight.direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	directionalLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLight.castShadow = 0;
	directionalLight.shadowMapIndex = 0;
	directionalLight.useCascades = 0;
	directionalLight.numCascades = 0;

	// Compilar el shader de culling si es posible
	if (GLEW_ARB_compute_shader) {
		if (!CompileLightCullingShader()) {
			LOG(LogType::LOG_ERROR, "Error: No se pudo compilar el shader de culling de luces");
		}
	}

	// Inicializar buffers de clústeres cuando usemos clustered forward
	if (useClusteredLighting) {
		// Por implementar: crear estructuras para clustering 3D
	}

	// Bindear buffers al shader bindless_pbr
	GLuint bindlessPBRShader = ShaderManager::GetInstance().GetShaderProgram(ShaderType::BINDLESS_PBR);
	if (bindlessPBRShader != 0) {
		glUseProgram(bindlessPBRShader);

		// Configurar binding points
		GLuint pointLightBindingPoint = 3;
		GLuint dirLightBindingPoint = 4;
		GLuint lightGridBindingPoint = 5;
		GLuint lightIndicesBindingPoint = 6;

		// Asignar buffers a binding points específicos
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, pointLightBindingPoint, pointLightBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, dirLightBindingPoint, directionalLightBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, lightGridBindingPoint, lightGridBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, lightIndicesBindingPoint, lightIndicesBuffer);

		// Configurar uniforms
		glUniform1i(glGetUniformLocation(bindlessPBRShader, "tileSize"), tileSize);
		glUniform2i(glGetUniformLocation(bindlessPBRShader, "screenSize"), screenWidth, screenHeight);
		glUniform1i(glGetUniformLocation(bindlessPBRShader, "useForwardPlus"), 1);

		glUseProgram(0);
	}

	return true;
}

void ForwardPlusLighting::Shutdown() {
	if (pointLightBuffer) glDeleteBuffers(1, &pointLightBuffer);
	if (directionalLightBuffer) glDeleteBuffers(1, &directionalLightBuffer);
	if (lightGridBuffer) glDeleteBuffers(1, &lightGridBuffer);
	if (lightIndicesBuffer) glDeleteBuffers(1, &lightIndicesBuffer);

	if (lightCullingShader) glDeleteProgram(lightCullingShader);

	pointLightBuffer = 0;
	directionalLightBuffer = 0;
	lightGridBuffer = 0;
	lightIndicesBuffer = 0;
	lightCullingShader = 0;

	pointLights.clear();
	visibleLightCount = 0;
}

void ForwardPlusLighting::Resize(int width, int height) {
	screenWidth = width;
	screenHeight = height;

	RecalculateTiles();

	if (lightGridBuffer) {
		glDeleteBuffers(1, &lightGridBuffer);
		glCreateBuffers(1, &lightGridBuffer);
		glNamedBufferStorage(lightGridBuffer,
			tilesX * tilesY * sizeof(glm::uvec2),
			nullptr,
			GL_DYNAMIC_STORAGE_BIT);
	}

	if (lightIndicesBuffer) {
		glDeleteBuffers(1, &lightIndicesBuffer);
		glCreateBuffers(1, &lightIndicesBuffer);
		glNamedBufferStorage(lightIndicesBuffer,
			tilesX * tilesY * maxLightsPerTile * sizeof(uint32_t),
			nullptr,
			GL_DYNAMIC_STORAGE_BIT);
	}
}

void ForwardPlusLighting::CollectLights(const std::vector<GameObject*>& gameObjects) {
	pointLights.clear();

	bool foundDirectional = false;

	for (auto* gameObject : gameObjects) {
		if (!gameObject || !gameObject->IsActive()) continue;

		if (gameObject->HasComponent<LightComponent>()) {
			LightComponent* light = gameObject->GetComponent<LightComponent>();

			if (light->GetLightType() == LightType::DIRECTIONAL) {
				UpdateDirectionalLight(light);
				foundDirectional = true;
			}
			else if (light->GetLightType() == LightType::POINT) {
				if (pointLights.size() < MAX_POINT_LIGHTS) {
					pointLights.push_back(ConvertToGPULight(light));
				}
				else {
					LOG(LogType::LOG_WARNING, "Warning: Excedido máximo de luces puntuales");
				}
			}
		}
	}

	// Si no hay luz direccional, dejamos la luz por defecto
	if (!foundDirectional) {
		directionalLight.direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
		directionalLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		directionalLight.castShadow = 0;
		directionalLight.shadowMapIndex = 0;
		directionalLight.useCascades = 0;
		directionalLight.numCascades = 0;
	}
}

void ForwardPlusLighting::UpdateLights() {
	if (!pointLights.empty()) {
		glNamedBufferSubData(pointLightBuffer, 0,
			pointLights.size() * sizeof(GPUPointLight),
			pointLights.data());
	}

	glNamedBufferSubData(directionalLightBuffer, 0,
		sizeof(GPUDirectionalLight),
		&directionalLight);
}

void ForwardPlusLighting::PerformLightCulling(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
	if (!GLEW_ARB_compute_shader || !lightCullingShader || pointLights.empty()) {
		// Si no podemos usar compute shaders o no hay luces, usamos el fallback
		CPUFallbackCulling(viewMatrix, projMatrix);
		return;
	}

	// Ejecutar shader de culling de luces
	glUseProgram(lightCullingShader);

	// Pasar matrices al shader
	glUniformMatrix4fv(glGetUniformLocation(lightCullingShader, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(lightCullingShader, "projMatrix"), 1, GL_FALSE, glm::value_ptr(projMatrix));

	// Pasar otros parámetros
	glUniform1i(glGetUniformLocation(lightCullingShader, "numLights"), (int)pointLights.size());
	glUniform1i(glGetUniformLocation(lightCullingShader, "tileSize"), tileSize);
	glUniform1i(glGetUniformLocation(lightCullingShader, "maxLightsPerTile"), maxLightsPerTile);
	glUniform2i(glGetUniformLocation(lightCullingShader, "screenSize"), screenWidth, screenHeight);

	// Bindear buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pointLightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightGridBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, lightIndicesBuffer);

	// Dispatch compute shader
	int computeGroupsX = (tilesX + 15) / 16;
	int computeGroupsY = (tilesY + 15) / 16;
	glDispatchCompute(computeGroupsX, computeGroupsY, 1);

	// Barrera de memoria para asegurar que los resultados estén disponibles
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glUseProgram(0);

	// Por ahora, asumimos que todas las luces son visibles para estadísticas
	visibleLightCount = static_cast<int>(pointLights.size());
}

void ForwardPlusLighting::CPUFallbackCulling(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
	// Simplemente asignamos todas las luces a todos los tiles como fallback
	int totalTiles = tilesX * tilesY;

	std::vector<glm::uvec2> tileData(totalTiles);
	std::vector<uint32_t> lightIndices(totalTiles * maxLightsPerTile);

	// Para cada tile, asignamos todas las luces
	for (int tileIdx = 0; tileIdx < totalTiles; tileIdx++) {
		uint32_t offset = tileIdx * maxLightsPerTile;
		uint32_t count = std::min((uint32_t)pointLights.size(), (uint32_t)maxLightsPerTile);

		// Almacenar offset y conteo en el grid
		tileData[tileIdx] = glm::uvec2(offset, count);

		// Asignar índices de luces al tile
		for (uint32_t lightIdx = 0; lightIdx < count; lightIdx++) {
			lightIndices[offset + lightIdx] = lightIdx;
		}
	}

	// Actualizar buffers
	glNamedBufferSubData(lightGridBuffer, 0, tileData.size() * sizeof(glm::uvec2), tileData.data());
	glNamedBufferSubData(lightIndicesBuffer, 0, lightIndices.size() * sizeof(uint32_t), lightIndices.data());

	visibleLightCount = static_cast<int>(pointLights.size());
}

void ForwardPlusLighting::RecalculateTiles() {
	tilesX = (screenWidth + tileSize - 1) / tileSize;
	tilesY = (screenHeight + tileSize - 1) / tileSize;
}

bool ForwardPlusLighting::CompileLightCullingShader() {
	const char* lightCullingSource = R"(
        #version 450 core
        
        layout(local_size_x = 16, local_size_y = 16) in;
        
        // Estructura para luz puntual
        struct PointLight {
            vec4 position;    // xyz = posición, w = radio
            vec4 color;       // rgb = color, a = intensidad
            vec4 attenuation; // x = constante, y = lineal, z = cuadrática, w = no usado
            uint lightType;   // 0 = point, 1 = spot (futuro)
            uint castShadow;  // 0 = no, 1 = yes
            uint shadowMapIndex; // índice a shadowmap (si usa sombras)
            uint padding;     // alineación
        };
        
        // Buffer de luces puntuales
        layout(std430, binding = 0) readonly buffer PointLightBuffer {
            PointLight lights[];
        };
        
        // Buffer para grid de tiles
        layout(std430, binding = 1) buffer LightGridBuffer {
            uvec2 data[];     // x = offset into light indices, y = light count
        } lightGrid;
        
        // Buffer para índices de luces por tile
        layout(std430, binding = 2) buffer LightIndicesBuffer {
            uint indices[];
        } lightIndices;
        
        // Uniforms
        uniform mat4 viewMatrix;
        uniform mat4 projMatrix;
        uniform int numLights;
        uniform int tileSize;
        uniform int maxLightsPerTile;
        uniform ivec2 screenSize;
        
        // Variables compartidas
        shared uint visibleLightCount;
        shared uint visibleLightIndices[64]; // Usar maxLightsPerTile pero 64 es un buen valor base
        
        // Estructura para planos de frustum
        struct Plane {
            vec3 normal;
            float distance;
        };
        
        // Verifica si la esfera está delante del plano
        bool sphereInsidePlane(vec4 sphere, Plane plane) {
            return dot(plane.normal, sphere.xyz) - plane.distance > -sphere.w;
        }
        
        void main() {
            // Obtener ID de tile
            uvec2 tileID = gl_WorkGroupID.xy;
            uint tileIndex = tileID.y * gl_NumWorkGroups.x + tileID.x;
            
            // Esquinas del tile en espacio de pantalla
            ivec2 tileStart = ivec2(tileID) * tileSize;
            ivec2 tileEnd = min(tileStart + ivec2(tileSize), screenSize);
            
            // Inicializar contador de luces visibles
            if (gl_LocalInvocationIndex == 0) {
                visibleLightCount = 0;
            }
            
            barrier();
            
            // Construir frustum para este tile
            vec2 tileSizeNDC = 2.0 * vec2(tileSize) / vec2(screenSize);
            vec2 tileStartNDC = 2.0 * vec2(tileStart) / vec2(screenSize) - 1.0;
            
            vec4 frustumCorners[8];
            // Esquinas near plane
            frustumCorners[0] = vec4(tileStartNDC.x, tileStartNDC.y, -1.0, 1.0);
            frustumCorners[1] = vec4(tileStartNDC.x + tileSizeNDC.x, tileStartNDC.y, -1.0, 1.0);
            frustumCorners[2] = vec4(tileStartNDC.x, tileStartNDC.y + tileSizeNDC.y, -1.0, 1.0);
            frustumCorners[3] = vec4(tileStartNDC.x + tileSizeNDC.x, tileStartNDC.y + tileSizeNDC.y, -1.0, 1.0);
            // Esquinas far plane
            frustumCorners[4] = vec4(tileStartNDC.x, tileStartNDC.y, 1.0, 1.0);
            frustumCorners[5] = vec4(tileStartNDC.x + tileSizeNDC.x, tileStartNDC.y, 1.0, 1.0);
            frustumCorners[6] = vec4(tileStartNDC.x, tileStartNDC.y + tileSizeNDC.y, 1.0, 1.0);
            frustumCorners[7] = vec4(tileStartNDC.x + tileSizeNDC.x, tileStartNDC.y + tileSizeNDC.y, 1.0, 1.0);
            
            // Transformar a world space
            mat4 invViewProj = inverse(projMatrix * viewMatrix);
            for (int i = 0; i < 8; i++) {
                frustumCorners[i] = invViewProj * frustumCorners[i];
                frustumCorners[i] /= frustumCorners[i].w;
            }
            
            // Calcular planos del frustum (en world space)
            Plane frustumPlanes[6];
            
            // Función para crear plano a partir de 3 puntos
            // Esta función asume que los puntos están en sentido horario vistos desde el lado positivo del plano
            vec3 a, b, c, normal;
            float d;
            
            // Plano cerca (near)
            a = frustumCorners[0].xyz;
            b = frustumCorners[1].xyz;
            c = frustumCorners[2].xyz;
            normal = normalize(cross(c - a, b - a));
            d = dot(normal, a);
            frustumPlanes[0] = Plane(normal, d);
            
            // Plano lejos (far)
            a = frustumCorners[4].xyz;
            b = frustumCorners[6].xyz;
            c = frustumCorners[5].xyz;
            normal = normalize(cross(c - a, b - a));
            d = dot(normal, a);
            frustumPlanes[1] = Plane(normal, d);
            
            // Plano izquierda (left)
            a = frustumCorners[0].xyz;
            b = frustumCorners[2].xyz;
            c = frustumCorners[4].xyz;
            normal = normalize(cross(c - a, b - a));
            d = dot(normal, a);
            frustumPlanes[2] = Plane(normal, d);
            
            // Plano derecha (right)
            a = frustumCorners[1].xyz;
            b = frustumCorners[5].xyz;
            c = frustumCorners[3].xyz;
            normal = normalize(cross(c - a, b - a));
            d = dot(normal, a);
            frustumPlanes[3] = Plane(normal, d);
            
            // Plano abajo (bottom)
            a = frustumCorners[0].xyz;
            b = frustumCorners[4].xyz;
            c = frustumCorners[1].xyz;
            normal = normalize(cross(c - a, b - a));
            d = dot(normal, a);
            frustumPlanes[4] = Plane(normal, d);
            
            // Plano arriba (top)
            a = frustumCorners[2].xyz;
            b = frustumCorners[3].xyz;
            c = frustumCorners[6].xyz;
            normal = normalize(cross(c - a, b - a));
            d = dot(normal, a);
            frustumPlanes[5] = Plane(normal, d);
            
            // Repartir trabajo entre threads
            uint lightIndexStart = gl_LocalInvocationIndex;
            uint lightIndexStep = gl_WorkGroupSize.x * gl_WorkGroupSize.y;
            
            // Recorrer todas las luces y verificar intersección con frustum
            for (uint i = lightIndexStart; i < numLights; i += lightIndexStep) {
                vec4 lightSphere = vec4(lights[i].position.xyz, lights[i].position.w);
                
                // Verificar si la luz intersecta con el frustum
                bool isVisible = true;
                for (int j = 0; j < 6; j++) {
                    if (!sphereInsidePlane(lightSphere, frustumPlanes[j])) {
                        isVisible = false;
                        break;
                    }
                }
                
                // Si es visible, añadir a la lista compartida
                if (isVisible) {
                    uint index = atomicAdd(visibleLightCount, 1);
                    if (index < 64) { // Usar maxLightsPerTile en lugar de 64
                        visibleLightIndices[index] = i;
                    }
                }
            }
            
            barrier();
            
            // Solo el primer thread por grupo escribe los resultados
            if (gl_LocalInvocationIndex == 0) {
                uint count = min(visibleLightCount, 64u); // Usar maxLightsPerTile en lugar de 64
                
                // Escribir información del tile en grid
                uint offset = tileIndex * maxLightsPerTile;
                lightGrid.data[tileIndex] = uvec2(offset, count);
                
                // Escribir índices de luces para este tile
                for (uint i = 0; i < count; i++) {
                    lightIndices.indices[offset + i] = visibleLightIndices[i];
                }
            }
        }
    )";

	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &lightCullingSource, nullptr);
	glCompileShader(computeShader);

	GLint success;
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(computeShader, 512, nullptr, infoLog);
		LOG(LogType::LOG_ERROR, "Error compilando compute shader: %s", infoLog);
		glDeleteShader(computeShader);
		return false;
	}

	lightCullingShader = glCreateProgram();
	glAttachShader(lightCullingShader, computeShader);
	glLinkProgram(lightCullingShader);

	glGetProgramiv(lightCullingShader, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(lightCullingShader, 512, nullptr, infoLog);
		LOG(LogType::LOG_ERROR, "Error linkando programa compute: %s", infoLog);
		glDeleteShader(computeShader);
		glDeleteProgram(lightCullingShader);
		lightCullingShader = 0;
		return false;
	}

	glDeleteShader(computeShader);

	return true;
}

GPUPointLight ForwardPlusLighting::ConvertToGPULight(const LightComponent* light) {
	GPUPointLight gpuLight;

	gpuLight.position = glm::vec4(light->owner->GetTransform()->GetPosition(), light->GetRadius());
	gpuLight.color = glm::vec4(light->GetAmbient(), light->GetIntensity());
	gpuLight.attenuation = glm::vec4(
		light->GetConstant(),
		light->GetLinear(),
		light->GetQuadratic(),
		0.0f
	);

	gpuLight.lightType = 0; // point light
	gpuLight.castShadow = 0; // no shadows yet
	gpuLight.shadowMapIndex = 0;
	gpuLight.padding = 0;

	return gpuLight;
}


void ForwardPlusLighting::UpdateDirectionalLight(const LightComponent* light) {
	if (!light) return;

	glm::vec3 direction = light->owner->GetTransform()->GetForward();
	directionalLight.direction = glm::vec4(direction, light->GetIntensity());
	directionalLight.color = glm::vec4(light->GetAmbient(), 1.0f);
	directionalLight.castShadow = 0; // no shadows yet
}