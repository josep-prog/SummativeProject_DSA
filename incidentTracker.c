#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_INCIDENTS 25

typedef struct Incident
{
    int id;
    char source[20];
    char description[100];
    char timestamp[30];

    struct Incident *older;
    struct Incident *newer;

} Incident;

Incident *oldest = NULL;
Incident *newest = NULL;
Incident *current = NULL;

int totalIncidents = 0;
int nextIncidentID = 1;
int liveMonitoring = 0;


// Timestamp

void makeTimestamp(char timestamp[])
{
    time_t now = time(NULL);

    struct tm *timeInfo = localtime(&now);

    strftime(
        timestamp,
        30,
        "%Y-%m-%d %H:%M:%S",
        timeInfo
    );
}


// Create Incident


Incident *makeIncident(
    int id,
    char source[],
    char description[]
)
{
    Incident *newIncident =
        (Incident *)malloc(sizeof(Incident));

    if (newIncident == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    newIncident->id = id;

    strcpy(newIncident->source, source);
    strcpy(newIncident->description, description);

    makeTimestamp(newIncident->timestamp);

    newIncident->older = NULL;
    newIncident->newer = NULL;

    return newIncident;
}


// Remove Oldest


void removeOldest()
{
    if (oldest == NULL)
    {
        return;
    }

    Incident *toDelete = oldest;

    oldest = oldest->newer;

    if (oldest != NULL)
    {
        oldest->older = NULL;
    }
    else
    {
        newest = NULL;
    }

    if (current == toDelete)
    {
        current = oldest;
    }

    free(toDelete);

    totalIncidents--;
}


// Add Incident


void addIncident(
    int id,
    char source[],
    char description[]
)
{
    if (totalIncidents == MAX_INCIDENTS)
    {
        removeOldest();
    }

    Incident *newIncident =
        makeIncident(id, source, description);

    if (oldest == NULL)
    {
        oldest = newIncident;
        newest = newIncident;
        current = oldest;
    }
    else
    {
        newest->newer = newIncident;
        newIncident->older = newest;

        newest = newIncident;
    }

    totalIncidents++;
}

// Show Current

void showCurrent()
{
    if (current == NULL)
    {
        printf("\nNo incidents available.\n");
        return;
    }

    printf("\n-------------------------\n");
    printf("ID: %d\n", current->id);
    printf("Source: %s\n", current->source);
    printf("Description: %s\n",
           current->description);
    printf("Timestamp: %s\n",
           current->timestamp);
    printf("-------------------------\n");
}


// Navigation

void goForward()
{
    if (current == NULL)
    {
        printf("No incidents.\n");
        return;
    }

    if (current->newer != NULL)
    {
        current = current->newer;
    }
    else
    {
        printf("Already at newest incident.\n");
    }

    showCurrent();
}

void goBack()
{
    if (current == NULL)
    {
        printf("No incidents.\n");
        return;
    }

    if (current->older != NULL)
    {
        current = current->older;
    }
    else
    {
        printf("Already at oldest incident.\n");
    }

    showCurrent();
}

// Delete All

void clearAll()
{
    Incident *temp = oldest;

    while (temp != NULL)
    {
        Incident *next = temp->newer;

        free(temp);

        temp = next;
    }

    oldest = NULL;
    newest = NULL;
    current = NULL;

    totalIncidents = 0;

    printf("All incidents deleted.\n");
}

// Save Session

void saveSession()
{
    FILE *file =
        fopen("incident_log.txt", "w");

    if (file == NULL)
    {
        printf("Could not save file.\n");
        return;
    }

    Incident *temp = oldest;

    while (temp != NULL)
    {
        fprintf(
            file,
            "ID: %d\n"
            "Source: %s\n"
            "Description: %s\n"
            "Timestamp: %s\n\n",

            temp->id,
            temp->source,
            temp->description,
            temp->timestamp
        );

        temp = temp->newer;
    }

    fclose(file);

    printf("Session saved.\n");
}

// Random Incident Generator

void generateIncident()
{
    char *sources[] =
    {
        "Police",
        "Fire",
        "Ambulance"
    };

    char *descriptions[] =
    {
        "Traffic accident",
        "House fire",
        "Medical emergency",
        "Robbery reported",
        "Flood warning",
        "Power outage",
        "Gas leak",
        "Road blockage"
    };

    int sourceIndex =
        rand() % 3;

    int descriptionIndex =
        rand() % 8;

    addIncident(
        nextIncidentID++,
        sources[sourceIndex],
        descriptions[descriptionIndex]
    );

    printf(
        "\n[Live] New incident recorded.\n"
    );
}

int main()
{
    srand(time(NULL));

    char choice;

    printf("Emergency Dispatch Incident Tracker\n");

    addIncident(
        nextIncidentID++,
        "Police",
        "System startup incident"
    );

    while (1)
    {
        if (liveMonitoring)
        {
            generateIncident();
        }

        printf("\n");
        printf("f - View newer incident\n");
        printf("b - View older incident\n");
        printf("l - Enable live monitoring\n");
        printf("s - Stop live monitoring\n");
        printf("d - Delete all incidents\n");
        printf("v - View current incident\n");
        printf("q - Save session and quit\n");

        printf("\nChoice: ");

        scanf(" %c", &choice);

        switch (choice)
        {
            case 'f':
                goForward();
                break;

            case 'b':
                goBack();
                break;

            case 'l':
                liveMonitoring = 1;
                printf(
                    "Live monitoring enabled.\n"
                );
                break;

            case 's':
                liveMonitoring = 0;
                printf(
                    "Live monitoring stopped.\n"
                );
                break;

            case 'v':
                showCurrent();
                break;

            case 'd':
                clearAll();
                break;

            case 'q':
                saveSession();
                clearAll();
                printf("Goodbye.\n");
                return 0;

            default:
                printf("Invalid command.\n");
        }
    }

    return 0;
}
