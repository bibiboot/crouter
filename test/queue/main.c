#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

struct tailq_entry {
	int value;

	TAILQ_ENTRY(tailq_entry) entries;
};

TAILQ_HEAD(, tailq_entry) my_tailq_head;

void add_item(int value){
    struct tailq_entry *item = malloc(sizeof(struct tailq_entry));
    item->value = value;
    TAILQ_INSERT_TAIL(&my_tailq_head, item, entries);
}

void printq(){
    struct tailq_entry *item;
    TAILQ_FOREACH(item, &my_tailq_head, entries) {
        printf("%d ", item->value);
    }
    printf("\n");
}

void pop_all(){
    struct tailq_entry *item;
    while (!TAILQ_EMPTY(&my_tailq_head)) {
        item = TAILQ_FIRST(&my_tailq_head);
        TAILQ_REMOVE(&my_tailq_head, item, entries);
        printf("Popped: %d\n", item->value);
    }
}

int main(int argc, char *argv[]){
    struct tailq_entry *item;
    struct tailq_entry *temp_item;
    TAILQ_INIT(&my_tailq_head);

    add_item(100);
    add_item(200);
    add_item(300);

    printq();

    pop_all();

    printq();

    return 0;
}
