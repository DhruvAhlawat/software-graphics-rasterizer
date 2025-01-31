#ifndef SW_HPP
#define SW_HPP

#include <glm/glm.hpp>
#include <map>
#include <SDL2/SDL.h>
#include <string>
#include <vector>

namespace COL781 {
	namespace Software {

		class Attribs {
			// A class to contain the attributes of ONE vertex
		public:
			// only float, glm::vec2, glm::vec3, glm::vec4 allowed
			template <typename T> T get(int attribIndex) const;
			template <typename T> void set(int attribIndex, T value);
		private:
			std::vector<glm::vec4> values;
			std::vector<int> dims;
		};

		class Uniforms {
			// A class to contain all the uniform variables
		public:
			// any type allowed
			template <typename T> T get(const std::string &name) const;
			template <typename T> void set(const std::string &name, T value);
		private:
			std::map<std::string,void*> values;
		};

		/* A vertex shader is a function that:
		   reads the uniform variables and one vertex's input attributes,
		   writes the output attributes for interpolation to fragments,
		   and returns the vertex position in NDC as a homogeneous vec4. */
		using VertexShader = glm::vec4(*)(const Uniforms &uniforms, const Attribs &in, Attribs &out);

		/* A fragment shader is a function that:
		   reads the uniform variables and one fragment's interpolated attributes,
		   and returns the colour of the fragment as an RGBA value. */
		using FragmentShader = glm::vec4(*)(const Uniforms &uniforms, const Attribs &in);

		struct ShaderProgram {
			VertexShader vs;
			FragmentShader fs;
			Uniforms uniforms;
		};

		struct Object {
			std::vector<Attribs> vertexAttributes;
			std::vector<glm::ivec3> indices;
		};
		class triangle
		{
			public:
			glm::vec3 v[3]; //vertices of the triangle
			glm::vec4 color[3]; //colors of the vertices
			triangle(glm::vec3 v0,glm::vec3 v1,glm::vec3 v2) {v[0] = v0; v[1] = v1; v[2] = v2;}
			triangle(glm::vec3 verts[3], glm::vec4 col[3]) 
			{
				for(int i = 0; i < 3; i++)
				{
					v[i] = verts[i];
					color[i] = col[i];
				}
			}

			float area() const 
			{ return 0.5*(v[1].x - v[0].x) * (v[2].y - v[0].y) - (v[2].x - v[0].x) * (v[1].y - v[0].y); } //area of the triangle


			bool inside(glm::vec2 p) const 
			{ //write this 
				float a = (v[1].x - v[0].x) * (p.y - v[0].y) - (p.x - v[0].x) * (v[1].y - v[0].y);
				float b = (v[2].x - v[1].x) * (p.y - v[1].y) - (p.x - v[1].x) * (v[2].y - v[1].y);
				float c = (v[0].x - v[2].x) * (p.y - v[2].y) - (p.x - v[2].x) * (v[0].y - v[2].y);
				
				if (a >= 0 && b >= 0 && c >= 0) return 1;
				if (a <= 0 && b <= 0 && c <= 0) return 1;
				return 0;
			} //checks if the point is inside the triangle
		};
		class Rasterizer {
		public:
		#include "api.inc"

		Uint32 colorLerp(Uint32 c1, Uint32 c2, float t); // t must be between 0 and 1. 
		void drawTriangle(triangle t, int supersampling = 1);

		private:
			SDL_Window *window;
			bool quit;
			// Create window and rasterizer attributes here
			//additional attributes.
			int frameWidth, frameHeight, spp;
			std::vector<float> zbuffer; //need to change this later
			bool depthTest = false;
			SDL_Surface *windowSurface;
			SDL_Surface *framebuffer;
			ShaderProgram *currentShader = nullptr;
		};
		


		// Functions we gotta create. 


	}
}

#endif
