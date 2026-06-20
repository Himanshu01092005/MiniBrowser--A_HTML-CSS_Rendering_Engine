#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>
using namespace std;

struct LayoutBox {
    StyledNode* styledNode;  // non-owning
    float x, y, width, height;                 // the border-box outer size
    float contentX, contentY, contentWidth, contentHeight;  // inside padding+border
    vector<unique_ptr<LayoutBox>> children;
};

float getFloatStyle(StyledNode* node, const string& property, float defaultValue){
    if (!node->computedStyle.count(property)) return defaultValue;
    string val = node->computedStyle.at(property);
    //Extra wayy but i did not took care of negative and decimals so
    // float num=0;
    // for (int i=0;i<val.length();i++){
    //     if((int)val[i] >= 48 && (int)val[i]<=57){
    //         num = num*10 + ((int)val[i]-48);
    //     }else break;
    // }
    // return num;

    try {
        return stof(val);
    } catch (...) {
        return defaultValue;
    }

}

unique_ptr<LayoutBox> computeLayout(StyledNode* node, float parentContentX, float parentContentY, float parentContentWidth) {


    auto box = make_unique<LayoutBox>();
    box->styledNode = node;

    if(node->domNode->type == NodeType::Text){
        box->height=20;
        box->width=parentContentWidth;
        box->x=parentContentX;
        box->y=parentContentY;
    }else{

        float marginLeft = getFloatStyle(node, "margin", 0);   // noted to be in document too: one "margin" value for all sides for now
        float paddingVal = getFloatStyle(node, "padding", 0);

        box->x = parentContentX + marginLeft;
        box->y = parentContentY;  
        box->width = parentContentWidth - (2 * marginLeft);

        box->contentX = box->x + paddingVal;
        box->contentY = box->y + paddingVal;
        box->contentWidth = box->width - (2 * paddingVal);

        

        // lay out children, stacking vertically
        float currentY = box->contentY;
        for (auto& child : node->children) {
            auto childBox = computeLayout(child.get(), box->contentX, currentY, box->contentWidth);
            currentY += childBox->height;  // advance cursor by child's full height
            box->children.push_back(move(childBox));
        }

        // this node's content height = however far the cursor moved, plus padding on both sides
        box->contentHeight = currentY - box->contentY;
        box->height = box->contentHeight + (2 * paddingVal);
    }

    return box;
}

void printLayoutTree(LayoutBox* box, int depth){
    if(box->styledNode->domNode->type == NodeType::Element){
        cout<<box->styledNode->domNode->tagName;
        cout<<endl;
        for(const auto&kv : box->styledNode->computedStyle){
            cout<<kv.first<<" : "<<kv.second <<endl ;
        }
        cout<<"x:"<<box->x<<" | "<< "y:"<<box->y<<" | "<<"width:"<<box->width<<" | "<<"height:"<<box->height<<" | "<<endl;
    }
    else {
        cout<<box->styledNode->domNode->textContent;
    }
    cout << "\n";
    for(int i=0; i < box->children.size() ; i++){
        for(int l=0;l<depth;l++) cout<<" ";
        // cout << node->children[i]->name;
        printLayoutTree(box->children[i].get(),depth+1);
    }
}
