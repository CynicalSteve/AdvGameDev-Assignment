#ifndef ENEMY_H
#define ENEMY_H
#include "MemoryLeakCheck.h"
#include "Physics.h"
#include "MyMath.h"

class Enemy
{
public:
    Enemy(unsigned int id, Vector3* pos = new Vector3(0.f, 0.f, 0.f),
        unsigned int maxHP = 100, unsigned int dmg = 2, float spd = 5.f, float atkSpd = 0.75f,
        float atkRange = .8f, float range = 10.f);
    ~Enemy();

    void Init();
    void Update(double dt, Vector3 playerPos);

    void SetStartingPos(Vector3 pos);
    void SetStartingPos(float x, float y, float z);
    void AddWaypoints(Vector3 pos);
    void AddWaypoints(float x, float y, float z);

    bool Attacking();
    void Attacking(bool in);
    bool isActive();
    std::string CurrentHealth();
    int getHealth();
    float getAngle();
    float getDistance(Vector3 playerPos);
    Vector3* GrabPosition();
    Vector3 getPosition();
    Hitbox** getHitBox();

    int attack();
    void hitEnemy(int dmg = 10);
    void reviveEnemy();

    bool debug;

private:
    const unsigned int EnemyID;
    const unsigned int EnemyDmg;
    const int EnemyMaxHP;
    const float EnemySpd;
    const float EnemyAtkSpd;
    const float AtkRange;
    const float DetectRange;
    const float hitboxSizeX, hitboxSizeY, hitboxSizeZ;
    std::vector<Vector3> waypoints;
    Vector3* position;
    Vector3 aim;
    bool isAttacking;
    bool active;
    unsigned int wpIndx;
    int enemyCurrentHP;
    float atkTimer;
    float atkLastTimer;
	Hitbox** hitbox;

    bool playerDetection(Vector3 playerPos);
    bool isWithinRange(Vector3 playerPos);
};

#endif