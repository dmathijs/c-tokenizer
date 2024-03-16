#include "tokenizer.h"

/*
Tokenizer which uses UTF-8
author: Diederik Mathijs
Based on video by Andrej Karpathy
run with: gcc tokenizer.c decoder.c encoder.c -o tokenizer && ./tokenizer < input.txt
*/

#define INTERMEDIATE_TOKENS_LIST_MAX_SIZE 4096u

// Unlike in GPT2, there's no collapsing of spaces, this is because negative lookahead is not included in POSIX regex
char *GPT_2_PATTERN_SPLITTER = "'s|'t|'re|'ve|'m|'ll|'d| ?[[:alpha:]]+| ?[[:digit:]]+| ?[^[:space:][:alpha:][:digit:]]+|[:space:]+";

unsigned *readTextFile();

extern VocabularyItem *getBaseVocabulary(unsigned *s);
VocabularyItem *buildVocabulary(unsigned *text);
unsigned *mergeTokenPairInText(unsigned *text, TokenPair *tokenPair, unsigned idx, int freeTxtPtr);
unsigned getCharStringLength(unsigned *string);

extern unsigned *encode(char *string, VocabularyItem *vocabulary);
extern char *decode(unsigned *string, VocabularyItem *vocabulary);

extern TokenPair *getOrderedTokenBytePairs(unsigned *s);

int main()
{
  unsigned *s = readTextFile();

  VocabularyItem *vocabulary = buildVocabulary(s);

  // printf("%s\n", decode(encode("Can I be sure that this actually works as intended?", vocabulary), vocabulary));

  regex_t regex;
  int status = regcomp(&regex, GPT_2_PATTERN_SPLITTER, REG_EXTENDED);
  if (status != 0)
  {
    char error_message[100];
    regerror(status, &regex, error_message, sizeof(error_message));
    fprintf(stderr, "Regex compilation failed: %s\n", error_message);
    exit(EXIT_FAILURE);
  }

  // Calculate the length of the unsigned array
  unsigned *sPtr = s;
  // Allocate memory for the character array
  char *charArray = malloc(MAX_LENGTH * sizeof(char));
  char *charPtr = charArray;
  // Copy unsigned integers to characters, discarding overflows
  while (*sPtr != '\0')
  {
    // Check if the unsigned integer fits within the range of a char
    if (*sPtr <= UCHAR_MAX)
    {
      *charPtr++ = (char)*sPtr;
    }
    else
    {
      // Handle overflow by setting to a default value
      *charPtr++ = '?'; // For example, you can use '?' character
    }

    sPtr++;
  }

  charPtr = '\0';

  // Execute the regex split
  char *token;
  char *str = strdup(charArray); // Duplicate the string because regexec modifies it
  if (str == NULL)
  {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  while ((token = strsep(&str, " \t\n")) != NULL)
  {
    regmatch_t pmatch;
    if (regexec(&regex, token, 1, &pmatch, 0) == 0)
    {
      // If token matches the regex pattern, print it
      printf("%.*s, ", (int)(pmatch.rm_eo - pmatch.rm_so), token + pmatch.rm_so);
    }
    else
    {
      // If token does not match the regex pattern, print it as is
      // printf("%s\n", token);
    }
  }

  // Free the memory and compiled regex
  free(str);
  regfree(&regex);
}

extern VocabularyItem *getBaseVocabulary(unsigned *s)
{
  VocabularyItem *vocabulary = malloc(sizeof(VocabularyItem) * VOCABULARY_SIZE); // arbitrary number of tokens
  VocabularyItem *vocabularyPtr = vocabulary;

  for (int i = 0; i < 256; i++)
  {
    *vocabularyPtr++ = (VocabularyItem){is_pair : 0, vocabularyCharacter : i};
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

unsigned *mergeTokenPairInText(unsigned *text, TokenPair *tokenPair, unsigned idx, int freeTextPtr)
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

  if (freeTextPtr)
  {
    free(text);
  }

  return newString;
}

VocabularyItem *buildVocabulary(unsigned *text)
{
  // Vocabulary size can be higher because of UTF-8 encoding
  // thus we need to compress the vocabulary to it's target size
  // 256 characters are reserved for the first 256 ASCII characters
  unsigned merges = VOCABULARY_SIZE - 256;

  unsigned *txtPtr = text;

  VocabularyItem *vocabulary = getBaseVocabulary(txtPtr);

  unsigned originalLength = getCharStringLength(txtPtr);

  for (int i = 0; i < merges; i++)
  {
    TokenPair *tokenPairs = getOrderedTokenBytePairs(txtPtr);

    TokenPair *pairsPtr = tokenPairs;

    TokenPair *max = &tokenPairs[0];

    VocabularyPair pair = (VocabularyPair){first : max->first, second : max->second};
    union VocabularyCharacterUnion character;
    character.pair = pair;
    unsigned idx = 256 + i;
    vocabulary[idx] = (VocabularyItem){is_pair : 1, vocabularyCharacter : character};
    printf("Merging %d occurrences of: %d (%u, %u)\n", max->occurrences, idx, max->first, max->second);

    // Don't free the original text
    txtPtr = mergeTokenPairInText(txtPtr, max, idx, i > 1);

    free(tokenPairs);
  }

  unsigned compressedLength = getCharStringLength(txtPtr);

  printf("compression ratio: %0.2fX\n", (float)originalLength / (float)compressedLength);

  return vocabulary;
}

unsigned getCharStringLength(unsigned *string)
{
  unsigned *stringPtr = string;
  unsigned length = 0;
  while (*stringPtr != '\0')
  {
    length++;
    stringPtr++;
  }
  return length;
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
