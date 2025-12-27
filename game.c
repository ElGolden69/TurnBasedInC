#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "player.h"
#include "game.h"

#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define BLUE    "\x1b[34m"
#define sleep_ms(ms) Sleep(ms)

// ---------------- UTILITY FUNCTIONS ---------------- //

int rollChance(int percent) {
    return (rand() % 100) < percent;
}

void printHealthBar(int current, int max, const char *color) {
    int barLength = 20;
    int filled = (current * barLength) / max;

    printf("[");
    printf("%s", color);
    for (int i = 0; i < filled; i++) printf("#");
    printf(RESET);
    for (int i = filled; i < barLength; i++) printf("-");
    printf("] %d/%d\n", current, max);
}

void printEnemyASCII(const char *filename, int offset) {
    char path[256];
    snprintf(path, sizeof(path), "enemies/%s.txt", filename);
    FILE *file = fopen(path, "r");
    if (!file) {
        printf("Could not open ASCII file: %s\n", path);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        for (int i = 0; i < offset; i++) printf(" ");
        printf("%s", line);
    }
    fclose(file);
}

// ---------------- LEADERBOARD ---------------- //

int compareScores(const void *a, const void *b) {
    Player *p1 = *(Player **)a;
    Player *p2 = *(Player **)b;
    return p2->score - p1->score;  // descending order
}

void displayLeaderboard(Player *head) {
    system("cls");
    // Count players
    int count = 0;
    Player *temp = head;
    while (temp) {
        count++;
        temp = temp->next;
    }

    if (count == 0) {
        printf("\nNo players to show in leaderboard.\n");
        return;
    }

    // Put players into array for sorting
    Player **arr = malloc(sizeof(Player*) * count);
    temp = head;
    for (int i = 0; i < count; i++) {
        arr[i] = temp;
        temp = temp->next;
    }

    // Sort descending
    qsort(arr, count, sizeof(Player*), compareScores);

    // Display in a bordered box
    int boxWidth = 40;
    printf("\n");
    for (int i = 0; i < boxWidth; i++) printf("*");
    printf("\n");

    printf("*%*s*\n", boxWidth-1, " LEADERBOARD ");
    for (int i = 0; i < boxWidth; i++) printf("*");
    printf("\n");

    for (int i = 0; i < count; i++) {
        char line[100];
        snprintf(line, sizeof(line), "%d. %s - %d pts", i+1, arr[i]->name, arr[i]->score);
        int padding = (boxWidth - 2 - strlen(line)) / 2;
        printf("*%*s%s%*s*\n", padding, "", line, boxWidth - 2 - padding - strlen(line), "");
    }

    for (int i = 0; i < boxWidth; i++) printf("*");
    printf("\n\n");

    free(arr);
}


// ---------------- INVENTORY ---------------- //

