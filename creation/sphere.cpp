#include <vector>
#include <cmath>

const float PI = 3.14159265359f;

void generateSphere(int stacks, int slices, std::vector<float>& vertices, std::vector<float>& normals, std::vector<int>& indices) 
{
    for (int i = 0; i <= stacks; ++i) {
        float phi = PI * (-0.5f + (float)i / stacks);  // Latitude
        float y = sin(phi);
        float radius = cos(phi);

        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * PI * (float)j / slices;  // Longitude
            float x = radius * cos(theta);
            float z = radius * sin(theta);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(1.0f);  // Homogeneous coordinate

            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}
