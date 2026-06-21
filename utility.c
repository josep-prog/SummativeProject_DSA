#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256

typedef struct Node
{
    char ch;
    int freq;
    struct Node *left;
    struct Node *right;
} Node;

Node *list[MAX * 2];
int nodeCount = 0;
char code[MAX][50];

void freeTree(Node *root);

/* Create Node */
Node *newNode(char ch, int freq)
{
    Node *n = (Node *)malloc(sizeof(Node));

    n->ch = ch;
    n->freq = freq;
    n->left = NULL;
    n->right = NULL;

    return n;
}

/* Count Character Frequencies */
void countFreq(char file[], int freq[])
{
    FILE *fp = fopen(file, "r");

    if (fp == NULL)
    {
        printf("Cannot open %s\n", file);
        return;
    }

    int ch;

    while ((ch = fgetc(fp)) != EOF)
        freq[ch]++;

    fclose(fp);
}

/* Remove Smallest Frequency Node */
Node *removeMin()
{
    int pos = 0;

    for (int i = 1; i < nodeCount; i++)
    {
        if (list[i]->freq < list[pos]->freq)
            pos = i;
    }

    Node *small = list[pos];

    for (int i = pos; i < nodeCount - 1; i++)
        list[i] = list[i + 1];

    nodeCount--;

    return small;
}

/* Build Huffman Tree */
Node *buildTree(int freq[])
{
    nodeCount = 0;

    for (int i = 0; i < MAX; i++)
    {
        if (freq[i] > 0)
            list[nodeCount++] = newNode(i, freq[i]);
    }

    while (nodeCount > 1)
    {
        Node *left = removeMin();
        Node *right = removeMin();

        Node *parent =
            newNode('\0', left->freq + right->freq);

        parent->left = left;
        parent->right = right;

        list[nodeCount++] = parent;
    }

    return list[0];
}

/* Generate Huffman Codes */
void makeCode(Node *root, char temp[], int level)
{
    if (root == NULL)
        return;

    if (root->left == NULL && root->right == NULL)
    {
        temp[level] = '\0';
        strcpy(code[(unsigned char)root->ch], temp);
        return;
    }

    temp[level] = '0';
    makeCode(root->left, temp, level + 1);

    temp[level] = '1';
    makeCode(root->right, temp, level + 1);
}

/* Compress File */
void compress(char input[], char output[])
{
    int freq[MAX] = {0};

    countFreq(input, freq);

    Node *root = buildTree(freq);

    char temp[50];

    /* A file with only one distinct character produces a
       single-leaf tree; give it a 1-bit code so it still
       has something to pack. */
    if (root->left == NULL && root->right == NULL)
    {
        code[(unsigned char)root->ch][0] = '0';
        code[(unsigned char)root->ch][1] = '\0';
    }
    else
    {
        makeCode(root, temp, 0);
    }

    long totalBits = 0;

    for (int i = 0; i < MAX; i++)
    {
        if (freq[i] > 0)
            totalBits += (long)freq[i] * (long)strlen(code[i]);
    }

    FILE *in = fopen(input, "rb");
    FILE *out = fopen(output, "wb");

    if (in == NULL || out == NULL)
    {
        printf("File error during compression\n");
        freeTree(root);
        return;
    }

    /* Compact binary header: only the symbols actually present, instead of
       all 256 possible byte frequencies. Avoids a header bigger than small
       input files. */
    int numSymbols = 0;

    for (int i = 0; i < MAX; i++)
        if (freq[i] > 0)
            numSymbols++;

    fwrite(&numSymbols, sizeof(int), 1, out);

    for (int i = 0; i < MAX; i++)
    {
        if (freq[i] > 0)
        {
            unsigned char symbol = (unsigned char)i;

            fwrite(&symbol, sizeof(unsigned char), 1, out);
            fwrite(&freq[i], sizeof(int), 1, out);
        }
    }

    fwrite(&totalBits, sizeof(long), 1, out);

    /* Pack the Huffman bits 8 at a time into real bytes */
    unsigned char buffer = 0;
    int bitsInBuffer = 0;
    int ch;

    while ((ch = fgetc(in)) != EOF)
    {
        char *bits = code[ch];

        for (int i = 0; bits[i] != '\0'; i++)
        {
            buffer = (buffer << 1) | (unsigned char)(bits[i] - '0');
            bitsInBuffer++;

            if (bitsInBuffer == 8)
            {
                fputc(buffer, out);
                buffer = 0;
                bitsInBuffer = 0;
            }
        }
    }

    if (bitsInBuffer > 0)
    {
        buffer <<= (8 - bitsInBuffer);
        fputc(buffer, out);
    }

    fclose(in);
    fclose(out);

    freeTree(root);
}

