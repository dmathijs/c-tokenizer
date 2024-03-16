#include "tokenizer.h"

unsigned *encode(char *string, VocabularyItem *vocabulary);

extern unsigned getCharStringLength(unsigned *string);

unsigned *encode(char *string, VocabularyItem *vocabulary)
{
	printf("Length before encoding: %lu\n", strlen(string));
	char *stringPtr = string;

	unsigned *newString = malloc(sizeof(unsigned) * MAX_LENGTH);
	unsigned *newStringPtr = newString;
	// Ptr that should point to first pair in vocabulary
	VocabularyItem *mergePtr = vocabulary;

	while (!mergePtr->is_pair)
	{
		mergePtr++;
	}

	while (*stringPtr != '\0')
	{
		unsigned char1 = *stringPtr;
		stringPtr++;
		unsigned char2 = *stringPtr;

		// iterate merges
		int found = 0;
		VocabularyItem *mergePtrCpy = mergePtr;
		for (int i = 0; i < VOCABULARY_SIZE - 256; i++)
		{
			if (mergePtrCpy->vocabularyCharacter.pair.first == char1 && mergePtrCpy->vocabularyCharacter.pair.second == char2)
			{
				found = 1;

				*newStringPtr = i + 256;
				newStringPtr++;
				stringPtr++;

				break;
			}

			mergePtrCpy++;
		}

		if (!found)
		{
			*newStringPtr = char1;
			newStringPtr++;
		}
	}

	*newStringPtr = '\0';

	printf("Length after encoding: %d\n", getCharStringLength(newString));

	return newString;
}
