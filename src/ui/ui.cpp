#include "ui.hpp"
#include "common.hpp"
#include "Renderer.hpp"
#include <imgui.h>

namespace ar
{


class UIBaseElement
{
public:

  virtual void draw() = 0;
};

class UIButton : public UIBaseElement
{

public:

  virtual void draw() override;

  std::string _label;
  bool _pressed;

};

struct UISliderType
{
  enum Type
  {
    SliderType,
    DragType,
    SliderAngleType
  };
};

template <typename T>
class UISlider : public UIBaseElement
{
public:

  virtual void draw() override
  {
    drawInternal(_numValues);
  }

  UISliderType::Type _type;

  std::string _label;
  T _min;
  T _max;
  float _speed; // Only used for DragInt/DragFloat

  std::array<T, 4> _values;
  int _numValues;

private:

  void drawInternal(int n);
};

class UICheckBox : public UIBaseElement
{
public:

  virtual void draw() override;
  std::string _label;
  bool _checked;
};

class UIFloatRange : public UIBaseElement
{
public:

  virtual void draw() override;

  std::string _label;
  float _speed;
  float _min;
  float _max;
  float _lowerValue;
  float _upperValue;
};

class UIComboBox : public UIBaseElement
{
public:

  virtual void draw() override;

  std::string _label;
  Vector<std::string> _items;
  int _currentItem;
};

class UIInputText : public UIBaseElement
{
public:

  virtual void draw() override;

  std::string _label;
  std::string _text;
};

class UIText : public UIBaseElement
{
public:

  virtual void draw() override;

  std::string _text;
};

class UIAuxiliaryElement : public UIBaseElement
{
public:

  enum Type
  {
    SameLine,
    Separator
  };

  UIAuxiliaryElement(Type type) : _type(type) { }

  virtual void draw() override;

  Type _type;
};

class UIPlot : public UIBaseElement
{
public:

  virtual void draw() override;

  inline int size() { return static_cast<int>(_buffer.size()); }

  std::string _label;
  float _rangeMin;
  float _rangeMax;
  float _height;

  int _offset;
  Vector<float> _buffer;
};

struct UIElement
{
  UIElement(UIBaseElement* element) : _element(element) { }
  UniquePtr<UIBaseElement> _element;
};



class UIWindow : public IUIWindow
{
public:

  UIWindow(const char* name, float initSizeX, float initSizeY, Renderer* renderer)
    : _name(name), _initSize(initSizeX, initSizeY), _renderer(renderer) { }
  virtual ~UIWindow() { }

  virtual ui_element_handle AddButton(const char* label) override;
  virtual ui_element_handle AddSliderFloat(const char* label, float min, float max, float value = 0.0f) override;
  virtual ui_element_handle AddSliderFloat2(const char* label, float min, float max, const float* values = nullptr) override;
  virtual ui_element_handle AddSliderFloat3(const char* label, float min, float max, const float* values = nullptr) override;
  virtual ui_element_handle AddSliderFloat4(const char* label, float min, float max, const float* values = nullptr) override;
  virtual ui_element_handle AddSliderAngle(const char* label, float minDegree, float maxDegree, float valueRadians = 0.0f) override;
  virtual ui_element_handle AddSliderInt(const char* label, int min, int max, int value = 0) override;
  virtual ui_element_handle AddSliderInt2(const char* label, int min, int max, const int* values = nullptr) override;
  virtual ui_element_handle AddSliderInt3(const char* label, int min, int max, const int* values = nullptr) override;
  virtual ui_element_handle AddSliderInt4(const char* label, int min, int max, const int* values = nullptr) override;
  virtual ui_element_handle AddDragFloat(const char* label, float min, float max, float speed = 0.0f, float value = 0.0f) override;
  virtual ui_element_handle AddDragFloat2(const char* label, float min, float max, float speed = 0.0f, const float* values = nullptr) override;
  virtual ui_element_handle AddDragFloat3(const char* label, float min, float max, float speed = 0.0f, const float* values = nullptr) override;
  virtual ui_element_handle AddDragFloat4(const char* label, float min, float max, float speed = 0.0f, const float* values = nullptr) override;
  virtual ui_element_handle AddDragInt(const char* label, int min, int max, float speed = 0.0f, int value = 0) override;
  virtual ui_element_handle AddDragInt2(const char* label, int min, int max, float speed = 0.0f, const int* values = nullptr) override;
  virtual ui_element_handle AddDragInt3(const char* label, int min, int max, float speed = 0.0f, const int* values = nullptr) override;
  virtual ui_element_handle AddDragInt4(const char* label, int min, int max, float speed = 0.0f, const int* values = nullptr) override;
  virtual ui_element_handle AddCheckBox(const char* label, bool checked = false) override;
  virtual ui_element_handle AddFloatRange(const char* label, float speed = 1.0f, float min = 0.0f, float max = 0.0f, float lower = 0.0f, float upper = 0.0f) override;
  virtual ui_element_handle AddComboBox(const char* label, const char** items, int itemsCount, int selectedItem = 0) override;
  virtual ui_element_handle AddInputText(const char* label, const char* text = nullptr) override;
  virtual ui_element_handle AddText(const char* fmt, ...) override;

