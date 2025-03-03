#ifndef BUTTON_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define BUTTON_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <glad/glad.h>

#include <iostream>
#include <string>

#include "../vmlib/vec2.hpp"
#include "../vmlib/vec4.hpp"
#include "mesh.hpp"
#include "state.hpp"

class Button {
 public:
  Button(const std::string& text, Vec4f (*genScreenCoords)(float, float),
         const Vec4f& idleColor, const Vec4f& activeColor,
         const Vec4f& pressedColor, float borderWidth, const Vec4f& borderColor,
         void (*callback)(State*))
      : text(text),
        genScreenCoords(genScreenCoords),
        idleColor(idleColor),
        activeColor(activeColor),
        pressedColor(pressedColor),
        borderWidth(borderWidth),
        borderColor(borderColor),
        callback(callback) {
    prevFbwidth = 0.f;
    prevFbheight = 0.f;
    isInRect = false;
    isPressedInRect = false;

    // Other initialisation
    currentColor = &this->idleColor;
    fillVao = 0;
    fillVertices = 0;
    outlineVao = 0;
    outlineVertices = 0;
    screenRect = Vec4f{0.f, 0.f, 0.f, 0.f};
  }

  void updateSize(float fbwidth, float fbheight) {
    // Only update if change has occured
    if (prevFbwidth == fbwidth && prevFbheight == fbheight) {
      return;
    }
    prevFbwidth = fbwidth;
    prevFbheight = fbheight;

    // Use provided genScreenCoords
    screenRect = genScreenCoords(fbwidth, fbheight);

    // Construct Fill Data
    std::vector<Vec2f> fillData = {
        {screenRect.x, screenRect.y}, {screenRect.z, screenRect.y},
        {screenRect.z, screenRect.w}, {screenRect.x, screenRect.y},
        {screenRect.z, screenRect.w}, {screenRect.x, screenRect.w}};

    // Construct Ouline Data
    float w = borderWidth;
    std::vector<Vec2f> outlineData = {{screenRect.x - w, screenRect.y - w},
                                      {screenRect.z + w, screenRect.y - w},
                                      {screenRect.x - w, screenRect.y},

                                      {screenRect.x - w, screenRect.y},
                                      {screenRect.z + w, screenRect.y - w},
                                      {screenRect.z + w, screenRect.y},

                                      {screenRect.x - w, screenRect.w},
                                      {screenRect.z + w, screenRect.w},
                                      {screenRect.x - w, screenRect.w + w},

                                      {screenRect.x - w, screenRect.w + w},
                                      {screenRect.z + w, screenRect.w},
                                      {screenRect.z + w, screenRect.w + w},

                                      {screenRect.x - w, screenRect.y},
                                      {screenRect.x, screenRect.y},
                                      {screenRect.x - w, screenRect.w},

                                      {screenRect.x - w, screenRect.w},
                                      {screenRect.x, screenRect.y},
                                      {screenRect.x, screenRect.w},

                                      {screenRect.z, screenRect.y},
                                      {screenRect.z + w, screenRect.y},
                                      {screenRect.z, screenRect.w},

                                      {screenRect.z, screenRect.w},
                                      {screenRect.z + w, screenRect.y},
                                      {screenRect.z + w, screenRect.w}};

    fillVao = createVao(fillData);
    fillVertices = GLuint(fillData.size());

    outlineVao = createVao(outlineData);
    outlineVertices = GLuint(outlineData.size());
  }

  GLuint createVao(std::vector<Vec2f> data) const {
    // Convert to NDC System
    for (Vec2f& v : data) {
      v.x = 2 * v.x / prevFbwidth - 1;
      v.y = 2 * v.y / prevFbheight - 1;
    }

    GLuint positionVBO = 0;
    glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vec2f), data.data(),
                 GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Clean Up State
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &positionVBO);
    return vao;
  }

  void draw() const {
    //// Outline
    glUniform4fv(0, 1, &borderColor.x);
    glBindVertexArray(outlineVao);
    glDrawArrays(GL_TRIANGLES, 0, outlineVertices);

    //// Then Fill
    glUniform4fv(0, 1, &currentColor->x);
    glBindVertexArray(fillVao);
    glDrawArrays(GL_TRIANGLES, 0, fillVertices);
  }

  void updateMouseMove(double aX, double aY) {
    // Mouse coordinates provided from top left. Flip y coordinate
    aY = prevFbheight - aY;
    isInRect = (aX >= screenRect.x && aX <= screenRect.z &&
                aY >= screenRect.y && aY <= screenRect.w);
    if (isInRect) {
      if (!isPressedInRect) {
        currentColor = &activeColor;
      }
    } else {
      currentColor = &idleColor;
      isPressedInRect = false;
    }
  }

  void updateMousePress(int aButton, int aAction, State* state) {
    if (isInRect) {
    }
    if (isInRect && aButton == GLFW_MOUSE_BUTTON_LEFT) {
      if (aAction == GLFW_PRESS) {
        currentColor = &pressedColor;
        isPressedInRect = true;
      } else if (aAction == GLFW_RELEASE) {
        currentColor = &activeColor;
        isPressedInRect = false;
        // Callback Function
        callback(state);
      }
    }
  }

 private:
  std::string text;

  // Rectangle with bottomleft, topright
  Vec4f (*genScreenCoords)(float, float);
  Vec4f screenRect;
  float prevFbwidth;
  float prevFbheight;

  // Data
  GLuint fillVao;
  GLuint outlineVao;
  GLsizei fillVertices;
  GLsizei outlineVertices;

  // Appearance
  Vec4f idleColor;
  Vec4f activeColor;
  Vec4f pressedColor;
  Vec4f* currentColor;
  float borderWidth;
  Vec4f borderColor;

  // State
  bool isInRect;
  bool isPressedInRect;
  void (*callback)(State*);
};

Vec4f topLeft(float, float fbheight) {
  float margin = 40.f;
  float width = 160.f;
  float height = 50.f;

  return Vec4f{margin, fbheight - margin - height, margin + width,
               fbheight - margin};
}

Vec4f bottomCentreLeft(float fbwidth, float) {
  float margin = 40.f;
  float width = 140.f;
  float height = 60.f;

  return Vec4f{fbwidth / 2 - margin - width, margin, fbwidth / 2 - margin,
               margin + height};
}

Vec4f bottomCentreRight(float fbwidth, float) {
  float margin = 40.f;
  float width = 140.f;
  float height = 60.f;

  return Vec4f{fbwidth / 2 + margin, margin, fbwidth / 2 + margin + width,
               margin + height};
}

#endif  // BUTTON_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9