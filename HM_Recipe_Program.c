/******************************************************************/
/* This program serves as a utility to read, create and otherwise */
/* manage files following a certain format used for organizing    */
/* recipes (ie. a file management system)                         */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "HM_Recipe_Program.h"

//global variable should remain consistent for all functions and should not
//be changed after initialization
char fileName[100];

//function to get the file to be used from the user
void setFile(){
  char input[100];
  printf("Which file would you like to use (include the file extension): ");
  scanf("%99s", input);
  while(getchar() != '\n');
  //check if file exists
  if(access(input, F_OK) != 0){
    printf("File does not exist\n");
    exit(1);
  }
  strcpy(fileName, input);
}

//this function adds a .txt extension to a string
void addExtension(char file[]){
  int length = strlen(file);
  file[length] = '.';
  file[length + 1] = 't';
  file[length + 2] = 'x';
  file[length + 3] = 't';
  file[length + 4] = '\0';
}

//trims leading spaces
void trimLeading(char str[]){
  int index = 0;
  while(str[index] == ' '){
    index++; 
  }
  if(index == 0) return;
 
  int i = 0; 
  while(str[i + index] != '\0'){
    str[i] = str[i + index];
    i++;
  }
  str[i] = '\0';
}

//pads a string to the given length to keep string comparison and file format consistent
void padString(char str[], int arraySize){
  int length = strlen(str);
  for(int i = length; i < arraySize; i++){
    str[i] = ' ';
  }
  str[arraySize] = '\0';
}

/* function used for testing, not part of assignment spec
void readFile(){
  FILE *fd;
  char fileName[] = "recipes.txt";
  fd = fopen(fileName, "r");
  char name[26];
  char ingredients[46];
  char secret[11];

  while(fscanf(fd, "%25c", name) > 0){
    fscanf(fd, "%45c", ingredients);
    fscanf(fd, "%10c\n", secret);
    name[25] = '\0';
    ingredients[45] = '\0';
    secret[10] = '\0';
    printf("%s ", name);
    printf("%s ", ingredients);
    printf("%s\n", secret);
  }
  fclose(fd);
}*/

//Prints all recipes alongside their corresponding IDs
void accessFile(){
  FILE *fd;
  fd = fopen(fileName, "r");
  char recipe[25];
  char buffer[100];
  printf("All Recipes\n");

  //skips header line 
  fgets(buffer, 100, fd);
  int id = 1;
  //scans recipe until end of file
  while(fscanf(fd, "%25c", recipe) > 0){
    //gets rid of rest of line
    fgets(buffer, 100, fd);
    //adds the null terminating character so it can be printed
    recipe[25] = '\0';
    printf("ID: %d  Name: %s\n", id, recipe);
    id++;
  }
  fclose(fd);
}

//Searches for a recipe in the file by its name
void searchRecipe(){
  FILE *fd;
  fd = fopen(fileName, "r");
  char recipe[25];
  //gets user input for recipe
  printf("Enter name of Recipe: ");
  scanf("%24[^\n]", recipe);
  padString(recipe, 24);

  char search[25];
  int isFound = 0;
  //scans recipe until end of file
  while(fscanf(fd, "%25c", search) > 0){
    search[24] = '\0';
    //checks if input string and scanned string are equal
    if(strcmp(recipe, search) == 0){
      char ingredients[45];
      char secret[10];
      //if found, read the ingredient and secret ingredients
      fscanf(fd, "%45c", ingredients);
      fscanf(fd, "%10c\n", secret);
      //add null terminators
      ingredients[44] = '\0';
      secret[9] = '\0';
      //output to user before exiting loop
      printf("Name: %s\n", search);
      printf("Ingredients: %s\n", ingredients);
      printf("Secret Ingredients: %s\n", secret);
      isFound = 1;
      break;
    }
    else{
      //clears rest of line so fscanf can read the next recipe
      char buffer[100];
      fgets(buffer, 100, fd);
    }
  }
  fclose(fd);
  if(!isFound){
    printf("Recipe not found\n");
  }
}

//searches for recipes that contain a certain ingredient
void searchIngredient(){
  FILE *fd;
  fd = fopen(fileName, "r");
  char ingredient[46];
  printf("Enter the ingredient you want to search for: ");
  scanf("%45[^\n]", ingredient);
  //trimmed is a copy of the original user input kept to create the file name
  char trimmed[50];
  strcpy(trimmed, ingredient);
  padString(ingredient, 45);
 
  //create new file 
  char newFile[50];
  strcpy(newFile, trimmed);
  addExtension(newFile);
  //deletes file if it already exists
  if(access(newFile, F_OK) == 0){
    if(remove(newFile)){
      printf("Some error occured when replacing the file.\n");
      return;
    }
  }
  FILE *fw;
  fw = fopen(newFile, "w");
  
  char buffer[100]; 
  int found = 0; 
  //clears header line and writes it to file
  fgets(buffer, 100, fd);
  fprintf(fw, "%s", buffer);
  //loop reads recipe name to buffer as it is not needed
  while(fscanf(fd, "%25c", buffer) > 0){
    char search[46];
    //scanfs ingredients section
    fscanf(fd, "%45c", search);
    search[45] = '\0';
    //clears rest of line
    fgets(buffer, 100, fd);
    //splits the ingredients into tokens on each '+'
    char *parsed = strtok(search, "+");
    int copyLine = 0;
    //iterates through each token
    while(parsed != NULL){
      char copy[46];
      //formats token to be the same at the input was formatted
      strcpy(copy, parsed);
      trimLeading(copy);
      padString(copy, 45);
      //compares strings
      if(strcmp(ingredient, copy) == 0){
        copyLine = 1;
        found++;
      }
      parsed = strtok(NULL, "+");
    }
    //if ingredient was found, move file marker to beginning of line and writes
    //the line to the new file
    if(copyLine){
      fseek(fd, -81, 1);
      fgets(buffer, 100, fd);
      fprintf(fw, "%s", buffer);
    }
  }

  //if no recipes were found notify the user and delete the new file as it only
  //contains the header line
  if(found){
    printf("Recipes with desired secret ingredient have been stored in the file '%s.txt'\n", trimmed);
    printf("Found %d, matched records in the file.\n", found);
  }
  else{
    printf("No recipes found with ingredient: %s\n", trimmed);
    fclose(fw);
    fclose(fd);
    remove(newFile);
    return;
  }

  fclose(fw);
  fclose(fd);
}

