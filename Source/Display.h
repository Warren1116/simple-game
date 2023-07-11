#pragma once

#include "Character.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"

class Display : public Character {
private:
    Display();
    ~Display();

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // ディスプレイ回転
    void DisplayTurn(float elapsedTime);
};