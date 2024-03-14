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
