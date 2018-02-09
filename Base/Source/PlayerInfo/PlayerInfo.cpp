#include "PlayerInfo.h"
#include <iostream>

#include "MouseController.h"
#include "KeyboardController.h"
#include "Mtx44.h"
#include "../Projectile/Projectile.h"
#include "../WeaponInfo/Pistol.h"
#include "../WeaponInfo/LaserBlaster.h"
#include "../WeaponInfo/GrenadeThrow.h"

// Allocating and initializing CPlayerInfo's static data member.  
// The pointer is allocated but not the object's constructor.
CPlayerInfo *CPlayerInfo::s_instance = 0;

CPlayerInfo::CPlayerInfo(void)
	: m_dSpeed(40.0)
	, m_dAcceleration(10.0)
	, m_bJumpUpwards(false)
	, m_dJumpSpeed(10.0)
	, m_dJumpAcceleration(-10.0)
	, m_bFallDownwards(false)
	, m_dFallSpeed(0.0)
	, m_dFallAcceleration(-10.0)
	, attachedCamera(nullptr)
	, m_pTerrain(nullptr)
	, primaryWeapon(nullptr)
	, secondaryWeapon(nullptr)
	, currentWeapon(InventoryWeapons::PISTOL)
{
	for (unsigned short i = 0; i < InventoryWeapons::WEAPON_INVENTORY_TOTAL; i++)
	{
		weaponInventory[i] = nullptr;
	}

	//Default keys to move player
	keyMoveForward = 'W';
	keyMoveBackward = 'S';
	keyMoveLeft = 'A';
	keyMoveRight = 'D';
}

CPlayerInfo::~CPlayerInfo(void)
{
	if (secondaryWeapon)
	{
		delete secondaryWeapon;
		secondaryWeapon = nullptr;
	}
	if (primaryWeapon)
	{
		delete primaryWeapon;
		primaryWeapon = nullptr;
	}

	for (unsigned short i = 0; i < InventoryWeapons::WEAPON_INVENTORY_TOTAL; i++)
	{
		if (weaponInventory[i])
		{
			delete weaponInventory[i];
			weaponInventory[i] = nullptr;
		}
	}

	m_pTerrain = nullptr;
}

// Initialise this class instance
void CPlayerInfo::Init(void)
{
	// Set the default values
	defaultPosition.Set(0,0,10);
	defaultTarget.Set(0,0,0);
	defaultUp.Set(0,1,0);

	// Set the current values
	position = CLuaInterface::GetInstance()->getVector3Values("PlayerPos");
	target.Set(0, 0, 0);
	pitch = yaw = 0;
	up.Set(0, 1, 0);

	// Set Boundary
	maxBoundary.Set(1,1,1);
	minBoundary.Set(-1, -1, -1);

	// Set the pistol as the primary weapon
	/*primaryWeapon = new CPistol();
	primaryWeapon->Init();*/
	// Set the laser blaster as the secondary weapon
	//secondaryWeapon = new CLaserBlaster();
	//secondaryWeapon->Init();
	/*secondaryWeapon = new CGrenadeThrow();
	secondaryWeapon->Init();*/

	weaponInventory[InventoryWeapons::PISTOL] = new CPistol();
	weaponInventory[InventoryWeapons::PISTOL]->Init();

	weaponInventory[InventoryWeapons::LASER] = new CLaserBlaster();
	weaponInventory[InventoryWeapons::LASER]->Init();

	weaponInventory[InventoryWeapons::GRENADE] = new CGrenadeThrow();
	weaponInventory[InventoryWeapons::GRENADE]->Init();
}

void CPlayerInfo::InitKeyInputs(void)
{
	//init the custom keyboard inputs from lua
	keyMoveForward = CLuaInterface::GetInstance()->getCharValue("moveForward");
	keyMoveBackward = CLuaInterface::GetInstance()->getCharValue("moveBackward");
	keyMoveLeft = CLuaInterface::GetInstance()->getCharValue("moveLeft");
	keyMoveRight = CLuaInterface::GetInstance()->getCharValue("moveRight");
}

