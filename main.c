#include <stdio.h>
  #include <stdbool.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>

#define Q_MAX 15
#define RESET       "\033[0m"
#define RED         "\033[1;31m"
#define GREEN       "\033[1;32m"

typedef struct game {
float totalScore;
int passedQuestions;
char user[30];
bool lost;
} game;

typedef struct question {
char text[300];
char option[4][100];
int correct;
} question;

typedef struct user {
char name[30];
float score;
} user;

question questions[Q_MAX];
game g;
user users[100];
int user_count = 0;

void clear_screen() {
#ifdef _WIN32
system("cls");
#else
system("clear");
#endif
}

void loadQ() {
FILE *file = fopen("questions.txt", "r");
if (file == NULL) {
printf("The file couldn't be opened.");
return;
}
for (int i = 0; i < Q_MAX; i++) {
fgets(questions[i].text, 299, file);
questions[i].text[strcspn(questions[i].text, "\n")] = '\0';
for (int j = 0; j < 4; j++) {
fgets(questions[i].option[j], 99, file);
questions[i].option[j][strcspn(questions[i].option[j], "\n")] = '\0';
}
fscanf(file, "%d", &questions[i].correct);
fgetc(file);
}
fclose(file);
}

void randomizeQ() {
srand(time(NULL));
for (int i = Q_MAX - 1; i > 0; i--) {
int j = rand() % (i + 1);
question temp = questions[i];
questions[i] = questions[j];
questions[j] = temp;
}
}

void header() {
clear_screen();
printf("\n");
printf("Welcome to the game! " RED "Please choose an option and press enter:\n" RESET);
printf(GREEN "1" RESET ". Play the game;\n");
printf(GREEN "2" RESET ". View the leaderboard;\n");
printf(GREEN "3" RESET ". View user game history;\n");
printf(GREEN "0" RESET ". Exit.\n");
}

void winQ() {
char a;
g.totalScore += 1;
g.passedQuestions += 1;
printf(GREEN "Correct! Press enter to continue." RESET);
getchar();
scanf("%c", &a);
}

void loseQ() {
char a;
g.lost = true;
printf(RED "Wrong! You have lost." RESET " You have passed %d questions, with a total score of %.1f. Press enter to continue.", g.passedQuestions, g.totalScore);
getchar();
scanf("%c", &a);
}

void save_score() {
FILE *file = fopen("leaderboard.csv", "r");
user temp_users[100];
int count = 0;
bool exists = false;
if (file != NULL) {
    while (fscanf(file, "%[^,],%f\n", temp_users[count].name, &temp_users[count].score) == 2) {
        if (strcmp(temp_users[count].name, g.user) == 0) {
            temp_users[count].score += g.totalScore;
            exists = true;
        }
        count++;
    }
    fclose(file);
}
if (!exists) {
    strcpy(temp_users[count].name, g.user);
    temp_users[count].score = g.totalScore;
    count++;
}
for (int i = 0; i < count - 1; i++) {
    for (int j = i + 1; j < count; j++) {
        if (temp_users[j].score > temp_users[i].score) {
            user temp = temp_users[i];
            temp_users[i] = temp_users[j];
            temp_users[j] = temp;
        }
    }
}
file = fopen("leaderboard.csv", "w");
for (int i = 0; i < count; i++) {
    fprintf(file, "%s,%.1f\n", temp_users[i].name, temp_users[i].score);
}
fclose(file);
}

