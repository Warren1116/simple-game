#pragma once

#include "Character.h"
#include "Graphics/Model.h"

class Display : public Character {
private:
    Display();
    ~Display();
    void DisplayTurn(float elapsedTime);
};