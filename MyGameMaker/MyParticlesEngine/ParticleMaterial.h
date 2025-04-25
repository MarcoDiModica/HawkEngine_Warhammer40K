#pragma once
#include "../MyGameEngine/Material.h"
#include "../MyGameEngine/ShaderManager.h"

enum class ParticleType {
    // PLACE_HOLDERS
    DEFAULT = 0,
    SMOKE = 1,
    FIRE = 2,
    MUZZLE_FLASH = 3,
    EXPLOSION = 4,
    // ENVIRONMENT
    FLAME = 5,
    ENVIRONMENT_SMOKE = 6,
    ENVIRONMENT_EXPLOSION = 7,
    ENVIRONMENT_DROPPLET = 15,
    ENVIRONMENT_SPARK = 16,
    // Enemies
    ENEMY_DASH  = 9,
    ACID_ATK    = 10,
    // ACID_ATK_EXPLODE can use the same vfx structure as ACID_ATK with different texture
    ACID_PUDDLE = 11,

    // Player / Weapons
    ENERGY_BALL = 12,
    RAILGUN_AUTO = 13,
    RAILGUN_SEMI = 14,
    RIFFLE_SHOT = 8,
    ARC_SNARE_IMPACT = 17,
    MEDICAE_STIM = 18,
    BLOOD_SPLASH = 19,
	SHOTGUN_SHOT = 20,
	MEDICAE_STIM_SPEED = 21,
};

class ParticleMaterial : public Material {
public:
    ParticleMaterial() {
        shaderType = ShaderType::PARTICLE;
        billboardType = 0; //screen-aligned
        particleType = static_cast<int>(ParticleType::DEFAULT);
        softness = 0.0f;
        useColorGradient = false;
		startColor = glm::vec4(1.0f);
		endColor = glm::vec4(1.0f);
		spriteSize = glm::vec2(1.0f);
		sheetSize = glm::vec2(1.0f);
		useAnimation = false;
		spriteIndex = 0;
    }

    ~ParticleMaterial() override = default;

    void bind() const override {
        bindTexture(imagePtr, GL_TEXTURE0);

        if (useColorGradient && colorGradientMap) {
            bindTexture(colorGradientMap, GL_TEXTURE1);
        }
    }

    void ApplyShader(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const override {
        Shaders* shader = ShaderManager::GetInstance().GetShader(shaderType);
        if (!shader) {
            return;
        }

        shader->Bind();

        shader->SetUniform("model", model);
        shader->SetUniform("view", view);
        shader->SetUniform("projection", projection);

        shader->SetUniform("billboardType", billboardType);
        shader->SetUniform("particleType", particleType);
        shader->SetUniform("softness", softness);
        shader->SetUniform("useColorGradient", useColorGradient ? 1 : 0);

		shader->SetUniform("startColor", startColor);
		shader->SetUniform("endColor", endColor);

        //shader->SetUniform("spriteIndex",spriteIndex);
		//shader->SetUniform("spriteSize",spriteSize );
		//shader->SetUniform("sheetSize",sheetSize );
		//shader->SetUniform("useAnimation",useAnimation );

        glm::vec3 cameraPosition(0.0f);
        glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

        shader->SetUniform("cameraPosition", cameraPosition);
        shader->SetUniform("cameraUp", cameraUp);
        shader->SetUniform("billboardAxis", billboardAxis);

        if (imagePtr && imagePtr->id() != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, imagePtr->id());
            shader->SetUniform("particleTexture", 0);
        }

        if (colorGradientMap && colorGradientMap->id() != 0 && useColorGradient) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, colorGradientMap->id());
            shader->SetUniform("colorGradient", 1);
        }
    }

    void SetBillboardType(int type) { billboardType = type; }
    int GetBillboardType() const { return billboardType; }

    void SetParticleType(ParticleType type) { particleType = static_cast<int>(type); }
    int GetParticleType() const { return particleType; }

    void SetSoftness(float value) { softness = value; }
    float GetSoftness() const { return softness; }

    void SetBillboardAxis(const glm::vec3& axis) { billboardAxis = axis; }
    const glm::vec3& GetBillboardAxis() const { return billboardAxis; }

    void SetColorGradientMap(const std::shared_ptr<Image>& img) {
        colorGradientMap = img;
        useColorGradient = (img != nullptr);
    }

    void DisableColorGradient() { useColorGradient = false; }
    bool GetUseColorGradient() const { return useColorGradient; }

    std::shared_ptr<Image> GetColorGradientMap() const { return colorGradientMap; }

private:
	int billboardType;
	int particleType;
	float softness;
	bool useColorGradient;
	glm::vec3 billboardAxis;
    std::shared_ptr<Image> colorGradientMap = nullptr; 
	glm::vec4 startColor;
	glm::vec4 endColor;
	glm::vec2 spriteSize;
	glm::vec2 sheetSize;
	bool useAnimation;
	int spriteIndex;
};