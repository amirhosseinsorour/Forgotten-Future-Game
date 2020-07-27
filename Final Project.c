#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

int people , court , fund ;
int problem_num ;
char player_name[30] ;

typedef struct {
int people ;
int court ;
int fund ;
}effect;

struct node{
char problem[200] ;
char first_solution[200] ;
effect first_effect ;
char second_solution[200] ;
effect second_effect ;
int possibility ;
struct node * next ;
};

void print_list(){

printf("Please choose one of these options:\n\n" , player_name);
printf("[1] Start a new game\n");
printf("[2] Resume a game\n");
printf("[3] Scoreboard\n");
printf("[4] Design a new problem\n");
printf("[5] Exit\n\n>");
}

void add_end(struct node * new_node , struct node * list){
    struct node * current = list ;
    while(current->next != NULL)
        current = current->next ;
    current->next = new_node ;
    new_node->next = NULL ;
}

void check_file_handler(FILE* file_to_check){
if(file_to_check == NULL){
    printf("Cannot Open File\n");
    exit(-1) ;
}
}

void color(void){
float average = (people+court+fund)/3 ;

if(people>100) people = 100 ;
if(court>100)  court = 100 ;
if(fund>100)   fund = 100 ;

if(people<=10 || court<=10 || fund<=10 || average<=20)
    system("color 4E");
else if(people == 100 || court == 100 || fund == 100)
    system("color 2E");
else
    system("color 1E");
}

struct node * create_list(){
FILE* choices_file = fopen("CHOICES.txt" , "r");
check_file_handler(choices_file);

struct node * list = NULL ;
char problem_file_name[20];

while(fscanf(choices_file , " %s" , problem_file_name) != EOF ){
    FILE* problem_file = fopen(problem_file_name , "r");
    check_file_handler(problem_file);
    struct node * new_node = (struct node *)malloc(sizeof(struct node)) ;
    fgets(new_node->problem , 200 , problem_file);
    fgets(new_node->first_solution , 200 , problem_file);
    fscanf(problem_file , "%d" , &(new_node->first_effect.people));
    fscanf(problem_file , "%d" , &(new_node->first_effect.court));
    fscanf(problem_file , "%d" , &(new_node->first_effect.fund));
    fseek(problem_file , 2 , SEEK_CUR);
    fgets(new_node->second_solution , 200 , problem_file);
    fscanf(problem_file , "%d" , &(new_node->second_effect.people));
    fscanf(problem_file , "%d" , &(new_node->second_effect.court));
    fscanf(problem_file , "%d" , &(new_node->second_effect.fund));
    new_node->possibility = 3 ;
    new_node->next = NULL ;
    if(list == NULL)
        list = new_node ;
    else
        add_end(new_node , list);

    fclose(problem_file);
    problem_num++ ;
}
fclose(choices_file);
return list ;
}

void delete_node(struct node * node_to_delete , struct node ** list){

struct node * current = *list ;

if(current == node_to_delete){
    *list = (*list)->next ;
    free(node_to_delete);
    }
else{
    while(current->next != node_to_delete)
        current = current->next ;
    current->next = node_to_delete->next ;
    free(node_to_delete);
}
}

int check_save(FILE * save_file){

if(save_file == NULL)
    return 0 ;

struct {
char name[30] ;
int flag ;
int people , court , fund ;
int remaining_possibility[problem_num] ;
}info;

int count = 0 ;
while(fread(&info , sizeof(info) , 1 , save_file) >= 1){
    count++ ;
    if(strcmp(info.name , player_name) == 0){
        rewind(save_file);
        return count ;
    }
}

return 0 ;
}

int in_list(char problem[] , struct node * list){
for(struct node * current = list ; current != NULL ; current = current->next)
    if(strcmp(current->problem , problem) == 0)
        return current->possibility ;
return 0 ;
}

void save(int flag , struct node * list_to_save){
problem_num = 0 ;
struct node * list = create_list() ;

struct info{
char name[30] ;
int flag ;
int people , court , fund ;
int remaining_possibility[problem_num] ;
};
struct info save_info ;

strcpy(save_info.name , player_name) ;
save_info.flag = flag ;
save_info.people = people ;
save_info.court = court ;
save_info.fund = fund ;

struct node * current = list ;
for(int i = 0 ; i<problem_num ; i++){
    save_info.remaining_possibility[i] = in_list(current->problem , list_to_save) ;
    current = current->next ;
}

FILE * save_file = fopen("save.bin" , "rb");
if(save_file == NULL || !check_save(save_file)){
    if(save_file != NULL)
        fclose(save_file);
    save_file = fopen("save.bin" , "ab");
    check_file_handler(save_file);
    fwrite(&save_info , sizeof(struct info) , 1 , save_file);
    fclose(save_file);
}
else{
    printf("\nYou already have a saved game.Do you want to overwrite it?[y/n]\n\n>");
    getchar();      char choice = getchar();
    if(choice == 'y'){
    struct info tmp_info ;
    FILE * tmp_file = fopen("tmp.bin" , "wb");
    while(fread(&tmp_info , sizeof(struct info) , 1 , save_file) >= 1){
        if(strcmp(tmp_info.name , player_name) == 0)
            fwrite(&save_info , sizeof(struct info) , 1 , tmp_file);
        else
            fwrite(&tmp_info , sizeof(struct info) , 1 , tmp_file);
    }

    fclose(save_file);
    remove("save.bin");
    fclose(tmp_file);
    rename("tmp.bin" , "save.bin");
    }
}

}