/* Decompress File */
void decompress(char input[], char output[])
{
    FILE *in = fopen(input, "rb");
    FILE *out = fopen(output, "wb");

    if (in == NULL || out == NULL)
    {
        printf("File error during decompression\n");
        return;
    }

    int freq[MAX] = {0};

    int numSymbols = 0;
    fread(&numSymbols, sizeof(int), 1, in);

    for (int i = 0; i < numSymbols; i++)
    {
        unsigned char symbol;
        int count;

        fread(&symbol, sizeof(unsigned char), 1, in);
        fread(&count, sizeof(int), 1, in);

        freq[symbol] = count;
    }

    long totalBits = 0;
    fread(&totalBits, sizeof(long), 1, in);

    Node *root = buildTree(freq);

    if (root->left == NULL && root->right == NULL)
    {
        /* Single-leaf tree: every bit just means "emit this char" */
        for (long i = 0; i < totalBits; i++)
            fputc(root->ch, out);
    }
    else
    {
        Node *cur = root;
        long bitsRead = 0;
        int byteVal;

        while (bitsRead < totalBits && (byteVal = fgetc(in)) != EOF)
        {
            for (int b = 7; b >= 0 && bitsRead < totalBits; b--)
            {
                int bit = (byteVal >> b) & 1;

                cur = bit ? cur->right : cur->left;
                bitsRead++;

                if (cur->left == NULL && cur->right == NULL)
                {
                    fputc(cur->ch, out);
                    cur = root;
                }
            }
        }
    }

    fclose(in);
    fclose(out);

    freeTree(root);
}

/* Compare Two Files */
int verify(char file1[], char file2[])
{
    FILE *a = fopen(file1, "r");
    FILE *b = fopen(file2, "r");

    if (a == NULL || b == NULL)
        return 0;

    int ch1, ch2;

    do
    {
        ch1 = fgetc(a);
        ch2 = fgetc(b);

        if (ch1 != ch2)
        {
            fclose(a);
            fclose(b);
            return 0;
        }

    } while (ch1 != EOF);

    fclose(a);
    fclose(b);

    return 1;
}

/* Get File Size */
long fileSize(char file[])
{
    FILE *fp = fopen(file, "rb");

    if (fp == NULL)
        return 0;

    fseek(fp, 0, SEEK_END);

    long size = ftell(fp);

    fclose(fp);

    return size;
}

/* Free Tree Memory */
void freeTree(Node *root)
{
    if (root == NULL)
        return;

    freeTree(root->left);
    freeTree(root->right);

    free(root);
}

/* Main */
int main()
{
    char original[] = "telemetry.txt";
    char compressed[] = "telemetry.huff";
    char restored[] = "telemetry_restored.txt";

    compress(original, compressed);

    decompress(compressed, restored);

    long originalSize = fileSize(original);
    long compressedSize = fileSize(compressed);

    printf("\n===== REPORT =====\n");

    printf("Original Size   : %ld bytes\n",
           originalSize);

    printf("Compressed Size : %ld bytes\n",
           compressedSize);

    if (originalSize > 0)
    {
        float ratio =
            ((float)(originalSize - compressedSize)
             / originalSize) * 100;

        printf("Compression Ratio : %.2f%%\n",
               ratio);
    }

    if (verify(original, restored))
        printf("Verification : SUCCESS\n");
    else
        printf("Verification : FAILED\n");

    return 0;
}
