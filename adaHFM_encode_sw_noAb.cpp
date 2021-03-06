#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <stack>
#include <queue>

using namespace std;

int char2bin(int ascii_num){
    int i = 0, binary_data[8] = {0, 0, 0, 0, 0, 0, 0, 0}, binary_code = 0;
    while(ascii_num > 0) {
        binary_data[i] = ascii_num % 2;
        ascii_num = ascii_num / 2;
        i++;
    }
    for(i = 7; i >= 0; i--)binary_code = binary_code * 10 + binary_data[i];
    return binary_code;
}

struct Node{
	Node();
    int data;
    int weight;
    struct Node *parent_node, *left_node, *right_node;
};

Node::Node(){
	data = 0;
	weight = 0;
	parent_node = NULL;
	left_node = NULL;
	right_node = NULL;
}

struct Node *HFM_root = new struct Node();
struct Node *HFM_NYT = new struct Node();

Node *q, *leaf;

stack <struct Node*> stack_internal_block;
stack <struct Node*> stack_leaf_block;
stack <struct Node*> stack_leaf_block_one_plus;

stack <struct Node*> stack_follow_block;

void init_huffman(){
	HFM_root->data = 0;
	HFM_root->weight = 0;
	HFM_root->parent_node = NULL;
	HFM_root->left_node = NULL;
	HFM_root->right_node = NULL;

	HFM_NYT->data = 0;
	HFM_NYT->weight = 0;
	HFM_NYT->parent_node = NULL;
	HFM_NYT->left_node = NULL;
	HFM_NYT->right_node =NULL;

	return;
}

void Print_Path(Node *node_position){
	stack <int> path;
	Node *temp_Node;
	temp_Node = node_position;

	while (temp_Node != HFM_root){
		if (temp_Node == temp_Node->parent_node->left_node)path.push(0);
		else path.push(1);

		temp_Node = temp_Node->parent_node;
	}

	while (!path.empty()){
		cout<< path.top();
		path.pop();
	}

	return;
}

bool Search_element(int ins_element){
	queue <Node *> search_queue;
	Node *temp_Node;
	bool find_element = false;

	search_queue.push(HFM_root);

	while (!search_queue.empty()){
		temp_Node = search_queue.front();
		search_queue.pop();

		if (temp_Node->left_node != NULL){
			search_queue.push(temp_Node->right_node);
			search_queue.push(temp_Node->left_node);
		}
		else{
			if (temp_Node->data == ins_element){
				q = temp_Node;
				find_element = true;
				break;
			}
		}
	}

	if (find_element){
		Print_Path(q);
		return true;
	}
	else{
        if (HFM_NYT->parent_node != NULL)Print_Path(HFM_NYT);

        cout << setw(8) << setfill('0') << char2bin(ins_element);
		return false;
	}
}

void search_same_block(void){
    queue <Node *> temp_queue;
    Node *search_node;
    temp_queue.push(HFM_root);

    while (!temp_queue.empty()){
        search_node = temp_queue.front();
        temp_queue.pop();

        if (search_node->left_node != NULL){
            temp_queue.push(search_node->right_node);
            temp_queue.push(search_node->left_node);
        }

        if ((search_node->left_node == NULL) && (search_node->weight == q->weight)){
            Node *a_parent;

            a_parent = search_node->parent_node;

            if (a_parent != q->parent_node) {
                if (a_parent->left_node == search_node)a_parent->left_node = q;
                else a_parent->right_node = q;

                if (q->parent_node->left_node == q)q->parent_node->left_node = search_node;
                else q->parent_node->right_node = search_node;

                search_node->parent_node = q->parent_node;
                q->parent_node = a_parent;
            }
            else{
                if (search_node != q){
                    if (a_parent->left_node == search_node){
                        a_parent->left_node = q;
                        a_parent->right_node = search_node;
                    }
                    else{
                        a_parent->left_node = search_node;
                        a_parent->right_node = q;
                    }
                }
            }

            break;
        }
    }

    return;
}

