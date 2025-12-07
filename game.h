#ifndef GAME_H
#define GAME_H

#include "player.h"

typedef struct Enemy {
    char name[50];
    int hp;
    int attack;
    int defense;
    int maxHP;
    char asciiFile[100];
} Enemy;


int rollChance(int percent);
void printHealthBar(int current, int max, const char *color);
void printEnemyASCII(const char *filename, int offset);
int openInventory(Player *player);
void displayBattleScreen(Player *player, Enemy *enemy, int offset);
void checkEnemyMercy(Player *player, Enemy *enemy);
int playerTurn(Player *player, Enemy *enemy, int *critHits, int *usedPotion);
void enemyTurn(Player *player, Enemy *enemy, int *enemyMisses);
void fightEnemy(Player *player, Enemy enemy);
void battle(Player *player, Player *players);
// Leaderboard
void displayLeaderboard(Player *head);


#endif
