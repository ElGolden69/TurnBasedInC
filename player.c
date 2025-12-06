#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"

int isNameTaken(Player *head, const char *name) {
    Player *temp = head;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) return 1;
        temp = temp->next;
    }
    return 0;
}

Player* createPlayer(Player *head) {
    Player *p = (Player*)malloc(sizeof(Player));
    if (!p) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    while (1) {
        printf("Enter player name: ");
        scanf(" %[^\n]", p->name);

        if (p->name[0] == '\0') {
            printf("Name cannot be empty.\n");
            continue;
        }

        if (isNameTaken(head, p->name)) {
            printf("Name already taken. Choose a different name.\n");
        } else {
            break;
        }
    }

    p->level = 1;
    p->maxHP = 100;
    p->hp = 100;
    p->attack = 20;
    p->defense = 10;
    p->score = 0;
    p->magic = 20;
    p->maxMagic = 20;
    p->healPotions = 5;
    p->defensePotions = 5;
    p->attackPotions = 5;
    p->magicPotions = 5;
    p->next = NULL;

    printf("Player '%s' created successfully!\n", p->name);
    return p;
}


Player* choosePlayer(Player *head) {
    if (head == NULL) {
        printf("No players available. Create a player first.\n");
        return NULL;
    }

    Player *temp = head;
    int i = 1;

    printf("\n--- Choose a Player ---\n");
    while (temp != NULL) {
        printf("%d. %s (Level: %d, HP: %d)\n", i, temp->name, temp->level, temp->hp);
        temp = temp->next;
        i++;
    }

    int choice;
    printf("Enter the number of the player to start playing: ");
    scanf("%d", &choice);
    getchar();

    if (choice < 1 || choice >= i) {
        printf("Invalid choice.\n");
        return NULL;
    }

    temp = head;
    for (int j = 1; j < choice; j++) {
        temp = temp->next;
    }

    printf("You selected: %s\n", temp->name);
    return temp;
}

void updatePlayer(Player *head) {
    if (head == NULL) {
        printf("No players to update.\n");
        return;
    }

    Player *temp = head;
    int i = 1;

    printf("\n--- Players List ---\n");
    while (temp != NULL) {
        printf("%d. %s (Level: %d, HP: %d)\n", i, temp->name, temp->level, temp->hp);
        temp = temp->next;
        i++;
    }

    int choice;
    printf("Enter the number of the player you want to update: ");
    scanf("%d", &choice);
    getchar();

    if (choice < 1 || choice >= i) {
        printf("Invalid choice.\n");
        return;
    }


    temp = head;
    for (int j = 1; j < choice; j++) {
        temp = temp->next;
    }


    printf("Enter new name for '%s': ", temp->name);
    fgets(temp->name, 50, stdin);
    temp->name[strcspn(temp->name, "\n")] = '\0';

    printf("Player updated successfully!\n");
}


void displayPlayer(Player *p) {
    if (p == NULL) return;
    printf("Name: %s | Level: %d | HP: %d/%d | ATK: %d | DEF: %d | Score: %d\n",
           p->name, p->level, p->hp, p->maxHP, p->attack, p->defense, p->score);
}