void searchSecret(){
  FILE *fd;
  fd = fopen(fileName, "r");
  char secret[11];
  //gets input for secret ingredient from user
  printf("Enter the secret ingredient you want to search for: ");
  scanf("%10[^\n]", secret);
  //trimmed will store input to create text file
  char trimmed[15];
  strcpy(trimmed, secret);
  padString(secret, 10);

  char buffer[100];
  int found = 0;
  //skips first line
  fgets(buffer, 100, fd);
  //reads recipe and ingredients to buffer because they are not needed
  while(fscanf(fd, "%70c", buffer) > 0){
    char search[11];
    fscanf(fd, "%10c\n", search);
    search[10] = '\0';
    //counts number of matching recipes
    if(strcmp(search, secret) == 0){
      found++;
    }
  }

  //if no matching recipes were found notify user and exit function
  if(!found){
    printf("No recipes found with secret ingredient: %s\n", trimmed);
    fclose(fd);
    return;
  }
  else{
    printf("Recipes with desired secret ingredient have been stored in the file '%s.txt'\n", trimmed);
    printf("Found %d matched records in the file.\n", found);
  }

  //return to beginning of file
  rewind(fd);

  //creates array of strings to save the data that will be stored in the file
  char **matches = malloc(sizeof(*matches) * (found + 1));
  //saves the header string to the 0th index of the array
  matches[0] = malloc(sizeof(buffer));
  fgets(buffer, 100, fd);
  buffer[99] = '\0';
  strcpy(matches[0], buffer);
  //keeps track of the index for the string array
  int i = 1;
  //loops through file again looking for matching secret ingredients
  while(fscanf(fd, "%81c\n", buffer) > 0){
    //moves forwards to read the secret ingredient
    fseek(fd, -11, 1);
    char copy[11];
    fscanf(fd, "%10c\n", copy);
    copy[10] = '\0';
    //if match is found allocate memory for new string and copy the line to it
    if(strcmp(copy, secret) == 0){
      char line[100];
      fseek(fd, -81, 1);
      fgets(line, 100, fd);
      line[99] = '\0';
      matches[i] = malloc(sizeof(line));
      strcpy(matches[i], line);
      i++;
    }
  }
  fclose(fd);

  addExtension(trimmed);
  //if the file already exists remove it and make a new one
  if(access(trimmed, F_OK) == 0){
    if(remove(trimmed)){
      printf("Some error occured when replacing the file.\n");
      exit(1);
    }
  }
  FILE *fw;
  fw = fopen(trimmed, "w");

  //iterates through array of strings and saves them to the new file and the
  //frees the memory
  for(int i = 0; i < found + 1; i++){
    fprintf(fw, "%s", matches[i]);
    free(matches[i]);
  }
  free(matches);
  fclose(fw);
}

//adds a recipe to the end of the file
void writeRecipe(){
  FILE *fd;
  fd = fopen(fileName, "a");
  char name[26];
  char ingredients[46];
  char secret[11];
  //gets user input for recipe and its ingredients
  printf("Enter recipe details: \n");
  printf("Name: ");
  scanf("%25[^\n]", name);
  while(getchar() != '\n');
  printf("Ingredients: ");
  scanf("%45[^\n]", ingredients);
  while(getchar() != '\n');
  printf("Secret Ingredients: ");
  scanf("%10[^\n]", secret);
  while(getchar() != '\n');
  //formats all the strings for the file
  padString(name, 25);
  padString(ingredients, 45);
  padString(secret, 10);

  //writes recipe to the file
  if(fprintf(fd, "%s%s%s\n", name, ingredients, secret) > 0){
    printf("The recipe has been added\n");
  }
  else{
    printf("error adding recipe\n");
  }
  fclose(fd);
}

//displays menu and processes user input for the options
int displayMenu(){
  int input;
  void (*func)();
  printf("1) Print All Records\n");
  printf("2) Search Recipe By Name\n");
  printf("3) Write Recipe To File\n");
  printf("4) Search By Secret Ingredient\n");
  printf("5) Search By Ingredient\n");
  printf("6) Store User Recipes\n");
  printf("7) Quit\n");  
  scanf("%d", &input);
  while(getchar() != '\n');

  if(input == 1){
    func = accessFile;
  }
  else if(input == 2){
    func = searchRecipe;
  }
  else if(input == 3){
    func = writeRecipe;
  }
  else if(input == 4){
    func = searchSecret;
  }
  else if(input == 5){
    func = searchIngredient;
  }
  else if(input == 6){
    return 1;
  }
  else if(input == 7){
    return 0;
  }
  else{
    printf("Not a valid selection, please try again\n");
    return 1;
  }
  func();
  return 1;
}

//runs function to set file name and then displays menu until user quits
void main(){
  setFile();
  while(displayMenu());
}
