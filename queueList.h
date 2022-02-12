#ifndef QUEUELIST_H
#define QUEUELIST_H

typedef struct queueList {
    int type;
    char * value;
    struct queueList * next;
} QUEUE;

QUEUE * conf_queue();
void add(QUEUE * head, char * value, int type);
char * erase(QUEUE * head);
void copy_queue(QUEUE * dest, QUEUE * source);
int get_type(QUEUE * head);
char * get_string(QUEUE * head);
void delete_queue(QUEUE * head);

#endif //QUEUELIST_H
