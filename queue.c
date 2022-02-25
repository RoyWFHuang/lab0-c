#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    // struct list_head *head = malloc(sizeof(struct list_head));
    // if (NULL == head)
    //     return NULL;
    // memset(head, 0, sizeof(struct list_head));
    struct list_head *head = calloc(1, sizeof(struct list_head));
    if (NULL == head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (NULL == l)
        return;
    while (!list_empty(l)) {
        struct list_head *del_node = l->next;
        list_del(del_node);
        element_t *e = container_of(del_node, element_t, list);
        free(e->value);
        free(e);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (NULL == head)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (NULL == e)
        return false;
    memset(e, 0, sizeof(element_t));
    e->value = calloc(1, (strlen(s) + 1) * sizeof(char));
    if (NULL == e->value) {
        free(e);
        return false;
    }
    // memset(e->value, 0, (strlen(s) + 1) * sizeof(char));
    memcpy(e->value, s, sizeof(char) * strlen(s));
    list_add(&e->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (NULL == head)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (NULL == e)
        return false;
    memset(e, 0, sizeof(element_t));
    e->value = malloc((strlen(s) + 1) * sizeof(char));
    if (NULL == e->value) {
        free(e);
        return false;
    }
    memset(e->value, 0, (strlen(s) + 1) * sizeof(char));
    memcpy(e->value, s, sizeof(char) * strlen(s));
    list_add_tail(&e->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (NULL == head || NULL == sp)
        return NULL;
    if (list_empty(head))
        return NULL;
    element_t *e = list_first_entry(head, element_t, list);
    if (NULL == e)
        return NULL;
    list_del(&(e->list));

    memset(sp, 0, sizeof(char) * bufsize);
    strncpy(sp, e->value, bufsize - 1);
    return e;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (NULL == head || NULL == sp)
        return NULL;
    if (list_empty(head))
        return NULL;
    element_t *e = list_last_entry(head, element_t, list);

    list_del(&(e->list));
    memset(sp, 0, sizeof(char) * bufsize);
    strncpy(sp, e->value, bufsize - 1);
    return e;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (NULL == head)
        return 0;
    int len = 0;
    struct list_head *node;
    list_for_each (node, head) {
        len++;
    }
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // if (list_is_singular(head)) {
    //     list_del(head);
    //     q_release_element(container_of(head, element_t, list));
    // }
    int tar_len = q_size(head) / 2;
    struct list_head *node = NULL;
    list_for_each (node, head) {
        if (0 == tar_len) {
            element_t *e = container_of(node, element_t, list);
            list_del(node);
            q_release_element(e);
            break;
        }
        tar_len--;
    }
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    if (list_empty(head))
        return true;
    struct list_head *node = head->next, *chk;
    if ((chk = node->next) == head)
        return true;

    while (chk != head) {
        struct list_head *kep;
        kep = chk = node->next;
        while (chk != head &&
               !strcmp(container_of(node, element_t, list)->value,
                       container_of(chk, element_t, list)->value)) {
            element_t *e = container_of(chk, element_t, list);
            list_del(chk);
            q_release_element(e);
            chk = node->next;
        }
        if (chk != kep) {
            element_t *e = container_of(node, element_t, list);
            list_del(node);
            q_release_element(e);
        }
        node = chk;
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *swp1 = head->next, *swp2;

    while (swp1 != head && swp1->next != head) {
        swp2 = swp1->next;
        char *tmp = container_of(swp1, element_t, list)->value;
        container_of(swp1, element_t, list)->value =
            container_of(swp2, element_t, list)->value;
        container_of(swp2, element_t, list)->value = tmp;
        swp1 = swp2->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (NULL == head || list_empty(head))
        return;

    struct list_head *hd = head->next;
    struct list_head *ta = head->prev;
    while (hd != ta) {
        char *tmp = container_of(hd, element_t, list)->value;
        container_of(hd, element_t, list)->value =
            container_of(ta, element_t, list)->value;
        container_of(ta, element_t, list)->value = tmp;
        if (ta == hd->next)
            break;
        ta = ta->prev;
        hd = hd->next;
    }
}



static struct list_head *mergeTwoLists(struct list_head *l1,
                                       struct list_head *l2)
{
    if (NULL == l2 && NULL == l1)
        return NULL;

    if (NULL == l1)
        return l2;
    else if (NULL == l2)
        return l1;

    struct list_head *ret_list = NULL, *tmp_list;
    if (strcmp(container_of(l1, element_t, list)->value,
               container_of(l2, element_t, list)->value) < 0) {
        ret_list = l1;
        l1 = l1->next;
    } else {
        ret_list = l2;
        l2 = l2->next;
    }
    tmp_list = ret_list;
    while (l1 && l2) {
        if (strcmp(container_of(l1, element_t, list)->value,
                   container_of(l2, element_t, list)->value) < 0) {
            tmp_list->next = l1;
            l1 = l1->next;
        } else {
            tmp_list->next = l2;
            l2 = l2->next;
        }
        tmp_list = tmp_list->next;
    }
    tmp_list->next = (struct list_head *) ((u_int64_t) l1 | (u_int64_t) l2);
    return ret_list;
}

static struct list_head *m_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *fast = head, *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }

    slow->prev->next = NULL;
    struct list_head *l1 = m_sort(head);
    struct list_head *l2 = m_sort(slow);
    return mergeTwoLists(l1, l2);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    head->prev->next = NULL;
    head->next = m_sort(head->next);

    struct list_head *target = head;
    while (target->next) {
        target->next->prev = target;
        target = target->next;
    }

    head->prev = target;
    target->next = head;
}