  virtual ui_element_handle AddPlot(const char* label, float rangeMin, float rangeMax, int bufferSize = 128, float height = 0.0f) override;
  virtual void PushPlotValue(ui_element_handle handle, float value) override;

  virtual void AddSeparator() override;
  virtual void SameLine() override;

  virtual bool GetButtonState(ui_element_handle handle) override;
  virtual float GetSliderFloatValue(ui_element_handle handle) const override;
  virtual void GetSliderFloatValues(ui_element_handle handle, float* values) const override;
  virtual int GetSliderIntValue(ui_element_handle handle) const override;
  virtual void GetSliderIntValues(ui_element_handle handle, int* values) const override;
  virtual bool GetCheckBoxState(ui_element_handle handle) const override;
  virtual void GetFloatRangeValues(ui_element_handle handle, float& lower, float& upper) const override;
  virtual int GetSelectedComboBoxItem(ui_element_handle handle) const override;
  virtual std::string GetInputTextValue(ui_element_handle handle) const override;

  virtual void UpdateText(ui_element_handle handle, const char* fmt, ...) override;

  virtual void Set3DPosition(const double* position) override
  {
    _isOverlay = true;
    _position3D = glm::vec3((float)position[0], (float)position[1], (float)position[2]);
  }
private:

  virtual void drawElements() override
  {
    MutexLockGuard lockGuard(_mutex);

    if (_initSize.x > 0.0f || _initSize.y > 0.0f)
      ImGui::SetNextWindowSize(_initSize, ImGuiSetCond_FirstUseEver);

    ImGuiWindowFlags windowFlags = 0;
    float windowBgAlpha = -0.1f;
    if (_isOverlay)
    {
      windowFlags |= ImGuiWindowFlags_NoTitleBar;
      windowFlags |= ImGuiWindowFlags_NoResize;
      windowFlags |= ImGuiWindowFlags_NoMove;
      windowFlags |= ImGuiWindowFlags_NoScrollbar;
      windowFlags |= ImGuiWindowFlags_NoCollapse;
      windowFlags |= ImGuiWindowFlags_NoSavedSettings;
      windowFlags |= ImGuiWindowFlags_NoInputs;
      windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
      windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
      windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
      windowBgAlpha = 0.0f;
    }

    if (!ImGui::Begin(_name.c_str(), nullptr, ImVec2(), windowBgAlpha, windowFlags))
    {
      ImGui::End();
      return;
    }

    if (_isOverlay)
    {
      glm::vec2 pixelPos;
      if (!_renderer->ProjectPointToPixel(_position3D, pixelPos))
      {
        // Don't show the window when we can't get a projection, e.g. when the position is behind the near plane
        ImGui::End();
        return;
      }

      ImGui::SetWindowPos(ImVec2(pixelPos.x, pixelPos.y));
      // Pure white text for maximum contrast
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    ImGui::PushItemWidth(-100);

    for (int handle = 0; handle < (int)_elements.size(); handle++)
    {
      ImGui::PushID(handle); // Set the handle as the unique ID for ImGui
      _elements[handle]._element->draw();
      ImGui::PopID();
    }

    ImGui::PopItemWidth();

    if (_isOverlay)
    {
      ImGui::PopStyleColor();
    }

    ImGui::End();
  }

  inline ui_element_handle getLastElementHandle() const
  {
    return static_cast<ui_element_handle>(_elements.size()) - 1;
  }

  std::string _name;
  ImVec2 _initSize;

  bool _isOverlay = false;
  glm::vec3 _position3D;

  Renderer* _renderer;

  mutable std::mutex _mutex;
  Vector<UIElement> _elements;
};

IUIWindow* createUIWindow(const char* name, float initialWidth, float initialHeight, Renderer* renderer)
{
  return new UIWindow(name, initialWidth, initialHeight, renderer);
}

void UIButton::draw()
{
  const bool pressed = ImGui::Button(_label.c_str());
  _pressed = _pressed || pressed;
}

template <>
void UISlider<float>::drawInternal(int n)
{
  static const std::function<bool(const char*, float*, float, float, const char*, float)> sliderFloatFn[4]
    { ImGui::SliderFloat, ImGui::SliderFloat2, ImGui::SliderFloat3, ImGui::SliderFloat4 };

  static const std::function<bool(const char*, float*, float, float, float, const char*, float)> dragFloatFn[4]
    { ImGui::DragFloat, ImGui::DragFloat2, ImGui::DragFloat3, ImGui::DragFloat4 };

  switch (_type)
  {
    case UISliderType::SliderType:
    {
      sliderFloatFn[n-1](_label.c_str(), _values.data(), _min, _max, "%.3f", 1.0f);
      break;
    }
    case UISliderType::DragType:
    {
      dragFloatFn[n-1](_label.c_str(), _values.data(), _speed, _min, _max, "%.3f", 1.0f);
      break;
    }
    case UISliderType::SliderAngleType:
    {
      ImGui::SliderAngle(_label.c_str(), _values.data(), _min, _max);
      break;
    }
  }
}

template <>
void UISlider<int>::drawInternal(int n)
{
  static const std::function<bool(const char*, int*, int, int, const char*)> sliderIntFn[4]
    { ImGui::SliderInt, ImGui::SliderInt2, ImGui::SliderInt3, ImGui::SliderInt4 };

  static const std::function<bool(const char*, int*, float, int, int, const char*)> dragIntFn[4]
    { ImGui::DragInt, ImGui::DragInt2, ImGui::DragInt3, ImGui::DragInt4 };

  switch (_type)
  {
    case UISliderType::SliderType:
    {
      sliderIntFn[n-1](_label.c_str(), _values.data(), _min, _max, "%.0f");
      break;
    }
    case UISliderType::DragType:
    {
      dragIntFn[n-1](_label.c_str(), _values.data(), _speed, _min, _max, "%.0f");
      break;
    }
    case UISliderType::SliderAngleType:
    {
      ImGui::SliderAngle(_label.c_str(), (float*)_values.data(), (float)_min, (float)_max);
      break;
    }
  }
}


void UICheckBox::draw()
{
  ImGui::Checkbox(_label.c_str(), &_checked);
}

void UIFloatRange::draw()
{
  ImGui::DragFloatRange2(_label.c_str(), &_lowerValue, &_upperValue, _speed, _min, _max);
}

void UIComboBox::draw()
{
  const char* items[32]; // 32 items max
  for (size_t i = 0; i < _items.size(); i++)
    items[i] = _items[i].c_str();

  ImGui::Combo(_label.c_str(), &_currentItem, items, static_cast<int>(_items.size()));
}

void UIInputText::draw()
{
  static constexpr int bufferSize = 512;
  static char buffer[bufferSize] { 0 };
  if (!_text.empty())
    strncpy(buffer, _text.c_str(), bufferSize);
  if (ImGui::InputText(_label.c_str(), buffer, bufferSize))
    _text = buffer;
}

void UIText::draw()
{
  ImGui::Text("%s", _text.c_str());
}

void UIAuxiliaryElement::draw()
{
  switch (_type)
  {
    case SameLine:
      ImGui::SameLine();
      break;
    case Separator:
      ImGui::Separator();
      break;
  }
}

void UIPlot::draw()
{
  ImGui::PlotLines(_label.c_str(), _buffer.data(), static_cast<int>(_buffer.size()), _offset, nullptr, _rangeMin, _rangeMax, ImVec2(0, _height));
}


template <typename T, std::size_t N, UISliderType::Type TType>
UISlider<T>* constructSlider(const char* label, T min, T max, const T* values)
{
  static_assert(TType != UISliderType::DragType, "");

  UISlider<T>* slider = new UISlider<T>;
  slider->_numValues = N;
  slider->_type = TType;
  slider->_label = label;
  slider->_min = min;
  slider->_max = max;
  if (values != nullptr)
  {
    for (size_t i = 0; i < N; i++)
      slider->_values[i] = values[i];
  }
  else
  {
    for (size_t i = 0; i < N; i++)
      slider->_values[i] = 0;
  }

  return slider;
};

template <typename T, std::size_t N, UISliderType::Type TType>
UISlider<T>* constructSlider(const char* label, T min, T max, float speed, const T* values)
{
  static_assert(TType == UISliderType::DragType, "");

  UISlider<T>* slider = new UISlider<T>;
  slider->_numValues = N;
  slider->_type = UISliderType::DragType;
  slider->_label = label;
  slider->_min = min;
  slider->_max = max;
  slider->_speed = speed;
  if (values != nullptr)
  {
    for (size_t i = 0; i < N; i++)
      slider->_values[i] = values[i];
  }
  else
  {
    for (size_t i = 0; i < N; i++)
      slider->_values[i] = 0;
  }

  return slider;
};

// Button
ui_element_handle UIWindow::AddButton(const char* label)
{
  MutexLockGuard lockGuard(_mutex);

  UIButton* button = new UIButton;
  button->_label = label;
  button->_pressed = false;
  _elements.emplace_back(button);
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderFloat(const char* label, float min, float max, float value)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 1, UISliderType::SliderType>(label, min, max, &value));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderFloat2(const char* label, float min, float max, const float* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 2, UISliderType::SliderType>(label, min, max, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderFloat3(const char* label, float min, float max, const float* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 3, UISliderType::SliderType>(label, min, max, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderFloat4(const char* label, float min, float max, const float* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 4, UISliderType::SliderType>(label, min, max, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderAngle(const char* label, float minDegree, float maxDegree, float valueRadians)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 1, UISliderType::SliderAngleType>(label, minDegree, maxDegree, &valueRadians));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderInt(const char* label, int min, int max, int value)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<int, 1, UISliderType::SliderType>(label, min, max, &value));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderInt2(const char* label, int min, int max, const int* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<int, 2, UISliderType::SliderType>(label, min, max, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderInt3(const char* label, int min, int max, const int* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<int, 3, UISliderType::SliderType>(label, min, max, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddSliderInt4(const char* label, int min, int max, const int* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<int, 4, UISliderType::SliderType>(label, min, max, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddDragFloat(const char* label, float min, float max, float speed, float value)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 1, UISliderType::DragType>(label, min, max, speed, &value));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddDragFloat2(const char* label, float min, float max, float speed, const float* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 2, UISliderType::DragType>(label, min, max, speed, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddDragFloat3(const char* label, float min, float max, float speed, const float* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 3, UISliderType::DragType>(label, min, max, speed, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddDragFloat4(const char* label, float min, float max, float speed, const float* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<float, 4, UISliderType::DragType>(label, min, max, speed, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddDragInt(const char* label, int min, int max, float speed, int value)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<int, 1, UISliderType::DragType>(label, min, max, speed, &value));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddDragInt2(const char* label, int min, int max, float speed, const int* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<int, 2, UISliderType::DragType>(label, min, max, speed, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddDragInt3(const char* label, int min, int max, float speed, const int* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<int, 3, UISliderType::DragType>(label, min, max, speed, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddDragInt4(const char* label, int min, int max, float speed, const int* values)
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(constructSlider<int, 4, UISliderType::DragType>(label, min, max, speed, values));
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddCheckBox(const char* label, bool checked)
{
  MutexLockGuard lockGuard(_mutex);

  UICheckBox* checkBox = new UICheckBox();
  checkBox->_label = label;
  checkBox->_checked = checked;
  _elements.emplace_back(checkBox);
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddFloatRange(const char* label, float speed, float min, float max, float lower, float upper)
{
  MutexLockGuard lockGuard(_mutex);

  UIFloatRange* floatRange = new UIFloatRange();
  floatRange->_label = label;
  floatRange->_speed = speed;
  floatRange->_min = min;
  floatRange->_max = max;
  floatRange->_lowerValue = lower;
  floatRange->_upperValue = upper;
  _elements.emplace_back(floatRange);
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddComboBox(const char* label, const char** items, int itemsCount, int selectedItem)
{
  MutexLockGuard lockGuard(_mutex);

  UIComboBox* comboBox = new UIComboBox();
  comboBox->_label = label;
  for (int i = 0; i < itemsCount; i++)
    comboBox->_items.emplace_back(items[i]);

  comboBox->_currentItem = selectedItem;
  _elements.emplace_back(comboBox);
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddInputText(const char* label, const char* text)
{
  MutexLockGuard lockGuard(_mutex);

  UIInputText* inputText = new UIInputText();
  inputText->_label = label;
  if (text != nullptr)
    inputText->_text = text;
  _elements.emplace_back(inputText);
  return getLastElementHandle();
}

constexpr int _textBufferSize = 512;

ui_element_handle UIWindow::AddText(const char* fmt, ...)
{
  MutexLockGuard lockGuard(_mutex);

  UIText* uiText = new UIText();

  va_list args;
  va_start(args, fmt);

  char buf[_textBufferSize];
  std::vsnprintf(buf, _textBufferSize, fmt, args);
  uiText->_text = buf;
  va_end(args);

  _elements.emplace_back(uiText);
  return getLastElementHandle();
}

ui_element_handle UIWindow::AddPlot(const char* label, float rangeMin, float rangeMax, int bufferSize, float height)
{
  MutexLockGuard lockGuard(_mutex);

  UIPlot* plot = new UIPlot();
  plot->_label = label;
  plot->_rangeMin = rangeMin;
  plot->_rangeMax = rangeMax;
  plot->_height = height;
  plot->_offset = 0;
  plot->_buffer.resize(bufferSize);
  _elements.emplace_back(plot);
  return getLastElementHandle();
}

void UIWindow::PushPlotValue(ui_element_handle handle, float value)
{
  MutexLockGuard lockGuard(_mutex);

  UIPlot* plot = dynamic_cast<UIPlot*>(_elements[handle]._element.get());

  plot->_buffer[plot->_offset] = value;
  plot->_offset = (plot->_offset + 1) % plot->size();
}

void UIWindow::UpdateText(ui_element_handle handle, const char* fmt, ...)
{
  MutexLockGuard lockGuard(_mutex);

  UIText* uiText = dynamic_cast<UIText*>(_elements[handle]._element.get());

  va_list args;
  va_start(args, fmt);

  char buf[_textBufferSize];
  std::vsnprintf(buf, _textBufferSize, fmt, args);
  uiText->_text = buf;
  va_end(args);
}

void UIWindow::AddSeparator()
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(new UIAuxiliaryElement(UIAuxiliaryElement::Separator));
}

void UIWindow::SameLine()
{
  MutexLockGuard lockGuard(_mutex);

  _elements.emplace_back(new UIAuxiliaryElement(UIAuxiliaryElement::SameLine));
}

bool UIWindow::GetButtonState(ui_element_handle handle)
{
  MutexLockGuard lockGuard(_mutex);

  UIButton* button = dynamic_cast<UIButton*>(_elements[handle]._element.get());
  const bool pressed = button->_pressed;
  button->_pressed = false;
  return pressed;
}

float UIWindow::GetSliderFloatValue(ui_element_handle handle) const
{
  MutexLockGuard lockGuard(_mutex);

  UISlider<float>* slider = dynamic_cast<UISlider<float>*>(_elements[handle]._element.get());
  if (slider == nullptr)
    throw new std::exception;

  return slider->_values[0];
}

void UIWindow::GetSliderFloatValues(ui_element_handle handle, float* values) const
{
  MutexLockGuard lockGuard(_mutex);

  UISlider<float>* slider = dynamic_cast<UISlider<float>*>(_elements[handle]._element.get());
  for (int i = 0; i < slider->_numValues; i++)
    values[i] = slider->_values[i];
}

int UIWindow::GetSliderIntValue(ui_element_handle handle) const
{
  MutexLockGuard lockGuard(_mutex);

  UISlider<int>* slider = dynamic_cast<UISlider<int>*>(_elements[handle]._element.get());
  return slider->_values[0];
}

void UIWindow::GetSliderIntValues(ui_element_handle handle, int* values) const
{
  MutexLockGuard lockGuard(_mutex);

  UISlider<int>* slider = dynamic_cast<UISlider<int>*>(_elements[handle]._element.get());
  for (int i = 0; i < slider->_numValues; i++)
    values[i] = slider->_values[i];
}

bool UIWindow::GetCheckBoxState(ui_element_handle handle) const
{
  MutexLockGuard lockGuard(_mutex);

  UICheckBox* checkBox = dynamic_cast<UICheckBox*>(_elements[handle]._element.get());
  return checkBox->_checked;
}

void UIWindow::GetFloatRangeValues(ui_element_handle handle, float& lower, float& upper) const
{
  MutexLockGuard lockGuard(_mutex);

  UIFloatRange* floatRange = dynamic_cast<UIFloatRange*>(_elements[handle]._element.get());
  lower = floatRange->_lowerValue;
  upper = floatRange->_upperValue;
}

int UIWindow::GetSelectedComboBoxItem(ui_element_handle handle) const
{
  MutexLockGuard lockGuard(_mutex);

  UIComboBox* comboBox = dynamic_cast<UIComboBox*>(_elements[handle]._element.get());
  return comboBox->_currentItem;
}

std::string UIWindow::GetInputTextValue(ui_element_handle handle) const
{
  MutexLockGuard lockGuard(_mutex);

  UIInputText* inputText = dynamic_cast<UIInputText*>(_elements[handle]._element.get());
  return inputText->_text;
}

} // namespace ar
