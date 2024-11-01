/***************************************************************************
** words.c
**
** These functions are used to load the WORDS.TOK file. The words are
** stored in a sorted list along with their synonyms numbers. This allows
** us to use a binary search when we need to determine a words synonym
** number.
**
** (c) 1997 Lance Ewing - Initial code (27 Aug 97)
***************************************************************************/

#include <stdio.h>

#include "general.h"
#include "words.h"

wordType *words;
int numWords, numSynonyms;  /* Big difference between the two */

/**************************************************************************
** loadWords
**
** Purpose: Load words in from the WORDS.TOK file.
**************************************************************************/
void loadWords()
{
   FILE *wordFile;
   byte data, wordPos, newWord[80], temp[80];
   long startPos;
   word wordNum, synNum;

   if ((wordFile = fopen("WORDS.TOK","rb")) == NULL) {
      printf("Cannot find file : WORDS.TOK\n");
      exit(1);
   }

   calcNumWords(wordFile);
   words = (wordType *)malloc(sizeof(wordType)*numWords);

   startPos = (byte)fgetc(wordFile) * (long)256 + (byte)fgetc(wordFile);
   fseek(wordFile, startPos, SEEK_SET);

   for (wordNum=0; wordNum<numWords; wordNum++) {
      data = fgetc(wordFile);
      wordPos = data;
      do {
	      data = fgetc(wordFile);
	      newWord[wordPos++] = ((data ^ 0x7F) & 0x7F);
      } while (data < 0x80);
      newWord[wordPos] = NULL;
      synNum = (byte)fgetc(wordFile)*256 + (byte)fgetc(wordFile);
	   words[wordNum].wordText = (char *)strdup(newWord);
      words[wordNum].synonymNum = synNum;
   }

   fclose(wordFile);
}

/**************************************************************************
** calcNumWords
**
** Purpose: Calculate the number of different words (including synonyms)
** and the number of synonyms which are contained in the WORDS.TOK file.
** The number of words will determine how much memory to allocate for the
** words list. The number of synonyms will be used to check against the
** parameters to said() to determine whether those synonyms exist.
**************************************************************************/
int calcNumWords(FILE *wordFile)
{
   byte data;
   long startPos;
   int synMax, synNum, wordCount=0;

   fseek(wordFile, 0, SEEK_SET);
   startPos = (byte)fgetc(wordFile) * (long)256 + (byte)fgetc(wordFile);
   fseek(wordFile, startPos, SEEK_SET);

   data = fgetc(wordFile);
   while (!feof(wordFile)) {
      if (data > 0x80) {  /* Top bit set marks the end of a word */
         wordCount++;
	      synNum = (byte)fgetc(wordFile) * 256 + (byte)fgetc(wordFile);
	      if ((synNum > synMax) && (synNum != 9999)) synMax = synNum;
      }
      data = fgetc(wordFile);
   }

   numWords = wordCount;
   numSynonyms = synMax;
   fseek(wordFile, 0, SEEK_SET);
}

/***************************************************************************
** discardWords
**
** Purpose: To deallocate all memory associated with the words array.
***************************************************************************/
void discardWords()
{
   int wordNum;

   for (wordNum=0; wordNum<numWords; wordNum++)
      free(words[wordNum].wordText);

   free(words);
}

/***************************************************************************
** findSynonymNum
**
** Purpose: Returns the synonym number for the given word if it is
** contained in WORDS.TOK, otherwise it returns -1. This function performs
** a binary search to locate the correct word entry. Some games would have
** a search depth of about 10 or 11 (1000+ words).
***************************************************************************/
int findSynonymNum(char *userWord)
{
   boolean found=FALSE;
   int top=numWords-1, bottom=0, mid, strCompVal;

   while ((!found) && (bottom <= top)) {
      mid = (top + bottom) / 2;
      strCompVal = strcmp(userWord, words[mid].wordText);
      if (strCompVal == 0)
         found = TRUE;
      else if (strCompVal < 0)
         top = mid - 1;
      else
         bottom = mid + 1;
   }

   if (found) return (words[mid].synonymNum);
   else return (-1);
}

void showWords()
{
   int wordNum;

   for (wordNum=0; wordNum<numWords; wordNum++)
      printf("%-14s%5d ", words[wordNum].wordText, words[wordNum].synonymNum);
}

void testWords()
{
   loadWords();
   showWords();
   printf("Number for 'yes' is: %d\n", findSynonymNum("yes"));
   discardWords();
}
