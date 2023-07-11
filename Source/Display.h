#pragma once

#include "Character.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"

class Display : public Character {
private:
    Display();
    ~Display();

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // �f�B�X�v���C��]
    void DisplayTurn(float elapsedTime);
};