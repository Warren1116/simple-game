#include  "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

// 初期化
void SceneLoading::Initialize() {
    // スプライト初期化
    sprite = new Sprite("Data/Sprite/road.png");
    Enter = new Sprite("Data/Sprite/Enter.png");
    tutorial[0] = new Sprite("Data/Sprite/cyutoriaru.png");
    tutorial[1] = new Sprite("Data/Sprite/chuto2.png");

    // スレッド開始
    thread = new std::thread(SceneLoading::LoadingThread, this); // threadを起動する際はスレッド用関数とインスタンスのthisポインタを引数に渡す
}

// 終了化
void SceneLoading::Finalize() {
    // スレッド終了化

    if (thread != nullptr) {
        thread->join();
        delete thread;
        thread = nullptr;
    }

    if (Enter != nullptr)
    {
        delete Enter;
        Enter = nullptr;
    }

    if (tutorial[0] != nullptr)
    {
        delete tutorial[0];
        tutorial[0] = nullptr;
    }

    if (tutorial[1] != nullptr)
    {
        delete tutorial[1];
        tutorial[1] = nullptr;
    }

    // スプライト終了化
    if (sprite != nullptr) {
        delete sprite;
        sprite = nullptr;
    }
}

// 更新処理
void SceneLoading::Update(float elapsedTime) {
    constexpr float speed = 180;
    angle += speed * elapsedTime;

    // 次のシーンの準備が完了したときになにかボタンを押したらシーンを切り替える
    GamePad& gamePad = Input::Instance().GetGamePad();
    const GamePadButton anyButton = GamePad::BTN_ENTER;
    float ax = gamePad.GetAxisLX();

    if (ax < 0) tutorialNum = 0;
    else if(ax > 0)  tutorialNum = 1;

    if (nextScene->IsReady() && (gamePad.GetButtonDown() & anyButton)) {
        SceneManager::Instance().ChangeScene(nextScene);
        nextScene = nullptr;
    }
}

// 描画処理
void SceneLoading::Render() {
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // 画面クリア&レンダーターゲット設定
    FLOAT color[] = { 0.0f,0.0f ,0.5f,1.0f }; // RGB(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //2Dスプライト描画
    {
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());
        float textureWidth = static_cast<float>(tutorial[tutorialNum]->GetTextureWidth());
        float textureHeight = static_cast<float>(tutorial[tutorialNum]->GetTextureHeight());

        tutorial[tutorialNum]->Render(dc,
            0, 0, screenWidth, screenHeight,
            0, 0, textureWidth, textureHeight,
            0,
            1, 1, 1, 1);

        textureWidth = static_cast<float>(sprite->GetTextureWidth());
        textureHeight = static_cast<float>(sprite->GetTextureHeight());
        float positionX = screenWidth - textureWidth;
        float positionY = screenHeight - textureHeight;


        // 画面右下にローディングアイコンを描画
        if (nextScene != nullptr && !nextScene->IsReady())
        {
            sprite->Render(dc,
                positionX, positionY, textureWidth, textureHeight,
                0, 0, textureWidth, textureHeight,
                angle,
                1, 1, 1, 1);
        }
        else
        {
            textureWidth = static_cast<float>(Enter->GetTextureWidth());
            textureHeight = static_cast<float>(Enter->GetTextureHeight());

            Enter->Render(dc,
                positionX - 500, positionY + 100, textureWidth, textureHeight,
                0, 0, textureWidth, textureHeight,
                0,
                1, 1, 1, 1);
        }
    }

}

// ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene) {
    // COM関連の初期化でスレッド毎に呼ぶ必要がある
    CoInitialize(nullptr);

    // 次のシーンの初期化を行う
    scene->nextScene->Initialize();

    // スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    // 次のシーンの準備完了設定
    scene->nextScene->SetReady();
}