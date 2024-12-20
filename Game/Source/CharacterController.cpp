#include "CharacterController.h"
#ifndef VK_F
#define VK_F 0x46 // 定义 'F' 键的虚拟键码
#endif
// Control of character movement jump.
// simple physic gravity
CharacterController::CharacterController(Vec3 _pos, Vec3 _rot, Vec3 _scale) :Behaviour(_pos, _rot, _scale) 
{
	win = Window::GetInstance();
	driver = &win->GetDevice();
	camera = Camera::GetInstance();

	camera->transform.rotation = transform.rotation;
	camera->transform.position = transform.position + Vec3(0, 4, 0) - camera->transform.forward * 20;
	camera->transform.Update();
	camera->UpdateMat();

	visuals.Init("Resources/TRex/TRex.gem", driver, "Resources/TRex/Textures/");
	instance.animation = &visuals.animation;

	moveSpeed = 10;
	rotSpeed = 20;

	tag = "Player";
	size = Vec3::one * 4;
	offset = Vec3::up * 2;
	attack = false;
	Collisions::AddCollider(this);
}

void CharacterController::Update(float _dt)
{
	static bool togglePressed = false;//check if F is pressed
	// If need to update camera
	bool camUpdate = false;
	// set move speed
	moveSpeed = Lerp(moveSpeed, win->inputs.KeyPressed(VK_SHIFT) ? 20.f : 10.f, 2 * _dt);

	// set grounded
	isGrounded = colliding && gravity < 0;

	// get inputs
	Vec2 moveDelta = win->inputs.GetAxis() * moveSpeed * _dt;
	Vec2 rotDelta = win->inputs.GetCursorLock() ? -win->inputs.MouseDelta() * rotSpeed * _dt : Vec2(0, 0);

	// check for jump
	if (isGrounded && win->inputs.KeyPressed(VK_SPACE))
	{
		isGrounded = false;
		gravity = 20;
	}

	if (rotDelta.Length() > 0)  // update camera rotation
	{
		//update camera rotation
		Vec3 clampRot = camera->transform.rotation;
		rotDelta %= 20.f;
		clampRot.x = clamp(clampRot.x + rotDelta.y, -60, 10);
		clampRot.y += rotDelta.x;

		camera->transform.rotation = clampRot;
		camera->transform.Update();

		camUpdate = true;
	}

	// update gravity
	if (isGrounded)
		gravity = -2.f;
	else
		gravity = clamp(gravity - 40.f * _dt, -20, 20);

	if (moveDelta.Length() > 0 || gravity != 0) // update character move  
	{
		// update character movement and rotation
		if (moveDelta.Length() > 0)
			transform.rotation = Lerp(transform.rotation, Vec3(0, camera->transform.rotation.y, 0), 2 * _dt);
		TranslateRel(Vec3(moveDelta.x, gravity * _dt, moveDelta.y));

		camUpdate = true;
	}

	// update animations
	if (moveDelta.Length() > 0)
		instance.update("Run", _dt * (moveDelta.y < 0 ? -1 : 1) * moveSpeed / 10);
	else
		instance.update("Idle", _dt);

	
	if (win->inputs.KeyDown(VK_F)) { // 如果按下 F 键
		if (!togglePressed) { // 确保只触发一次切换
			camera->ToggleMode(); // 切换第一人称或第三人称模式

			// 获取当前相机模式并显示提示
			std::string mode = (camera->GetMode() == Camera::CameraMode::FirstPerson)
				? "First Person"
				: "Third Person";
			//MessageBoxA(NULL, mode.c_str(), "Camera Mode", MB_OK); // 显示切换后的模式
			togglePressed = true; // 标记 F 键已触发
		}
	}
	else {
		togglePressed = false; // 如果 F 键松开，允许再次切换
	}


	if (camUpdate)
	{
		if (camera->GetMode() == Camera::CameraMode::FirstPerson) {
			// First Person 
			camera->transform.position = transform.position + Vec3(0, 2, 0); // character head
			camera->transform.forward = transform.forward; // 
		}
		else {
			// update camera movement
			camera->transform.position = transform.position + Vec3(0, 4, 0) - camera->transform.forward * 20;
		}
		
		camera->transform.Update();
		// update camera matrices
		camera->UpdateMat();
	}

	// reset collision
	colliding = false;
}

void CharacterController::Draw() //draw character
{
	Matrix vp = camera->GetViewProjMat();

	ShaderManager::Set("TRex");
	ShaderManager::UpdateConstant(ShaderStage::VertexShader, "ConstBuffer", "W", &transform.worldMat);
	ShaderManager::UpdateConstant(ShaderStage::VertexShader, "ConstBuffer", "bones", &instance.matrices);

	ShaderManager::UpdateTexture(ShaderStage::PixelShader, "tex", TextureManager::find("T-rex_Base_Color.png"));
	ShaderManager::UpdateTexture(ShaderStage::PixelShader, "nor", TextureManager::find("T-rex_Normal_OpenGL.png"));
	ShaderManager::Apply();
	visuals.Draw(driver);
}

void CharacterController::OnCollision(const Collider& _other)
{
	colliding = true;
}

CharacterController::~CharacterController()
{
	Collisions::RemoveCollider(this);
}