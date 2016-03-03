#ifndef _UI_H
#define _UI_H

#include <string>
#include <iostream>
#include <cstdarg>

namespace ar
{

typedef unsigned int ui_element_handle;

class IUIWindow
{
public:

  virtual ~IUIWindow() { }

  virtual ui_element_handle AddButton(const char* label) = 0;
  virtual ui_element_handle AddSliderFloat(const char* label, float min, float max, float value = 0.0f) = 0;
  virtual ui_element_handle AddSliderFloat2(const char* label, float min, float max, const float* values = NULL) = 0;
  virtual ui_element_handle AddSliderFloat3(const char* label, float min, float max, const float* values = NULL) = 0;
  virtual ui_element_handle AddSliderFloat4(const char* label, float min, float max, const float* values = NULL) = 0;
  virtual ui_element_handle AddSliderAngle(const char* label, float minDegree, float maxDegree, float valueRadians = 0.0f) = 0;
  virtual ui_element_handle AddSliderInt(const char* label, int min, int max, int value = 0) = 0;
  virtual ui_element_handle AddSliderInt2(const char* label, int min, int max, const int* values = NULL) = 0;
  virtual ui_element_handle AddSliderInt3(const char* label, int min, int max, const int* values = NULL) = 0;
  virtual ui_element_handle AddSliderInt4(const char* label, int min, int max, const int* values = NULL) = 0;
  virtual ui_element_handle AddDragFloat(const char* label, float min, float max, float speed = 0.0f, float value = 0.0f) = 0;
  virtual ui_element_handle AddDragFloat2(const char* label, float min, float max, float speed = 0.0f, const float* values = NULL) = 0;
  virtual ui_element_handle AddDragFloat3(const char* label, float min, float max, float speed = 0.0f, const float* values = NULL) = 0;
  virtual ui_element_handle AddDragFloat4(const char* label, float min, float max, float speed = 0.0f, const float* values = NULL) = 0;
  virtual ui_element_handle AddDragInt(const char* label, int min, int max, float speed = 0.0f, int value = 0) = 0;
  virtual ui_element_handle AddDragInt2(const char* label, int min, int max, float speed = 0.0f, const int* values = NULL) = 0;
  virtual ui_element_handle AddDragInt3(const char* label, int min, int max, float speed = 0.0f, const int* values = NULL) = 0;
  virtual ui_element_handle AddDragInt4(const char* label, int min, int max, float speed = 0.0f, const int* values = NULL) = 0;
  virtual ui_element_handle AddCheckBox(const char* label, bool checked = false) = 0;
  virtual ui_element_handle AddFloatRange(const char* label, float speed = 1.0f, float min = 0.0f, float max = 0.0f, float lower = 0.0f, float upper = 0.0f) = 0;
  virtual ui_element_handle AddInputText(const char* label) = 0;
  virtual ui_element_handle AddText(const char* fmt, ...) = 0;

  virtual void AddSeparator() = 0;
  virtual void SameLine() = 0;

  virtual bool GetButtonState(ui_element_handle handle) = 0;
  virtual float GetSliderFloatValue(ui_element_handle handle) const = 0;
  virtual void GetSliderFloatValues(ui_element_handle handle, float* values) const = 0;
  virtual int GetSliderIntValue(ui_element_handle handle) const = 0;
  virtual void GetSliderIntValues(ui_element_handle handle, int* values) const = 0;
  virtual bool GetCheckBoxState(ui_element_handle handle) const = 0;
  virtual void GetFloatRangeValues(ui_element_handle handle, float& lower, float& upper) const = 0;
  virtual std::string GetInputTextValue(ui_element_handle handle) const = 0;

  virtual void UpdateText(ui_element_handle handle, const char* fmt, ...) = 0;

private:

  friend class UserInterface;
  virtual void drawElements() = 0;
};

} // namespace ar

#endif // _UI_H
