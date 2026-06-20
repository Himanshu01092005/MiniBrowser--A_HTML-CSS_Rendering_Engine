#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
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

int main()
{

    // <div><p>Hello</p></div>
    printTokens(tokenize("<div class=\"box\" id=\"main\"><p>Hello</p></div>" ));
    return 0;
}
