#include <stdio.h>
#include <string.h>

char GetGuess(int gd[26]){
	printf("Guess a Letter: ");
	char g = ' ';
	scanf(" %c", &g);
	printf("\033[1A");
	if((g <= 'Z' && g >= 'A') || (g <= 'z' && g >= 'a')){
		printf("\033[1A\033[K\n");
		if((g - 'A' < 26 && gd[g - 'A'] < 0) || (g - 'a' < 26 && g - 'a' > -1 && gd[g - 'a'] < 0)){
			// Guess has been guessedi
			printf("\033[1A\033[KYou have already guessed '%c'.\n", g);
			return GetGuess(gd);
		}
		return g;
	}else{
		printf("\033[1A\033[KTry Again, '%c' is not a letter\n", g);
		return GetGuess(gd);
	}
}

void UpdateLetters(int gd[26]){
	printf("\033[6A\033[99D");
	for(int i = 0; i < 3; i++){
		printf("            |");
		for(int j = 0; j < 9; j++){
			if(j != 0){
				printf(" ");
			}
			if(!(i == 2 && j == 8) && gd[(9*i) + j] == -1){
				char k = (9*i) + j + 'A';
				printf("%c",k);
			}else{
				printf(" ");
			}
		}
		printf("|");
		printf("\033[1B\033[99D\033[K");
	}
	printf("            ~~~~~~~~~~~~~~~~~~~");
	printf("\033[3B\033[99D\033[K");
}

void UpdateStickman(int wrong){
	printf("\033[19A\033[99D\033[K");
	printf("                      ____\033[1B\033[99D\033[K                     |    |\033[B\033[99D\033[K                     |   ");
	if(wrong > 0){
		printf("(?)");
	}
	printf("\033[1B\033[99D\033[K                     |   ");
	if(wrong >= 4){
		printf("\\|/");
	}else if(wrong >= 3){
		printf("\\|");
	}else if(wrong >= 2){
		printf(" |");
	}
	printf("\033[1B\033[99D\033[K                     |    ");
	if(wrong >= 2){
		printf("|");
	}
	printf("\033[1B\033[99D\033[K                     |   ");
	if(wrong >= 6){
		printf("/ \\");
	}else if(wrong >= 5){
		printf("/");
	}
	printf("\033[1B\033[99D\033[K                     |");
	printf("\033[1B\033[99D\033[K                _____|_____\033[12B\033[99D\033[K");
}

void UpdateWord(int gd[26], int space, char wd[64], int l){
	printf("\033[9A\033[99D");
	for(int i = 0; i < space; i++){
		printf(" ");
	}
	for(int i = 0; i < l; i++){
		printf(" ");
		char m = wd[i];
		if((m <= 'Z' && m >= 'A') || (m <= 'z' && m >= 'a')){
			if(m - 'a' < 0){
				if(gd[m - 'A'] == -1){
					printf("%c", m);
				}else{
					printf("_");
				}
			}else{
				if(gd[m - 'a'] == -1){
					printf("%c", m);
				}else{
					printf("_");
				}
			}
		}else if (m == ' '){
			printf("/");
		}else{
			printf("%c", m);
		}
	}
	printf("\033[9B\033[99D\033[K");
}
int WinLose(int gd[26], char wd[64], int l, int wrong, int space){
	int lett;
	for(int i = 0; i < l; i ++){
		if(wd[i] - 'a' < 0){
			lett = wd[i] - 'A';
		}else{
			lett = wd[i] - 'a';
		}
		if(lett >= 0 && lett < 26 && gd[lett] != -1){
			break;
		}
		if(i == l - 1){
			// Win
			printf("You Win!\n");
			return 1;
		}
	}
	if(wrong >= 6){
		// Lose
		for(int i =0; i < 26; i++){
			gd[i] = -1;
		}
		UpdateWord(gd, space, wd, l);
		printf("You Lose!\n");
		return 1;
	}else{
		return 0;
	}
}

int main(){
	char word[64];
	int guessed[26] = {0};
	int wrongAnswers = 0;
	printf(" _   _   ___   _   _  ___ _     _   ___   _   _\n| |_| | / _ \\ | \\ | |/ __| \\   / | / _ \\ | \\ | |\n|  _  |/ /_\\ \\|  \\| | |_/|  \\_/  |/ /_\\ \\|  \\| |\n|_| |_/_/   \\_|_|\\__|\\___|_|\\_/|_/_/   \\_|_|\\__|\n");
	printf("                      ____\n                     |    |\n                     |      \n                     |      \n                     |      \n                     |      \n                     |      \n                _____|_____\n\n\n");
	printf("Enter your word: ");
	fgets(word,64,stdin);
	
	int len = strlen(word) - 1;
	word[len] = '\0';
	
	printf("\033[1A");

	int spaces = 21 - len;
	for(int i = 0; i < spaces; i++){
		printf(" ");
	}
	for(int i = 0; i < len; i++){
		printf(" ");
		char l = word[i];
		if((l <= 'Z' && l >= 'A') || (l <= 'z' && l >= 'a')){
			printf("_");
			if(l - 'a' < 0){
				guessed[l - 'A'] = 1;
			}else{
				guessed[l - 'a'] = 1;
			}
		} else if(l == ' '){
			printf("/");
		} else{
			printf("%c",l);
		}
	}
	printf("\n\n            ~~~Letters Tried~~~\n            |                 |\n            |                 |\n            |                 |\n            ~~~~~~~~~~~~~~~~~~~\n\n\n");
	
	int loop = 0;
	char guess = ' ';
	while(loop == 0){
		guess = GetGuess(guessed);
		if((guess - 'A' < 26 && guessed[guess - 'A'] > 0) || (guess - 'a' < 26 && guess - 'a' > -1 && guessed[guess - 'a'] > 0)){
			// Guess is correct
			if(guess - 'a' < 0){
				guessed[guess-'A'] = -1;
			}else{
				guessed[guess-'a'] = -1;
			}
			UpdateWord(guessed, spaces, word, len);
			UpdateLetters(guessed);
		}else{
			// Guess is incorrect
			if(guess - 'a' < 0){
				guessed[guess-'A'] = -1;
			}else{
				guessed[guess-'a'] = -1;
			}
			wrongAnswers++;
			UpdateLetters(guessed);
			UpdateStickman(wrongAnswers);
		}
		loop = WinLose(guessed, word, len, wrongAnswers, spaces);
	}

	return 0;
}