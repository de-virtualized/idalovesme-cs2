#include "Renderer.h"
#include "../../Backend/Misc/Fonts/Fonts.h"
#include "../../Backend/Misc/Textures/Textures.h"
#include "../../../imgui/imgui_impl_win32.h"
#include "../../../imgui/imgui_impl_dx11.h"
#include "../../../imgui/imgui_freetype.h"
#include "visualicon.h"
#include "rageweaponicon.h"
#include "miicon.h"
#include "configicon.h"

std::unique_ptr<Render::CDraw> Render::Draw = std::make_unique<Render::CDraw>();

using namespace Render;
using namespace IdaLovesMe;

namespace Render 
{
    namespace Fonts 
    {
        LPD3DXFONT TabIcons;
        LPD3DXFONT LegitTabIcons;
        LPD3DXFONT Verdana;
        LPD3DXFONT Tahombd;
        LPD3DXFONT vsicon;
        LPD3DXFONT rwicon;
        LPD3DXFONT misicon;
        LPD3DXFONT conficon;
    }
}

void CDraw::CreateObjects()
{
    Fonts::Verdana = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 12.0f);
    Fonts::Tahombd = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdanab.ttf", 12.0f);

    ImFontConfig cfg;
    cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_Monochrome;

    Fonts::TabIcons = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(FontsData::TabIcons, 5192, 47.0f);
    Fonts::LegitTabIcons = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(FontsData::LegitTabIcons, 47320, 32.0f);
    Fonts::vsicon = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(visic, 1964, 47.0f);
    Fonts::misicon = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(misic, 2484, 47.0f);
    Fonts::conficon = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(confic, 1756, 47.0f);


    D3DX11CreateShaderResourceViewFromMemory(m_Device, TexturesData::BgTexture, 424852, NULL, NULL, &this->m_BgTexture, NULL);
    D3DX11CreateShaderResourceViewFromMemory(m_Device, RageAdaptiveGlobalIcon, 1394, NULL, NULL, &this->rwtexture, NULL);
}

void CDraw::NewFont(int script_id, std::string path, int size, std::string id)
{
    if (GetFontByName(script_id, id))
        return;

    font_queue.at(script_id).push_back(std::make_tuple(id, path, size));
}

ImFont* CDraw::GetFontByName(int script_id, std::string id)
{
    return fonts.at(script_id)[id];
}

void CDraw::ReleaseObjects()
{

}

void CDraw::Init(ID3D11Device* D3dDevice, HWND window)
{
    if (!Initialized) 
    {
        m_Device = D3dDevice;
        m_Window = window;
        CreateObjects();
        Initialized = true;
    }
}

void CDraw::Reset()
{
    Screen.Width = (float)ImGui::GetIO().DisplaySize.x;
    Screen.Height = (float)ImGui::GetIO().DisplaySize.y;
    Screen.x_center = Screen.Width / 2;
    Screen.y_center = Screen.Height / 2;
}

void CDraw::Line(Vec2 Pos, Vec2 Pos2, D3DCOLOR Color)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(Pos.x, Pos.y), ImVec2(Pos2.x, Pos2.y), Color);
}

void CDraw::Rect(Vec2 Pos, Vec2 Size, float LineWidth, D3DCOLOR Color, bool Antialias)
{
    if (LineWidth == 0 || LineWidth == 1)
    {
        FilledRect(Pos, Vec2(Size.x, 1), Color, Antialias);             // Top
        FilledRect(Vec2(Pos.x, Pos.y + Size.y - 1), Vec2(Size.x, 1), Color, Antialias);         // Bottom
        FilledRect(Vec2(Pos.x, Pos.y + 1), Vec2(1, Size.y - 2 * 1), Color, Antialias);       // Left
        FilledRect(Vec2(Pos.x + Size.x - 1, Pos.y + 1), Vec2(1, Size.y - 2 * 1), Color, Antialias);   // Right
    }
    else
    {
        FilledRect(Pos, Vec2(Size.x, LineWidth), Color, Antialias);                                     // Top
        FilledRect(Vec2(Pos.x, Pos.y + Size.y - LineWidth), Vec2(Size.x, LineWidth), Color, Antialias);                         // Bottom
        FilledRect(Vec2(Pos.x, Pos.y + LineWidth), Vec2(LineWidth, Size.x - 2 * LineWidth), Color, Antialias);               // Left
        FilledRect(Vec2(Pos.x + Size.x - LineWidth, Pos.y + LineWidth), Vec2(LineWidth, Size.y - 2 * LineWidth), Color, Antialias);   // Right
    }
}

void CDraw::FilledRect(Vec2 Pos, Vec2 Size, D3DCOLOR Color, bool Antialias)
{
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(Pos.x, Pos.y), ImVec2(Pos.x + Size.x, Pos.y + Size.y), Color);
}


void CDraw::BorderedRect(Vec2 Pos, Vec2 Size, float BorderWidth, D3DCOLOR Color, D3DCOLOR BorderColor)
{
    FilledRect(Pos, Size, Color);
    Rect(Vec2(Pos.x - BorderWidth, Pos.y - BorderWidth), Vec2(Size.x + 2 * BorderWidth, Size.y + BorderWidth), BorderWidth, BorderColor);
}

void CDraw::Gradient(Vec2 Pos, Vec2 Size, D3DCOLOR LColor, D3DCOLOR ROtherColor, bool Vertical, bool Antialias)
{
    if (Vertical)
        ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(ImVec2(Pos.x, Pos.y), ImVec2(Pos.x + Size.x, Pos.y + Size.y), LColor, LColor, ROtherColor, ROtherColor);
    else
        ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(ImVec2(Pos.x, Pos.y), ImVec2(Pos.x + Size.x, Pos.y + Size.y), LColor, ROtherColor, ROtherColor, LColor);
}

