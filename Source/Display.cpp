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

}

void Display::DisplayTurn(float elapsedTime) {
    angle.y += DirectX::XMConvertToRadians(45) * elapsedTime;
}