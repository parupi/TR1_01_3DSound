#include <Novice.h>
#include "Sound.h"
#include <function.h>

const char kWindowTitle[] = "LC1B_10_カワグチ_ハルキ_TR";

void CameraMove(Vector3& cameraRotate, Vector3& cameraTranslate, Vector2Int& clickPosition, char* keys, char* preKeys);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };
	Vector3 cameraPosition{ 0.0f, 1.0f, -5.0f };
	Vector2Int clickPos{};

	Sphere player{ {0.0f, 0.0f, 0.0f}, 0.1f };
	Sphere source{ {1.0f, 1.0f, 0.0f}, 0.1f };
	bool isSourceRotate = true;
	float roatteSpeed = 5.0f;

	Sound* sound_ = new Sound();
	sound_->Initialize();
	float turningRadius = 3.0f;
	float rotate = 0;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraPosition + cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		
		if (isSourceRotate) {
			//ソース(音源)を空間座標中でぐるぐる回す
			source.center = {(cosf(1.0f * float(M_PI) * rotate / 360.0f)) * turningRadius, 0.0f, (sinf(1.0f * float(M_PI) * rotate / 360.0f)) * turningRadius };
			rotate += roatteSpeed;
		}
		ALfloat SourcePos[] = { source.center.x, source.center.y, source.center.z };
		ALfloat ListenerPos[] = { player.center.x, player.center.y, player.center.z };
		sound_->PlayAudio(ListenerPos, SourcePos);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		CameraMove(cameraRotate, cameraPosition, clickPos, keys, preKeys);

		ImGui::Begin("TR");
		ImGui::DragFloat3("sourcePos", &source.center.x, 0.01f);
		ImGui::DragFloat("sourceSpeed", &roatteSpeed, 0.01f);
		ImGui::DragFloat("turningRadius", &turningRadius, 0.01f);
		ImGui::Checkbox("sourceFlag", &isSourceRotate);
		ImGui::End();

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawSphere(player, viewProjectionMatrix, viewportMatrix, 0x000000FF);
		DrawSphere(source, viewProjectionMatrix, viewportMatrix, 0xFF0000FF);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	delete sound_;

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void CameraMove(Vector3& cameraRotate, Vector3& cameraTranslate, Vector2Int& clickPosition, char* keys, char* preKeys) {
	// カーソルを動かすときの感度
	const float mouseSensitivity = 0.003f;
	// カメラの移動速度
	const float moveSpeed = 0.005f;

	// 各フラグ
	static bool isLeftClicked = false;
	static bool isWheelClicked = false;
	static bool isDebugCamera = false;

	// 回転を考慮する
	Matrix4x4 rotationMatrix = MakeRotateXYZMatrix(cameraRotate);
	Vector3 X = { 1.0f, 0.0f, 0.0f };
	Vector3 Y = { 0.0f, 1.0f, 0.0f };
	Vector3 Z = { 0.0f, 0.0f, -1.0f };

	Vector3 rotatedX = Transform(X, rotationMatrix);
	Vector3 rotatedY = Transform(Y, rotationMatrix);
	Vector3 rotatedZ = Transform(Z, rotationMatrix);

	if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
		isDebugCamera = !isDebugCamera;
	}

	if (isDebugCamera) {

		/// ========カメラ操作========
		// カメラの回転を更新する
		if (Novice::IsPressMouse(0) == 1) {
			if (!isLeftClicked) {
				// マウスがクリックされたときに現在のマウス位置を保存する
				Novice::GetMousePosition(&clickPosition.x, &clickPosition.y);
				isLeftClicked = true;
			}
			else {
				// マウスがクリックされている間はカメラの回転を更新する
				Vector2Int currentMousePos;
				Novice::GetMousePosition(&currentMousePos.x, &currentMousePos.y);

				float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
				float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

				cameraRotate.x += deltaY * mouseSensitivity;
				cameraRotate.y += deltaX * mouseSensitivity;

				// 現在のマウス位置を保存する
				clickPosition = currentMousePos;
			}
		}
		else {
			// マウスがクリックされていない場合はフラグをリセットする
			isLeftClicked = false;
		}

		// カメラの位置を更新する
		if (Novice::IsPressMouse(2) == 1) {
			if (!isWheelClicked) {
				// マウスがクリックされたときに現在のマウス位置を保存する
				Novice::GetMousePosition(&clickPosition.x, &clickPosition.y);
				isWheelClicked = true;
			}
			else {
				// マウスがクリックされている間はカメラの位置を更新する
				Vector2Int currentMousePos;
				Novice::GetMousePosition(&currentMousePos.x, &currentMousePos.y);

				float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
				float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

				cameraTranslate -= rotatedX * deltaX * mouseSensitivity;
				cameraTranslate += rotatedY * deltaY * mouseSensitivity;

				// 現在のマウス位置を保存する
				clickPosition = currentMousePos;
			}
		}
		else {
			// マウスがクリックされていない場合はフラグをリセットする
			isWheelClicked = false;
		}

		// マウスホイールの移動量を取得する
		int wheelDelta = -Novice::GetWheel();

		// マウスホイールの移動量に応じてカメラの移動を更新する
		cameraTranslate += rotatedZ * float(wheelDelta) * moveSpeed;
		/// =====================
	}
	/*ImGui::Begin("camera explanation");
	ImGui::Text("DebugCamera = %d (0 = false , 1 = true)\nPressingMouseLeftbutton : moveCameraRotate\nPressingMouseWheelbutton : moveCameraTranslate", isDebugCamera);
	ImGui::End();*/
}