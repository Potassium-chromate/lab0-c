#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */


struct list_head *merge(struct list_head *left, struct list_head *right, bool descend){
    if(!left || !right){
        return NULL;
    }
    
    struct list_head *temp,*ret;
    bool first_node_flag = true;
    bool cmp_flag;
      
    //deal with if in descend order
    while(left && right){
        cmp_flag = strcmp(list_entry(left, element_t, list)->value, 
        list_entry(right, element_t, list)->value) <= 0;
        if((cmp_flag && !descend) || (!cmp_flag && descend)){
            temp = left->next;     
            if(first_node_flag){
                ret = left;
                INIT_LIST_HEAD(ret);
                first_node_flag = false;
            }
            list_add(left, ret->prev);
            left = temp;
        }else{
            temp = right->next;
            if(first_node_flag){
                ret = right;
                INIT_LIST_HEAD(ret);
                first_node_flag = false;
            }
            list_add(right, ret->prev);
            right = temp;
        }
    }
    
    //if left is not finished yet
    if(left){
        while(left){
            temp = left->next;
            list_add(left, ret->prev);
            left = temp;
        }
    }
    
    //if right is not finished yet
    if(right){
        while(right){
            temp = right->next;
            list_add(right, ret->prev);
            right = temp;
        }
    }
    
    ret->prev->next = NULL;
    ret->prev = NULL;

    return (ret);
}


struct list_head *merge_sort(struct list_head *head, bool descend){
     //the head should not contain head of queue
     if (!head->next){
         return head;
     }
     
     //find the middle node
     struct list_head *slow = head;
     struct list_head *fast = head;
     
     while(fast->next){
          fast = fast->next;
          slow = slow->next;
          if(fast->next) fast = fast->next;
     }
     
     struct list_head *mid = slow;
     if(!mid){
         return NULL;
     }
     slow->prev->next = NULL;
     
     /*
     element_t *mid_node = list_entry(mid, element_t, list);
     printf("mid: %s\n", mid_node->value);
     mid_node = list_entry(head, element_t, list);
     printf("head: %s\n\n\n", mid_node->value);
     */
     struct list_head *left = merge_sort(head, descend);
     struct list_head *right = merge_sort(mid, descend);
     
     return merge(left,right, descend);
}

struct list_head *remove_head(struct list_head *head){
     //remove head and make the linked list become not singular
     if(!head)
        return NULL;
     
     head->prev->next = NULL;
     head->prev = NULL;
     
     struct list_head *next = head->next;
     head->next->prev = NULL;
     head->next = NULL;
     
     return next;
}

struct list_head *connect_head(struct list_head *head, struct list_head *list){
     //connect the head back to the non sigular linked list
     if(!head)
        return NULL;

     head->next = list;
     list->prev = head;
     
     struct list_head *current = head;
     while(current->next){
         current = current->next;
     }
     current->next = head;
     head->prev = current;
     
     return head;
}
/*
 *
 * Native function
 *
*/

struct list_head *q_new()
{    
    //printf("q_new\n");
    struct list_head *new_node = malloc(sizeof(struct list_head));
    if(!new_node){
        return NULL;
    }
    INIT_LIST_HEAD(new_node);
    return new_node;
}

