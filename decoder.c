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
			// Small exclusion of special characters which are part of the text
			// I do this because they're anoying to print. e.g. 27 will end the program
			// without first converting to an escape sequence (feel free to add if you read this).
			if (idx > 31)
			{
				*newStringPtr = idx;
				newStringPtr++;
			}
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