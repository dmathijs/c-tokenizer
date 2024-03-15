#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

/*
Tokenizer which uses UTF-8
author: Diederik Mathijs
Based on video by Andrej Karpathy
run with: gcc tokenizer.c -o tokenizer && ./tokenizer < input.txt
*/

// Arbitrary max length for our input buffer
#define MAX_LENGTH 1048576
#define INTERMEDIATE_TOKENS_LIST_MAX_SIZE 4096u

#define VOCABULARY_SIZE 276

unsigned *readTextFile();

extern VocabularyItem *getVocabularyFromText(unsigned *s);
unsigned *compressVocabulary(unsigned *text);
unsigned *mergeTokenPairInText(unsigned *text, TokenPair *tokenPair, unsigned idx);

extern TokenPair *getOrderedTokenBytePairs(unsigned *s);

int main()
{
  unsigned *s = readTextFile();

  unsigned *tokensIncludingBytePairs = compressVocabulary(s);
}

extern VocabularyItem *getVocabularyFromText(unsigned *s)
{
  VocabularyItem *vocabulary = malloc(sizeof(VocabularyItem) * VOCABULARY_SIZE); // arbitrary number of tokens
  VocabularyItem *vocabularyPtr = vocabulary;

  for (unsigned *stringPtr = s; *stringPtr != '\0'; stringPtr++)
  {
    unsigned token = *stringPtr;
    short found = 0;
    for (int i = 0; i < VOCABULARY_SIZE; i++)
    {
      if (vocabulary[i].vocabularyCharacter.character == token)
      {
        found = 1;
        break;
      }
    }

    if (found == 0)
    {
      *vocabularyPtr++ = (VocabularyItem){is_pair : 0, vocabularyCharacter : token};
    }
  }

  return vocabulary;
}

TokenPair *getOrderedTokenBytePairs(unsigned *s)
{
  TokenPair *tokenPairs = malloc(sizeof(TokenPair) * INTERMEDIATE_TOKENS_LIST_MAX_SIZE); // arbitrary number of pairs
  TokenPair *tokenPairsPtr = tokenPairs;

  unsigned *stringPtr = s;

  while (*stringPtr != '\0')
  {
    unsigned char1 = *stringPtr;

    if (*stringPtr++ == '\0')
      break;

    unsigned char2 = *stringPtr;

    int found = 0;
    for (int j = 0; j < tokenPairsPtr - tokenPairs; j++)
    {
      if (tokenPairs[j].first == char1 && tokenPairs[j].second == char2)
      {
        tokenPairs[j].occurrences += 1;
        found = 1;
        break;
      }
    }

    if (!found)
    {
      tokenPairsPtr->first = char1;
      tokenPairsPtr->second = char2;
      tokenPairsPtr->occurrences = 1;

      tokenPairsPtr++;
    }

    for (int i = tokenPairsPtr - tokenPairs - 1; i >= 1; i--)
    {
      if (tokenPairs[i - 1].occurrences < tokenPairs[i].occurrences)
      {
        TokenPair temp = tokenPairs[i - 1];
        tokenPairs[i - 1] = tokenPairs[i];
        tokenPairs[i] = temp;
      }
    }
  }

  return tokenPairs;
}

unsigned *mergeTokenPairInText(unsigned *text, TokenPair *tokenPair, unsigned idx)
{
  unsigned *stringPtr = text;
  unsigned *newString = malloc(sizeof(unsigned) * MAX_LENGTH);
  unsigned *newStringPtr = newString;

  while (*stringPtr != '\0')
  {
    unsigned char1 = *stringPtr;
    stringPtr++;
    unsigned char2 = *stringPtr;

    if (char1 == tokenPair->first && char2 == tokenPair->second)
    {
      *newStringPtr = idx;
      newStringPtr++;
    }
    else
    {
      stringPtr--;
      *newStringPtr = *stringPtr;
      newStringPtr++;
    }

    stringPtr++;
  }

  *newStringPtr = '\0';

  // old text can be discarded..
  free(text);

  return newString;
}

unsigned *compressVocabulary(unsigned *text)
{
  // Vocabulary size can be higher because of UTF-8 encoding
  // thus we need to compress the vocabulary to it's target size
  // 256 characters are reserved for the first 256 ASCII characters
  unsigned merges = VOCABULARY_SIZE - 256;

  unsigned *txtPtr = text;

  VocabularyItem *vocabulary = getVocabularyFromText(txtPtr);

  for (int i = 0; i < merges; i++)
  {
    TokenPair *tokenPairs = getOrderedTokenBytePairs(txtPtr);

    TokenPair *pairsPtr = tokenPairs;

    TokenPair *max = &tokenPairs[0];

    VocabularyPair pair = (VocabularyPair){first : max->first, second : max->second};
    union VocabularyCharacterUnion character;
    character.pair = pair;
    vocabulary[256 + i] = (VocabularyItem){is_pair : 1, vocabularyCharacter : character};
    printf("Merging %d occurrences of: %d (%u, %u)\n", max->occurrences, 256 + i, max->first, max->second);

    txtPtr = mergeTokenPairInText(txtPtr, max, 256 + i);

    free(tokenPairs);
  }
}

unsigned *readTextFile()
{
  unsigned *buffer = malloc(sizeof(unsigned) * MAX_LENGTH);
  int index = 0;
  while ((buffer[index++] = getchar()) != EOF)
    ;
  buffer[index - 1] = '\0';
  return buffer;
}