/* Free all storage used by queue */
void q_free(struct list_head *head) {
   //printf("q_free\n");
   if (!head) return;
   struct list_head *current = head->next;
   
   while (current != head) {
        struct list_head *next = current->next;

        element_t *node = list_entry(current, element_t, list);

        // Free the allocated string in the node
        if (node->value) {
            free(node->value);
        }

        // Free the list node itself (the element_t)
        free(node);

        current = next;  // Move to the next element
   }
   free(head);
   return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{   
    if(!head){
        return false;
    }else{
    	element_t *new_node = malloc(sizeof(element_t));
    	if(!new_node){ //free the memeory if allocation fails
    	    return false;
    	}
    	//write the string into element
    	new_node->value = malloc(sizeof(char) * (strlen(s) + 1));
    	if (!new_node->value) {
            free(new_node);  // Free the element if allocation fails
            return false;
    	}
    	strcpy(new_node->value, s);
    	
    	list_add(&new_node->list, head);
		
  	return true;
    }
    
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{   
    if(head){
        return q_insert_head(head->prev, s);
    }
    return false;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{   
    if(!head || list_empty(head)){
        return NULL;
    }
    element_t *rm_element = list_entry(head->next, element_t, list);
    if(sp){
        strncpy(sp, rm_element->value, bufsize - 1);
        sp[bufsize-1] = '\0';
    }
    
    list_del(head->next);
    return rm_element;
    
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if(head){
        return q_remove_head(head->prev->prev, sp, bufsize);
    }
    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{   
    if(head){
        int count = 0;
        struct list_head *current = head->next;
        while(current != head){
            count ++;
            current = current->next;
        }
        return count;
    }
    return -1;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if(!head){
       return false;
    }
    
    struct list_head *fast = head->next->next;
    struct list_head *slow = head->next;
    
    while(fast != head){
    	fast = fast->next;
    	slow = slow->next;
    	if(fast != head){
    	    fast = fast->next;
    	}
    }
    slow->prev->next = slow->next;
    slow->next->prev = slow->prev;
    element_t *node = list_entry(slow, element_t, list);
    
    if (node->value) {
        free(node->value);
    }
    
    free(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if(!head){
       return;
    }
    
    struct list_head *first = head->next;
    struct list_head *second = head->next->next;
    element_t *first_node, *second_node;
    
    
    while(first != head && second != head ){
        first_node = list_entry(first, element_t, list);
        second_node = list_entry(second, element_t, list);
        //swap
        char* temp = first_node->value;
        first_node->value = second_node->value;
        second_node->value = temp;
        //move
        first = first->next->next;
        second = second->next->next;
    }
    return;
    
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {
    printf("reverse\n");
    if(head && !list_empty(head)){
        struct list_head *temp;
        struct list_head *current = head->prev;

        while(current != head){
            temp = current->prev;
            list_del_init(current);
            list_add_tail(current, head);
            current = temp;        
        }
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {
    if(!head){
        return;
    }
    head->prev->next = NULL;
    head->next = merge_sort(head->next,descend);
    head->next->prev = head;
    
    struct list_head *current = head;
    while(current->next){
       current  = current->next;
    }
    current->next = head;   
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */


int q_merge(struct list_head *head, bool descend)
{   printf("merge\n");
    // Check if the chain of queues is empty or has only one queue
    if (head == NULL || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_entry(head, queue_contex_t, chain)->size;
    
    queue_contex_t *first_queue = list_entry(head->next, queue_contex_t, chain);
    
    // Traverse through the remaining queues in the chain and merge them
    struct list_head *pos, *next;
    
    list_for_each_safe(pos, next, head) {
        // Skip the first queue (it’s the target of the merge)
        if (pos == head->next)
            continue;
        
        // Get the current queue context (queue to be merged)
        queue_contex_t *current_queue = list_entry(pos, queue_contex_t, chain);
        
        // Merge current_queue->q into first_queue->q
        //first_queue->q = merge_sorted_queues(first_queue->q, current_queue->q, descend);
        struct list_head *left = first_queue->q;
        left = remove_head(left);
        struct list_head *right = current_queue->q;
        right = remove_head(right);
        
        struct list_head *merged_queue  = merge(left, right, descend);
        
        connect_head(first_queue->q, merged_queue);
        
        // Update the size of the first queue
        first_queue->size += current_queue->size;
        
        // Mark the current queue as empty
        current_queue->q = NULL;
        current_queue->size = 0;
    }
    if(!first_queue){
        printf("first_queue is null\n");
    }else{
        printf("first_queue is not null\n");
    }
        
    
    printf("merge done\n");
    return first_queue->size;  // Return the number of elements in the merged queue
}









