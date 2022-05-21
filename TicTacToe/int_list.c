#include "int_list.h"

void free_int_list(struct int_list* list) {
	struct int_list* l = list;
	while (list!=NULL) {
		l=list->next;
		free(list);
		list = l;
	}
}

void add_head_list(int elt, struct int_list** list) {
	struct int_list* new_list = malloc(sizeof(struct int_list));
	if (new_list==NULL) {
		printf("Erreur mémoire\n");
		return;
	}
	new_list->val = elt;
	new_list->next = *list;
	*list = new_list;
}

int del_i_indix_from_list(int i, struct int_list** p_list) {
	if (i==0) {
		struct int_list* frst = *p_list;
		int val = frst->val;
		*p_list=(*p_list)->next;
		free(frst);
		return val;
	}
	struct int_list* list = *p_list;
	for (int j=0;j<i-1;j++) {
		list=list->next;
	}
	int val = (list->next)->val;
	struct int_list* tmp = list->next;
	list->next = (list->next)->next;
	free(tmp);
	return val;
}

void del_val_from_list(int val, struct int_list** p_list) {
	if (val==(*p_list)->val) {
		struct int_list* frst = *p_list;
		*p_list=(*p_list)->next;
		free(frst);
	} else {
		struct int_list* list = *p_list;
		while ((list->next)->val != val) {
			list=list->next;
		}
		struct int_list* tmp = list->next;
		list->next = (list->next)->next;
		free(tmp);
	}
}

void print_int_list(struct int_list* list) {
	while (list!=NULL) {
		printf("%d ", list->val);
		list = list->next;
	}
	printf("\n");
}

bool is_in_list(int elt, struct int_list* list) {
	while (list!=NULL) {
		if (list->val==elt) return true;
		list=list->next;
	}
	return false;
}