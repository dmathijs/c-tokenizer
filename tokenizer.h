#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <regex.h>

// Arbitrary max length for text in input
#define MAX_LENGTH 1048576
// Size of our target vocabulary
#define VOCABULARY_SIZE 277

typedef struct
{
	unsigned first;
	unsigned second;
	int occurrences;
} TokenPair;

typedef struct
{
	unsigned first;
	unsigned second;
} VocabularyPair;

union VocabularyCharacterUnion
{
	unsigned character;
	VocabularyPair pair;
};

typedef struct
{
	unsigned is_pair : 1;
	union VocabularyCharacterUnion vocabularyCharacter;
} VocabularyItem;

unsigned *encode(char *string, VocabularyItem *vocabulary);
char *decode(unsigned *string, VocabularyItem *vocabulary);