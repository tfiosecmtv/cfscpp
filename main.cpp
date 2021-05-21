#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;
class Process {
public:
    int number;
    int nice;
    int burst_time_ms;
    int arrival_time_ms;
    float vruntime = 0;
    float weight = 0;
    int exectime = 0;
    int time_slice;
    int preempted = 0;
    int waitingqueue = 0;
    int responsetime = 0;

    int lastinserted;
    int currexec;
} ;

class RBTreeNode {
public:
    RBTreeNode* left;
    RBTreeNode* right;
    RBTreeNode* parent;
    char color;
    Process* proc;

    RBTreeNode(RBTreeNode *left, RBTreeNode *right, RBTreeNode *parent, char color, Process *proc) : left(left),
                                                                                                     right(right),
                                                                                                     parent(parent),
                                                                                                     color(color),
                                                                                                     proc(proc) {}

};

class RBTree {
public:
    RBTreeNode *root = NULL;
    RBTreeNode *nullnode = new RBTreeNode(NULL, NULL, NULL, 'b', NULL);

    RBTreeNode *crnode(Process *p, RBTreeNode *par, RBTreeNode *left, RBTreeNode *right) { //done
        return new RBTreeNode(left, right, par, 'r', p);
    }

    RBTreeNode *rotateleft(RBTreeNode *node) { //done
        RBTreeNode *t = node->right;
        t->parent = node->parent;
        node->right = t->left;
        if (node->right != nullnode)
            node->right->parent = node;
        t->left = node;
        node->parent = t;

        if (t->parent != nullnode) {
            if (node == t->parent->left)
                t->parent->left = t;
            else t->parent->right = t;
        } else root = t;
        return t;
    }

    RBTreeNode *rotateright(RBTreeNode *node) { //done
        RBTreeNode *t = node->left;
        t->parent = node->parent;
        node->left = t->right;
        if (node->left != nullnode)
            node->left->parent = node;
        t->right = node;
        node->parent = t;

        if (t->parent != nullnode) {
            if (node == t->parent->left)
                t->parent->left = t;
            else t->parent->right = t;
        } else root = t;
        return t;
    }


    RBTreeNode *insert(Process *p) { //done
        if (root == NULL) {
            root = crnode(p, NULL, NULL, NULL);
            return root;
        }
        RBTreeNode *parent = NULL;
        RBTreeNode *t = root;
        while (t != NULL && t->proc != NULL) {
            parent = t;
            if (p->vruntime < t->proc->vruntime)
                t = t->left;
            else t = t->right;
        }
        RBTreeNode *newnode = crnode(p, parent, NULL, NULL);
        if (parent->proc->vruntime > newnode->proc->vruntime)
            parent->left = newnode;
        else parent->right = newnode;
        newnode->parent = parent;
        return newnode;
    }

    RBTreeNode *ins(Process *p) { //done
        RBTreeNode *node = insert(p);
        node->left = nullnode;
        node->right = nullnode;
        if(node == root)
            node->parent = nullnode;
        balanceinsertion(node);
        return node;
    }

    void balanceinsertion(RBTreeNode *node) { //done
        while (node->parent != root && node->parent->color == 'r') {
            RBTreeNode *parent = node->parent;
            RBTreeNode *grandparent = node->parent->parent;
            if (parent == grandparent->left) {
                RBTreeNode *temp = grandparent->right;
                if (temp->color == 'r') {
                    parent->color = 'b';
                    temp->color = 'b';
                    grandparent->color = 'r';
                    node = grandparent;
                } else {
                    if (node == parent->right) {

                        node = parent;
                        rotateleft(node);
                        parent = node->parent;
                    }
                    parent->color = 'b';
                    grandparent->color = 'r';
                    rotateright(grandparent);
                }
            } else if (parent == grandparent->right) {
                RBTreeNode *temp = grandparent->left;
                if (temp->color == 'r') {
                    parent->color = 'b';
                    temp->color = 'b';
                    grandparent->color = 'r';
                    node = grandparent;
                } else {
                    if (node == parent->left) {
                        node = parent;
                        rotateright(node);
                        parent = node->parent;
                    }
                    parent->color = 'b';
                    grandparent->color = 'r';
                    rotateleft(grandparent);
                }
            }
        }
        root->color = 'b';
    }

    RBTreeNode *exchange(RBTreeNode *v, RBTreeNode *u) { //done
        if (v->parent == nullnode)
            root = u;
        else if (v == v->parent->left)
            v->parent->left = u;
        else v->parent->right = u;
        u->parent = v->parent;
        return u;
    }

