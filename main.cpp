#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <optional>
#include <fstream>
#include <sstream>
using namespace std;

enum class TokenType
{
    OpenTag,
    CloseTag,
    SelfClosingTag,
    Text
};

struct Token
{
    TokenType type;
    string tagName;
    unordered_map<string, string> attribute;
    string text;
};

enum class NodeType {Element,Text};

struct DOMNode{
    NodeType type;
    string tagName;
    string textContent;
    unordered_map<string , string> attributes;
    vector<unique_ptr<DOMNode>> children;
    DOMNode* parent = nullptr;
};

enum class SelectorType {Tag,ID,Class};

struct CSSDeclaration{
    string property;
    string value;
};

struct CSSRule{
    SelectorType type;
    string selectorValue;
    vector<CSSDeclaration> declarations;
};

vector<Token> tokenize(const string &html)
{

    vector<Token> ans;
    int i = 0;

    while (i < html.length())
    {
        Token token;
        if (html[i] == '<')
        {
            if (html[i + 1] == '/')
            {
                string name;
                i++;
                i++;
                while (html[i] != '>')
                {
                    name = name + html[i];
                    i++;
                }

                {
                    token.type = TokenType::CloseTag;
                    token.tagName = name;
                    ans.push_back(token);
                }
                i++;
            }
            else
            {
                string name;
                i++;
                while (html[i] != '>' && html[i] != '/' && html[i] != ' ')
                {
                    name = name + html[i];
                    i++;
                }
                while (html[i] == ' ')
                {
                    i++;
                } // skipping white space

                if (html[i] == '/')
                {
                    token.type = TokenType::SelfClosingTag;
                    token.tagName = name;
                    ans.push_back(token);
                    i++;
                    i++;
                }
                else if (html[i] == '>')
                {
                    token.type = TokenType::OpenTag;
                    token.tagName = name;
                    ans.push_back(token);
                    i++;
                }
                else
                {
                    token.type = TokenType::OpenTag;
                    token.tagName = name;

                    while (html[i] != '>')
                    {

                        while (html[i] == ' ')
                        {
                            i++;
                        }

                        string key;
                        while (html[i] != '=')
                        {
                            key = key + html[i];
                            i++;
                        }
                        i++;

                        while (html[i] == ' ')
                        {
                            i++;
                        }

                        string value;
                        if (html[i] == '"')
                        {
                            i++;
                            while (html[i] != '"')
                            {
                                value = value + html[i];
                                i++;
                            }
                            i++;

                            token.attribute[key] = value;
                        }
                    }
                    ans.push_back(token);
                    i++;
                }
            }
        }
        else
        {
            string name;
            while (html[i] != '<')
            {
                name = name + html[i];
                i++;
            }
            token.type = TokenType::Text;
            token.tagName = "Text";
            token.text = name;
            ans.push_back(token);
        }
    }

    return ans;
}

void printTokens(const vector<Token> &tokens)
{
    for (int i = 0; i < tokens.size(); i++)
    {
        Token token = tokens[i];
        if (token.type == TokenType::OpenTag)
        {
            cout << "OpenTag: " << token.tagName << endl;

            for (auto const& pair : token.attribute) {
                cout << "    Attribute -> " << pair.first << ": " << pair.second << endl;
            }
        }
        else if (token.type == TokenType::CloseTag)
        {
            cout << "CloseTag: " << token.tagName << endl;
        }
        else if (token.type == TokenType::Text)
        {
            cout << "Text: " << token.text << endl;
        }
        else
        {
            cout << "selfClosingTag: " << token.tagName << endl;
        }
    }
}


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

bool isWhitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

