#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME 50
#define MAX_PROCEDURES 50

typedef struct Procedure
{
    char name[MAX_NAME];
    struct Procedure *left;
    struct Procedure *right;

} Procedure;

/* ROOT */
Procedure *root = NULL;
int procCount = 0;

/* CREATE NODE */
Procedure *createNode(char name[])
{
    Procedure *newNode = malloc(sizeof(Procedure));
    strcpy(newNode->name, name);
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

/* COMPARATOR FOR SORTING NAMES */
int compareNames(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

/* BUILD A HEIGHT-BALANCED BST FROM SORTED NAMES (guarantees O(log n) lookup) */
Procedure *buildBalanced(char names[][MAX_NAME], int low, int high)
{
    if (low > high)
        return NULL;

    int mid = (low + high) / 2;

    Procedure *node = createNode(names[mid]);

    node->left = buildBalanced(names, low, mid - 1);
    node->right = buildBalanced(names, mid + 1, high);

    return node;
}

/* SEARCH */
Procedure *search(Procedure *root, char name[])
{
    if (root == NULL)
        return NULL;

    int cmp = strcmp(name, root->name);

    if (cmp == 0)
        return root;
    else if (cmp < 0)
        return search(root->left, name);
    else
        return search(root->right, name);
}

/* EDIT DISTANCE (simple Levenshtein-lite) */
int similarity(char a[], char b[])
{
    int lenA = strlen(a);
    int lenB = strlen(b);

    int dp[51][51];

    for (int i = 0; i <= lenA; i++)
    {
        for (int j = 0; j <= lenB; j++)
        {
            if (i == 0) dp[i][j] = j;
            else if (j == 0) dp[i][j] = i;
            else
            {
                int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;

                int del = dp[i - 1][j] + 1;
                int ins = dp[i][j - 1] + 1;
                int sub = dp[i - 1][j - 1] + cost;

                int min = del;
                if (ins < min) min = ins;
                if (sub < min) min = sub;

                dp[i][j] = min;
            }
        }
    }

    return dp[lenA][lenB];
}

/* FIND CLOSEST MATCH (no nested function) */
void traverse(Procedure *node, char name[], char bestMatch[], int *bestScore)
{
    if (node == NULL)
        return;

    int score = similarity(name, node->name);

    if (score < *bestScore)
    {
        *bestScore = score;
        strcpy(bestMatch, node->name);
    }

    traverse(node->left, name, bestMatch, bestScore);
    traverse(node->right, name, bestMatch, bestScore);
}

char *findClosest(char name[])
{
    static char bestMatch[MAX_NAME];

    int bestScore = 999999; // lower = better match

    traverse(root, name, bestMatch, &bestScore);

    int len = strlen(name);

    /* threshold: must be reasonably close */
    if (bestScore > len / 2)
        return NULL;

    return bestMatch;
}

/* LOG INVALID */
void logAttempt(char name[])
{
    FILE *file = fopen("audit.log", "a");
    if (!file) return;

    time_t now = time(NULL);

    fprintf(file,
            "TIME: %sCOMMAND: %s\nSTATUS: REJECTED\n\n",
            ctime(&now),
            name);

    fclose(file);
}

/* LOAD FILE */
void loadProcedures(char filename[])
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Cannot open file.\n");
        return;
    }

    char names[MAX_PROCEDURES][MAX_NAME];
    int count = 0;

    char line[MAX_NAME];

    while (count < MAX_PROCEDURES && fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0)
            continue;

        strcpy(names[count], line);
        count++;
    }

    fclose(file);

    /* sort first so a balanced tree can be built regardless of file order */
    qsort(names, count, MAX_NAME, compareNames);

    /* drop duplicates so no two nodes share a key */
    int unique = 0;
    for (int i = 0; i < count; i++)
    {
        if (unique == 0 || strcmp(names[unique - 1], names[i]) != 0)
        {
            strcpy(names[unique], names[i]);
            unique++;
        }
    }

    root = buildBalanced(names, 0, unique - 1);
    procCount = unique;
}

/* VERIFY */
void verify(char input[])
{
    Procedure *found = search(root, input);

    if (found)
    {
        printf("APPROVED: %s\n", input);
        return;
    }

    char *suggest = findClosest(input);

    if (suggest)
    {
        printf("UNKNOWN COMMAND.\nDid you mean: %s ?\n", suggest);
    }
    else
    {
        printf("REJECTED: %s\n", input);
        logAttempt(input);
    }
}

/* DISPLAY */
void display(Procedure *root)
{
    if (!root) return;

    display(root->left);
    printf("%s\n", root->name);
    display(root->right);
}

/* FREE TREE */
void freeTree(Procedure *root)
{
    if (!root) return;

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/* MAIN */
int main()
{
    loadProcedures("procedures.txt");

    int choice;
    char input[MAX_NAME];

    while (1)
    {
        printf("\n1. Verify Procedure\n");
        printf("2. Show All Procedures\n");
        printf("3. Exit\n");
        printf("Choice: ");

        scanf("%d", &choice);

        if (choice == 1)
        {
            printf("Enter procedure: ");
            scanf("%49s", input);   // FIXED BUFFER OVERFLOW
            verify(input);
        }
        else if (choice == 2)
        {
            display(root);
        }
        else if (choice == 3)
        {
            freeTree(root);
            printf("System closed.\n");
            break;
        }
        else
        {
            printf("Invalid option.\n");
        }
    }

    return 0;
}
