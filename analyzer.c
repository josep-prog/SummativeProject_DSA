#include <stdio.h>
#include <string.h>

#define MAX_AIRPORTS 20

// GRAPH STRUCTURE

char airports[MAX_AIRPORTS][5];
int graph[MAX_AIRPORTS][MAX_AIRPORTS];

int airportCount = 0;

//FIND AIRPORT INDEX

int findAirport(char code[])
{
    for (int i = 0; i < airportCount; i++)
    {
        if (strcmp(airports[i], code) == 0)
            return i;
    }
    return -1;
}

//ADD AIRPORT

void addAirport(char code[])
{
    if (findAirport(code) != -1)
        return;

    if (airportCount >= MAX_AIRPORTS)
    {
        printf("Cannot add airport: maximum of %d airports reached.\n", MAX_AIRPORTS);
        return;
    }

    strcpy(airports[airportCount], code);
    airportCount++;
}

//ADD ROUTE

void addRoute(char from[], char to[])
{
    int i = findAirport(from);
    int j = findAirport(to);

    if (i == -1 || j == -1)
    {
        printf("Invalid airport(s)\n");
        return;
    }

    graph[i][j] = 1;
}

//REMOVE ROUTE

void removeRoute(char from[], char to[])
{
    int i = findAirport(from);
    int j = findAirport(to);

    if (i == -1 || j == -1)
        return;

    graph[i][j] = 0;
}

//REMOVE AIRPORT

void removeAirport(char code[])
{
    int index = findAirport(code);

    if (index == -1)
        return;

    //shift matrix rows up to close the gap
    for (int i = index; i < airportCount - 1; i++)
    {
        for (int j = 0; j < airportCount; j++)
        {
            graph[i][j] = graph[i + 1][j];
        }
    }

    //shift matrix columns left to close the gap
    for (int j = index; j < airportCount - 1; j++)
    {
        for (int i = 0; i < airportCount - 1; i++)
        {
            graph[i][j] = graph[i][j + 1];
        }
    }

    //shift airports
    for (int i = index; i < airportCount - 1; i++)
    {
        strcpy(airports[i], airports[i + 1]);
    }

    airportCount--;
}

//OUTGOING FLIGHTS
void showOutgoing(char code[])
{
    int i = findAirport(code);

    if (i == -1)
    {
        printf("Airport not found\n");
        return;
    }

    printf("Direct flights FROM %s:\n", code);

    for (int j = 0; j < airportCount; j++)
    {
        if (graph[i][j] == 1)
        {
            printf(" -> %s\n", airports[j]);
        }
    }
}

//INCOMING FLIGHTS
void showIncoming(char code[])
{
    int j = findAirport(code);

    if (j == -1)
    {
        printf("Airport not found\n");
        return;
    }

    printf("Direct flights INTO %s:\n", code);

    for (int i = 0; i < airportCount; i++)
    {
        if (graph[i][j] == 1)
        {
            printf(" <- %s\n", airports[i]);
        }
    }
}

//DISPLAY MATRIX

void showMatrix()
{
    printf("\nAdjacency Matrix:\n\n");

    printf("     ");

    for (int i = 0; i < airportCount; i++)
        printf("%s ", airports[i]);

    printf("\n");

    for (int i = 0; i < airportCount; i++)
    {
        printf("%s  ", airports[i]);

        for (int j = 0; j < airportCount; j++)
        {
            printf("  %d   ", graph[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    addAirport("KGL");
    addAirport("NBO");
    addAirport("EBB");
    addAirport("ADD");
    addAirport("CAI");
    addAirport("JNB");
    addAirport("CPT");

    addRoute("KGL", "NBO");
    addRoute("KGL", "EBB");
    addRoute("NBO", "JNB");
    addRoute("EBB", "ADD");
    addRoute("ADD", "CAI");
    addRoute("JNB", "CPT");

    int choice;
    char from[5], to[5];

    while (1)
    {
        printf("\n... AIRLINE ROUTE SYSTEM ...\n");
        printf("1. Show outgoing flights\n");
        printf("2. Show incoming flights\n");
        printf("3. Show adjacency matrix\n");
        printf("4. Add route\n");
        printf("5. Remove route\n");
        printf("6. Add airport\n");
        printf("7. Remove airport\n");
        printf("8. Exit\n");

        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            printf("Airport: ");
            scanf("%s", from);
            showOutgoing(from);
        }
        else if (choice == 2)
        {
            printf("Airport: ");
            scanf("%s", from);
            showIncoming(from);
        }
        else if (choice == 3)
        {
            showMatrix();
        }
        else if (choice == 4)
        {
            printf("From: ");
            scanf("%s", from);
            printf("To: ");
            scanf("%s", to);

            addAirport(from);
            addAirport(to);

            addRoute(from, to);
        }
        else if (choice == 5)
        {
            printf("From: ");
            scanf("%s", from);
            printf("To: ");
            scanf("%s", to);

            removeRoute(from, to);
        }
        else if (choice == 6)
        {
            printf("Airport code: ");
            scanf("%s", from);

            addAirport(from);
        }
        else if (choice == 7)
        {
            printf("Airport code: ");
            scanf("%s", from);

            removeAirport(from);
        }
        else if (choice == 8)
        {
            break;
        }
        else
        {
            printf("Invalid choice\n");
        }
    }

    return 0;
}