void game(struct node ** list){
Sleep(200);    system("cls");   color();
printf("*****   People:%d       Court:%d        Treasury:%d    *****\n\n" , people , court , fund) ;
while(1){

srand(time(NULL)) ;
int r = rand()%problem_num ;
struct node * current = *list ;
for(int i = 0 ; i<r ; i++)
    current = current->next ;

puts(current->problem);
printf("[1] %s\n" , current->first_solution);
printf("[2] %s\n" , current->second_solution);
printf(">");
getchar();  char choice = getchar() ;
switch (choice){
case '1' :
    people += current->first_effect.people ;
    court += current->first_effect.court ;
    fund += current->first_effect.fund ;
    break ;
case '2' :
    people += current->second_effect.people ;
    court += current->second_effect.court ;
    fund += current->second_effect.fund ;
    break ;
default :
    printf("\nDo you want to exit?[y/n]\n\n>");
    char choice2 = getchar() ;
    if(choice2 == '\n')
        choice2 = getchar() ;
    if(choice2 == 'n'){
        Sleep(200);    system("cls");
        printf("*****   People:%d       Court:%d        Treasury:%d    *****\n\n" , people , court , fund) ;
        continue ;
    }
    else{
    printf("\nDo you want to save your current game?[y/n]\n\n>");
    getchar();  choice2 = getchar() ;
    if(choice2 == 'y')
        save(1 , *list);
    printf("\nOk!Goodbye.\n");
    exit(0);
    }
}
Sleep(200);    system("cls");   color();
printf("*****   People:%d       Court:%d        Treasury:%d    *****\n\n" , people , court , fund) ;

(current->possibility)-- ;
if(current->possibility == 0){
    delete_node(current , list) ;
    problem_num-- ;
}

float average = (people + court + fund) / 3 ;
if(people<=0 || court<=0 || fund<=0 || average<10){
    printf("            ******************************\n");
    printf("            *                            *\n");
    printf("            *                            *\n");
    printf("            *          YOU LOOSE         *\n");
    printf("            *                            *\n");
    printf("            *                            *\n");
    printf("            ******************************\n");
    printf("\n\nDo you want to save the game?[y/n]\n\n>");
    getchar();  char choice3 = getchar() ;
    if(choice3 == 'y')
        save(-1 , *list);
    printf("\nOk!Goodbye.\n");
    exit(0) ;
}
if(problem_num == 0)
    *list = create_list();
}

}

void load(struct node ** list){

struct info{
char name[30] ;
int flag ;
int people , court , fund ;
int remaining_possibility[problem_num] ;
};
struct info load_info ;

FILE * load_file = fopen("save.bin" , "rb");
int num = check_save(load_file) ;
if(num == 0){
    Sleep(200);    system("cls");
    printf("You dont have any saved game %s !\n\n" , player_name);
    printf("Do you want to start a new game?[y/n]\n\n>");
    getchar();  char choice = getchar() ;
    if(choice == 'y')
        game(list);
    else{
        printf("\nOk!Goodbye.\n");
        exit(0) ;
    }

}
else{
    fseek(load_file ,(num-1)*sizeof(struct info) , SEEK_SET ) ;
    fread(&load_info , sizeof(struct info) , 1 , load_file);
    fclose(load_file);
    if(load_info.flag == -1)
        game(list);
    else{
    people = load_info.people ;
    court = load_info.court ;
    fund = load_info.fund ;
    int i = 0 ;
    for(struct node * current = *list ; current != NULL ; current = current->next , i++){
        if(load_info.remaining_possibility[i] != 0)
            current->possibility = load_info.remaining_possibility[i] ;

        else{
            delete_node(current , list);
            problem_num-- ;
        }
    }
    game(list);
    }
}
}