vector<CSSRule> parseCSS(const string& css){
    int i=0;
    vector<CSSRule> ans;
    while(i<css.length()){
        CSSRule CSS;

        //Selector
        while(isWhitespace(css[i])) i++;
        if(i>=css.length()) break;
        if(css[i]=='.'){
            i++;
            string name;
            while(css[i]!=' ' && css[i]!='{'){
                name = name + css[i];
                i++;
            }
            CSS.type = SelectorType::Class;
            CSS.selectorValue = name;
            while(isWhitespace(css[i])) i++;
            if(css[i]=='{') i++;

        }else if(css[i]=='#'){
            i++;
            string name;
            while(css[i]!=' ' && css[i]!='{'){
                name = name + css[i];
                i++;
            }
            CSS.type = SelectorType::ID;
            CSS.selectorValue = name;
            while(isWhitespace(css[i])) i++;
            if(css[i]=='{') i++;

        }else{
            string name;
            while(css[i]!=' ' && css[i]!='{'){
                name = name + css[i];
                i++;
            }
            CSS.type = SelectorType::Tag;
            CSS.selectorValue = name;
            while(isWhitespace(css[i])) i++;
            if(css[i]=='{') i++;
        }
        while(css[i]!='}'){
        //Declaration
        while(isWhitespace(css[i])) i++;
        CSSDeclaration Decleration;
        string property;
        while(css[i]!=' '&& css[i]!=':'){
            property = property + css[i];
            i++;
        }
        Decleration.property = property;
        while(isWhitespace(css[i])) i++;
        if(css[i]==':')i++;
        while(isWhitespace(css[i])) i++;
        string value;
        while(css[i]!=' '&& css[i]!=';'){
            value = value + css[i];
            i++;
        }
        Decleration.value = value;

        while(isWhitespace(css[i])) i++;
        if(css[i]==';')i++;
        while(isWhitespace(css[i])) i++;

        CSS.declarations.push_back(Decleration);

    }
    ans.push_back(CSS);
    i++;
    }
    return ans;
}

void printCSSRules(const vector<CSSRule>& rules){
    for(int i=0; i<rules.size();i++){
        cout<<endl;
        if(rules[i].type == SelectorType::Class){
            cout <<"Class"<<" "<<rules[i].selectorValue <<endl;
        }else if(rules[i].type == SelectorType::ID){
            cout <<"ID"<<" "<<rules[i].selectorValue <<endl;
        }else if(rules[i].type == SelectorType::Tag){
            cout <<"Tag"<<" "<<rules[i].selectorValue <<endl;
        }
        for(const CSSDeclaration &key_value : rules[i].declarations){
            cout<<key_value.property<<" : "<<key_value.value;
        }
    }
}

vector<string> splitOnSpaces(const string& s){
    vector<string> ans;
    int i=0;
    while(i<s.length()){
        if(s[i]==' ') i++;
        else{
            string value;
            while(i < s.length() && s[i] != ' '){
                value+=s[i];
                i++;
            }
            ans.push_back(value);
        }
    }

    return ans;
    
}

bool ruleMatches(const CSSRule& rule, DOMNode* node){
    if(rule.type == SelectorType::Tag  ){
        if(rule.selectorValue == node->tagName){
            return true;
        }else return false;
    }else if(rule.type == SelectorType::Class  ){
            vector<string> classes = splitOnSpaces(node->attributes["class"]);
            for (int i = 0; i < classes.size(); i++) {
                if (classes[i] == rule.selectorValue) return true;
            }
            return false;
    }else if(rule.type == SelectorType::ID  ){
        if(rule.selectorValue == (node->attributes["id"]) ){
            return true;
        }else return false;
    }

    return false;
}

int specificity(SelectorType type) {
    if (type == SelectorType::Tag) return 1;
    if (type == SelectorType::Class) return 2;
    return 3; // for Id
}

unordered_map<string,string> computeStyleForNode(DOMNode* node, const vector<CSSRule>& allRules){
    vector<CSSRule> matching;
    for (const CSSRule& rule : allRules) {
        if (ruleMatches(rule, node)) {
            matching.push_back(rule);
        }
    }
    
    //Here we have used lambda function rember this structure 
    sort(matching.begin(), matching.end(), [](const CSSRule& a, const CSSRule& b) {
        return specificity(a.type) < specificity(b.type);
    });

    unordered_map<string,string> result;
    for (const CSSRule& rule : matching) {
        for(const CSSDeclaration &dec : rule.declarations){
            result[dec.property] = dec.value;
        }
    }



    // for (const auto& check : result) {
    //     cout << check.first << " : " << check.second << endl;
    // }


    return result;
}

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

sf::Color colorNameToSFColor(const string& name){
    // red, blue, green, white, black
    if(name =="red") return sf::Color::Red;
    else if(name =="blue") return sf::Color::Blue;
    else if(name =="green") return sf::Color::Green;
    else if(name =="white") return sf::Color::White ;
    else if(name =="black") return sf::Color::Black ;
    else return sf::Color::White ;
}

