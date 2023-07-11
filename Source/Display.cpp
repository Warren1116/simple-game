#include "Display.h"

Display::Display() {
    model = std::make_unique<Model>("Data/Model/Player/Rocket.mdl");

    angle.z = 50.0f;

}

Display::~Display() {

}

void Display::Update(float elapsedTime) {
    DisplayTurn(elapsedTime);
}

void Display::Render(ID3D11DeviceContext* dc, Shader* shader) {
    if (model != nullptr)
    {
        shader->Draw(dc, model.get());
    }
}

void Display::DisplayTurn(float elapsedTime) {
    angle.y += DirectX::XMConvertToRadians(45) * elapsedTime;
    if (angle.y < -DirectX::XM_2PI) angle.y += DirectX::XM_2PI;
    if (angle.y > DirectX::XM_2PI) angle.y -= DirectX::XM_2PI;
}