void new_problem(struct node * list){
FILE * choices_file = fopen("CHOICES.txt" , "a+");
check_file_handler(choices_file);
Sleep(200);    system("cls");

char new_problem_file_name[30] ;
printf("Enter title of your new problem:\n\n");
scanf("%s" , new_problem_file_name);
strcat(new_problem_file_name , ".txt") ;
fprintf(choices_file , "\n%s" , new_problem_file_name) ;
fclose(choices_file);

FILE * new_problem_file = fopen(new_problem_file_name , "w");
check_file_handler(new_problem_file) ;

Sleep(200);    system("cls");
struct node * new_problem = (struct node *)malloc(sizeof(struct node)) ;

printf("Enter new problem:\n\n");
getchar() ;
gets(new_problem->problem) ;
new_problem->problem[strlen(new_problem->problem)+1] = '\0' ;
new_problem->problem[strlen(new_problem->problem)] = '\n' ;
fprintf(new_problem_file , "%s" , new_problem->problem) ;

Sleep(200);    system("cls");
printf("Enter first solution:\n\n");
//getchar();
gets(new_problem->first_solution) ;
new_problem->first_solution[strlen(new_problem->first_solution)+1] = '\0' ;
new_problem->first_solution[strlen(new_problem->first_solution)] = '\n' ;
fprintf(new_problem_file , "%s" , new_problem->first_solution) ;

printf("\nEnter effect of this solution on people : ");
scanf("%d" , &new_problem->first_effect.people);
fprintf(new_problem_file , "%d\n" , new_problem->first_effect.people);

printf("\nEnter effect of this solution on court : ");
scanf("%d" , &new_problem->first_effect.court);
fprintf(new_problem_file , "%d\n" , new_problem->first_effect.court);

printf("\nEnter effect of this solution on treasury : ");
scanf("%d" , &new_problem->first_effect.fund);
fprintf(new_problem_file , "%d\n" , new_problem->first_effect.fund);

Sleep(200);    system("cls");
printf("Enter second solution:\n\n");
getchar() ;
gets(new_problem->second_solution) ;
new_problem->second_solution[strlen(new_problem->second_solution)+1] = '\0' ;
new_problem->second_solution[strlen(new_problem->second_solution)] = '\n' ;
fprintf(new_problem_file , "%s" , new_problem->second_solution) ;

printf("\nEnter effect of this solution on people : ");
scanf("%d" , &new_problem->second_effect.people);
fprintf(new_problem_file , "%d\n" , new_problem->second_effect.people);

printf("\nEnter effect of this solution on court : ");
scanf("%d" , &new_problem->second_effect.court);
fprintf(new_problem_file , "%d\n" , new_problem->second_effect.court);

printf("\nEnter effect of this solution on treasury : ");
scanf("%d" , &new_problem->second_effect.fund);
fprintf(new_problem_file , "%d" , new_problem->second_effect.fund);

new_problem->possibility = 3 ;
add_end(new_problem , list) ;
problem_num++ ;

Sleep(200);    system("cls");
printf("New problem added!\n\n");

fclose(new_problem_file);
remove("save.bin");

getchar() ;
}

int cmp(const void * i1 , const void * i2){

struct info{
char name[30] ;
int flag ;
int people , court , fund ;
int remaining_possibility[problem_num] ;
} ;
struct info s1 , s2 ;

s1 = *((struct info *)i1);
s2 = *((struct info *)i2);

if(s1.people < s2.people)
    return 1;
else if(s1.people == s2.people)
    if(s1.court < s2.court)
        return 1 ;
    else if(s1.court == s2.court)
        if(s1.fund < s2.fund)
            return 1 ;
        else if(s1.fund == s2.fund)
            return 0 ;
        else return -1 ;
    else return -1 ;
else return -1 ;
}

void scoreboard(){
getchar();
Sleep(200);    system("cls");   system("color 3E");

struct info{
char name[30] ;
int flag ;
int people , court , fund ;
int remaining_possibility[problem_num] ;

};

FILE * saves = fopen("save.bin" , "rb") ;
if(saves == NULL){
    printf("\nThere is no saved game to load!\n\n\n\n");
    printf("Press any key to continue...\n");
    getch();    system("cls");      system("color 0E");
    return ;
}

int saves_counter = 0 ;
struct info temp ;
while(fread(&temp , sizeof(struct info) , 1 , saves) >= 1)
    saves_counter++ ;
rewind(saves) ;

struct info players[saves_counter] ;
fread(players , sizeof(struct info) , saves_counter , saves);
fclose(saves);

qsort(players , saves_counter , sizeof(struct info) , cmp) ;

printf("                 ****************************************\n");
printf("                 *                                      *\n");
printf("                 *               SCOREBOARD             *\n");
printf("                 *                                      *\n");
printf("                 ****************************************\n\n");
for(int i = 0 ; i<saves_counter ; i++){
    printf("RANK [%d] : %s" , i+1 , players[i].name );
    printf(" \t%s: %d" , "People" , players[i].people ) ;
    printf(" \t%s: %d" , "Court" , players[i].court ) ;
    printf(" \t%s: %d\n\n" , "Treasury" , players[i].fund ) ;
}

printf("Press any key to continue...\n");
getch();    system("cls");      system("color 0E");
}

int main(){
people = court = fund = 50 ;
system("color 0E");

printf("            ************** WELCOME TO YOUR KINGDOM **************\n\n");
printf("Enter your name: ");
scanf("%s" , player_name);

printf("\nWELCOME \"%s\" .");
print_list();
char choice ;

struct node * problems = create_list();

getchar() ;
while(1){
choice = getchar() ;

if(choice == '1')
    game(&problems) ;

else if(choice == '2')
    load(&problems);

else if(choice == '3'){
    scoreboard();
    print_list();
    continue ;
}

else if(choice == '4'){
    new_problem(problems);
    print_list();
    continue ;
}

else if(choice == '5'){
    printf("\nBye!\n");
    break ;
}

else{
    Sleep(200);    system("cls");
    printf("Invalid request!\n\n");
    print_list();
}

getchar();
}

return 0 ;
}