    RBTreeNode *balancedeletion(RBTreeNode *node) { //done
        while (node != root && node->color == 'b') {
            if (node == node->parent->left) {
                RBTreeNode *temp = node->parent->right;
                if (temp->color == 'r') {
                    temp->color = 'b';
                    node->parent->color = 'r';
                    rotateleft(node->parent);
                    temp = node->parent->right;
                }
                if (temp->left->color == 'b' && temp->right->color == 'b') {
                    temp->color = 'r';
                    node = node->parent;
                } else if (temp != nullnode) {
                    if (temp->right->color == 'b') {
                        temp->left->color = 'b';
                        temp->color = 'r';
                        rotateright(temp);
                        temp = node->parent->right;
                    }
                    temp->color = node->parent->color;
                    node->parent->color = 'b';
                    temp->right->color = 'b';
                    rotateleft(node->parent);
                    node = root;
                } else {
                    node->color = 'b';
                    node = node->parent;
                }
            } else {
                RBTreeNode *temp = node->parent->left;
                if (temp->color == 'r') {
                    temp->color = 'b';
                    node->parent->color = 'r';
                    rotateright(node->parent);
                    temp = node->parent->left;
                }
                if (temp->left->color == 'b' && temp->right->color == 'b') {
                    temp->color = 'r';
                    node = node->parent;
                } else if (temp != nullnode) {
                    if (temp->left->color == 'b') {
                        temp->right->color = 'b';
                        temp->color = 'r';
                        rotateleft(temp);
                        temp = node->parent->left;
                    }
                    temp->color = node->parent->color;
                    node->parent->color = 'b';
                    temp->left->color = 'b';
                    rotateright(node->parent);
                    node = root;
                } else {
                    node->color = 'b';
                    node = node->parent;
                }
            }
        }
    }


    RBTreeNode *del(RBTreeNode *node) { //done
        RBTreeNode *rplc = NULL;
        if (node && node != nullnode) {
            RBTreeNode *rmv = node;
            char c = rmv->color;
            if (node->left == nullnode) {
                rplc = node->right;
                exchange(node, node->right);
            } else

            if (node->right == nullnode) {
                rplc = node->left;
                exchange(node, node->left);
            } else {
                rmv = minnode(node->right);
                c = rmv->color;
                rplc = rmv->right;
                if (rmv->parent == node)
                    rplc->parent = rmv;
                else {
                    exchange(rmv, rmv->right);
                    rmv->right = node->right;
                    rmv->right->parent = rmv;
                }
                exchange(node, rmv);
                rmv->left = node->left;
                rmv->left->parent = rmv;
                rmv->color = node->color;
            }
            if (c == 'b')
                balancedeletion(rplc);
        }
        if(root == nullnode)
            root = NULL;
        return rplc;
    }
    RBTreeNode* leftmost(RBTreeNode* node) {
        if(node == NULL || node == nullnode)
            return NULL;
        while(node->left != nullnode) {
            node = node->left;
        }
        return node;
    }

    RBTreeNode* minnode(RBTreeNode* node) { //done
        while(node->left != nullnode)
            node = node->left;
        return node;
    }
};

float calculate_weight(int nice) {
    return 1024/(pow(1.25, nice));
}

float calculate_vruntime(Process* p) {
    p->vruntime += (1024*p->exectime/p->weight);
    return  p->vruntime;
}

