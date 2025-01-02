#ifndef HM_RECIPE_PROGRAM_H
#define HM_RECIPE_PROGRAM_H

typedef struct {
  char name[25];
  char ingredients[45];
  char secret_ingredients[10];
  char buffer;
} Recipe; 

//helper methods 

void addExtension(char file[]);

void trimLeading(char str[]);

void padString(char str[], int arraySize);

//functionality methods

void accessFile();

void searchRecipe();

void searchIngredient();

void searchSecret();

void writeRecipe();

void storeRecipes();

int displayMenu();

#endif