void DrawT(ImFont* Font, ImColor Color, const char* text, bool NoClipRect, ImVec2 pos, ImVec2 clip, bool ncl)
{
    ImVec2 d_min = ImGui::GetForegroundDrawList()->GetClipRectMin();
    ImVec2 d_max = ImGui::GetForegroundDrawList()->GetClipRectMax();

    if (!NoClipRect)
        ImGui::GetForegroundDrawList()->PushClipRect(pos, clip, true);
    else if (ncl)
        ImGui::GetForegroundDrawList()->PopClipRect();

    ImGui::GetForegroundDrawList()->AddText(Font, Font->FontSize, pos, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text);

    if (!NoClipRect)
        ImGui::GetForegroundDrawList()->PopClipRect();
    else if (ncl)
        ImGui::GetForegroundDrawList()->PushClipRect(d_min, d_max);
}

void CDraw::Text(const char* text, float x_, float y_, int Orientation, LPD3DXFONT Font, bool Bordered, D3DCOLOR Color, Vec2 TextClipSize, bool ncl)
{
    ImVec2 d_min = ImGui::GetForegroundDrawList()->GetClipRectMin();
    ImVec2 d_max = ImGui::GetForegroundDrawList()->GetClipRectMax();

    bool NoClipRect = (TextClipSize.x == 0 && TextClipSize.y == 0) ? true : false;

    if (Orientation == CENTER)
    {
        x_ -= GetTextSize(Font, text).x * 0.5f;
        y_ -= GetTextSize(Font, text).y * 0.5f;
    }
    else if (Orientation == RIGHT)
        x_ -= GetTextSize(Font, text).x;

    if (Bordered)
    {
        DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_ - 1, y_), ImVec2((int)TextClipSize.x - 1, (int)TextClipSize.y), ncl);
        DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_ + 1, y_), ImVec2((int)TextClipSize.x + 1, (int)TextClipSize.y), ncl);
        DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_, y_ - 1), ImVec2((int)TextClipSize.x, (int)TextClipSize.y - 1), ncl);
        DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_, y_ + 1), ImVec2((int)TextClipSize.x, (int)TextClipSize.y + 1), ncl);

        if (NoClipRect)
        {
            DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_ - 1, y_ - 1), ImVec2((int)TextClipSize.x - 1, (int)TextClipSize.y - 1), ncl);
            DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_ + 1, y_ + 1), ImVec2((int)TextClipSize.x + 1, (int)TextClipSize.y + 1), ncl);
            DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_ - 1, y_ + 1), ImVec2((int)TextClipSize.x - 1, (int)TextClipSize.y + 1), ncl);
            DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_ + 1, y_ - 1), ImVec2((int)TextClipSize.x + 1, (int)TextClipSize.y - 1), ncl);
        }
        else
        {
            DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_ - 1, y_), ImVec2((int)TextClipSize.x - 1, (int)TextClipSize.y), ncl);
            DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_ + 1, TextClipSize.y), ImVec2((int)TextClipSize.x + 1, (int)TextClipSize.y), ncl);
            DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_, y_ - 1), ImVec2((int)TextClipSize.x, (int)TextClipSize.y - 1), ncl);
            DrawT(Font, D3DCOLOR_RGBA(15, 15, 15, int(Color.Value.w * 255.0f)), text, NoClipRect, ImVec2(x_, y_ + 1), ImVec2((int)TextClipSize.x, (int)TextClipSize.y + 1), ncl);
        }
    }

    if (!NoClipRect)
        ImGui::GetForegroundDrawList()->PushClipRect(ImVec2(x_, y_), ImVec2(TextClipSize.x, TextClipSize.y), true);
    else if (ncl)
        ImGui::GetForegroundDrawList()->PopClipRect();

    ImGui::GetForegroundDrawList()->AddText(Font, Font->FontSize, ImVec2(x_, y_), Color, text);

    if (!NoClipRect)
        ImGui::GetForegroundDrawList()->PopClipRect();
    else if (ncl)
        ImGui::GetForegroundDrawList()->PushClipRect(d_min, d_max);
}

void CDraw::Sprite(ID3D11ShaderResourceView* Texture, Vec2 Pos, Vec2 Size, D3DCOLOR Color)
{
    ImGui::GetForegroundDrawList()->AddImage((ImTextureID)Texture, ImVec2(Pos.x, Pos.y), ImVec2(Pos.x + Size.x, Pos.y + Size.y), ImVec2(0, 0), ImVec2(1, 1), Color);
}

void CDraw::Triangle(Vec2 Top, Vec2 Left, Vec2 Right, D3DCOLOR Color, bool antialias)
{
    ImGui::GetForegroundDrawList()->AddTriangleFilled(ImVec2(Top.x, Top.y), ImVec2(Right.x, Right.y), ImVec2(Left.x, Left.y), Color);
}

ID3D11ShaderResourceView* CDraw::GetBgTexture()
{
    return this->m_BgTexture;
}

ID3D11ShaderResourceView* CDraw::GetRwTexture()
{
    return this->rwtexture;
}

Vec2 CDraw::GetTextSize(ID3DXFont* Font, const char* Text)
{
    ImVec2 size = Font->CalcTextSizeA(Font->FontSize, FLT_MAX, 0.0f, Text);

    return Vec2(size.x, size.y);
}

ID3D11Device* CDraw::GetD3dDevice()
{
    return this->m_Device;
}

HWND CDraw::GetWindow()
{
    return this->m_Window;
}