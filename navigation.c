#include <stdio.h>
#include <string.h>

#define MAX 20
#define INF 999999

//GRAPH 

char buildings[MAX][30];
int graph[MAX][MAX];

int n = 0;

//FIND INDEX

int find(char name[])
{
    for(int i = 0; i < n; i++)
    {
        if(strcmp(buildings[i], name) == 0)
            return i;
    }
    return -1;
}

//ADD BUILDING
void addBuilding(char name[])
{
    if(find(name) != -1)
        return;

    strcpy(buildings[n], name);
    n++;
}

//ADD EDGE

void addEdge(char a[], char b[], int w)
{
    int i = find(a);
    int j = find(b);

    if(i == -1 || j == -1)
        return;

    graph[i][j] = w;
    graph[j][i] = w; // undirected graph
}

//DIJKSTRA

void dijkstra(int start, int target)
{
    int dist[MAX];
    int visited[MAX];
    int parent[MAX];

    for(int i = 0; i < n; i++)
    {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }

    dist[start] = 0;

    for(int count = 0; count < n - 1; count++)
    {
        int min = INF, u = -1;

        for(int i = 0; i < n; i++)
        {
            if(!visited[i] && dist[i] < min)
            {
                min = dist[i];
                u = i;
            }
        }

        if(u == -1)
            break;

        visited[u] = 1;

        for(int v = 0; v < n; v++)
        {
            if(!visited[v] &&
               graph[u][v] &&
               dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    //PRINT PATH

    if(dist[target] == INF)
    {
        printf("No route found.\n");
        return;
    }

    int path[MAX];
    int count = 0;
    int current = target;

    while(current != -1)
    {
        path[count++] = current;
        current = parent[current];
    }

    printf("\nShortest path:\n");

    for(int i = count - 1; i >= 0; i--)
    {
        printf("%s", buildings[path[i]]);
        if(i != 0)
            printf(" -> ");
    }

    printf("\nTotal distance: %d\n", dist[target]);
}

//MAIN

int main()
{
    /* Buildings */
    addBuilding("Library");
    addBuilding("Cafeteria");
    addBuilding("Science Block");
    addBuilding("Engineering");
    addBuilding("Administration");
    addBuilding("Dormitory");
    addBuilding("Charging Station");

    /* Paths */
    addEdge("Library", "Cafeteria", 6);
    addEdge("Library", "Engineering", 15);
    addEdge("Cafeteria", "Science Block", 4);
    addEdge("Science Block", "Dormitory", 8);
    addEdge("Engineering", "Administration", 5);
    addEdge("Administration", "Dormitory", 3);
    addEdge("Cafeteria", "Charging Station", 2);
    addEdge("Charging Station", "Administration", 4);

    char start[30];
    int s, d;

    printf("Enter start building: ");
    fgets(start, sizeof(start), stdin);
    start[strcspn(start, "\r\n")] = 0;

    s = find(start);
    d = find("Dormitory");

    if(s == -1 || d == -1)
    {
        printf("Invalid building name.\n");
        return 0;
    }

    dijkstra(s, d);

    return 0;
}
