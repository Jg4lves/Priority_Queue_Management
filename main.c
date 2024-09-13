#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Client {
  char name[100];
  uint32_t age;
  struct Client *next;
} Client;

typedef struct Institution {
  char name[100];
  uint32_t attendants;
  Client *queue;
} Institution;

void insertClient(char name[], uint32_t age, Institution *institution) {
  Client *newClient = (Client *)malloc(sizeof(Client));
  strcpy(newClient->name, name);
  newClient->age = age;
  newClient->next = NULL;

  if (institution->queue == NULL) {
    institution->queue = newClient;
  } else {
    Client *current = institution->queue;
    Client *previous = NULL;
    Client *lastAge60OrMore = NULL; // Store the last client with 60 years or more

    while (current != NULL && (current->age >= age || (age >= 60 && current->age >= 60))) {
      previous = current;

      if (current->age >= 60) {
        lastAge60OrMore = current;
      }

      current = current->next;
    }

    if (previous == NULL) {
      if (age >= 60 && lastAge60OrMore != NULL) {
        while (lastAge60OrMore->next != NULL && lastAge60OrMore->next->age >= age) {
          lastAge60OrMore = lastAge60OrMore->next;
        }
        newClient->next = lastAge60OrMore->next;
        lastAge60OrMore->next = newClient;
      } else {
        newClient->next = institution->queue;
        institution->queue = newClient;
      }
    } else {
      if (age >= 60 && lastAge60OrMore != NULL) {
        while (previous->next != NULL && previous->next->age >= age) {
          previous = previous->next;
        }
        newClient->next = previous->next;
        previous->next = newClient;
      } else {
        previous->next = newClient;
        newClient->next = current;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");
  uint32_t numInstitutions;

  fscanf(input, "%u", &numInstitutions);
  fgetc(input);

  Institution institutions[numInstitutions];
  for (uint32_t i = 0; i < numInstitutions; i++) {
    fscanf(input, "%s %u", institutions[i].name, &institutions[i].attendants);
    institutions[i].queue = NULL;
  }

  uint32_t numClients;
  fscanf(input, "%u", &numClients);
  fgetc(input);
  for (uint32_t i = 0; i < numClients; i++) {
    char line[100];
    uint32_t clientAge = 0;
    char chosenInstitution[50];
    char clientName[50];

    fgets(line, sizeof(line), input);

    char *token = strtok(line, "|");
    if (token != NULL) {
      strcpy(chosenInstitution, token);
    }
    token = strtok(NULL, "|");
    if (token != NULL) {
      strcpy(clientName, token);
    }
    token = strtok(NULL, "|");
    if (token != NULL) {
      clientAge = atoi(token);
    }

    uint32_t institutionIndex;
    for (uint32_t j = 0; j < numInstitutions; j++) {
      if (strcmp(chosenInstitution, institutions[j].name) == 0) {
        institutionIndex = j;
        break;
      }
    }

    insertClient(clientName, clientAge, &institutions[institutionIndex]);
  }

  while (1) {
    uint32_t served = 0;
    for (uint32_t i = 0; i < numInstitutions; i++) {
      Institution *institution = &institutions[i];
      Client *client = institution->queue;
      if (client != NULL) {
        fprintf(output, "%s:%s", institution->name, client->name);

        if (institution->attendants > 1) {
          for (uint32_t j = 1; j < institution->attendants && client->next != NULL; j++) {
            fprintf(output, ",%s", client->next->name);
            client = client->next;
          }
        }

        fprintf(output, "\n");

        // Correctly update the queue
        institution->queue = client->next;
        free(client);
        served++;
      }
    }

    if (served == 0) {
      break;
    }
  }

  fclose(input);
  fclose(output);

  return 0;
}
