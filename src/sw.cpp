#include "sw.hpp"

#include <iostream>
#include <vector>

namespace COL781 {
	namespace Software {

		// Forward declarations

		template <> float Attribs::get(int index) const;
		template <> glm::vec2 Attribs::get(int index) const;
		template <> glm::vec3 Attribs::get(int index) const;
		template <> glm::vec4 Attribs::get(int index) const;

		template <> void Attribs::set(int index, float value);
		template <> void Attribs::set(int index, glm::vec2 value);
		template <> void Attribs::set(int index, glm::vec3 value);
		template <> void Attribs::set(int index, glm::vec4 value);

		glm::vec4 depthpos_to_screenpos(glm::vec4 in)
		{
			glm::vec4 out = in;
			out.x = in.x/in.w;
			out.y = in.y/in.w;
			// out.z = in.z/in.w;
			out.z = -in.w;
			// out.w = in.w; //stores the -z coordinate now for further processing later.
			return out;
		}


		// Built-in shaders


		VertexShader Rasterizer::vsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				return depthpos_to_screenpos(vertex);
			};
		}

		VertexShader Rasterizer::vsTransform() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				vertex = transform * vertex;
				vertex = depthpos_to_screenpos(vertex);
				return vertex;

			};
		}

		VertexShader Rasterizer::vsColor() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				out.set<glm::vec4>(1, color);
				vertex = depthpos_to_screenpos(vertex);
				return vertex;
			};
		}

	

		FragmentShader Rasterizer::fsConstant() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = uniforms.get<glm::vec4>("color");
				return color;
			};
		}

		FragmentShader Rasterizer::fsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = in.get<glm::vec4>(1);
				return color;
			};
		}

		
		FragmentShader Rasterizer::fsDiffuseLighting()
		{
			return [](const Uniforms &uniforms, const Attribs &in) 
			{
				glm::vec4 color = in.get<glm::vec4>(1);
				return color;
			};
		}

		// Implementation of Attribs and Uniforms classes

		VertexShader Rasterizer::vsNormalTransform()
		{
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) 
			{
				glm::mat4 wsTransform = uniforms.get<glm::mat4>("wsTransform");
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");

				glm::vec4 vertex = in.get<glm::vec4>(0);
                glm::vec4 screenVertex = depthpos_to_screenpos(transform * vertex); //screen position for the vertex
				glm::vec4 worldVertex = wsTransform * vertex; //world position for the vertex.

				glm::vec3 normal = in.get<glm::vec3>(1);
				normal = glm::mat3(glm::transpose(glm::inverse(wsTransform))) * normal; 

				out.set<glm::vec4>(0, screenVertex);
				out.set<glm::vec3>(1, normal);
				out.set<glm::vec4>(2, worldVertex);

				return screenVertex;
			};
		}

		VertexShader Rasterizer::vsColorTransform() 
		{
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				glm::vec4 color = in.get<glm::vec4>(1);
				out.set<glm::vec4>(1, color);
				glm::vec4 transformed_vertex = transform * vertex; 
				//now we divide by the last w coordinate to get the screen positions that we need. 
				transformed_vertex = depthpos_to_screenpos(transformed_vertex);
				return transformed_vertex;
			};
		}

		void checkDimension(int index, int actual, int requested) {
			if (actual != requested) {
				std::cout << "Warning: attribute " << index << " has dimension " << actual << " but accessed as dimension " << requested << std::endl;
			}
		}

		template <> float Attribs::get(int index) const {
			checkDimension(index, dims[index], 1);
			return values[index].x;
		}

		template <> glm::vec2 Attribs::get(int index) const {
			checkDimension(index, dims[index], 2);
			return glm::vec2(values[index].x, values[index].y);
		}

		template <> glm::vec3 Attribs::get(int index) const {
			checkDimension(index, dims[index], 3);
			return glm::vec3(values[index].x, values[index].y, values[index].z);
		}

		template <> glm::vec4 Attribs::get(int index) const {
			checkDimension(index, dims[index], 4); 
			return values[index];
		}

		void expand(std::vector<int> &dims, std::vector<glm::vec4> &values, int index) {
			if (dims.size() < index+1)
				dims.resize(index+1);
			if (values.size() < index+1)
				values.resize(index+1);
		}

		template <> void Attribs::set(int index, float value) {
			expand(dims, values, index);
			dims[index] = 1;
			values[index].x = value;
		}

		template <> void Attribs::set(int index, glm::vec2 value) {
			expand(dims, values, index);
			dims[index] = 2;
			values[index].x = value.x;
			values[index].y = value.y;
		}

		template <> void Attribs::set(int index, glm::vec3 value) {
			expand(dims, values, index);
			dims[index] = 3;
			values[index].x = value.x;
			values[index].y = value.y;
			values[index].z = value.z;
		}

		template <> void Attribs::set(int index, glm::vec4 value) {
			expand(dims, values, index);
			dims[index] = 4;
			values[index] = value;
		}

		template <typename T> T Uniforms::get(const std::string &name) const {
			if (!values.count(name))
			{
				throw std::runtime_error("name not found in Uniform");
			}
			return *(T*)values.at(name);
		}

		template <typename T> void Uniforms::set(const std::string &name, T value) {

			auto it = values.find(name);
			if (it != values.end()) {
				delete static_cast<T*>(it->second); // deleting the object contained in this location.
			}
			values[name] = (void*)(new T(value));
		}
		//
		// Create the object.
		// API functions implemented.

		bool Rasterizer::initialize(const std::string &title, int width, int height, int spp)
		{
			//since we have a specific spp (samples per pixel), we will update our screen size appropriately.
			bool successful = true; 
		 	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        		printf("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        		return false;
			}
			window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * spp, height * spp, SDL_WINDOW_SHOWN);
			if (!window) {
				std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
				return false;
			}
			windowSurface = SDL_GetWindowSurface(window);
			framebuffer = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
			Rasterizer::quit = false;
			this->frameWidth = width; this->frameHeight = height; this->spp = spp;

			zbuffer = std::vector<float>(frameWidth*frameHeight, 1.0f);

			return true; //successfully created the window and framebuffer.
		}

		ShaderProgram Rasterizer::createShaderProgram(const VertexShader &vs, const FragmentShader &fs)
		{
			ShaderProgram program;
			program.vs = vs;
			program.fs = fs;
			return program; 
		}

		Object Rasterizer::createObject()
		{
			Object obj;
			return obj; //this should be it?
		}

		void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, int d, const float* data)
		{

			if(object.vertexAttributes.size() != n) //INcase size doesn't match, it erases all vertex attributes
			{
				object.vertexAttributes = std::vector<Attribs>(n);
			}
			for(int i = 0; i < n; i++)
			{
				std::vector<float> vals;
				for(int j = 0; j < d; j++)
				{
					vals.push_back(data[i*d + j]);
				}

				if( d == 4)
				{
					glm::vec4 vals_vec4(vals[0], vals[1], vals[2], vals[3]);
					object.vertexAttributes[i].set(attribIndex, vals_vec4);
				}else if( d == 3)
				{
					glm::vec3 vals_vec3(vals[0], vals[1], vals[2]);
					object.vertexAttributes[i].set(attribIndex, vals_vec3);
				}else if( d == 2)
				{
					glm::vec2 vals_vec2(vals[0], vals[1]);
					object.vertexAttributes[i].set(attribIndex, vals_vec2);
				}else if( d == 1)
				{
					object.vertexAttributes[i].set(attribIndex, vals[0]);
				}
			}
		}
		void Rasterizer::setTriangleIndices(Object &object, int n, int* indices){
			if (object.indices.size() != n)
			{
				object.indices = std::vector<glm::ivec3>(n);
			}
			for(int i = 0; i < n; i++)
			{
				glm::ivec3 vals(indices[3*i], indices[3*i + 1], indices[3*i + 2]);
				object.indices[i] = vals;
			}
		};

		void Rasterizer::clear(glm::vec4 color)
		{
			//clear the screen, so we set all the values in the framebuffer to the color.
            Uint32 *pixels = (Uint32*)framebuffer->pixels;
            SDL_PixelFormat *format = framebuffer->format;
			Uint32 colorUint = SDL_MapRGBA(format, (Uint8)(color.r * 255), (Uint8)(color.g * 255), (Uint8)(color.b * 255), (Uint8)(color.a * 255));
    		for (int x = 0; x < frameWidth; x++) {
                for (int y = 0; y < frameHeight; y++) {
                    pixels[x + frameWidth*y] = colorUint;
					zbuffer[x + frameWidth*y] = 1.0f;
                }
            }
		}

		void Rasterizer::useShaderProgram(const ShaderProgram &program)
		{
			currentShader = (ShaderProgram*)&program;
		}
		
		void Rasterizer::deleteShaderProgram(ShaderProgram &program)
		{
			// delete currentShader;
			currentShader = NULL;
		}
		Uint32 Rasterizer::colorLerp(Uint32 c1, Uint32 c2, float t) // t must be between 0 and 1. 
		{
		    Uint8 r1, g1, b1, a1;
		    Uint8 r2, g2, b2, a2;
		    SDL_GetRGBA(c1, framebuffer->format, &r1, &g1, &b1, &a1);
		    SDL_GetRGBA(c2, framebuffer->format, &r2, &g2, &b2, &a2);
		    return SDL_MapRGBA(framebuffer->format, r1 + (r2 - r1) * t, g1 + (g2 - g1) * t, b1 + (b2 - b1) * t, a1 + (a2 - a1) * t);
		}

		float areaTriangle(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2)
		{
			return 0.5*((v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y));
		}

		void Rasterizer::drawTriangle(triangle t, int supersampling)
		{
			Uint32 *pixels = (Uint32*)framebuffer->pixels;
		    int x_min = std::min(std::min(t.v[0].x, t.v[1].x), t.v[2].x);
		    int x_max = std::max(std::max(t.v[0].x, t.v[1].x), t.v[2].x);
		    int y_min = std::min(std::min(t.v[0].y, t.v[1].y), t.v[2].y);
		    int y_max = std::max(std::max(t.v[0].y, t.v[1].y), t.v[2].y);
			float totalArea = areaTriangle(t.v[0], t.v[1], t.v[2]);
		    //supersampling. 
		    int total = supersampling * supersampling; 
		    for (int x = x_min; x <= x_max; x++)
		    {
		        for(int y = y_min; y <= y_max; y++)
		        {
		            float x_supersampled = x;
		            float y_supersampled = y;
		            int count = 0;
		            for (int i = 0; i < supersampling; i++)
		            {
		                for (int j = 0; j < supersampling; j++)
						{    
		                    glm::vec2 p((float)x_supersampled + ((float)i)/supersampling, (float)y_supersampled + ((float)j)/supersampling);
		     				// glm::vec2 x(0,1);               
							if (t.inside(p))
		                    {
		                        count++;
		                    }
		                }
		            }
		            if (count > 0)
		            {	

						//Works in 2d. the color gradient that I am providing right now. 
						float a = areaTriangle(glm::vec2(x,y), t.v[1], t.v[2])/totalArea;
						float b = areaTriangle(t.v[0], glm::vec2(x,y), t.v[2])/totalArea;
						float c = areaTriangle(t.v[0], t.v[1], glm::vec2(x,y))/totalArea;
						float z = a * t.v[0].z + b * t.v[1].z + c * t.v[2].z;
						if (depthTest && zbuffer[x + frameWidth*y] < z)
						{
							continue;
						}
						if(!depthTest)
						{
							t.v[0].z = 1; t.v[1].z = 1; t.v[2].z = 1;
							z = 1;
						}

						//else we are using depth information.
						float iz = a / t.v[0].z + b / t.v[1].z + c / t.v[2].z;	// interpolated 1/z

						zbuffer[x + frameWidth*y] = z;
						
						Attribs curattrib;
						
						glm::vec4 color = (a*t.color[0] /t.v[0].z + b*t.color[1] /t.v[1].z + c*t.color[2] /t.v[2].z)/iz;
						//calculate the gradient over here using the formula of area with this as the dividing point.
						Uint32 colorUint = SDL_MapRGBA(framebuffer->format, (Uint8)(color.r * 255), (Uint8)(color.g * 255), (Uint8)(color.b * 255), (Uint8)(color.a * 255));
		                pixels[x + frameWidth*y] = colorUint; // colorLerp(pixels[x + frameWidth*y], color,  (float)count/(float)total);
		                // pixels[x + frameWidth*y] = colorLerp(pixels[x + frameWidth*y], colorUint,  (float)count/(float)total);
		            }
		        }
		    }
		}


		void Rasterizer::drawObject(const Object &object)
		{
			// //draw the object.
			// //we will draw the object using the vertex shader and fragment shader
			// //use the shaders from the currentShader. 
			int cntVert = object.vertexAttributes.size();	
			std::vector<Attribs> screenVertAttributes;
			for(int i = 0; i < cntVert; i++)
			{
				screenVertAttributes.push_back(object.vertexAttributes[i]);
				glm::vec4 screenPos = currentShader->vs(currentShader->uniforms, object.vertexAttributes[i], screenVertAttributes[i]);
				screenVertAttributes[i].set(0, screenPos); //not really necessary actually.
			}
			//color determined from the fragment shader
			//now we go over all the triangles and draw the object.
			//DRAW the object.
			for(int i = 0; i < object.indices.size(); i++)
			{
				//drawing triangles one by one without considering clipping for now.
				triangle t(screenVertAttributes[object.indices[i].x], screenVertAttributes[object.indices[i].y], screenVertAttributes[object.indices[i].z]);
				for(int tvert = 0; tvert < 3; tvert++)
				{
					t.v[tvert].x = (t.v[tvert].x + 1)*frameWidth/2; 
					t.v[tvert].y = (-t.v[tvert].y + 1)* frameHeight/2; //from 0-1 to screen space transform that is required. maybe should do this in some vector way?
					t.color[tvert] = currentShader->fs(currentShader->uniforms, screenVertAttributes[object.indices[i][tvert]]);
				}		
				drawTriangle(t,1); //draw the triangle.
			}	
		}
	
		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec4 value)
		{
			program.uniforms.set<glm::vec4>(name, value);
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::mat4 value)
		{
			program.uniforms.set<glm::mat4>(name, value);
		}

		bool Rasterizer::shouldQuit()
		{
			return Rasterizer::quit;
		}
		bool handleEvents(bool quit){
				SDL_Event e;
				while (SDL_PollEvent(&e) != 0) {
					if (e.type == SDL_QUIT) {
						return true;
					}
				}
				return false;
			}
		
		void Rasterizer::show()
		{
			SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
			SDL_UpdateWindowSurface(window);
			if (handleEvents(Rasterizer::quit)){
				Rasterizer::quit = true;
			};
		}
		void Rasterizer::enableDepthTest(){
			depthTest=true;
		}
	}
}