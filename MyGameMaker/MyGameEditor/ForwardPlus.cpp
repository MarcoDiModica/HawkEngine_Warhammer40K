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

	glCreateBuffers(1, &pointLightBuffer);
	glNamedBufferStorage(pointLightBuffer,
		MAX_POINT_LIGHTS * sizeof(GPUPointLight),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &directionalLightBuffer);
	glNamedBufferStorage(directionalLightBuffer,
		sizeof(GPUDirectionalLight),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT);

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

	directionalLight.direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	directionalLight.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLight.castShadow = 0;
	directionalLight.shadowMapIndex = 0;
	directionalLight.useCascades = 0;
	directionalLight.numCascades = 0;

	if (GLEW_ARB_compute_shader) {
		if (!CompileLightCullingShader()) {
			LOG(LogType::LOG_ERROR, "Error: No se pudo compilar el shader de culling de luces");
		}
	}

	if (useClusteredLighting) {
		//aqui no hay nada quizas si forward no va bien
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

    UpdateShadowSettings(shadowSettings);
}

void ForwardPlusLighting::PerformLightCulling(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {
	if (!GLEW_ARB_compute_shader || !lightCullingShader || pointLights.empty()) {
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
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

	float radius = light->GetRadius();
	if (radius <= 0.0f) {
		float threshold = 0.001f; // Lower threshold for smoother falloff
		float linear = light->GetLinear();
		float quadratic = light->GetQuadratic();
		float constant = light->GetConstant();

		// Ensure quadratic is not zero to avoid division by zero
		if (quadratic > 0.0f) {
			radius = (-linear + sqrtf(linear * linear - 4 * quadratic * (constant - (256.0f / threshold)))) / (2 * quadratic);
		}
		else {
			// Fallback to linear attenuation if quadratic is zero
			radius = 256.0f / (linear + threshold);
		}

		// Clamp the radius to a minimum value
        radius = std::max(radius, 1.0f); // Minimum radius of 1.0
	}
	gpuLight.position.w = radius;
	
	
	gpuLight.lightType = 0; // point light
    gpuLight.castShadow = 1;
    gpuLight.shadowMapIndex = gpuLight.castShadow ? AcquirePointShadowIndex() : 0;
	gpuLight.padding = 0;

	return gpuLight;
}


void ForwardPlusLighting::UpdateDirectionalLight(const LightComponent* light) {
	if (!light) return;

	glm::vec3 direction = light->GetDirection();
	directionalLight.direction = glm::vec4(direction, light->GetIntensity());
	directionalLight.color = glm::vec4(light->GetAmbient(), 1.0f);
    directionalLight.castShadow = 1;
    directionalLight.shadowMapIndex = directionalLight.castShadow ? AcquireDirShadowIndex() : 0;
}

void ForwardPlusLighting::SetupShadowMaps() {
    if (shadowMapFBO == 0) {
        glGenFramebuffers(1, &shadowMapFBO);
    }

    // Point light shadow maps (cubemap array)
    if (pointShadowMapArray == 0) {
        glGenTextures(1, &pointShadowMapArray);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, pointShadowMapArray);
        glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT32F,
            shadowMapSize, shadowMapSize,
            maxPointLightShadows * 6, // 6 faces per cubemap
            0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }

    // Directional light shadow maps (2D array)
    if (directionalShadowMapArray == 0) {
        glGenTextures(1, &directionalShadowMapArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, directionalShadowMapArray);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
            shadowMapSize, shadowMapSize,
            maxDirectionalLightShadows * maxCascades,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    // Check FBO completeness
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    if (pointShadowMapArray != 0) {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            pointShadowMapArray, 0, 0); // Layer 0
    }
    else if (directionalShadowMapArray != 0) {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            directionalShadowMapArray, 0, 0); // Layer 0
    }

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);



    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG(LogType::LOG_ERROR, "Shadow FBO not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardPlusLighting::UpdateShadowSettings(const ShadowSettings& settings) {
    shadowSettings = settings;
    shadowMapSize = settings.shadowMapSize;

    // Recreate textures if size changed
    if (pointShadowMapArray != 0) {
        glDeleteTextures(1, &pointShadowMapArray);
        pointShadowMapArray = 0;
    }
    if (directionalShadowMapArray != 0) {
        glDeleteTextures(1, &directionalShadowMapArray);
        directionalShadowMapArray = 0;
    }

    SetupShadowMaps();
}

glm::mat4 ForwardPlusLighting::CalculatePointLightMatrix(const glm::vec3& lightPos, int face) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, shadowSettings.shadowFarPlane);

    glm::mat4 shadowView;
    switch (face) {
    case 0: // +X
        shadowView = glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        break;
    case 1: // -X
        shadowView = glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        break;
    case 2: // +Y
        shadowView = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        break;
    case 3: // -Y
        shadowView = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
        break;
    case 4: // +Z
        shadowView = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
        break;
    case 5: // -Z
        shadowView = glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
        break;
    }

    return shadowProj * shadowView;
}

std::vector<glm::mat4> ForwardPlusLighting::CalculatePointLightMatrices(const glm::vec3& lightPos) {
    std::vector<glm::mat4> matrices(6);
    for (int i = 0; i < 6; ++i) {
        matrices[i] = CalculatePointLightMatrix(lightPos, i);
    }
    return matrices;
}

