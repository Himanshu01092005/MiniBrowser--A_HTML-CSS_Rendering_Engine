#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>
using namespace std;

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

vector<CSSRule> parseCSS(const string& css){
    int i=0;
    vector<CSSRule> ans;
    while(i<css.length()){
        CSSRule CSS;

        //Selector
        while(css[i]==' ') i++;
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
            while(css[i]==' ') i++;
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
            while(css[i]==' ') i++;
            if(css[i]=='{') i++;

        }else{
            string name;
            while(css[i]!=' ' && css[i]!='{'){
                name = name + css[i];
                i++;
            }
            CSS.type = SelectorType::Tag;
            CSS.selectorValue = name;
            while(css[i]==' ') i++;
            if(css[i]=='{') i++;
        }
        while(css[i]!='}'){
        //Declaration
        while(css[i]==' ') i++;
        CSSDeclaration Decleration;
        string property;
        while(css[i]!=' '&& css[i]!=':'){
            property = property + css[i];
            i++;
        }
        Decleration.property = property;
        while(css[i]==' ') i++;
        if(css[i]==':')i++;
        while(css[i]==' ') i++;
        string value;
        while(css[i]!=' '&& css[i]!=';'){
            value = value + css[i];
            i++;
        }
        Decleration.value = value;

        while(css[i]==' ') i++;
        if(css[i]==';')i++;
        while(css[i]==' ') i++;

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

int main(){

    printCSSRules(parseCSS("div { color: red; padding: 10px; } .box { background-color: blue; } #main { font-size: 20px; }"));

    return 0;
}