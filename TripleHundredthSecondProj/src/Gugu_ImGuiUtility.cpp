////////////////////////////////////////////////////////////////
// Header

#include "Gugu_Common.h"
#include "Gugu_ImGuiUtility.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace ImGui {

void Text(const std::string& text)
{
    ImGui::Text(text.c_str());
}

bool InputInt2(const char* label, int* v1, int* v2, ImGuiInputTextFlags flags)
{
    int temp[2] = { *v1, *v2 };

    bool updated = ImGui::InputInt2(label, temp, flags);
    if (updated)
    {
        *v1 = temp[0];
        *v2 = temp[1];
    }

    return updated;
}

bool InputInt2(const char* label, gugu::Vector2i* v, ImGuiInputTextFlags flags)
{
    return InputInt2(label, &v->x, &v->y, flags);
}

bool InputInt2(const char* label, gugu::Vector2u* v, ImGuiInputTextFlags flags)
{
    gugu::Vector2i temp = gugu::Vector2i(*v);
    if (InputInt2(label, &temp, flags))
    {
        *v = gugu::Vector2u(temp);
        return true;
    }

    return false;
}

bool InputInt4(const char* label, sf::IntRect* v, ImGuiInputTextFlags flags)
{
    int temp[4] = { v->left, v->top, v->width, v->height };

    bool updated = ImGui::InputInt4(label, temp, flags);
    if (updated)
    {
        v->left = temp[0];
        v->top = temp[1];
        v->width = temp[2];
        v->height = temp[3];
    }

    return updated;
}

bool InputFloat2(const char* label, float* v1, float* v2, const char* format, ImGuiInputTextFlags flags)
{
    float temp[2] = { *v1, *v2 };

    bool updated = ImGui::InputFloat2(label, temp, format, flags);
    if (updated)
    {
        *v1 = temp[0];
        *v2 = temp[1];
    }

    return updated;
}

bool InputFloat2(const char* label, gugu::Vector2f* v, const char* format, ImGuiInputTextFlags flags)
{
    return InputFloat2(label, &v->x, &v->y, format, flags);
}

bool InputFloat4(const char* label, gugu::Vector2f* v1, gugu::Vector2f* v2, const char* format, ImGuiInputTextFlags flags)
{
    float temp[4] = { v1->x, v1->y, v2->x, v2->y };

    bool updated = ImGui::InputFloat4(label, temp, format, flags);
    if (updated)
    {
        v1->x = temp[0];
        v1->y = temp[1];
        v2->x = temp[2];
        v2->y = temp[3];
    }

    return updated;
}

bool InputFloat4(const char* label, gugu::UDim2* v, const char* format, ImGuiInputTextFlags flags)
{
    return InputFloat4(label, &v->relative, &v->absolute, format, flags);
}

bool ColorEdit4(const char* label, sf::Color* color, ImGuiColorEditFlags flags)
{
    ImVec4 temp = ImGui::ColorConvertSfmlToFloat4(*color);

    bool updated = ImGui::ColorEdit4(label, (float*)&temp, flags);
    if (updated)
    {
        *color = ImGui::ColorConvertFloat4ToSfml(temp);
    }

    return updated;
}

ImVec4 ColorConvertSfmlToFloat4(const sf::Color& color)
{
    return ImGui::ColorConvertU32ToFloat4((color.a << 24) | (color.b << 16) | (color.g << 8) | color.r);
}

sf::Color ColorConvertFloat4ToSfml(const ImVec4& color)
{
    sf::Uint32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);
    return sf::Color(
        (sf::Uint8)((colorU32 & 0x000000ff) >> 0),
        (sf::Uint8)((colorU32 & 0x0000ff00) >> 8),
        (sf::Uint8)((colorU32 & 0x00ff0000) >> 16),
        (sf::Uint8)((colorU32 & 0xff000000) >> 24)
    );
}

bool Combo(const char* label, const std::vector<std::string>& comboValues, size_t* selectedIndex, ImGuiComboFlags flags)
{
    bool updated = false;
    if (ImGui::BeginCombo(label, comboValues[*selectedIndex].c_str(), flags))
    {
        for (size_t i = 0; i < comboValues.size(); ++i)
        {
            bool selected = (*selectedIndex == i);
            if (ImGui::Selectable(comboValues[i].c_str(), selected))
            {
                *selectedIndex = i;
                updated = true;
            }

            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    return updated;
}

bool ListBox(const char* label, const std::vector<std::string>& values, size_t* selectedIndex)
{
    bool updated = false;
    if (ImGui::BeginListBox(label))
    {
        for (size_t i = 0; i < values.size(); ++i)
        {
            if (selectedIndex != nullptr)
            {
                bool selected = (*selectedIndex == i);
                if (ImGui::Selectable(values[i].c_str(), selected))
                {
                    *selectedIndex = i;
                    updated = true;
                }

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            else
            {
                ImGui::Text(values[i]);
            }
        }

        ImGui::EndListBox();
    }

    return updated;
}

}   // namespace ImGui
