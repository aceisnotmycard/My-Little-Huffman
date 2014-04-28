#include "huffman.h"


//Function for make_list
Node *create_node(int ch, size_t weight) {
	Node *result = (Node *) malloc(sizeof(Node));
	result->ch = ch;
	result->weight = weight;
    result->next = NULL;
    result->left = NULL;
    result->right = NULL;
	return result;
}


void insert_node (Node *where, Node *inserting) {
	inserting->next = where->next;
	where->next = inserting;
}


int insert_node_in_list(Node *head, Node *inserting) {
	if (inserting->weight <= head->weight) {

		size_t tmp_weight = head->weight;
        int tmp_ch = head->ch;
        Node *tmp_left = head->left;
        Node *tmp_right = head->right;

		head->weight = inserting->weight;
        head->ch = inserting->ch;
		inserting->next = head->next;
		head->next = inserting;
        head->left = inserting->left;
        head->right = inserting->right;

        inserting->left = tmp_left;
        inserting->right = tmp_right;
		inserting->weight = tmp_weight;
        inserting->ch = tmp_ch;
		return 0;

	}

	Node *p = head;

	while (p->next != NULL && inserting->weight > p->next->weight)
		p = p->next;
	insert_node (p, inserting);

	return 0;
}


size_t *count_frequency(FILE *file) {
    size_t *table_of_frequencies = (size_t *) calloc(ALPHABET, sizeof(size_t));
    int ch;

    while ((ch = fgetc(file)) != EOF) {
        table_of_frequencies[ch]++;
    }

    return table_of_frequencies;
}


Node *make_list(size_t *table) {
	Node *head = create_node(0,-1);
    int i;

    for(i = 0; i < ALPHABET; i++) {
        if (table[i] != 0) {
            if(head->weight == -1) {
                head = create_node(i, table[i]);
            } else {
                insert_node_in_list(head, create_node(i, table[i]));
            }
        }
    }
    return head;
}


Node *make_huffman_tree(Node *current) {
    Node *branch = create_node(ALPHABET, 0);
    branch->left = current;
    current = current->next;
    branch->right = current;
    branch->weight = branch->left->weight + branch->right->weight;
    branch->next = NULL;
    if (current->next == NULL)
        return branch;
    insert_node_in_list(current, branch);
    return make_huffman_tree(current->next);
}


void create_coding_table(Node *node, char *prev, Code *table) {
	if (node != NULL) {
        if (node->ch < ALPHABET) {
            strcpy(table[node->ch].code, prev);
            table[node->ch].size = strlen(prev);
        } else {
            if (node->left) {
                char s1[TREE_HEIGHT];
                strcpy(s1, prev);
                create_coding_table(node->left, strcat(s1, "0"), table);
            }
            if (node->right) {
                char s1[TREE_HEIGHT];
                strcpy(s1, prev);
                create_coding_table(node->right, strcat(s1, "1"), table);
            }
        }
    }
}


Node *create_huffman_tree(FILE *file) {
	Node *head = make_list(count_frequency(file));

	return make_huffman_tree(head);
}
