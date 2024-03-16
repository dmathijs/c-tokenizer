#include "tokenizer.h"

char *decode(unsigned *string, VocabularyItem *vocabulary);

char *decode(unsigned *string, VocabularyItem *vocabulary)
{
	unsigned *stringPtr = string;
	char *newString = malloc(sizeof(char) * MAX_LENGTH);
	char *newStringPtr = newString;

	while (*stringPtr != '\0')
	{
		unsigned idx = *stringPtr;

		if (idx < 256)
		{
			*newStringPtr = idx;
			newStringPtr++;
		}
		else
		{
			VocabularyItem *vocabularyPtr = &vocabulary[idx];
			VocabularyPair pair = vocabularyPtr->vocabularyCharacter.pair;
			*newStringPtr = pair.first;
			newStringPtr++;
			*newStringPtr = pair.second;
			newStringPtr++;
		}

		stringPtr++;
	}

	*newStringPtr = '\0';

	return newString;
}