int main() {

        RBTree rbtree;
    unordered_map<int, vector<Process*> > hashmap; //processes by their starttime
    unordered_map<int, Process*> procmap; //processes by their number
    int isbeingexecuted = 0;

    cout << "ENTER NUMBER: FILE (0) OR RANDOM INPUT (1)" << endl;
    int menu;
    cin >> menu;
    if(menu == 0) {
        fstream fp;
        fp.open("test.txt", ios::in);
        if (!fp) {
            cout << "ERROR";
        }
        else {
            int n;
            int i = 0;
            while(true) {
                Process* p = new Process();
                fp >> p->number;
                if(p->number == NULL)
                    break;
                fp >> p->nice;
                fp >> p->burst_time_ms;
                fp >> p->arrival_time_ms;
                hashmap[p->arrival_time_ms].push_back(p);
                p->weight = calculate_weight(p->nice);
                p->vruntime = 0;
                procmap[p->number] = p;
            }
        }
        fp.close();
    } else {
        srand(time(NULL));
        int numberofprocesses = rand() % 100 + 1;
        for(int i = 0; i < numberofprocesses; i++) {
            Process* p = new Process();
            p->number = i+1;
            p->nice = rand() % 20 - 20;
            p->burst_time_ms = rand() % 100 + 1;
            p->arrival_time_ms = rand() % 100 + 1;
            hashmap[p->arrival_time_ms].push_back(p);
            p->weight = calculate_weight(p->nice);
            p->vruntime = 0;
            procmap[p->number] = p;
        }

    }


    int totalweight = 0;
    RBTreeNode* minvruntime;
    int latency = 5;
    int period = max((int)hashmap.size(), latency); //min_granularity is 1
    vector<int> vec(procmap.size()+1, 0); // to track if process was executed for response time
    int procsize = procmap.size();
    int j = 1;
    for(; j < INT_MAX && procsize != 0; j++) {
        cout << "CPU time = " << j << endl;
        cout << "Curr executing PROCESS # " << isbeingexecuted << endl;
        for(int i = 0; i < hashmap[j].size(); i++) {
            totalweight += hashmap[j][i]->weight;
            procmap[hashmap[j][i]->number]->vruntime = calculate_vruntime(procmap[hashmap[j][i]->number]);
            rbtree.ins(procmap[hashmap[j][i]->number]);
            cout << "inserted proc #" << procmap[hashmap[j][i]->number]->number << ", weight =  "<< procmap[hashmap[j][i]->number]->weight << endl;
            procmap[hashmap[j][i]->number]->lastinserted = j;
        }
        if(isbeingexecuted == 0) {
            minvruntime = rbtree.leftmost(rbtree.root);
            if(minvruntime == NULL || minvruntime == rbtree.nullnode)
                continue;
            isbeingexecuted = minvruntime->proc->number;
            cout << "deleted proc #" << minvruntime->proc->number << " exec time = " << minvruntime->proc->exectime << " vr = " << minvruntime->proc->vruntime << endl;
            if(vec[minvruntime->proc->number] == 0) {
                procmap[minvruntime->proc->number]->responsetime = j - procmap[minvruntime->proc->number]->arrival_time_ms;
                vec[minvruntime->proc->number] = 1;
            }
            procmap[minvruntime->proc->number]->waitingqueue += (j-procmap[minvruntime->proc->number]->lastinserted);
            rbtree.del(minvruntime);// TODO
            procmap[isbeingexecuted]->time_slice = period*procmap[isbeingexecuted]->weight/totalweight; // added 1 for ceiling val
        }
        if(isbeingexecuted != 0) {
            procmap[isbeingexecuted]->exectime++;
            procmap[isbeingexecuted]->currexec++;
            if(procmap[isbeingexecuted]->burst_time_ms <= procmap[isbeingexecuted]->exectime) {
                totalweight -= procmap[isbeingexecuted]->weight;
                //no need to update vr because process finishes its execution
                cout << "OVER: proc #" << procmap[isbeingexecuted]->number << " exec time = " << procmap[isbeingexecuted]->exectime << " vr = " << procmap[isbeingexecuted]->vruntime << endl;
                isbeingexecuted = 0;
                procsize--;
            }
            else if(procmap[isbeingexecuted]->time_slice <= procmap[isbeingexecuted]->currexec ) {
                if(rbtree.root == NULL or rbtree.root == rbtree.nullnode)
                    continue;
                procmap[isbeingexecuted]->vruntime = calculate_vruntime(procmap[isbeingexecuted]);
                rbtree.ins(procmap[isbeingexecuted]);
                cout << "inserted proc #" << procmap[isbeingexecuted]->number << " exec time = " << procmap[isbeingexecuted]->exectime << " vr = " << procmap[isbeingexecuted]->vruntime << endl;
                procmap[isbeingexecuted]->currexec = 0;
                procmap[isbeingexecuted]->lastinserted = j;
                procmap[isbeingexecuted]->preempted++;
                isbeingexecuted = 0;
            }
        }
    }
    float avgwaitingtime = 0, responsetime = 0, preemption = 0;
    cout << "PROCESS # " << "WAIT     " << "RESPONSE TIME " << "PREEMPTION" << endl;
    for(int i = 1; i <= procmap.size(); i++) {
        cout << i << "          " << procmap[i]->waitingqueue << "          " << procmap[i]->responsetime << "          " << procmap[i]->preempted << endl;
        avgwaitingtime += procmap[i]->waitingqueue;
        responsetime += procmap[i]->responsetime;
        preemption += procmap[i]->preempted;
    }
    cout << "TOTAL RUNTIME = " << j-1 << endl;
    cout << "AVG: WAIT = " << avgwaitingtime/procmap.size() << ", RESPONSE TIME = " << responsetime/procmap.size() << ", PREEMPTION = " << preemption/procmap.size() << endl;
    return 0;
}