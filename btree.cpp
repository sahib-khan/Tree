#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <queue>
using namespace std;



class BTreeNode{
    vector<int> keys;
    vector<BTreeNode *> children;
    bool leaf;
    int t;
public:
    BTreeNode(int t,int leaf){
        this->t = t;
        this->leaf= leaf;
    }
    bool isfull(){
        return keys.size()==2*t-1;
    }
    int getKeyIndex(int k){
        int index;
        for(index=0;index < keys.size() && keys[index] < k;index++);
        return index;
    }
    BTreeNode *search(int k){
        if(leaf){
            for(int i=0;i<keys.size();i++){
                if(keys[i]==k)
                    return this;
            }
            return NULL;
        }
        return children[this->getKeyIndex(k)]->search(k);
    }
    pair<BTreeNode*,int> split(int value){

        BTreeNode * secondChild = new BTreeNode(this->t,this->leaf);
        int index= this->getKeyIndex(value);

        this->keys.insert(this->keys.begin()+index,value);

        secondChild->keys.assign(keys.begin()+t+1,keys.end());
        secondChild->children.assign(children.begin()+t+1,children.end());        

        this->children.resize(t+1);
        this->keys.resize(t);
        return {secondChild,keys[t]};


        /*
                1 2 3 . . . t [t+1] [] [] . . . [2t-1] [2t] [2t+1]
        */

    }
    pair<BTreeNode*,int> insert(int k){
        int child=this->getKeyIndex(k);
        // find position of the element in keys

        pair<BTreeNode*,int> child_key_pair;

        BTreeNode *childNode = children[child];

        if(this->leaf)
            child_key_pair = {NULL,k};
        else
            child_key_pair= childNode->insert(k);

        BTreeNode * secondChild ;
        int value;
        if(child_key_pair.second==INT_MAX){
            return {NULL,INT_MAX};
        }
        secondChild = child_key_pair.first;
        value = child_key_pair.second;
        // if(secondChild !=NULL){
            children.insert(children.begin()+child+1,secondChild);
        // }
        if(this->isfull()){
            return this->split(value);
        }
        else{
            keys.insert(this->keys.begin()+child,value);
            return  {NULL,INT_MAX};
        }
    }

friend class BTree;
};
class BTree{
    int tsize;
    BTreeNode * root=NULL;

public:
    BTreeNode *search(int k){
        if(root==NULL)  return NULL;
        return root->search(k);
    }
    void createTree(int t){
        root = NULL;
        tsize=t;
    }
    void insertInTree(int k){
        cout<<"Inserting "<<k<<endl;
        pair<BTreeNode*,int> t;
        if(root == NULL)
            t= {NULL,k};
        else
            t = root->insert(k);
        
        if(t.second != INT_MAX){
            BTreeNode * temp = new BTreeNode(tsize,root == NULL ? true:false);
            temp->keys.push_back(t.second);
            temp->children.push_back(root);
            temp->children.push_back(t.first);
            root=temp;
        }
    }
    void printLevelOrder(){
        if(root==NULL)  return ;
        queue <pair<BTreeNode *,int>> q;
        pair<BTreeNode *,int> elem;
        q.push({root,1});
        int level=1;
        while(!q.empty()){
            elem=q.front();
            q.pop();
            for(auto itr :elem.first->children)
                if(itr!=NULL)
                    q.push({itr,elem.second+1});
            if(level!=elem.second){
                level = elem.second;
                cout<<endl;
            }
            for(auto i:elem.first->keys)
                cout<<"("<<i<<","<<level<<") ";
            cout<<"\t";            
        }
    }
};
int main(){
    BTree * tree= new BTree();
    int t;
    int c;
    while(true){
        cout<<"1. create new tree 2.insert 3.Print"<<endl;
        cin>>c;
        switch(c){
            case 1: cout<<"Enter t "; cin>>t; tree->createTree(t);
            break;
            case 2: cout<<"Enter value"; int k; cin>>k; tree->insertInTree(k);
            break;
            case 3:  tree->printLevelOrder();
            break;
            default:
                    exit(0);
        }
    }   
    
    
    
}