void paintLayoutTree(LayoutBox* box, sf::RenderWindow& window, sf::Font& font) {
    if (box->styledNode->domNode->type == NodeType::Element) {
        // draw background rectangle if a background-color is set
        if (box->styledNode->computedStyle.count("background-color")) {
            sf::RectangleShape rect(sf::Vector2f(box->width, box->height));
            rect.setPosition(sf::Vector2f(box->x, box->y));
            // rect.setFillColor(/* convert computedStyle["background-color"] to sf::Color */);
            rect.setFillColor(colorNameToSFColor(box->styledNode->computedStyle.at("background-color")));
            window.draw(rect);
        }
    } else {
        // text node — draw its text content
        sf::Text text(font, box->styledNode->domNode->textContent, 16);
        text.setPosition(sf::Vector2f(box->x, box->y));
        text.setFillColor(sf::Color::Black);
        window.draw(text);
    }

    for (auto& child : box->children) {
        paintLayoutTree(child.get(), window, font);
    }
}

string readFile(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Could not open file: " << path << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();  // reads complete file 
    return buffer.str();
}



int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "MiniBrowser");

    sf::Font font;
    if (!font.openFromFile("fonts/arial.ttf")) {
        cout << "Font not found" << endl;
        return 1;
    }

    unordered_map<string,string> emptyMap;

    string sample_1_html = readFile("samples/sample_1/index.html");
    string sample_1_css = readFile("samples/sample_1/index.css");

    string sample_2_html = readFile("samples/sample_2/index.html");
    string sample_2_css = readFile("samples/sample_2/index.css");

    string sample_3_html = readFile("samples/sample_3/index.html");
    string sample_3_css = readFile("samples/sample_3/index.css");

    auto domRoot = buildDOMTree(tokenize(sample_1_html));
    auto styledRoot = buildStyledTree(domRoot.get(), emptyMap, parseCSS(sample_1_css));
    auto layoutRoot = computeLayout(styledRoot.get(), 0, 0, 800);
    // auto layoutRoot = computeLayout(
    //     buildStyledTree(
    //         buildDOMTree(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>")).get(),
    //         emptyMap,
    //         parseCSS("div { color: red; padding: 10px; } .box { background-color: blue; } #main { font-size: 20px; }")
    //     ).get(),
    //     0, 0, 100
    // );

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        window.clear(sf::Color::White);
        paintLayoutTree(layoutRoot.get(), window, font);
        window.display();
    }

    return 0;
    }

    // ----Below these are my old text cases--


// int main()
// {

    // <div><p>Hello</p></div>
    // printTokens(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>" ));

    // printDOMTree(buildDOMTree(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>" )).get(),1);

    // computeStyleForNode(buildDOMTree(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>" )).get(),parseCSS("div { color: red; padding: 10px; } .box { background-color: blue; } #main { font-size: 20px; }"));

    // unordered_map<string,string> map;
    // printStyledDOM(
    //     buildStyledTree(
    //         buildDOMTree(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>" )).get(),
    //         map,
    //         parseCSS("div { color: red; padding: 10px; } .box { background-color: blue; } #main { font-size: 20px; }")
    //     ).get()
    // , 1 );

    // printLayoutTree(
    //     computeLayout
    //         (buildStyledTree(
    //             buildDOMTree(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>" )).get(),
    //             map,
    //             parseCSS("div { color: red; padding: 10px; } .box { background-color: blue; } #main { font-size: 20px; }")
    //         ).get(),
    //         0,
    //         0,
    //         100).get()
    //     ,1);

    // while (window.isOpen()) {
    // while (const std::optional event = window.pollEvent()) {
    //     if (event->is<sf::Event::Closed>()) window.close();
    // }
    // window.clear(sf::Color::White);

    // font.openFromFile("fonts\arial.ttf");

    // paintLayoutTree(
    //     computeLayout
    //         (buildStyledTree(
    //             buildDOMTree(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>" )).get(),
    //             map,
    //             parseCSS("div { color: red; padding: 10px; } .box { background-color: blue; } #main { font-size: 20px; }")
    //         ).get(),
    //         0,
    //         0,
    //         100
    //     ).get(), window, font);

    // window.display();
    // }

//     return 0;
// }



//Testing for layout 
// int main(){
//     StyledNode node;
//     cout << "Testing missing property (should use default, never call stof):" << endl;
//     cout << getFloatStyle(&node, "margin", 99) << " (expect 99)" << endl;

//     cout << "\nTesting present, valid value:" << endl;
//     node.computedStyle["padding"] = "20px";
//     cout << getFloatStyle(&node, "padding", 0) << " (expect 20)" << endl;

//     cout << "\nTesting empty string VALUE present (simulates malformed CSS, e.g. 'margin:;'):" << endl;
//     node.computedStyle["weird"] = "";
//     cout << getFloatStyle(&node, "weird", 0) << " (expect 0, but watch for crash)" << endl;

//     return 0;
// }