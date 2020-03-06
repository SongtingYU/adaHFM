#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <stack>
#include <queue>

using namespace std;

/**************************************************************
/*function bin2char:
/*	input 8 bit stack with binary code
/*	output ascii code in int.
/*	use pow in c math
/*************************************************************/
int bin2char(stack <int> *code){
    int i, ascii_element = 0, digital;
    
    for (i = 0; i < 8; i++){
    	digital = (int)(*code).top() - 48;				//from ascii code to real input number
    	(*code).pop();
    	ascii_element += digital * (pow(2, i));			//add 2 pow bit into front count change to ascii
    }

    while (!(*code).empty())(*code).pop();				//clean stack
    
    return ascii_element;								//return ascii code
}

/**************************************************************
/*struct Node:
/*	basic struct of one huffman node
/*	include data, weight, address of parent, left and right
/*	with building functions Node
/*************************************************************/
struct Node{
	Node();
    int data;
    int weight;
    struct Node *parent_node, *left_node, *right_node;
};

/**************************************************************
/*building functions Node:
/*	for Node building
/*	init new node with no data, weight and all address to NULL
/*************************************************************/
Node::Node(){
	data = 0;
	weight = 0;
	parent_node = NULL;
	left_node = NULL;
	right_node = NULL;
}

/**************************************************************
/*GLOBAL variables init
/*Fixed root and NYT
/*GLOBAL node point q and leaf
/*GLOBAL stack for follow block search
/*************************************************************/
struct Node *HFM_root = new struct Node();
struct Node *HFM_NYT = new struct Node();
Node *q, *leaf;
stack <struct Node*> stack_follow_block;

/**************************************************************
/*function init huffman tree
/*	give no data, weight and all address to NULL
/*************************************************************/
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

/**************************************************************
/*function search insert element in the huffman tree
/*	give GLOBAL q to the exist node or NYT
/*	return bool flag find_element
/*************************************************************/
bool Search_element(int ins_element){
	queue <struct Node *> search_queue;
	Node *temp_Node;
	bool find_element = false;							//find flag

	search_queue.push(HFM_root);				

	while (!search_queue.empty()){						//BFS search tree
		temp_Node = search_queue.front();
		search_queue.pop();

		if (temp_Node->left_node != NULL){
			search_queue.push(temp_Node->right_node);
			search_queue.push(temp_Node->left_node);
		}
		else{
			if (temp_Node->data == ins_element){
				q = temp_Node;							//find the node q point to the node
				find_element = true;					//set flag
				break;
			}
		}
	}

	if (find_element)return true;						//return bool for next step
	else return false;
}

