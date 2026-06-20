#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
using namespace std;

struct TreeNode{
    string name ;
    vector< unique_ptr<TreeNode> > children ; 
    TreeNode* parent = nullptr;
};



void printTree(TreeNode* node, int depth){
    cout<<node->name;
    cout << "\n";
    for(int i=0; i < node->children.size() ; i++){
        for(int l=0;l<depth;l++) cout<<" ";
        // cout << node->children[i]->name;
        printTree(node->children[i].get(),depth+1);
    }
   
}



int main(){

    // TreeNode *root = new TreeNode(); NOte for myself i am not suing this beacuase i want to use unique_pte inseteaadd of new/delete

    auto root = make_unique<TreeNode>();
    root->name = "root";

    auto child1 = make_unique<TreeNode>();
    child1->name = "child1";
    child1->parent = root.get();

    auto grandChild = make_unique<TreeNode>();
    grandChild->name = "grandChild";
    grandChild->parent = child1.get();
    child1->children.push_back(move(grandChild));

    root->children.push_back(move(child1));

    auto child2 = make_unique<TreeNode>();
    child2->name = "child2";
    child2->parent = root.get();
    root->children.push_back(move(child2));

    


    // printTree(root.get(),1);



    return 0;
}