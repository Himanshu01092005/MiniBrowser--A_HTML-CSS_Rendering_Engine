#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>
using namespace std;

enum class NodeType {Element,Text};

struct DOMNode{
    NodeType type;
    string tagName;
    string textContent;
    unordered_map<string , string> attributes;
    vector<unique_ptr<DOMNode>> children;
    DOMNode* parent = nullptr;
};

unique_ptr<DOMNode> buildDOMTree(const vector<Token> &tokens){
    // stack <DOMNode*>;

    auto root = make_unique<DOMNode>();
    root-> type = NodeType :: Element;
    root-> tagName = "document";

    stack <DOMNode*> openElements;
    openElements.push(root.get());

    for(const Token& token:tokens){
        if(token.type == TokenType::OpenTag){
            auto node = make_unique<DOMNode>();
            node-> type = NodeType :: Element;
            node-> tagName = token.tagName;
            node->attributes = token.attribute;
            node->parent = openElements.top();

            DOMNode* rawPtr = node.get();
            openElements.top()->children.push_back(move(node));
            openElements.push(rawPtr);

        }else if(token.type == TokenType::CloseTag){

            openElements.pop();

        }else if(token.type == TokenType::Text){
            auto node = make_unique<DOMNode>();
            node-> type = NodeType :: Text;
            node->textContent = token.text;
            node->parent = openElements.top();

            openElements.top()->children.push_back(move(node)); //attaching but not pushing

        }else if(token.type == TokenType::SelfClosingTag){
            auto node = make_unique<DOMNode>();
            node->attributes=token.attribute;
            node->tagName=token.tagName;
            node->type = NodeType::Element;
            node->parent = openElements.top();
            openElements.top()->children.push_back(move(node));
        }
    }

    return root;

}

void printDOMTree(DOMNode* node, int depth){
    if(node->type == NodeType::Element)
        cout<<node->tagName;
    else 
        cout<<node->textContent;
    cout << "\n";
    for(int i=0; i < node->children.size() ; i++){
        for(int l=0;l<depth;l++) cout<<" ";
        // cout << node->children[i]->name;
        printDOMTree(node->children[i].get(),depth+1);
    }
}