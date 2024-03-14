#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

/*
Tokenizer which uses UTF-8
author: Diederik Mathijs
Based on video by Andrej Karpathy
*/

// Arbitrary max length for our input buffer
#define MAX_LENGTH 1048576

char *readTextFile();
extern TokenPair *getTokenBytePairs();

int main()
{
  char *s = readTextFile();

  TokenPair *tokenPairs = getTokenBytePairs(s);

  for (; tokenPairs->first != -1; tokenPairs++)
  {
    printf("%d, %d: %d\n", tokenPairs->first, tokenPairs->second, tokenPairs->occurrences);
  }
}

TokenPair *getTokenBytePairs(char *s)
{
  TokenPair *tokenPairs = malloc(sizeof(TokenPair) * 1024); // arbitrary number of pairs

  int i = 0;
  unsigned char *stringPtr = s;

  for (; *stringPtr != '\0'; stringPtr++)
  {
    unsigned char1 = *(stringPtr++);
    unsigned char2 = *stringPtr;
    // return 1 char after fetching pair
    stringPtr--;

    short found = 0;
    for (int i = 0; i < 1024; i++)
    {
      if (tokenPairs[i].first == char1 && tokenPairs[i].second == char2)
      {
        tokenPairs[i].occurrences++;
        found = 1;
        break;
      }
    }

    if (found == 0)
    {
      tokenPairs[i++] = (TokenPair){char1, char2, 1};
    }
  }

  tokenPairs[i] = (TokenPair){-1, -1, -1};

  return tokenPairs;
}

char *readTextFile()
{
  char *buffer = malloc(sizeof(char) * MAX_LENGTH);
  int index = 0;
  while ((buffer[index++] = getchar()) != EOF)
    ;
  buffer[index] = '\0';
  return buffer;
}