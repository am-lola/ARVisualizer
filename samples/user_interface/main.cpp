#include <iostream>
#include <vector>
#include "arvisualizer.hpp"
#include <thread>

int main(void)
{
  ar::ARVisualizer* visualizer = new ar::ARVisualizer();

  visualizer->Start();

  // No need to free these pointers
  // We're passing an optional initial window size when first loaded. On subsequent loads, the UI configuration will be saved in imgui.ini
  ar::IUIWindow* testWindow = visualizer->AddUIWindow("Test Window", 350.0f, 0.0f); // 0.0f for height = determine automatically
  ar::IUIWindow* valuesWindow = visualizer->AddUIWindow("Values", 200.0f, 0.0f);

  ar::ui_element_handle buttonExit = testWindow->AddButton("Close application");
  // Call SameLine() to put the next element on the same line
  testWindow->SameLine();
  testWindow->AddButton("Button on same line");

  testWindow->AddSeparator();

  ar::ui_element_handle sliderFloat1 = testWindow->AddSliderFloat("SliderFloat1", 0.0f, 10.0f, 5.0f);
  ar::ui_element_handle sliderFloat2 = testWindow->AddSliderFloat2("SliderFloat2", 0.0f, 10.0f);
  ar::ui_element_handle sliderFloat3 = testWindow->AddSliderFloat3("SliderFloat3", 0.0f, 10.0f);
  ar::ui_element_handle sliderFloat4 = testWindow->AddSliderFloat4("SliderFloat4", 0.0f, 10.0f);

  testWindow->AddSeparator();

  ar::ui_element_handle sliderInt1 = testWindow->AddSliderInt("SliderInt1", 0, 10, 5);
  ar::ui_element_handle sliderInt2 = testWindow->AddSliderInt2("SliderInt2", 0, 10);
  ar::ui_element_handle sliderInt3 = testWindow->AddSliderInt3("SliderInt3", 0, 10);
  ar::ui_element_handle sliderInt4 = testWindow->AddSliderInt4("SliderInt4", 0, 10);

  testWindow->AddSeparator();

  ar::ui_element_handle dragFloat1 = testWindow->AddDragFloat("DragFloat1", -20.0f, 30.0f);
  ar::ui_element_handle dragFloat2 = testWindow->AddDragFloat2("DragFloat2", -20.0f, 30.0f);
  ar::ui_element_handle dragFloat3 = testWindow->AddDragFloat3("DragFloat3", -20.0f, 30.0f);
  ar::ui_element_handle dragFloat4 = testWindow->AddDragFloat4("DragFloat4", -20.0f, 30.0f);

  testWindow->AddSeparator();

  ar::ui_element_handle dragInt1 = testWindow->AddDragInt("DragInt1", -20, 30);
  ar::ui_element_handle dragInt2 = testWindow->AddDragInt2("DragInt2", -20, 30);
  ar::ui_element_handle dragInt3 = testWindow->AddDragInt3("DragInt3", -20, 30);
  ar::ui_element_handle dragInt4 = testWindow->AddDragInt4("DragInt4", -20, 30);

  testWindow->AddSeparator();

  ar::ui_element_handle checkBox = testWindow->AddCheckBox("Checkbox");
  testWindow->SameLine();
  ar::ui_element_handle checkBox2 = testWindow->AddCheckBox("Checkbox 2", true);
  ar::ui_element_handle inputText = testWindow->AddInputText("InputText");
  ar::ui_element_handle sliderAngle = testWindow->AddSliderAngle("SliderAngle", -180.0f, 180.0f);
  ar::ui_element_handle floatRange = testWindow->AddFloatRange("FloatRange", 0.5f);


  // Add text elements to another window to show the values of the elements in the first window

  ar::ui_element_handle sliderFloat1_Text = valuesWindow->AddText("");
  ar::ui_element_handle sliderFloat2_Text = valuesWindow->AddText("");
  ar::ui_element_handle sliderFloat3_Text = valuesWindow->AddText("");
  ar::ui_element_handle sliderFloat4_Text = valuesWindow->AddText("");

  valuesWindow->AddSeparator();

  ar::ui_element_handle sliderInt1_Text = valuesWindow->AddText("");
  ar::ui_element_handle sliderInt2_Text = valuesWindow->AddText("");
  ar::ui_element_handle sliderInt3_Text = valuesWindow->AddText("");
  ar::ui_element_handle sliderInt4_Text = valuesWindow->AddText("");

  valuesWindow->AddSeparator();

  ar::ui_element_handle dragFloat1_Text = valuesWindow->AddText("");
  ar::ui_element_handle dragFloat2_Text = valuesWindow->AddText("");
  ar::ui_element_handle dragFloat3_Text = valuesWindow->AddText("");
  ar::ui_element_handle dragFloat4_Text = valuesWindow->AddText("");

  valuesWindow->AddSeparator();

  ar::ui_element_handle dragInt1_Text = valuesWindow->AddText("");
  ar::ui_element_handle dragInt2_Text = valuesWindow->AddText("");
  ar::ui_element_handle dragInt3_Text = valuesWindow->AddText("");
  ar::ui_element_handle dragInt4_Text = valuesWindow->AddText("");

  valuesWindow->AddSeparator();

  ar::ui_element_handle checkBox_Text = valuesWindow->AddText("");
  valuesWindow->SameLine();
  ar::ui_element_handle checkBox2_Text = valuesWindow->AddText("");
  ar::ui_element_handle inputText_Text = valuesWindow->AddText("");
  ar::ui_element_handle sliderAngle_Text = valuesWindow->AddText("");
  ar::ui_element_handle floatRange_Text = valuesWindow->AddText("");


  while (!testWindow->GetButtonState(buttonExit))
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(33));

    {
      float values[4];

      valuesWindow->UpdateText(sliderFloat1_Text, "%.3f", testWindow->GetSliderFloatValue(sliderFloat1));

      testWindow->GetSliderFloatValues(sliderFloat2, values);
      valuesWindow->UpdateText(sliderFloat2_Text, "%.3f %.3f", values[0], values[1]);
      testWindow->GetSliderFloatValues(sliderFloat3, values);
      valuesWindow->UpdateText(sliderFloat3_Text, "%.3f %.3f %.3f", values[0], values[1], values[2]);
      testWindow->GetSliderFloatValues(sliderFloat4, values);
      valuesWindow->UpdateText(sliderFloat4_Text, "%.3f %.3f %.3f %.3f", values[0], values[1], values[2], values[3]);
    }

    {
      int values[4];

      valuesWindow->UpdateText(sliderInt1_Text, "%d", testWindow->GetSliderIntValue(sliderInt1));

      testWindow->GetSliderIntValues(sliderInt2, values);
      valuesWindow->UpdateText(sliderInt2_Text, "%d %d", values[0], values[1]);
      testWindow->GetSliderIntValues(sliderInt3, values);
      valuesWindow->UpdateText(sliderInt3_Text, "%d %d %d", values[0], values[1], values[2]);
      testWindow->GetSliderIntValues(sliderInt4, values);
      valuesWindow->UpdateText(sliderInt4_Text, "%d %d %d %d", values[0], values[1], values[2], values[3]);
    }

    {
      float values[4];

      valuesWindow->UpdateText(dragFloat1_Text, "%.3f", testWindow->GetSliderFloatValue(dragFloat1));

      testWindow->GetSliderFloatValues(dragFloat2, values);
      valuesWindow->UpdateText(dragFloat2_Text, "%.3f %.3f", values[0], values[1]);
      testWindow->GetSliderFloatValues(dragFloat3, values);
      valuesWindow->UpdateText(dragFloat3_Text, "%.3f %.3f %.3f", values[0], values[1], values[2]);
      testWindow->GetSliderFloatValues(dragFloat4, values);
      valuesWindow->UpdateText(dragFloat4_Text, "%.3f %.3f %.3f %.3f", values[0], values[1], values[2], values[3]);
    }

    {
      int values[4];

      valuesWindow->UpdateText(dragInt1_Text, "%d", testWindow->GetSliderIntValue(dragInt1));

      testWindow->GetSliderIntValues(dragInt2, values);
      valuesWindow->UpdateText(dragInt2_Text, "%d %d", values[0], values[1]);
      testWindow->GetSliderIntValues(dragInt3, values);
      valuesWindow->UpdateText(dragInt3_Text, "%d %d %d", values[0], values[1], values[2]);
      testWindow->GetSliderIntValues(dragInt4, values);
      valuesWindow->UpdateText(dragInt4_Text, "%d %d %d %d", values[0], values[1], values[2], values[3]);
    }

    valuesWindow->UpdateText(checkBox_Text, "%d", (int)testWindow->GetCheckBoxState(checkBox));
    valuesWindow->UpdateText(checkBox2_Text, "%d", (int)testWindow->GetCheckBoxState(checkBox2));
    valuesWindow->UpdateText(inputText_Text, "%s", testWindow->GetInputTextValue(inputText).c_str());
    valuesWindow->UpdateText(sliderAngle_Text, "%.3f", testWindow->GetSliderFloatValue(sliderAngle));

    float lower, upper;
    testWindow->GetFloatRangeValues(floatRange, lower, upper);
    valuesWindow->UpdateText(floatRange_Text, "%.3f %.3f", lower, upper);
  }

  std::cout << "Shutting down..." << std::endl;

  // cleanup
  visualizer->Stop();
  delete visualizer;

  return 0;
}
