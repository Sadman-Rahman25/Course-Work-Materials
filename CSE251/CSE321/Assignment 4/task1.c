#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 3
#define MAX_RESOURCES 3
#define MAX_NAME_LEN 20

typedef enum{
    PERM_NONE = 0,
    PERM_READ = 1 << 0,
    PERM_WRITE = 1 << 1,
    PERM_EXEC = 1 << 2
} Permission;

//User and Resource Definitions
typedef struct {
    char name[MAX_NAME_LEN]; 
    } User;
typedef struct {
     char name[MAX_NAME_LEN];
     } Resource;

//ACL Entry
typedef struct {
    char username[MAX_NAME_LEN];
    int perms;
} ACLEntry;

typedef struct {
    Resource res;
    ACLEntry entries[MAX_USERS]; 
    int entryCount;
} ACLControlledResource;

//Capability Entry
typedef struct {
    char resourceName[MAX_NAME_LEN];
    int perms;
} Capability;

typedef struct {
    User user;
    Capability caps[MAX_RESOURCES]; 
    int capCount;                   
} CapabilityUser;

//Utility Functions
void printPermissions(int perm) {
    int first = 1;
    if (perm == PERM_NONE) { printf("NONE"); return; }
    if (perm & PERM_READ) { printf("READ"); first = 0; }
    if (perm & PERM_WRITE) { if (!first) printf("|"); printf("WRITE"); first = 0; }
    if (perm & PERM_EXEC) { if (!first) printf("|"); printf("EXECUTE"); }
}

int hasPermission(int userPerm, int requiredPerm) {
    return (userPerm & requiredPerm) == requiredPerm;
}

//ACL System
void checkACLAccess(ACLControlledResource *res, const char *userName, int perm) {
    for (int i = 0; i < res->entryCount; ++i) {
        if (strcmp(res->entries[i].username, userName) == 0) {
            int userPerm = res->entries[i].perms;
            printf("ACL Check: User %s requests ", userName);
            printPermissions(perm);
            if (hasPermission(userPerm, perm)) {
                printf(" on %s: Access GRANTED\n", res->res.name);
            } else {
                printf(" on %s: Access DENIED\n", res->res.name);
            }
            return;
        }
    }
    printf("ACL Check: User %s has NO entry for resource %s: Access DENIED\n",
           userName, res->res.name);
}


//Capability System
void checkCapabilityAccess(CapabilityUser *user, const char *resourceName, int perm) {
    for (int i = 0; i < user->capCount; ++i) {
        if (strcmp(user->caps[i].resourceName, resourceName) == 0) {
            int userPerm = user->caps[i].perms;
            printf("Capability Check: User %s requests ", user->user.name);
            printPermissions(perm);
            if (hasPermission(userPerm, perm)) {
                printf(" on %s: Access GRANTED\n", resourceName);
            } else {
                printf(" on %s: Access DENIED\n", resourceName);
            }
            return;
        }
    }
    printf("Capability Check: User %s has NO capability for %s: Access DENIED\n",
           user->user.name, resourceName);
}

int main() {
    //Sample users and resources
    User users[MAX_USERS] = { {"Alice"}, {"Bob"}, {"Charlie"} };
    Resource resources[MAX_RESOURCES] = { {"File1"}, {"File2"}, {"File3"} };

    //ACL Setup
    ACLControlledResource aclRes[MAX_RESOURCES];
    for (int i = 0; i < MAX_RESOURCES; ++i) {
        strncpy(aclRes[i].res.name, resources[i].name, MAX_NAME_LEN-1);
        aclRes[i].res.name[MAX_NAME_LEN-1] = '\0';
        aclRes[i].entryCount = 0;
    }

    int idx;
    idx = aclRes[0].entryCount++;
    strncpy(aclRes[0].entries[idx].username, "Alice", MAX_NAME_LEN-1);
    aclRes[0].entries[idx].username[MAX_NAME_LEN-1] = '\0';
    aclRes[0].entries[idx].perms = PERM_READ;

    idx = aclRes[0].entryCount++;
    strncpy(aclRes[0].entries[idx].username, "Bob", MAX_NAME_LEN-1);
    aclRes[0].entries[idx].username[MAX_NAME_LEN-1] = '\0';
    aclRes[0].entries[idx].perms = PERM_READ;

    //Capability Setup
    CapabilityUser capUsers[MAX_USERS];
    for (int i = 0; i < MAX_USERS; ++i) {
        capUsers[i].user = users[i];
        capUsers[i].capCount = 0;
    }

    int cidx;
    cidx = capUsers[0].capCount++;
    strncpy(capUsers[0].caps[cidx].resourceName, "File1", MAX_NAME_LEN-1);
    capUsers[0].caps[cidx].resourceName[MAX_NAME_LEN-1] = '\0';
    capUsers[0].caps[cidx].perms = PERM_WRITE;

    cidx = capUsers[1].capCount++;
    strncpy(capUsers[1].caps[cidx].resourceName, "File3", MAX_NAME_LEN-1);
    capUsers[1].caps[cidx].resourceName[MAX_NAME_LEN-1] = '\0';
    capUsers[1].caps[cidx].perms = PERM_READ | PERM_WRITE;

    cidx = capUsers[2].capCount++;
    strncpy(capUsers[2].caps[cidx].resourceName, "File3", MAX_NAME_LEN-1);
    capUsers[2].caps[cidx].resourceName[MAX_NAME_LEN-1] = '\0';
    capUsers[2].caps[cidx].perms = PERM_READ;

    //Test ACL
    checkACLAccess(&aclRes[0], "Alice", PERM_READ);
    checkACLAccess(&aclRes[0], "Bob", PERM_WRITE);
    checkACLAccess(&aclRes[0], "Charlie", PERM_READ);

    //Test Capability
    checkCapabilityAccess(&capUsers[0], "File1", PERM_WRITE);
    checkCapabilityAccess(&capUsers[1], "File1", PERM_WRITE);
    checkCapabilityAccess(&capUsers[2], "File2", PERM_READ);

    return 0;
}

