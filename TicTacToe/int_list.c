#include <stdbool.h>

//int list
struct int_list{
	int val;
	struct int_list* next;
};

void free_int_list(struct int_list* list) {
	struct int_list* l = list;
	while (list!=NULL) {
		l=list->next;
		free(list);
		list = l;
	}
}

//add elt at the head of the list
void add_head_list(int elt, struct int_list** list) {
	struct int_list* new_list = malloc(sizeof(struct int_list));
	if (new_list==NULL) {
		printf("Mem error\n");
		return;
	}
	new_list->val = elt;
	new_list->next = *list;
	*list = new_list;
}

//remove possible_actions[i] and return its value, i<nb_possible_actions
int del_i_indix_from_list(int i, struct int_list** possible_actions_p) {
	if (i==0) {
		struct int_list* frst = *possible_actions_p;
		int val = frst->val;
		*possible_actions_p=(*possible_actions_p)->next;
		free(frst);
		return val;
	}
	struct int_list* possible_actions = *possible_actions_p;
	for (int j=0;j<i-1;j++) {
		//printf("Marche\n");
		possible_actions=possible_actions->next;
	}
	//printf("Fin for\n");
	int val = (possible_actions->next)->val;
	struct int_list* tmp = possible_actions->next;
	possible_actions->next = (possible_actions->next)->next;
	free(tmp);
	return val;
}

//remove val from possible_actions, val is supposed in possible_actions
void del_val_from_list(int val, struct int_list** possible_actions_p) {
	if (val==(*possible_actions_p)->val) {
		struct int_list* frst = *possible_actions_p;
		*possible_actions_p=(*possible_actions_p)->next;
		free(frst);
	} else {
		struct int_list* possible_actions = *possible_actions_p;
		while ((possible_actions->next)->val != val) {
			possible_actions=possible_actions->next;
		}
		struct int_list* tmp = possible_actions->next;
		possible_actions->next = (possible_actions->next)->next;
		free(tmp);
	}
}

void print_int_list(struct int_list* l) {
	while (l!=NULL) {
		printf("%d ", l->val);
		l = l->next;
	}
	printf("\n");
}

bool is_in_list(int i, struct int_list* l) {
	while (l!=NULL) {
		if (l->val==i) return true;
		l=l->next;
	}
	return false;
}