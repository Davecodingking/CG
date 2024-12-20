#include "Camera.h"
#include "Window.h"

Camera* Camera::instance = nullptr;


Camera::Camera(Vec2 _size, Vec3 _pos, Vec3 _rot, float _nearPlane, float _farPlane) :Behaviour(_pos, _rot)
{
	if (instance != nullptr)
		return;

	projMat = Matrix::Projection(45, _size.x / _size.y, _nearPlane, _farPlane);
	UpdateMat();
	rotSpeed = 40;
	moveSpeed = 20;
	instance = this;
}

Camera* Camera::GetInstance() { return instance; }

void Camera::UpdateMat()
{
	if (mode == CameraMode::FirstPerson)
		viewMat = Matrix::View(transform.position,transform.forward);
	else if (mode == CameraMode::ThirdPerson)
	{
		viewMat = Matrix::View(transform.worldMat);
	}
	viewProj = projMat * viewMat;
}
 
void Camera::Update(float _dt)
{
	Window* win = Window::GetInstance(); 

	if (!win->inputs.GetCursorLock())  // if the cursor is unlock then skip update.
		return;

	moveSpeed = clamp(moveSpeed + win->inputs.MouseWheel() * 20 * _dt, 1, 2000);

	Vec2 moveDelta = win->inputs.GetAxis() * moveSpeed * _dt; //WASD control
	Vec2 rotDelta = -win->inputs.MouseDelta() * rotSpeed * _dt; // cursor control rotation

	if (moveDelta.Length() > 0)
		TranslateRel(Vec3(moveDelta.x, 0, moveDelta.y)); //camera movement
	if (rotDelta.Length() > 0)
		Rotate(Vec3(rotDelta.y, rotDelta.x, 0) % 20.f); //rotation camera

	UpdateMat(); 
}

void Camera::ToggleMode() {
	mode = (mode == CameraMode::FirstPerson) ? CameraMode::ThirdPerson : CameraMode::FirstPerson;
}

Matrix Camera::GetViewProjMat() const { return viewProj; }

Camera::~Camera() { if (instance == this)instance = nullptr; }