int openInventory(Player *player) {
    int choice;
    int usedItem = 0;

    while (1) {
        printf("\n--- Inventory ---\n");
        printf("1. Healing Potion (%d left)\n", player->healPotions);
        printf("2. Attack Potion (%d left)\n", player->attackPotions);
        printf("3. Defense Potion (%d left)\n", player->defensePotions);
        printf("4. Magic Potion (%d left)\n", player->magicPotions);
        printf("5. Back\nChoose item: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 5) break;

        switch (choice) {
            case 1:
                if (player->healPotions > 0) {
                    player->healPotions--;
                    player->hp += 30;
                    if (player->hp > player->maxHP) player->hp = player->maxHP;
                    printf("You used a Healing Potion! HP restored to %d/%d.\n", player->hp, player->maxHP);
                    usedItem = 1;
                } else printf("No Healing Potions left!\n");
                break;

            case 2:
                if (player->attackPotions > 0) {
                    player->attackPotions--;
                    player->attack += 5;
                    printf("Your Attack increased to %d!\n", player->attack);
                    usedItem = 1;
                } else printf("No Attack Potions left!\n");
                break;

            case 3:
                if (player->defensePotions > 0) {
                    player->defensePotions--;
                    player->defense += 5;
                    printf("Your Defense increased to %d!\n", player->defense);
                    usedItem = 1;
                } else printf("No Defense Potions left!\n");
                break;

            case 4:
                if (player->magicPotions > 0) {
                    player->magicPotions--;
                    player->magic += 10;
                    if (player->magic > player->maxMagic) player->magic = player->maxMagic;
                    printf("You restored some MP! MP: %d/%d.\n", player->magic, player->maxMagic);
                    usedItem = 1;
                } else printf("No Magic Potions left!\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

        printf("\nPress Enter to continue...\n");
        getchar();

        if (usedItem) break;   // BREAK after using an item
    }

    return usedItem;
}


// ---------------- DISPLAY ---------------- //

void displayBattleScreen(Player *player, Enemy *enemy, int offset) {
    system("cls");
    printf("=== Battle ===\n");
    printf("Player: %s | HP: ", player->name);
    printHealthBar(player->hp, player->maxHP, GREEN);

    printf("MP: ");
    printHealthBar(player->magic, player->maxMagic, BLUE);

    printf("Enemy: %s | HP: ", enemy->name);
    printHealthBar(enemy->hp, enemy->maxHP, RED);
    printf("\n");

    printEnemyASCII(enemy->asciiFile, offset);
    printf("\n");
}

// ---------------- MERCY EVENT ---------------- //

void checkEnemyMercy(Player *player, Enemy *enemy) {
    if (enemy->hp <= enemy->maxHP / 5 && rollChance(30)) {
        printf("\nThe enemy is begging for mercy!\n");
        printf("1. Refuse\n2. Ask for a random item\n3. Ask for next enemy weakness\n");
        int mercyChoice;
        scanf("%d", &mercyChoice);
        getchar();

        switch (mercyChoice) {
            case 1:
                printf("\nYou refused mercy. The battle continues!\n");
                break;
            case 2: {
                int item = rand() % 4;
                switch (item) {
                    case 0: player->healPotions++; printf("\nYou got a Healing Potion!\n"); break;
                    case 1: player->defensePotions++; printf("\nYou got a Defense Potion!\n"); break;
                    case 2: player->attackPotions++; printf("\nYou got an Attack Potion!\n"); break;
                    case 3: player->magicPotions++; printf("\nYou got a Magic Potion!\n"); break;
                }
                break;
            }
            case 3:
                
            int weaknessChoice;
                printf("\nChoose which enemy weakness you want:\n");
                printf("1. Zombie\n2. Vampire\n3. Dreadlord\n");
                scanf("%d", &weaknessChoice);
                getchar();

            switch (weaknessChoice) {
                case 1:
                    printf("\nZombie weakness: Fire attacks deal extra damage.\n");break;
                case 2:
                    printf("\nVampire weakness: Holy magic and sunlight.\n");break;
                case 3:
                    printf("\nDreadlord weakness: Low resistance to magic.\n");break;
            }
                break;
             default:
                printf("\nInvalid choice. Battle continues.\n");
        }

        printf("\nPress Enter to continue...\n");
        getchar();
    }
}




// ---------------- PLAYER TURN ---------------- //

int playerTurn(Player *player, Enemy *enemy) {
    int choice;

    printf("Actions:\n1. Attack\n2. Magic\n3. Inventory\n4. Run\nChoose action: ");
    scanf("%d", &choice);
    getchar();

    if (choice == 1) {
        int damageToEnemy = player->attack - enemy->defense;
        if (damageToEnemy < 0) damageToEnemy = 0;

        if (rollChance(10)) {
            printf("\nYour attack missed!\n");

            for (int i = 0; i < 10; i++) {
                displayBattleScreen(player, enemy, i);
                printf("\nYour attack missed!\n");
                sleep_ms(50);
            }

            return 1;
        } else {
            if (rollChance(20)) {
                damageToEnemy *= 2;
                printf("Critical Hit!\n");
            }
            enemy->hp -= damageToEnemy;
            if (enemy->hp < 0) enemy->hp = 0;
            printf("%s attacks %s for %d damage!\n", player->name, enemy->name, damageToEnemy);
        }
        return 1;

    } else if (choice == 2) {
        int magicChoice;
        while (1) {
            printf("\n--- Magic Menu ---\n");
            printf("1. Fire Attack (5 MP)\n2. Water Attack (5 MP)\n3. Ice Attack (5 MP)\n4. Back\nChoose magic: ");
            scanf("%d", &magicChoice);
            getchar();

            if (magicChoice == 4) return 0;

            if (player->magic < 5) {
                printf("\nNot enough MP!\n");
                continue;
            }

            int damage = 0;
            switch (magicChoice) {
                case 1: damage = 25; break;
                case 2: damage = 20; break;
                case 3: damage = 15; break;
                default: printf("Invalid choice.\n"); continue;
            }

            enemy->hp -= damage;
            player->magic -= 5;
            if (enemy->hp < 0) enemy->hp = 0;
            printf("\n%s casts a spell! %s takes %d damage. (MP left: %d)\n",
                   player->name, enemy->name, damage, player->magic);
            return 1;
        }

    } else if (choice == 3) {
        int used = openInventory(player);

        if (used)
            return 1;
        else
            return 0;
    } else if (choice == 4) {
        printf("\nYou ran away!\n");
        exit(0);
    } else {
        printf("\nInvalid choice.\n");
        return 0;
    }
}

// ---------------- ENEMY TURN ---------------- //

void enemyTurn(Player *player, Enemy *enemy) {
    printf("\n--- Enemy Turn ---\n");

    int baseDamage = enemy->attack - (player->defense / 2);
    if (baseDamage < 1) baseDamage = 1;
    int variation = rand() % 5 - 2;
    int damageToPlayer = baseDamage + variation;
    if (damageToPlayer < 1) damageToPlayer = 1;

    if (rollChance(10)) {
        printf("\n%s's attack missed!\n", enemy->name);
    } else {
        if (rollChance(15)) {
            damageToPlayer *= 2;
            printf("Enemy Critical Hit!\n");
        }

        player->hp -= damageToPlayer;
        if (player->hp < 0) player->hp = 0;
        printf("%s attacks %s for %d damage!\n", enemy->name, player->name, damageToPlayer);
    }
}

void fightEnemy(Player *player, Enemy enemy) {
    printf("\n--- Battle Start ---\n");
    printf("%s vs %s\n\n", player->name, enemy.name);

    while (player->hp > 0 && enemy.hp > 0) {
        displayBattleScreen(player, &enemy, 0);
        checkEnemyMercy(player, &enemy);  // Just call it, don't stop battle

        int turnUsed = playerTurn(player, &enemy);
        if (turnUsed && enemy.hp > 0) {
            enemyTurn(player, &enemy);
        }

        printf("\nPress Enter to continue...\n");
        getchar();
    }

    if (player->hp <= 0)
        printf("\n%s has been defeated!\n", player->name);
    else {
        printf("\n%s is defeated!\n", enemy.name);

        // Add score depending on enemy type
        if (strcmp(enemy.name, "Vampire") == 0)
            player->score += 50;
        else if (strcmp(enemy.name, "Zombie") == 0)
            player->score += 75;
        else if (strcmp(enemy.name, "Dreadlord") == 0)
            player->score += 150;

        printf("%s earned points! Current score: %d\n", player->name, player->score);
    }

    printf("\n--- Battle End ---\n");
}





// ---------------- MAIN BATTLE FUNCTION ---------------- //

void battle(Player *player, Player *players) {
    srand(time(NULL));

    // FIRST ENEMY
    Enemy enemy1 = {"Vampire", 50, 10, 5, 50, "vampire"};
    fightEnemy(player, enemy1);

    if (player->hp <= 0) {
        printf("\nGame Over!\n");
        displayLeaderboard(players);  // show leaderboard
        return;   // Player lost, no 2nd battle
    }

    printf("\nYou defeated the Vampire!\n");
    printf("Prepare for the next fight!\n\n");
    printf("Press Enter to continue...\n");
    getchar();

    // SECOND ENEMY
    Enemy enemy2 = {"Zombie", 70, 12, 6, 70, "zombie"};
    fightEnemy(player, enemy2);

    if (player->hp <= 0) {
        printf("\nYou were slain by the Zombie...\n");
        displayLeaderboard(players);  // show leaderboard
        return;
    }

    printf("\nYou defeated the Zombie!\n");
    printf("Prepare for the FINAL BOSS!\n\n");
    printf("Press Enter to continue...\n");
    getchar();

    // FINAL BOSS
    Enemy finalBoss = {"Dreadlord", 120, 18, 8, 120, "last_boss"};
    fightEnemy(player, finalBoss);

    if (player->hp <= 0) {
        printf("\nYou were defeated by the Dreadlord...\n");
    } else {
        printf("\nCONGRATULATIONS! You defeated all enemies and the FINAL BOSS!\n");
    }

    // Show leaderboard after game ends
    displayLeaderboard(players);
}


