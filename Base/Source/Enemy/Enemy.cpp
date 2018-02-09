#include "Enemy.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../PlayerInfo/PlayerInfo.h"
#include "../Lua/LuaInterface.h"

CEnemy::CEnemy()
	: GenericEntity(nullptr)
	, defaultPosition(Vector3(0.0f,0.0f,0.0f))
	, defaultTarget(Vector3(0.0f, 0.0f, 0.0f))
	, defaultUp(Vector3(0.0f, 0.0f, 0.0f))
	, target(Vector3(0.0f, 0.0f, 0.0f))
	, up(Vector3(0.0f, 0.0f, 0.0f))
	, maxBoundary(Vector3(0.0f, 0.0f, 0.0f))
	, minBoundary(Vector3(0.0f, 0.0f, 0.0f))
	, m_pTerrain(nullptr)
{
}


CEnemy::~CEnemy()
{
}

void CEnemy::Init(void)
{
	string originalFile = CLuaInterface::GetInstance()->GetTheLuaStateCurrentFilename();
	CLuaInterface::GetInstance()->SetLuaFile("Image//GameSaveData.lua", CLuaInterface::GetInstance()->theLuaState);

	// Set the default values
	defaultPosition.Set(0, 0, 10);
	defaultTarget.Set(0, 0, 0);
	defaultUp.Set(0, 1, 0);

	// Set the current values
	position = CLuaInterface::GetInstance()->getVector3Values("EnemyPos");
	target.Set(0.0f, 0.0f, 0.0f);
	up.Set(0.0f, 1.0f, 0.0f);

	// Set Boundary
	maxBoundary.Set(1, 1, 1);
	minBoundary.Set(-1, -1, -1);

	// Set speed
	m_dSpeed = 10.0;

	// Initialise the LOD meshes
	InitLOD("HQAlien", "LQAlien", "cubeSG");

	// Initialise the Collider
	this->SetCollider(true);
	this->SetAABB(Vector3(1, 1, 1), Vector3(-1, -1, -1));

	// FSM, waypoints
	state = patrol;
	index = CLuaInterface::GetInstance()->getIntValue("NoWaypoints");
	for (int a = 0; a < index; ++a)
	{
		Vector3 point = CLuaInterface::GetInstance()->getVector3Values("Waypoint" + to_string(a));
		AddWaypoints(point.x, point.y, point.z);
	}
	index = CLuaInterface::GetInstance()->getIntValue("WaypointID");

	// Add to EntityManager
	EntityManager::GetInstance()->AddEntity(this, true);

	CLuaInterface::GetInstance()->SetLuaFile(originalFile, CLuaInterface::GetInstance()->theLuaState);
}

// Reset this player instance to default
void CEnemy::Reset(void)
{
	// Set the current values to default values
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

// Set position
void CEnemy::SetPos(const Vector3& pos)
{
	position = pos;
}

// Set target
void CEnemy::SetTarget(const Vector3& target)
{
	this->target = target;
}
// Set Up
void CEnemy::SetUp(const Vector3& up)
{
	this->up = up;
}
// Set the boundary for the player info
void CEnemy::SetBoundary(Vector3 max, Vector3 min)
{
	maxBoundary = max;
	minBoundary = min;
}
// Set the terrain for the player info
void CEnemy::SetTerrain(GroundEntity* m_pTerrain)
{
	if (m_pTerrain != nullptr)
	{
		this->m_pTerrain = m_pTerrain;

		SetBoundary(this->m_pTerrain->GetMaxBoundary(), this->m_pTerrain->GetMinBoundary());
	}
}

// Get position
Vector3 CEnemy::GetPos(void) const
{
	return position;
}

// Get target
Vector3 CEnemy::GetTarget(void) const
{
	return target;
}
// Get Up
Vector3 CEnemy::GetUp(void) const
{
	return up;
}
// Get the terrain for the player info
GroundEntity* CEnemy::GetTerrain(void)
{
	return m_pTerrain;
}

// Update
void CEnemy::Update(double dt)
{
	if (state == patrol)
	{
		if ((CPlayerInfo::GetInstance()->GetPos() - position).LengthSquared() < 300)
		{
			state = chase;
		}
		else if ((target - position).Length() < m_dSpeed * dt)
		{
			cout << index << "\n";
			if (++index == Waypoints.size())
				index = 0;
			target = Waypoints[index];
		}
	}
	else if (state == chase)
	{
		if ((CPlayerInfo::GetInstance()->GetPos() - position).LengthSquared() > 500)
		{
			for (int a = 0; a < Waypoints.size(); ++a)
			{
				if ((Waypoints[a] - position).LengthSquared() <= (Waypoints[index] - position).LengthSquared())
					index = a;
			}
			target = Waypoints[index];
			state = patrol;
		}
		else target = CPlayerInfo::GetInstance()->GetPos();
	}

	Vector3 viewVector = (target - position).Normalized();
	position += viewVector * (float)m_dSpeed * (float)dt;
	//cout << position << "..." << viewVector << endl;

	// Constrain the position
	Constrain();

	// Update the target
	if (position.z > 400.0f)
		target.z = position.z * -1;
	else if (position.z < -400.0f)
		target.z = position.z * -1;
}

// Constrain the position within the borders
void CEnemy::Constrain(void)
{
	// Constrain player within the boundary
	if (position.x > maxBoundary.x - 1.0f)
		position.x = maxBoundary.x - 1.0f;
	if (position.z > maxBoundary.z - 1.0f)
		position.z = maxBoundary.z - 1.0f;
	if (position.x < minBoundary.x + 1.0f)
		position.x = minBoundary.x + 1.0f;
	if (position.z < minBoundary.z + 1.0f)
		position.z = minBoundary.z + 1.0f;

	// if the y position is not equal to terrain height at that position, 
	// then update y position to the terrain height
	if (position.y != m_pTerrain->GetTerrainHeight(position))
		position.y = m_pTerrain->GetTerrainHeight(position);
}

// Render
void CEnemy::Render(void)
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	if (theDetailLevel == MID_DETAILS)
		modelStack.Rotate(Math::RadianToDegree(atan2(CPlayerInfo::GetInstance()->GetPos().x - position.x, CPlayerInfo::GetInstance()->GetPos().z - position.z)), 0, 1, 0);
	modelStack.Scale(scale.x, scale.y, scale.z);
	if (GetLODStatus() == true)
	{
		if (theDetailLevel != NO_DETAILS)
		{
			//cout << theDetailLevel << endl;
			RenderHelper::RenderMesh(GetLODMesh());
		}
	}
	modelStack.PopMatrix();
}

void CEnemy::AddWaypoints(float x, float y, float z)
{
	Waypoints.push_back(Vector3(x, y, z));
	if (Waypoints.size() == 1)
		target = Waypoints[0];
}

void CEnemy::Save()
{
	string WaypointList = "Waypoint";
	CLuaInterface::GetInstance()->saveVector3Value("EnemyPos", position, "SaveToGameFile");
	CLuaInterface::GetInstance()->saveIntValue("WaypointID", index, "SaveToGameFile");
	CLuaInterface::GetInstance()->saveIntValue("NoWaypoints", Waypoints.size(), "SaveToGameFile");
	for (int a = 0; a < Waypoints.size(); ++a)
		CLuaInterface::GetInstance()->saveVector3Value("Waypoint" + to_string(a), Waypoints[a], "SaveToGameFile");
}

void CEnemy::Load()
{

}