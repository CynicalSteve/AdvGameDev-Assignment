#include "Enemy.h"
#include <string>

// id: Enemy ID || maxHP: Enemy Max HP || dmg: Enemy Damage || spd: Enemy Movement Speed
Enemy::Enemy(unsigned int id, Vector3* pos, unsigned int maxHP, unsigned int dmg, float spd, float atkSpd, float atkRange, float range)
: EnemyID(id), position(pos), EnemyMaxHP(maxHP), EnemyDmg(dmg), EnemySpd(spd), EnemyAtkSpd(atkSpd), AtkRange(atkRange), DetectRange(range),
hitboxSizeX(1.f), hitboxSizeY(3.5f), hitboxSizeZ(1.f), hitbox(0)
{
    Init();
}

Enemy::~Enemy()
{
    delete position;

	for (int k = 0; k < 12; k++)
		delete hitbox[k];
	delete[] hitbox;
}

void Enemy::Init()
{
    debug = false;

    active = true;
    enemyCurrentHP = EnemyMaxHP;
    wpIndx = 1;
    waypoints.push_back(*position);
    aim = Vector3(1.f, 0.f, 0.f);
    atkTimer = 0;
    atkLastTimer = 0;

    !hitbox ? (hitbox = Hitbox::CuboidHitbox(*position, hitboxSizeX, -hitboxSizeX, 0.f, hitboxSizeY, -hitboxSizeZ, hitboxSizeZ)) :
        Hitbox::SetHitboxPos(hitbox, *position, hitboxSizeX, 0.f, -hitboxSizeZ);
}

void Enemy::Update(double dt, Vector3 playerPos)
{
    atkTimer += (float)dt;

    if (!active || debug)
        return;

    if (playerDetection(playerPos) || enemyCurrentHP < EnemyMaxHP)
        aim = (playerPos - *position).Normalized();
    else
    {
        if (waypoints.size() < 2)
        {
            if (position->x > waypoints[0].x - .5f && position->x < waypoints[0].x + .5f &&
                position->z > waypoints[0].z - .5f && position->z < waypoints[0].z + .5f)
                return;

            aim = (waypoints[0] - playerPos).Normalized();
        }
        else
        {
            if (position->x > waypoints[wpIndx].x - .5f && position->x < waypoints[wpIndx].x + .5f &&
                position->z > waypoints[wpIndx].z - .5f && position->z < waypoints[wpIndx].z + .5f)
            {
                unsigned int prev = wpIndx;
                wpIndx++;

                if (wpIndx == waypoints.size())
                    wpIndx = 0;

                aim = (waypoints[wpIndx] - waypoints[prev]).Normalized();
            }
            else
                aim = (waypoints[wpIndx] - *position).Normalized();
        }
    }

    aim.y = 0;

    if (isWithinRange(playerPos))
    {
        if (atkLastTimer + EnemyAtkSpd < atkTimer)
            isAttacking = true;
    }
	else
    {
        Vector3 move = aim * EnemySpd * (float)dt;
        *position += move;

		//move hitbox
		for (int k = 0; k < 12; k++)
		{
			hitbox[k]->a += move;
            hitbox[k]->b += move;
            hitbox[k]->c += move;
		}
    }
}

// Change Starting Position
void Enemy::SetStartingPos(Vector3 pos)
{
    waypoints[0] = pos;
	Hitbox::SetHitboxPos(hitbox, pos, hitboxSizeX, 0.f, -hitboxSizeZ);
    *position = pos;

    if (waypoints.size() > 1)
        aim = (waypoints[1] - waypoints[0]).Normalized();
}

void Enemy::SetStartingPos(float x, float y, float z) { SetStartingPos(Vector3(x, y, z)); }

// Add Patrol Nodes
void Enemy::AddWaypoints(Vector3 pos)
{
    if (waypoints.size() == 1)
    {
        waypoints.push_back(pos);
        aim = (waypoints[1] - waypoints[0]).Normalized();
    }
    else
        waypoints.push_back(pos);
}

void Enemy::AddWaypoints(float x, float y, float z) { AddWaypoints(Vector3(x, y, z)); }

bool Enemy::Attacking() { return isAttacking; }

void Enemy::Attacking(bool in) { isAttacking = in; }

// Checks if enemy is active
bool Enemy::isActive() { return active; }

std::string Enemy::CurrentHealth() { return std::to_string(enemyCurrentHP); }
int Enemy::getHealth() { return enemyCurrentHP; }

// Get enemy's Forward Direction
float Enemy::getAngle()
{
    if (!active)
        return 0;

    float result = Physics::angleBetween(Vector3(1.f, 0.f, 0.f), aim);
    float checkResult = Physics::angleBetween(Vector3(0.f, 0.f, 1.f), aim);

    if (checkResult < 90.f)
        return (360 - result);
    
    return result;
}

float Enemy::getDistance(Vector3 playerPos) { return (playerPos - *position).Length(); }

// Get enemy's Current Position
Vector3* Enemy::GrabPosition() { return position; }
Vector3 Enemy::getPosition() { return *position; }

Hitbox** Enemy::getHitBox() { return hitbox; }

int Enemy::attack()
{
    if (atkLastTimer + EnemyAtkSpd < atkTimer)
    {
        atkLastTimer = atkTimer;
        return EnemyDmg;
    }
    else
        return 0;
}

void Enemy::hitEnemy(int dmg)
{
    if (!active || dmg == 0)
        return;

    enemyCurrentHP -= dmg;

    if (enemyCurrentHP <= 0)
    {
        active = false;
        waypoints.clear();
    }
}

void Enemy::reviveEnemy()
{
    if (active)
        return;

    Init();
}

bool Enemy::playerDetection(Vector3 playerPos)
{
    if (!active)
        return 0;

    if ((playerPos - *position).Length() * 0.2f <= DetectRange)
        return true;

    return 0;
}

bool Enemy::isWithinRange(Vector3 playerPos)
{
    if (!active)
        return 0;

    if ((playerPos - *position).Length() * 0.2f <= AtkRange)
        return true;

    return 0;
}