void play() {
clear_screen();
int current = 0, response;
g.totalScore = 0;
g.passedQuestions = 0;
g.lost = false;
loadQ();
randomizeQ();
while (current < 15 && !g.lost) {
    clear_screen();
    printf("This is question %d out of 15:\n", current + 1);
    printf("%s\n", questions[current].text);
    printf(GREEN "1" RESET ". %s;\n", questions[current].option[0]);
    printf(GREEN "2" RESET ". %s;\n", questions[current].option[1]);
    printf(GREEN "3" RESET ". %s;\n", questions[current].option[2]);
    printf(GREEN "4" RESET ". %s;\n", questions[current].option[3]);
    printf(GREEN "5" RESET". Fifty/fifty: remove two wrong answers for a small penalty.\n");
    while (scanf("%d", &response) != 1 || response < 0 || response > 5)  {
        char d;
        clear_screen();
        printf(RED "Invalid input! " RESET "Please enter a number between 1 and 5.\n Please press enter to try again:\n");
        getchar();
        scanf("%c", &d);
        clear_screen();
        printf("This is question %d out of 15:\n", current + 1);
        printf("%s\n", questions[current].text);
        printf(GREEN "1" RESET ". %s;\n", questions[current].option[0]);
        printf(GREEN "2" RESET ". %s;\n", questions[current].option[1]);
        printf(GREEN "3" RESET ". %s;\n", questions[current].option[2]);
        printf(GREEN "4" RESET ". %s;\n", questions[current].option[3]);
        printf(GREEN "5" RESET". Fifty/fifty: remove two wrong answers for a small penalty.\n");
    }
    clear_screen();
    if (response == questions[current].correct) {
        winQ();
    }
    else if (response == 5) {
        clear_screen();
        srand(time(NULL));
        int temp, correctIndex = questions[current].correct - 1;
        do {
            temp = rand() % 4;
        } while (temp == correctIndex);
        printf("%s\n", questions[current].text);
        if (temp < correctIndex) {
            printf(GREEN "%d" RESET ". %s\n", temp + 1, questions[current].option[temp]);
            printf(GREEN "%d" RESET ". %s\n", correctIndex + 1, questions[current].option[correctIndex]);
        } else {
            printf(GREEN "%d" RESET ". %s\n", correctIndex + 1, questions[current].option[correctIndex]);
            printf(GREEN "%d" RESET ". %s\n", temp + 1, questions[current].option[temp]);
        }
        while (scanf("%d", &response) != 1 || response < 0 || response > 5) {
            char e;
            clear_screen();
            printf(RED "Invalid input! " RESET "Please enter a valid option.\n Please press enter to try again:\n");
            getchar();
            scanf("%c", &e);
            clear_screen();
            printf("%s\n", questions[current].text);
            if (temp < correctIndex) {
                printf(GREEN "%d" RESET ". %s\n", temp + 1, questions[current].option[temp]);
                printf(GREEN "%d" RESET ". %s\n", correctIndex + 1, questions[current].option[correctIndex]);
            } else {
                printf(GREEN"%d" RESET ". %s\n", correctIndex + 1, questions[current].option[correctIndex]);
                printf(GREEN "%d" RESET ". %s\n", temp + 1, questions[current].option[temp]);
            }
        }
        clear_screen();
        if (response == questions[current].correct) {
            winQ();
            g.totalScore -= 0.5;
        } else {
            loseQ();
        }
    } else {
        loseQ();
    }
    current++;
}
if (g.passedQuestions == 15) {
    clear_screen();
    printf(GREEN "Congratulations! " RESET "You have won the game! Your total score is %.1f. \nPress enter to continue.\n", g.totalScore);
    getchar();
    char a;
    scanf("%c", &a);
}
clear_screen();
printf("\nPlease provide a username:\n");
scanf("%s", g.user);
save_score();

FILE *history = fopen("history.csv", "a");
if (history != NULL) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date[100];
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", t);
    fprintf(history, "%s,%s,%d,%.1f\n", g.user, date, g.passedQuestions, g.totalScore);
    fclose(history);
}
}

void view_leaderboard() {
clear_screen();
FILE *file = fopen("leaderboard.csv", "r");
if (file == NULL) {
printf(RED "No leaderboard data available.\n" RESET);
} else {
printf(GREEN "Leaderboard:\n" RESET);
char name[30];
float score;
while (fscanf(file, "%[^,],%f\n", name, &score) == 2) {
printf("%s - %.1f\n", name, score);
}
fclose(file);
}
printf("\nPress enter to return to the menu.\n");
getchar();
char a;
    scanf("%c", &a);
}

void usr_history() {
clear_screen();
char username[30];
printf("Enter the username to view their game history:\n");
scanf("%s", username);
getchar(); // Consume the leftover newline character
FILE *history = fopen("history.csv", "r");
if (history == NULL) {
    printf(RED "No game history available.\n" RESET);
} else {
    char fileUser[30], date[100];
    int passed;
    float score;
    bool found = false;
    clear_screen();
    printf(GREEN "\nGame History for %s:" RESET"\n", username);
    printf("--------------------------------------------\n");
    while (fscanf(history, "%[^,],%[^,],%d,%f\n", fileUser, date, &passed, &score) == 4) {
        if (strcmp(fileUser, username) == 0) {
            printf("Date: %s | Questions Passed: %d | Score: %.1f\n", date, passed, score);
            found = true;
        }
    }
    fclose(history);
    if (!found) {
        printf("No history found for user '%s'.\n", username);
    }
}
printf("\nPress enter to return to the menu.\n");
char a;
    scanf("%c", &a);
}

int main() {
int menuOption;
while (true) {
clear_screen();
header();
while (scanf("%d", &menuOption) != 1 || menuOption < 0 || menuOption > 3) {
clear_screen();
printf("Invalid menu option! Please provide a number between 0 and 3.\n");
getchar();
    char d;
    scanf("%c", &d);
header();
}
if (menuOption == 1)
play();
else if (menuOption == 2)
view_leaderboard();
else if (menuOption == 3)
usr_history();
else if (menuOption == 0)
break;
}
printf("Thank you for playing!");
return 0;
}
