#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <queue>
using namespace std;

class DataNode{
    vector<int> keys;
    DataNode *left,*right;
    vector<DataNode *>children;
    int nodeSize,t;
public:
    DataNode(int);
    DataNode *search(int);
    bool isfull();
    int getKeyIndex(int);
    pair<DataNode*,int> split(int);
    pair<DataNode*,int> insert(int);

friend class BTree;
};


class IndexNode{
    vector<int> keys;
    vector<void *> children;
    bool leaf;
    int t;
    int nodeSize;
public:
    IndexNode(bool,int);
    bool isfull();
    int getKeyIndex(int);
    DataNode *search(int);
    pair<IndexNode*,int> split(int);
    pair<IndexNode*,int> insert(int);

friend class BTree;
};


DataNode *toDataNodeType(void *p){
    return static_cast<DataNode *> (p);
}
IndexNode *toIndexNodeType(void *p){
    return static_cast<IndexNode *> (p);
}


DataNode::DataNode(int nodeSize){
    this->nodeSize=nodeSize;
    this->t=nodeSize/2;
    this->left=NULL;
    this->right=NULL;
}
DataNode * DataNode::search(int k){
    for(auto i:keys)
        if(i==k)
            return this;
    return NULL;
}
bool DataNode::isfull(){
    return keys.size()==2*t;
}
int DataNode::getKeyIndex(int k){
    int index;
    for(index=0;index < keys.size() && keys[index] < k;index++);
    return index;
}
pair<DataNode*,int> DataNode::split(int value){

    DataNode * secondChild = new DataNode(this->nodeSize);
    int index= this->getKeyIndex(value);

    this->keys.insert(this->keys.begin()+index,value);

    secondChild->keys.assign(keys.begin()+t,keys.end());    

    this->keys.resize(t);

    secondChild->left= this;
    secondChild->right = this->right;
    this->right=secondChild;

    return {secondChild,keys[t]};


    /*
            1 2 3 . . . t [t+1] [] [] . . . [2t-1] [2t] [2t+1]
    */

}
pair<DataNode*,int> DataNode::insert(int k){
    cout<<"Inserting in Data Node\n";
    
    int insertPos=this->getKeyIndex(k);
    if(this->isfull()){
        return this->split(k);
    }
    else{
        keys.insert(this->keys.begin()+insertPos,k);
        return  {NULL,INT_MAX};
    }

}


IndexNode::IndexNode(bool leaf,int nodeSize){
    this->t = (nodeSize+1)/2;
    this->nodeSize = nodeSize;
    this->leaf= leaf;
}
bool IndexNode::isfull(){
    return keys.size()==2*t-1;
}
int IndexNode::getKeyIndex(int k){
    int index;
    for(index=0;index < keys.size() && keys[index] < k;index++);
    return index;
}
DataNode * IndexNode::search(int k){
    void * child = children[this->getKeyIndex(k)];
    if(child==NULL) return NULL;
    if(leaf){
        return toDataNodeType(child)->search(k);
    }
    return toIndexNodeType(child)->search(k);
}
pair<IndexNode*,int> IndexNode::split(int value){

    IndexNode * secondChild = new IndexNode(this->leaf,this->nodeSize);
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
pair<IndexNode*,int> IndexNode::insert(int k){
    cout<<"Inserting in Index Node\n";
    int child=this->getKeyIndex(k);
    // find position of the element in keys

    pair<void*,int> child_key_pair;

    void *childNode = children[child];

    if(this->leaf)
        child_key_pair = toDataNodeType(childNode)->insert(k);
    else
        child_key_pair= toIndexNodeType(childNode)->insert(k);

    void * secondChild ;
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



class BTree{
    int isize;
    int dsize;
    void * root;
    int depth=-1;

public:
    DataNode *search(int k){
        if(root==NULL)  return NULL;
        if(depth==1)
            return toDataNodeType(root)->search(k);
        else
            return toIndexNodeType(root)->search(k);
    }
    void createTree(int t,int d){
        root=NULL;
        depth=0;
        isize=t;
        dsize = d;
    }
    void insertInTree(int k){
        if(depth==-1){
            cout<<"NO TREE FOUND\n";
            return;
        }

        if(depth==0){
            DataNode *temp = new DataNode(dsize);
            temp->keys.push_back(k);
            depth++;
            root=temp;
            return;
        }

        pair<void*,int> t;
        if(depth ==1)
            t= toDataNodeType(root)->insert(k);
        else
            t = toIndexNodeType(root)->insert(k);

        if(t.second != INT_MAX){
            IndexNode * temp = new IndexNode(depth == 1 ? true:false,isize);
            temp->keys.push_back(t.second);
            temp->children.push_back(root);
            temp->children.push_back(t.first);
            root=temp;
            depth++;
        }
    }
    void printLevelOrder(){
        if(root==NULL)  return ;
        queue <pair<void *,int>> q;
        pair<void *,int> elem;
        q.push({root,1});
        int level=1;
        while(!q.empty()){
            elem=q.front();
            q.pop();
            if(level!=elem.second){
                level = elem.second;
                cout<<endl;
            }
            if(level==depth){
                for(auto i :toDataNodeType(elem.first)->keys)
                    cout<<"("<<i<<","<<level<<") ";
                cout<<"\t";
            }
            else{
                for(auto itr :toIndexNodeType(elem.first)->children)
                    if(itr!=NULL)
                        q.push({itr,elem.second+1});
                for(auto i:toIndexNodeType(elem.first)->keys)
                    cout<<"("<<i<<","<<level<<") ";
                cout<<"\t";
            }            
        }
        cout<<endl;
    }
};
int main(){
    BTree * tree= new BTree();
    int t;
    int d;
    int c;
    while(true){
        cout<<"1.Create new tree \n2.Insert \n3.Print\n4.Search"<<endl;
        cin>>c;
        switch(c){
            case 1: cout<<"Enter Index Node size, Data Node Size: "; 
                cin>>t>>d; 
                if(d%2==0 && t%2)
                    tree->createTree(t,d);
                else
                    cout<<"Make Sure Index Size is odd and Data Size is Even"<<endl;
            break;
            case 2: cout<<"Enter value to be inserted "; int k; cin>>k; tree->insertInTree(k);
            break;
            case 3:  tree->printLevelOrder();
            break;
            case 4:
            break;
            default:
                    exit(0);
        }
    }       
}
