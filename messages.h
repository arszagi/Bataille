#ifndef BATAILLE_MESSAGES_H
#define BATAILLE_MESSAGES_H

#define CANCEL 0

#define REGISTER 1

#define INSCRIPTION_STATUS 2

typedef struct Message Message;

union Payload {
    int number;
    int hand[26];
    char name[8]; // Le nom fera maximum 8 caractères
};

struct Message {
    int type;
    union Payload payload;
};

// #define
#endif //BATAILLE_MESSAGES_H