void search_follow_block(Node *aim_node){
    queue <Node *> temp_queue;

    while (!stack_follow_block.empty())stack_follow_block.pop();

    Node *search_node, *follow_node = NULL;
    temp_queue.push(HFM_root);

    while (!temp_queue.empty()){
        search_node = temp_queue.front();
        temp_queue.pop();

        if (search_node->left_node != NULL){
            temp_queue.push(search_node->right_node);
            temp_queue.push(search_node->left_node);
        }

        if (temp_queue.front() == aim_node){
            follow_node = search_node;
            break;
        }
    }

    while (!stack_follow_block.empty())stack_follow_block.pop();

    temp_queue.push(HFM_root);

    while (!temp_queue.empty()){
        search_node = temp_queue.front();
        temp_queue.pop();

        if (search_node->left_node != NULL){
            temp_queue.push(search_node->right_node);
            temp_queue.push(search_node->left_node);
        }

        if ((search_node->left_node != NULL) && (follow_node->left_node != NULL)){
            if (search_node->weight == follow_node->weight)stack_follow_block.push(search_node);
        }
        else if((search_node->left_node == NULL) && (follow_node->left_node == NULL)){
            if (search_node->weight == follow_node->weight)stack_follow_block.push(search_node);
        }
    }

    return;
}

Node** block_node_swap(Node **a){
    Node *a_parent, *temp_node;

    while (!stack_follow_block.empty()) {
        temp_node = stack_follow_block.top();
        stack_follow_block.pop();

        a_parent = (*a)->parent_node;

        if (temp_node->parent_node != a_parent) {
            if (a_parent->left_node == *a)a_parent->left_node = temp_node;
            else a_parent->right_node = temp_node;


            if (temp_node->parent_node->left_node == temp_node)temp_node->parent_node->left_node = *a;
            else temp_node->parent_node->right_node = *a;

            (*a)->parent_node = temp_node->parent_node;
            temp_node->parent_node = a_parent;
        }
        else{
            if (a_parent->left_node == (*a)){
                a_parent->left_node = temp_node;
                a_parent->right_node = (*a);
            }
            else{
                a_parent->left_node = (*a);
                a_parent->right_node = temp_node;
            }
        }

    }

    return a;
}

void SlideAndIncrement(int parameter){
    Node *Slide_node;

    if (parameter == 1)Slide_node = q;
    else Slide_node = leaf;

    Node *former_temp_Node = Slide_node->parent_node;

    search_follow_block(Slide_node);

    if (((Slide_node->left_node == NULL) && (stack_follow_block.top()->left_node != NULL)
         && (stack_follow_block.top()->weight == Slide_node->weight))
        || ((Slide_node->left_node != NULL) && (stack_follow_block.top()->left_node == NULL)
            && (stack_follow_block.top()->weight == Slide_node->weight + 1))){

        if (!stack_follow_block.empty())Slide_node = *(block_node_swap(&Slide_node));

        Slide_node->weight += 1;

        if (Slide_node->left_node == NULL)Slide_node = Slide_node->parent_node;
        else Slide_node = former_temp_Node;
    }
    else{
        Slide_node->weight += 1;
        Slide_node = Slide_node->parent_node;
    }

    q = Slide_node;

    return;
}

void Ins_huffman(int ins_element){
	leaf = 0;

	if (!Search_element(ins_element))q = HFM_NYT;

	if (q == HFM_NYT){
		struct Node *new_Node_2 = new struct Node();

		if (HFM_root->left_node == NULL){
			HFM_root->left_node = HFM_NYT;
            HFM_root->right_node = new_Node_2;
			HFM_NYT->parent_node = HFM_root;
			new_Node_2->parent_node = HFM_root;
			new_Node_2->data = ins_element;

			q = HFM_root;
		}
		else{
			struct Node *new_Node_1 = new struct Node();

			new_Node_1->parent_node = HFM_NYT->parent_node;
			new_Node_2->parent_node = new_Node_1;
            new_Node_1->parent_node->left_node = new_Node_1;
            new_Node_1->right_node = new_Node_2;
            new_Node_1->left_node = HFM_NYT;
			HFM_NYT->parent_node = new_Node_1;
			new_Node_2->data = ins_element;

			q = new_Node_1;
		}

		leaf = new_Node_2;
	}
	else{
        search_same_block();
        Node* full_hum = HFM_root;

        if (q->parent_node == HFM_NYT->parent_node){
            leaf = q;
            q = leaf->parent_node;
        }

        Node* q_ghost = q;
        Node* q_ghost1 = q;
	}

	while(q != HFM_root){
        Node *temp = q;
        SlideAndIncrement(1);
    }

    q->weight += 1;

    if (leaf != 0)SlideAndIncrement(2);

    return;
}

int main(int argc, char *argv[]){
    int input_char;
    init_huffman();

    while ((input_char = getchar()) != -1){
        
        if (input_char != '\n'){
            if (HFM_root->left_node != NULL)if ((argc > 1) && (!strcmp(argv[1], "-s")))cout << " ";
            Ins_huffman(input_char);
        }
        else{
            cout << endl;
            init_huffman();
        }
    }

    return 0;
}