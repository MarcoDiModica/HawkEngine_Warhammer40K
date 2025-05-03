#version 460 core
#extension GL_ARB_shader_storage_buffer_object : require
#extension GL_ARB_base_instance : enable // For gl_BaseInstance
#extension GL_ARB_draw_indirect_fields : enable // For gl_DrawID (Provides gl_DrawID)

// No more 'in' attributes for vertex data!
// layout (location = 0) in vec3 aPos;      // REMOVE
// layout (location = 1) in vec2 aTexCoords; // REMOVE
// layout (location = 2) in vec3 aNormal;    // REMOVE

// Output to fragment shader (remain the same)
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
layout(location = 3) flat out uint vs_materialIndex;

// SSBO Structures (match C++ structs)
// Only include data relevant to the vertex shader
struct GPUMesh {
    // Add members needed to calculate global vertex index
    uint indexCount;       // Not strictly needed here, but good practice
    uint vertexCount;      // Not strictly needed here
    uint indexOffset;       // Offset into global index buffer (elements)
    uint baseVertexOffset;  // Offset into global vertex buffer (vertices)
    // ... other mesh metadata if needed ...
};

struct GPUInstance {
    mat4 modelMatrix;
    // prevModelMatrix, objectData, objectId, flags... (keep if part of struct, but might not be used)
    uint meshIndex;       // Index to the mesh metadata in the MeshBuffer
    uint materialIndex;   // Index to the material metadata in the MaterialBuffer (pass to fragment)
};

// *** NEW SSBO for Global Vertex Data ***
// Define the structure of a single vertex *as packed in your m_globalVertexBuffer*
// This must exactly match the data layout you use when appending to the buffer in BindlessManager.
struct VertexData {
    vec3 position;
    vec2 texCoord;
    vec3 normal;
    // ... include other attributes (tangent, bitangent, color) if present and used
};

// SSBO Bindings (Adjust binding points to match your GPUDrivenRenderer setup)
// Binding 0: GPUMesh metadata SSBO (as in your C++ GPUDrivenRenderer::RenderUnlitBatch)
layout(std430, binding = 0) buffer MeshBuffer { GPUMesh meshes[]; };
// Binding 2: GPUInstance metadata SSBO (as in your C++ GPUDrivenRenderer::RenderUnlitBatch)
layout(std430, binding = 2) buffer InstanceBuffer { GPUInstance instances[]; };
// Binding 1: GPUMaterial metadata SSBO - Keep here if vertex shader needed material data,
//            but your shader only needs materialIndex, which comes from InstanceBuffer.
//            If not used, you could remove this binding from the vertex shader.

// *** NEW Binding for the Global Vertex Data SSBO ***
// Choose a binding point not already used (0, 1, 2). Let's use 4 as in the previous example.
layout(std430, binding = 4) buffer GlobalVertexDataBuffer { VertexData allVertexData[]; };


// Uniforms - Keep View and Projection if they are still uniforms
uniform mat4 view;
uniform mat4 projection;
// uniform uint baseInstance; // REMOVE THIS REDUNDANT UNIFORM


void main()
{
    // Get the instance index for this draw call using the intrinsic gl_BaseInstance
    uint instanceIndex = gl_BaseInstance + gl_InstanceID;

    // Fetch instance data using the calculated instance index
    GPUInstance instanceData = instances[instanceIndex];

    // Get the material index from the instance data (pass to fragment shader)
    vs_materialIndex = instanceData.materialIndex;

    // *** Fetch Mesh Metadata ***
    // Use the meshIndex from the instance data to get mesh metadata from the MeshBuffer
    uint meshIndex = instanceData.meshIndex; // Get mesh index from instance data
    GPUMesh meshData = meshes[meshIndex];    // Fetch the GPUMesh data for this command's mesh

    // *** Calculate the index into the Global Vertex Data Array ***
    // gl_VertexID is the index read from the element array buffer *after* applying baseVertex from the DrawElementsCommand.
    // baseVertexOffset from GPUMesh is the starting vertex index for this mesh within the *global* vertex buffer.
    // The final index into the global vertex data SSBO is the mesh's start offset + the vertex index for this primitive.
    uint globalVertexIndex = meshData.baseVertexOffset + gl_VertexID;


    // *** Fetch Vertex Data from the Global Vertex Data SSBO ***
    VertexData vertex = allVertexData[globalVertexIndex];

    // Get the model matrix from instance data
    mat4 modelMatrix = instanceData.modelMatrix;

    // Calculate position in world space using the fetched position
    vec4 worldPos = modelMatrix * vec4(vertex.position, 1.0);
    FragPos = worldPos.xyz; // Pass world position to fragment shader

    // Pass fetched texture coordinates to fragment shader
    TexCoords = vertex.texCoord;

    // Transform fetched normal to world space
    Normal = mat3(transpose(inverse(modelMatrix))) * vertex.normal; // Pass world normal to fragment shader

    // Calculate final position in clip space using fetched position and uniforms
    gl_Position = projection * view * worldPos;
}