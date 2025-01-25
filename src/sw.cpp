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

		// Built-in shaders

		VertexShader Rasterizer::vsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				return vertex;
			};
		}

		VertexShader Rasterizer::vsTransform() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				return transform * vertex;
			};
		}

		VertexShader Rasterizer::vsColor() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				out.set<glm::vec4>(0, color);
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
				glm::vec4 color = in.get<glm::vec4>(0);
				return color;
			};
		}

		// Implementation of Attribs and Uniforms classes

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
			return *(T*)values.at(name);
		}

		template <typename T> void Uniforms::set(const std::string &name, T value) {
			auto it = values.find(name);
			if (it != values.end()) {
				delete it->second;
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
			
			return true;
		}


		ShaderProgram Rasterizer::createShaderProgram(const VertexShader &vs, const FragmentShader &fs)
		{
			ShaderProgram program;
			program.vs = vs;
			program.fs = fs;
			// What about Uniforms? created by default :) 
			return program; 
		}

		Object Rasterizer::createObject()
		{
			Object obj;
			return obj; //this should be it?
		}

		void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, int d, const float* data)
		{
			// object.vertexAttributes.clear();
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
					object.vertexAttributes[i] = Attribs(); 
					object.vertexAttributes[i].set(attribIndex, vals_vec4);
				}else if( d == 3)
				{
					glm::vec3 vals_vec3(vals[0], vals[1], vals[2]);
					object.vertexAttributes[i] = Attribs(); 
					object.vertexAttributes[i].set(attribIndex, vals_vec3);
				}else if( d == 2)
				{
					glm::vec2 vals_vec2(vals[0], vals[1]);
					object.vertexAttributes[i] = Attribs(); 
					object.vertexAttributes[i].set(attribIndex, vals_vec2);
				}else if( d == 1)
				{
					object.vertexAttributes[i] = Attribs(); 
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

		void Rasterizer::setTriangleIndices(Object &object, int n, int* indices)
		{

		}

		
		void Rasterizer::clear(glm::vec4 color)
		{

		}

		bool Rasterizer::shouldQuit()
		{
			return quit;
		}

	}
}
