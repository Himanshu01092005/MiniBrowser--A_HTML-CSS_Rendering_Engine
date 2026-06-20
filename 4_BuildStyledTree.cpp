#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>
using namespace std;

const vector<string> INHERITABLE_PROPERTIES = {"color", "font-size", "font-weight"};

struct StyledNode {
    DOMNode* domNode;  // non-owning — points into the existing DOM tree
    unordered_map<string, string> computedStyle;
    vector<unique_ptr<StyledNode>> children;
};

unique_ptr<StyledNode> buildStyledTree(DOMNode* domNode, const unordered_map<string,string>& parentStyle,const vector<CSSRule>& allRules){
    auto styledNode = make_unique<StyledNode>();
    styledNode->domNode = domNode;

    //  copy down inheritable properties from parent
    for (const string& prop : INHERITABLE_PROPERTIES) {
        if (parentStyle.count(prop)) {
            styledNode->computedStyle[prop] = parentStyle.at(prop);
        }
    }

    // overwriting 
    unordered_map<string,string> ownStyle = computeStyleForNode(domNode, allRules);
    for (const auto& kv : ownStyle) {
        styledNode->computedStyle[kv.first] = kv.second;
    }

    //recurse into children, passing THIS node's computed style down
    for (const auto& child : domNode->children) {
        styledNode->children.push_back(buildStyledTree(child.get(), styledNode->computedStyle, allRules));
    }

    return styledNode;
}

void printStyledDOM(StyledNode* node, int depth){
    if(node->domNode->type == NodeType::Element){
        cout<<node->domNode->tagName;
        cout<<endl;
        for(const auto&kv : node->computedStyle){
            cout<<kv.first<<" : "<<kv.second <<endl ;
        }
    }
    else {
        cout<<node->domNode->textContent;
    }
    cout << "\n";
    for(int i=0; i < node->children.size() ; i++){
        for(int l=0;l<depth;l++) cout<<" ";
        // cout << node->children[i]->name;
        printStyledDOM(node->children[i].get(),depth+1);
    }
}

int main(){

    unordered_map<string,string> map;
    printStyledDOM(
        buildStyledTree(
            buildDOMTree(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>" )).get(),
            map,
            parseCSS("div { color: red; padding: 10px; } .box { background-color: blue; } #main { font-size: 20px; }")
        ).get()
    , 1 );

}