std::vector<glm::mat4> ForwardPlusLighting::CalculateDirectionalLightMatrices(
    const glm::vec3& lightDir,
    const glm::vec3& cameraPos,
    const glm::mat4& cameraView) {

    std::vector<glm::mat4> matrices;
    matrices.reserve(maxCascades);

    // Calculate cascade splits
    float cascadeSplits[4]; // THIS HAS TO MATCH MAXCASCADES!!!!
    float nearClip = 0.1f;
    float farClip = shadowSettings.shadowFarPlane;
    float clipRange = farClip - nearClip;

    float minZ = nearClip;
    float maxZ = nearClip + clipRange;

    float range = maxZ - minZ;
    float ratio = maxZ / minZ;

    for (uint32_t i = 0; i < maxCascades; i++) {
        float p = (i + 1) / static_cast<float>(maxCascades);
        float log = minZ * std::pow(ratio, p);
        float uniform = minZ + range * p;
        float d = shadowSettings.cascadeLambda * (log - uniform) + uniform;
        cascadeSplits[i] = (d - nearClip) / clipRange;
    }

    float lastSplitDist = 0.0;
    for (uint32_t i = 0; i < maxCascades; i++) {
        float splitDist = cascadeSplits[i];

        glm::vec3 frustumCorners[8] = {
            // Near plane
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            // Far plane
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),
        };

        // Project frustum corners into world space
        glm::mat4 invCam = glm::inverse(cameraView);
        for (uint32_t i = 0; i < 8; i++) {
            glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
            frustumCorners[i] = invCorner / invCorner.w;
        }

        // Calculate frustum split corners
        for (uint32_t i = 0; i < 4; i++) {
            glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
            frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
            frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
        }

        // Calculate frustum centroid
        glm::vec3 frustumCenter = glm::vec3(0.0f);
        for (uint32_t i = 0; i < 8; i++) {
            frustumCenter += frustumCorners[i];
        }
        frustumCenter /= 8.0f;

        // Calculate light view matrix
        glm::mat4 lightView = glm::lookAt(
            frustumCenter - glm::normalize(lightDir) * 10.0f, // Move back a bit
            frustumCenter,
            glm::vec3(0.0f, 1.0f, 0.0f));

        // Calculate frustum bounds in light space
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();

        for (uint32_t i = 0; i < 8; i++) {
            glm::vec4 trf = lightView * glm::vec4(frustumCorners[i], 1.0f);
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // Stabilize the shadow map by snapping to texel increments
        float worldUnitsPerTexel = (maxX - minX) / shadowMapSize;
        glm::mat4 lightProjection = glm::ortho(
            minX, maxX,
            minY, maxY,
            -maxZ, -minZ);

        glm::vec4 shadowOrigin = lightProjection * lightView * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        shadowOrigin *= shadowMapSize / 2.0f;

        glm::vec2 roundedOrigin = glm::round(glm::vec2(shadowOrigin.x, shadowOrigin.y));
        glm::vec2 roundOffset = roundedOrigin - glm::vec2(shadowOrigin.x, shadowOrigin.y);
        roundOffset *= 2.0f / shadowMapSize;

        lightProjection[3][0] += roundOffset.x;
        lightProjection[3][1] += roundOffset.y;

        matrices.push_back(lightProjection * lightView);
        lastSplitDist = cascadeSplits[i];
    }

    return matrices;
}

uint32_t ForwardPlusLighting::AcquirePointShadowIndex() {
    if (!availablePointShadowIndices.empty()) {
        uint32_t index = availablePointShadowIndices.back();
        availablePointShadowIndices.pop_back();
        return index;
    }

    if (nextPointShadowIndex >= maxPointLightShadows) {
        LOG(LogType::LOG_WARNING, "Exceeded maximum point light shadow maps (%d)", maxPointLightShadows);
        return 0; // Fallback to first index
    }

    return nextPointShadowIndex++;
}

void ForwardPlusLighting::ReleasePointShadowIndex(uint32_t index) {
    if (index < maxPointLightShadows) {
        availablePointShadowIndices.push_back(index);
    }
}

uint32_t ForwardPlusLighting::AcquireDirShadowIndex() {
    if (!availableDirShadowIndices.empty()) {
        uint32_t index = availableDirShadowIndices.back();
        availableDirShadowIndices.pop_back();
        return index;
    }

    if (nextDirShadowIndex >= maxDirectionalLightShadows) {
        LOG(LogType::LOG_WARNING, "Exceeded maximum directional light shadow maps (%d)", maxDirectionalLightShadows);
        return 0; // Fallback to first index
    }

    return nextDirShadowIndex++;
}

void ForwardPlusLighting::ReleaseDirShadowIndex(uint32_t index) {
    if (index < maxDirectionalLightShadows) {
        availableDirShadowIndices.push_back(index);
    }
}

void ForwardPlusLighting::ResetShadowIndices() {
    nextPointShadowIndex = 0;
    nextDirShadowIndex = 0;
    availablePointShadowIndices.clear();
    availableDirShadowIndices.clear();
}