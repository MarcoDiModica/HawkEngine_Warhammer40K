#pragma once  
#include <string>  
#include <map>  
#include <glm/glm.hpp>  
#include <GL/glew.h>  
#include <ft2build.h>  
#include FT_FREETYPE_H 
#include "../MyGameEngine/Shaders.h"

struct Character {  
   GLuint TextureID;   
   glm::ivec2 Size;    
   glm::ivec2 Bearing;  
   GLuint Advance;     
};  

class FontManager {  
public:  
   static FontManager& GetInstance();  

   bool LoadFont(const std::string& fontPath, int fontSize);  

   void RenderText(Shaders* shader, const std::string& text, float x, float y, float scale, const glm::vec3& color);

private:  
   FontManager();  
   ~FontManager();  

   FT_Library ft; // Biblioteca FreeType  
   FT_Face face;  

   std::map<char, Character> Characters;  

   GLuint VAO, VBO;  
};