// Returns true if the player is on ground
bool CPlayerInfo::isOnGround(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is jumping upwards
bool CPlayerInfo::isJumpUpwards(void)
{
	if (m_bJumpUpwards == true && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is on freefall
bool CPlayerInfo::isFreeFall(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == true)
		return true;

	return false;
}

// Set the player's status to free fall mode
void CPlayerInfo::SetOnFreeFall(bool isOnFreeFall)
{
	if (isOnFreeFall == true)
	{
		m_bJumpUpwards = false;
		m_bFallDownwards = true;
		m_dFallSpeed = 0.0;
	}
}

// Set the player to jumping upwards
void CPlayerInfo::SetToJumpUpwards(bool isOnJumpUpwards)
{
	if (isOnJumpUpwards == true)
	{
		m_bJumpUpwards = true;
		m_bFallDownwards = false;
		m_dJumpSpeed = 4.0;
	}
}

// Set position
void CPlayerInfo::SetPos(const Vector3& pos)
{
	position = pos;
}

// Set target
void CPlayerInfo::SetTarget(const Vector3& target)
{
	this->target = target;
}

// Set position
void CPlayerInfo::SetUp(const Vector3& up)
{
	this->up = up;
}

// Set m_dJumpAcceleration of the player
void CPlayerInfo::SetJumpAcceleration(const double m_dJumpAcceleration)
{
	this->m_dJumpAcceleration = m_dJumpAcceleration;
}

// Set Fall Acceleration of the player
void CPlayerInfo::SetFallAcceleration(const double m_dFallAcceleration)
{
	this->m_dFallAcceleration = m_dFallAcceleration;
}

// Set the boundary for the player info
void CPlayerInfo::SetBoundary(Vector3 max, Vector3 min)
{
	maxBoundary = max;
	minBoundary = min;
}

// Set the terrain for the player info
void CPlayerInfo::SetTerrain(GroundEntity* m_pTerrain)
{
	if (m_pTerrain != nullptr)
	{
		this->m_pTerrain = m_pTerrain;

		SetBoundary(this->m_pTerrain->GetMaxBoundary(), this->m_pTerrain->GetMinBoundary());
	}
}

// Stop the player's movement
void CPlayerInfo::StopVerticalMovement(void)
{
	m_bJumpUpwards = false;
	m_bFallDownwards = false;
}

// Reset this player instance to default
void CPlayerInfo::Reset(void)
{
	// Set the current values to default values
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;

	// Stop vertical movement too
	StopVerticalMovement();
}

// Get position x of the player
Vector3 CPlayerInfo::GetPos(void) const
{
	return position;
}

// Get target
Vector3 CPlayerInfo::GetTarget(void) const
{
	return target;
}
// Get Up
Vector3 CPlayerInfo::GetUp(void) const
{
	return up;
}

// Get m_dJumpAcceleration of the player
double CPlayerInfo::GetJumpAcceleration(void) const
{
	return m_dJumpAcceleration;
}

// Get the terrain for the player info
GroundEntity* CPlayerInfo::GetTerrain(void)
{
	return m_pTerrain;
}

// Update Jump Upwards
void CPlayerInfo::UpdateJumpUpwards(double dt)
{
	if (m_bJumpUpwards == false)
		return;

	// Update position and target y values
	// Use SUVAT equation to update the change in position and target
	// s = u * t + 0.5 * a * t ^ 2
	position.y += (float)(m_dJumpSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	target.y += (float)(m_dJumpSpeed*dt + 0.5 * m_dJumpAcceleration * dt * dt);
	// Use this equation to calculate final velocity, v
	// SUVAT: v = u + a * t;
	// v is m_dJumpSpeed AFTER updating using SUVAT where u is the initial speed and is equal to m_dJumpSpeed
	m_dJumpSpeed = m_dJumpSpeed + m_dJumpAcceleration * dt;
	// Check if the jump speed is less than zero, then it should be falling
	if (m_dJumpSpeed < 0.0)
	{
		m_dJumpSpeed = 0.0;
		m_bJumpUpwards = false;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = true;
	}
}

// Update FreeFall
void CPlayerInfo::UpdateFreeFall(double dt)
{
	if (m_bFallDownwards == false)
		return;

	// Update position and target y values
	// Use SUVAT equation to update the change in position and target
	// s = u * t + 0.5 * a * t ^ 2
	position.y += (float)(m_dFallSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	target.y += (float)(m_dFallSpeed * dt + 0.5 * m_dJumpAcceleration * dt * dt);
	// Use this equation to calculate final velocity, v
	// SUVAT: v = u + a * t;
	// v is m_dJumpSpeed AFTER updating using SUVAT where u is the initial speed and is equal to m_dJumpSpeed
	m_dFallSpeed = m_dFallSpeed + m_dFallAcceleration * dt;
	// Check if the jump speed is below terrain, then it should be reset to terrain height
	if (position.y < m_pTerrain->GetTerrainHeight(position))
	{
		Vector3 viewDirection = target - position;
		position.y = m_pTerrain->GetTerrainHeight(position);
		target = position + viewDirection;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = false;
	}
}

/********************************************************************************
 Hero Update
 ********************************************************************************/
void CPlayerInfo::Update(double dt)
{
	Vector3 viewUV = target - position;
	double mouse_diff_x, mouse_diff_y;

	if (KeyboardController::GetInstance()->IsKeyDown(VK_LEFT))
		yaw += 0.001;
	if (KeyboardController::GetInstance()->IsKeyDown(VK_RIGHT))
		yaw -= 0.001;
	if (KeyboardController::GetInstance()->IsKeyDown(VK_UP))
		pitch += 0.001;
	if (KeyboardController::GetInstance()->IsKeyDown(VK_DOWN))
		pitch -= 0.001;

	MouseController::GetInstance()->GetMouseDelta(mouse_diff_x, mouse_diff_y);

	yaw += mouse_diff_x * 0.001;		// 3.142 / 180.0
	pitch -= mouse_diff_y * 0.001;	// 3.142 / 180.0

	if (pitch > Math::HALF_PI)
		pitch = Math::HALF_PI;
	else if (pitch < -Math::HALF_PI)
		pitch = -Math::HALF_PI;

	// Update the position if the WASD buttons were activated
	if (KeyboardController::GetInstance()->IsKeyDown(keyMoveForward))
		position += Vector3(cos(yaw), 0, sin(yaw)) * (float)m_dSpeed * (float)dt;
	if (KeyboardController::GetInstance()->IsKeyDown(keyMoveBackward))
		position -= Vector3(cos(yaw), 0, sin(yaw)) * (float)m_dSpeed * (float)dt;
	if (KeyboardController::GetInstance()->IsKeyDown(keyMoveLeft))
		position -= Vector3(-sin(yaw), 0, cos(yaw)) * (float)m_dSpeed * (float)dt;
	if (KeyboardController::GetInstance()->IsKeyDown(keyMoveRight))
		position += Vector3(-sin(yaw), 0, cos(yaw)) * (float)m_dSpeed * (float)dt;

	// Constrain the position
	Constrain();
	// Update the target
	target = position + viewUV;

	//Update the camera direction based on mouse move
	{
		viewUV.x = cos(yaw) * cos(pitch);
		viewUV.y = sin(pitch);
		viewUV.z = sin(yaw) * cos(pitch);
		up = Vector3(-sin(yaw), 0, cos(yaw)).Cross(viewUV).Normalized();
		target = position + viewUV;
	}

	// If the user presses SPACEBAR, then make him jump
	if (KeyboardController::GetInstance()->IsKeyDown(VK_SPACE) &&
		position.y == m_pTerrain->GetTerrainHeight(position))
	{
		SetToJumpUpwards(true);
	}

	// Update the weapons
	if (KeyboardController::GetInstance()->IsKeyReleased('R'))
	{
		//if (primaryWeapon)
		//{
		//	primaryWeapon->Reload();
		//	//primaryWeapon->PrintSelf();
		//}
		//if (secondaryWeapon)
		//{
		//	secondaryWeapon->Reload();
		//	//secondaryWeapon->PrintSelf();
		//}

		if (weaponInventory[currentWeapon])
		{
			weaponInventory[currentWeapon]->Reload();
		}
	}
	if (weaponInventory[currentWeapon])
		weaponInventory[currentWeapon]->Update(dt);
	/*if (secondaryWeapon)
		secondaryWeapon->Update(dt);*/

	// if Mouse Buttons were activated, then act on them
	if (MouseController::GetInstance()->IsButtonPressed(MouseController::LMB))
	{
		if (weaponInventory[currentWeapon])
			weaponInventory[currentWeapon]->Discharge(position, target, this);
	}/*
	else if (MouseController::GetInstance()->IsButtonPressed(MouseController::RMB))
	{
		if (secondaryWeapon)
			secondaryWeapon->Discharge(position, target, this);
	}*/

	// If the user presses R key, then reset the view to default values
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
	{
		Reset();
	}
	else
	{
		UpdateJumpUpwards(dt);
		UpdateFreeFall(dt);
	}

	// If a camera is attached to this playerInfo class, then update it
	if (attachedCamera)
	{
		attachedCamera->SetCameraPos(position);
		attachedCamera->SetCameraTarget(target);
		attachedCamera->SetCameraUp(up);
	}
}

// Constrain the position within the borders
void CPlayerInfo::Constrain(void)
{
	// Constrain player within the boundary
	if (position.x > maxBoundary.x - 1.0f)
		position.x = maxBoundary.x - 1.0f;
	//if (position.y > maxBoundary.y - 1.0f)
	//	position.y = maxBoundary.y - 1.0f;
	if (position.z > maxBoundary.z - 1.0f)
		position.z = maxBoundary.z - 1.0f;
	if (position.x < minBoundary.x + 1.0f)
		position.x = minBoundary.x + 1.0f;
	//if (position.y < minBoundary.y + 1.0f)
	//	position.y = minBoundary.y + 1.0f;
	if (position.z < minBoundary.z + 1.0f)
		position.z = minBoundary.z + 1.0f;

	// if the player is not jumping nor falling, then adjust his y position
	if ((m_bJumpUpwards == false) && (m_bFallDownwards == false))
	{
		// if the y position is not equal to terrain height at that position, 
		// then update y position to the terrain height
		if (position.y != m_pTerrain->GetTerrainHeight(position))
			position.y = m_pTerrain->GetTerrainHeight(position);
	}
}

void CPlayerInfo::AttachCamera(FPSCamera* _cameraPtr)
{
	attachedCamera = _cameraPtr;
}

void CPlayerInfo::DetachCamera()
{
	attachedCamera = nullptr;
}
