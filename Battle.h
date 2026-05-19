////---------------------------------------------- ( Battle.h ) ------------------------------------------------------
//#pragma once
//#include "Veilwalker.h"
//#include "Spirit.h"
//#include "GameWindow.h"
//#include "AudioManager.h"
//
//enum class BattleResult { PlayerWon, PlayerExited, PlayerDied };
//
//class Battle
//{
//private:
////AGGREGATION:- 
//GameWindow& gw;
//Veilwalker& player;
//Spirit& enemy;
//AudioManager& audio;
////.............
//sf::Font font;
//sf::Sprite* bgSprite;
////PLAYER PHYSICS:-
//sf::Vector2f playerPos;
//float playerSpeed = 300.f;
//bool playerJumping = false;
//float playerVelY = 0.f;
//const float  gravity = 600.f;
//const float  groundY = 480.f;   // floor level (pixels from top)..
//// ENEMY AI:-
//sf::Vector2f enemyPos;
//float enemySpeed = 200.f;
//bool enemyAttacking = false;
//sf::Clock enemyAttackClock;
//float enemyAttackCooldown = 0.5f;
//float enemyAttackDuration = 1.5f;
//bool enemyFacingLeft = true;
//bool bossPhaseTriggered = false;
//
////ACTION MENU:-
//int selectedAction = 0;   
//// 0:Attack 1:Ability 2:Item 3:Exit
//sf::RectangleShape menuButtons[4];
//sf::Text buttonLabels[4];
//// Sub-menu for ability selection...
//bool showAbilityMenu = false;
//int selectedAbilityIndex = 0;
//std::vector<sf::Text> abilityTexts;
//
////Player Attack Cooldown ..
//sf::Clock playerAttackClock;
//float playerAttackCooldown = 0.5f;
//bool playerCanAct = true;// Set to false when player chooses Exit; returns PlayerExited next frame...
//
////BATTLE LOG:-
//std::vector<std::string> battleLog;
//
////INTERNAL HELPERS:-
//void addLog(const std::string& msg);
//void handleInput(float dt);
//void handlePlayerAttack();
//void handlePlayerAbility();
//void handlePlayerItem();
//void handleExit();
//void updatePhysics(float dt);
//void updateEnemyAI(float dt);
//void updateAnimations(float dt);
//void applyDamageToPlayer(int dmg);
//void applyDamageToEnemy(int dmg);
//void drawHUD();
//void drawGradientBar(float x, float y, float w, float h,int current, int max,sf::Color left, sf::Color right);
//// Action menu at the bottom, left to right...
//void drawActionMenu();
//void drawAbilityMenu();
//void drawBattleLog();
//void drawPlayerSprite();
//void drawEnemySprite();
//public:
//Battle(GameWindow& window, Veilwalker& player, Spirit& enemy,AudioManager& audioRef, sf::Sprite* bgSpr = nullptr);
//BattleResult run();
//};




//---------------------------------------------- ( Battle.h ) ------------------------------------------------------
#pragma once
#include "Veilwalker.h"
#include "Spirit.h"
#include "GameWindow.h"
#include "AudioManager.h"

enum class BattleResult { PlayerWon, PlayerExited, PlayerDied };

class Battle
{
private:
	//AGGREGATION:- 
	GameWindow& gw;
	Veilwalker& player;
	Spirit& enemy;
	AudioManager& audio;
	//.............
	sf::Font font;
	sf::Sprite* bgSprite;

	// Battle frame UI overlay
	sf::Texture              battleFrameTex;
	std::optional<sf::Sprite> battleFrameSpr;

	//PLAYER PHYSICS:-
	sf::Vector2f playerPos;
	float playerSpeed = 300.f;
	bool playerJumping = false;
	float playerVelY = 0.f;
	const float  gravity = 600.f;
	const float  groundY = 480.f;   // floor level (pixels from top)..
	// ENEMY AI:-
	sf::Vector2f enemyPos;
	float enemySpeed = 200.f;
	bool enemyAttacking = false;
	sf::Clock enemyAttackClock;
	float enemyAttackCooldown = 0.5f;
	float enemyAttackDuration = 1.5f;
	bool enemyFacingLeft = true;
	bool bossPhaseTriggered = false;

	//ACTION MENU:-
	int selectedAction = 0;
	// 0:Attack 1:Ability 2:Item 3:Exit
	sf::RectangleShape menuButtons[4];
	sf::Text buttonLabels[4];
	// Sub-menu for ability selection...
	bool showAbilityMenu = false;
	int selectedAbilityIndex = 0;
	std::vector<sf::Text> abilityTexts;

	//Player Attack Cooldown ..
	sf::Clock playerAttackClock;
	float playerAttackCooldown = 0.5f;
	bool playerCanAct = true;// Set to false when player chooses Exit; returns PlayerExited next frame...

	//BATTLE LOG:-
	std::vector<std::string> battleLog;

	//INTERNAL HELPERS:-
	void addLog(const std::string& msg);
	void handleInput(float dt);
	void handlePlayerAttack();
	void handlePlayerAbility();
	void handlePlayerItem();
	void handleExit();
	void updatePhysics(float dt);
	void updateEnemyAI(float dt);
	void updateAnimations(float dt);
	void applyDamageToPlayer(int dmg);
	void applyDamageToEnemy(int dmg);
	void drawHUD();
	void drawGradientBar(float x, float y, float w, float h, int current, int max, sf::Color left, sf::Color right);
	// Action menu at the bottom, left to right...
	void drawActionMenu();
	void drawAbilityMenu();
	void drawBattleLog();
	void drawPlayerSprite();
	void drawEnemySprite();
public:
	Battle(GameWindow& window, Veilwalker& player, Spirit& enemy, AudioManager& audioRef, sf::Sprite* bgSpr = nullptr);
	BattleResult run();
};