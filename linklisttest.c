#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) -offsetof(type, member)))

struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

typedef struct {
    char *value;
    struct list_head list;
} element_t;


static inline void INIT_LIST_HEAD(struct list_head *head)
{
    head->next = head;
    head->prev = head;
}


static inline void list_add(struct list_head *node, struct list_head *head)
{
    struct list_head *next = head->next;
    next->prev = node;
    node->next = next;
    node->prev = head;
    head->next = node;
}


struct list_head *q_new()
{
    struct list_head *new_qhead =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!new_qhead) {
        return NULL;
    }
    INIT_LIST_HEAD(new_qhead);
    return new_qhead;
}

bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *new_qelement = (element_t *) malloc(sizeof(element_t));
    if (!new_qelement) {
        return false;
    }
    new_qelement->value = strdup(s);
    if (!new_qelement->value) {
        free(new_qelement);
        return false;
    }
    list_add(&new_qelement->list, head);
    return true;
}


char *generate_random_string(int len)
{
    char *str = malloc(len + 1);
    if (!str) {
        return NULL;
    }
    // ascii range: 48 ~ 122
    for (int i = 0; i < len; i++) {
        char key = rand() % (75) + 48;
        str[i] = key;
    }
    str[len] = '\0';
    return str;
}
struct list_head *merge(struct list_head *l1,
                        struct list_head *l2,
                        bool descend)
{
    if (!l2)
        return l1;
    if (!l1)
        return l2;


    struct list_head dummy;
    struct list_head *temp = &dummy;
    dummy.next = NULL;
    dummy.prev = NULL;

    while (l1 && l2) {
        element_t *node1 = container_of(l1, element_t, list);
        element_t *node2 = container_of(l2, element_t, list);

        if (!descend) {
            if (strcmp(node1->value, node2->value) <= 0) {
                temp->next = l1;
                temp = temp->next;
                l1 = l1->next;
            } else {
                temp->next = l2;
                temp = temp->next;
                l2 = l2->next;
            }
        } else {
            if (strcmp(node1->value, node2->value) < 0) {
                temp->next = l2;
                temp = temp->next;
                l2 = l2->next;
            } else {
                temp->next = l1;
                temp = temp->next;
                l1 = l1->next;
            }
        }
    }

    if (l1)
        temp->next = l1;
    if (l2)
        temp->next = l2;

    return dummy.next;
}

struct list_head *mergeSortList(struct list_head *head, bool descend)
{
    if (!head || !head->next) {
        return head;
    }

    struct list_head *fast = head->next;
    struct list_head *slow = head;

    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;


    // sort each list
    struct list_head *l1 = mergeSortList(head, descend);
    struct list_head *l2 = mergeSortList(fast, descend);

    // merge sorted l1 and sorted l2
    return merge(l1, l2, descend);
}


void q_sort(struct list_head *head, bool descend)
{
    struct list_head *temp = head;
    head->prev->next = NULL;
    head->next = mergeSortList(head->next, descend);

    struct list_head *curr = head->next;

    while (curr->next != NULL) {
        curr->prev = temp;
        temp = curr;
        curr = curr->next;
    }
    curr->prev = temp;
    curr->next = head;
    head->prev = curr;
}

void print_list(struct list_head *head, int max_print)
{
    struct list_head *pos;
    int count = 0;
    for (pos = head->next; pos != head && count < max_print; pos = pos->next) {
        element_t *elem = container_of(pos, element_t, list);
        printf("Element %d: %s\n", count, elem->value);
        count++;
    }
}
typedef int (*list_cmp_func_t)(void *,
                               const struct list_head *,
                               const struct list_head *);

static struct list_head *list_merge(void *priv,
                                    list_cmp_func_t cmp,
                                    struct list_head *a,
                                    struct list_head *b);

static void merge_final(void *priv,
                        list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *l1,
                        struct list_head *l2);



int cmp(void *priv, const struct list_head *a, const struct list_head *b)
{
    element_t *node_a = container_of(a, element_t, list);
    element_t *node_b = container_of(b, element_t, list);

    return strcmp(node_a->value, node_b->value);
}

void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = list_merge(priv, cmp, b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);
    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = list_merge(priv, cmp, pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(priv, cmp, head, pending, list);
}

static struct list_head *list_merge(void *priv,
                                    list_cmp_func_t cmp,
                                    struct list_head *a,
                                    struct list_head *b)
{
    // cppcheck-suppress unassignedVariable
    struct list_head *head, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void merge_final(void *priv,
                        list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;
    unsigned char count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        /*
         * If the merge is highly unbalanced (e.g. the input is
         * already sorted), this loop may run many iterations.
         * Continue callbacks to the client even though no
         * element comparison is needed, so the client's cmp()
         * routine can invoke cond_resched() periodically.
         */
        if (unlikely(!++count))
            cmp(priv, b, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

int main(void)
{
    srand((unsigned) time(NULL));

    struct list_head *queue = q_new();
    if (!queue) {
        fprintf(stderr, "Failed to create list.\n");
        return 1;
    }

    int num_elements = 45000;
    int str_length = 10;

    for (int i = 0; i < num_elements; i++) {
        char *rand_str = generate_random_string(str_length);
        if (!rand_str) {
            fprintf(stderr, "Failed to generate random string.\n");
            return 1;
        }
        if (!q_insert_head(queue, rand_str)) {
            fprintf(stderr, "Failed to insert element.\n");
            free(rand_str);
            return 1;
        }
        free(rand_str);
    }
    q_sort(queue, false);
    // list_sort(0, queue, cmp);
    return 0;
}