/**************************************************************
/*function search block exchange leader
/*	according the different location exchange
/*	BFS search first same attributes is leader
/*	change address and update parent relationship
/*************************************************************/
void search_same_block(void){
    queue <struct Node *> temp_queue;
    Node *search_node;
    temp_queue.push(HFM_root);

    while (!temp_queue.empty()){														//BFS
        search_node = temp_queue.front();
        temp_queue.pop();

        if (search_node->left_node != NULL){
            temp_queue.push(search_node->right_node);
            temp_queue.push(search_node->left_node);
        }

        if ((search_node->left_node == NULL) && (search_node->weight == q->weight)){	//BFS leaf node and same weight first find means leader
            Node *a_parent;

            a_parent = search_node->parent_node;

            if (a_parent != q->parent_node) {											//different parent
                if (a_parent->left_node == search_node)a_parent->left_node = q;
                else a_parent->right_node = q;

                if (q->parent_node->left_node == q)q->parent_node->left_node = search_node;
                else q->parent_node->right_node = search_node;

                search_node->parent_node = q->parent_node;
                q->parent_node = a_parent;
            }
            else{																		//same parent
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

            break;																		//only change leader
        }
    }

    return;
}

/**************************************************************
/*function search follow block
/*	twice BFS search
/*		1. get follow block element attributes
/*		2. use stack save follow block
/*	save to GLOBAL stack
/*************************************************************/
void search_follow_block(Node *aim_node){
    queue <struct Node *> temp_queue;

    while (!stack_follow_block.empty())stack_follow_block.pop();	//clean GLOBAL stack

    Node *search_node, *follow_node = NULL;
    temp_queue.push(HFM_root);

    while (!temp_queue.empty()){									//first BFS
        search_node = temp_queue.front();
        temp_queue.pop();

        if (search_node->left_node != NULL){
            temp_queue.push(search_node->right_node);
            temp_queue.push(search_node->left_node);
        }

        if (temp_queue.front() == aim_node){
            follow_node = search_node;								//get sample node attributes
            break;
        }
    }

    while (!stack_follow_block.empty())stack_follow_block.pop();	//clean GLOBAL stack

    temp_queue.push(HFM_root);

    while (!temp_queue.empty()){									//second BFS
        search_node = temp_queue.front();
        temp_queue.pop();

        if (search_node->left_node != NULL){
            temp_queue.push(search_node->right_node);
            temp_queue.push(search_node->left_node);
        }

        if ((search_node->left_node != NULL) && (follow_node->left_node != NULL)){
            if (search_node->weight == follow_node->weight)stack_follow_block.push(search_node);	//same attributes push to stack
        }
        else if((search_node->left_node == NULL) && (follow_node->left_node == NULL)){
            if (search_node->weight == follow_node->weight)stack_follow_block.push(search_node);	//same attributes push to stack
        }
    }

    return;
}

/**************************************************************
/*function swap node
/*	slide q from out end of block to the first
/*	use GLOBAL stack block
/*************************************************************/
Node** block_node_swap(Node **a){
    Node *a_parent, *temp_node;

    while (!stack_follow_block.empty()) {									//stack could auto reverse saved node, change all
        temp_node = stack_follow_block.top();
        stack_follow_block.pop();

        a_parent = (*a)->parent_node;

        if (temp_node->parent_node != a_parent) {							//same parent exchange
            if (a_parent->left_node == *a)a_parent->left_node = temp_node;
            else a_parent->right_node = temp_node;


            if (temp_node->parent_node->left_node == temp_node)temp_node->parent_node->left_node = *a;
            else temp_node->parent_node->right_node = *a;

            (*a)->parent_node = temp_node->parent_node;
            temp_node->parent_node = a_parent;
        }
        else{																//different parent exchange
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

/**************************************************************
/*function slide and adjust huffman tree to balance
/*	input parameter to distinguish exchange q or leaf
/*	no return just adjust before return
/*************************************************************/
void SlideAndIncrement(int parameter){
    Node *Slide_node;

    if (parameter == 1)Slide_node = q;								//set exchange mode q or leaf
    else Slide_node = leaf;

    Node *former_temp_Node = Slide_node->parent_node;				//save former parent node

    search_follow_block(Slide_node);								//get follow block

    if (((Slide_node->left_node == NULL) && (stack_follow_block.top()->left_node != NULL)		//make sure the slide is necessary or not
         && (stack_follow_block.top()->weight == Slide_node->weight))
        || ((Slide_node->left_node != NULL) && (stack_follow_block.top()->left_node == NULL)
            && (stack_follow_block.top()->weight == Slide_node->weight + 1))){

        if (!stack_follow_block.empty())Slide_node = *(block_node_swap(&Slide_node));			//swap two nodes

        Slide_node->weight += 1;

        if (Slide_node->left_node == NULL)Slide_node = Slide_node->parent_node;					//set next q or leaf for adjust
        else Slide_node = former_temp_Node;
    }
    else{
        Slide_node->weight += 1;																//update weight
        Slide_node = Slide_node->parent_node;
    }

    q = Slide_node;

    return;
}

/**************************************************************
/*function insert element to huffman tree
/*	input element from type or documents one by one
/*	both encode and decode would use
/*************************************************************/
void Ins_huffman(int ins_element){
	leaf = 0;

	if (!Search_element(ins_element))q = HFM_NYT;					//when input element set q

	if (q == HFM_NYT){												//new element
		struct Node *new_Node_2 = new struct Node();

		if (HFM_root->left_node == NULL){							//NYT: insert and update relationship
			HFM_root->left_node = HFM_NYT;
            HFM_root->right_node = new_Node_2;
			HFM_NYT->parent_node = HFM_root;
			new_Node_2->parent_node = HFM_root;
			new_Node_2->data = ins_element;

			q = HFM_root;
		}
		else{														//new element: insert and update relationship
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
	else{															//exist element: insert and update relationship
        search_same_block();										//exchange to leader
        Node* full_hum = HFM_root;

        if (q->parent_node == HFM_NYT->parent_node){				//according relationship with NYT set leaf and q
            leaf = q;
            q = leaf->parent_node;
        }
	}

	while(q != HFM_root)SlideAndIncrement(1);						//update all q data in huffman tree

    q->weight += 1;													//update root weight

    if (leaf != 0)SlideAndIncrement(2);								//update all leaf data in huffman tree

    return;
}

/**************************************************************
/*main function for decode
/*	input parameter for read huffman code
/*	output UTF-8 char to display
/*************************************************************/
int main(int argc, char *argv[]){
    int i, input_code, element;
    string path = "";
    Node *search_node;
    stack <int> bin_ascii_code;

    init_huffman();													//init huffman tree

    while ((input_code = getchar()) != -1){							//read EOF
        if (input_code != '\n'){									//read Enter
        	if (input_code == ' ')continue;							//ignore space in decode

        	if (HFM_root->left_node == NULL){						//first input 8-bit huffman code
        		if (bin_ascii_code.size() < 7){
        			bin_ascii_code.push(input_code);
        		}
        		else {
        			bin_ascii_code.push(input_code);
        			stack <int> *full_bit_code = &bin_ascii_code;
        			element = bin2char(full_bit_code);
        			Ins_huffman(element);
        			cout << (char)element;							//output UTF-8 code
        		}
        	}
        	else{													//next read one by one for path record
        		if (bin_ascii_code.empty())path += (char)input_code;

        		search_node = HFM_root;
        		for (i = 0; i < path.length(); i++){				//search tree element with input path
        			if (path[i] == '0')search_node = search_node->left_node;
        			else search_node = search_node->right_node;
        		}

        		if (search_node->left_node != NULL)continue;		//not leaf node continue read
        		else {
        			if (search_node == HFM_NYT){					//NYT node read 8-bit for new element
        				if (bin_ascii_code.size() < 8)bin_ascii_code.push(input_code);
        				else {
        					bin_ascii_code.push(input_code);
        					stack <int> *full_bit_code = &bin_ascii_code;
        					element = bin2char(full_bit_code);
        					Ins_huffman(element);
        					cout << (char)element;					//output new element
        					path.clear();
        				}
        			}
        			else{											//output huffman code compression element
        				element = search_node->data;
        				Ins_huffman(element);
        				cout << (char)element;
        				path.clear();
        			}
        			
        		}
        	}
        }
        else{														
        	cout << endl;											//new line
        	init_huffman();											//reset huffman tree
        }
    }

    return 0;
}