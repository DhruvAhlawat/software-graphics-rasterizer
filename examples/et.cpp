#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>
#include<iostream>

namespace R = COL781::Hardware;
using namespace glm;

int main() {
    R::Rasterizer r;
    int width = 640, height = 480;
    if (!r.initialize("Analogue Clock", width, height))
        return EXIT_FAILURE;

    R::ShaderProgram program = r.createShaderProgram(
        r.vsColorTransform(),
        r.fsConstant()
    );

    float vertices[] = {
        -0.5, -0.5, 0.0, 1.0,
         0.5, -0.5, 0.0, 1.0,
        -0.5,  0.5, 0.0, 1.0,
         0.5,  0.5, 0.0, 1.0
    };

    int triangles[] = {
        0, 1, 2,
        1, 2, 3
    };

    R::Object square = r.createObject();
    r.setVertexAttribs(square, 0, 4, 4, vertices);
    r.setTriangleIndices(square, 2, triangles);

    mat4 view = translate(mat4(1.0f), vec3(0.0f, 0.0f, -2.0f)); 
    mat4 projection = perspective(radians(60.0f), (float)width/(float)height, 0.1f, 100.0f);

    while (!r.shouldQuit()) {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        ltm->tm_gmtoff = 19800;
        ltm->tm_min = (ltm->tm_min + 30);
        if(ltm->tm_min >= 60)
        {
            ltm->tm_hour += 1;
            ltm->tm_min -= 60;
        }
        ltm->tm_hour += 5; 
        if(ltm->tm_hour >= 24)
        {
            ltm->tm_hour -= 24;
        }
        float hourAngle = -radians(30.0f * (ltm->tm_hour % 12) + (ltm->tm_min / 2.0f));
        float minuteAngle = -radians(6.0f * ltm->tm_min);
        float secondAngle = -radians(6.0f * ltm->tm_sec);

        r.clear(vec4(1.0, 1.0, 1.0, 1.0));
        r.useShaderProgram(program);

        // hour hand
        mat4 model = rotate(mat4(1.0f), hourAngle, vec3(0.0f, 0.0f, 1.0f)) * translate(mat4(1.0f), vec3(0.0f, 0.15f, 0.0f)) * scale(mat4(1.0f), vec3(0.08f, 0.3f, 1.0f));
        r.setUniform(program, "transform", projection * view * model);
        r.drawObject(square);

        // minute hand
        model = rotate(mat4(1.0f), minuteAngle, vec3(0.0f, 0.0f, 1.0f)) * translate(mat4(1.0f), vec3(0.0f, 0.2f, 0.0f)) * scale(mat4(1.0f), vec3(0.05f, 0.4f, 1.0f));
        r.setUniform(program, "transform", projection * view * model);
        r.drawObject(square);

        // second hand
        model = rotate(mat4(1.0f), secondAngle, vec3(0.0f, 0.0f, 1.0f)) * translate(mat4(1.0f), vec3(0.0f, 0.25f, 0.0f)) * scale(mat4(1.0f), vec3(0.03f, 0.6f, 1.0f));
        r.setUniform(program, "transform", projection * view * model);
        r.drawObject(square);

        // hour markings, can also make them cooler ig
        for (int i = 0; i < 12; ++i) {
            float angle = radians(30.0f * i);
            model = rotate(mat4(1.0f), angle, vec3(0.0f, 0.0f, 1.0f))
                    * translate(mat4(1.0f), vec3(0.0f, 0.75f, 0.0f))
                    * scale(mat4(1.0f), vec3(0.02f, 0.09f, 1.0f));

            r.setUniform(program, "transform", projection * view * model);
            r.setUniform<vec4>(program, "color", vec4(1.0, 0.0, 0.0, 1.0));
            r.drawObject(square);

            for (int j=0; j<5; j++){    // minute markings
                float s_angle = radians(30.0f*i+6*j);
                model = rotate(mat4(1.0f), s_angle, vec3(0.0f, 0.0f, 1.0f))
                    * translate(mat4(1.0f), vec3(0.0f, 0.75f, 0.0f))
                    * scale(mat4(1.0f), vec3(0.02f, 0.03f, 1.0f));

                r.setUniform(program, "transform", projection * view * model);
                r.drawObject(square);
            }
        }

        r.show();
    }

    r.deleteShaderProgram(program);
    return EXIT_SUCCESS;
}
