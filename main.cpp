#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

#define PRINT_PRETTY (std::cout << __PRETTY_FUNCTION__ << std::endl);

class PrefixNode
{
    PrefixNode* parent;
    char myKey;
    std::map<char, PrefixNode*> children;

public:
    PrefixNode() : parent(nullptr), children(), myKey('\0')
    {
        //PRINT_PRETTY
    }
    PrefixNode(PrefixNode* parent_, char key = '\0') : parent(parent_), children(), myKey(key)
    {
        //PRINT_PRETTY
    }
    ~PrefixNode() = default;

    bool haveChildren() const
    {
        return children.size() > 0;
    }

    bool isLeaf() const
    {
        return children.size() <= 0;
    }

    //Проверяет, является ли строка разветвлением (необходимо для поиска префикса)
    bool isBranch() const
    {
        return children.size() > 1;
    }

    //Функция, вызывающая утечки памяти
    auto addChildren(char c)
    {
        auto child = new PrefixNode(this, c);
        auto p = children.emplace(c, child);
        return p;
    }

    //Добавление строки в дерево (поддерево)
    void add(const std::string &s, size_t index = 0)
    {
        //PRINT_PRETTY
        auto p = addChildren(s[index]);

        if (index == s.size())
        {
            addChildren('\0');
            return;
        }
        else
        {
            auto it = p.first;
            it->second->add(s, ++index);
        }
    }
    //Alias для более удобного обращения к дочернему элементу, если этот элемент один
    inline PrefixNode* getFirstChild() const
    {
        if(children.empty())
            return nullptr;
        return children.begin()->second;
    }
    //Возвращает последний узел, относящийся к данной строке
    //(По последнему узлу можно однозначно восстановить строку просто поднимаясь от этого узла к корню)
    PrefixNode* getLeafOf(const std::string &s, size_t index = 0)
    {
        if (index == s.size() - 1)
            return children[s[index]];
        return children[s[index]]->getLeafOf(s, index + 1);
    }

    //Бежим снизу вверх по дереву пока не найдем разветвление.
    //После чего спускаемся обратно на один шаг и таким образом определяем префикс
    PrefixNode* getNearestBranch(char last_key = 0)
    {
        if (isBranch())
            return children[last_key];
        else
        {
            return parent->getNearestBranch(myKey);
        }
    }

    //Возвращает строку, состоящую из символов myKey от корня дерева до текущего узла
    std::string getStringToRoot() const
    {
        std::string result;
        for(auto ptr = this;(ptr->myKey!=0 || ptr==this);ptr = ptr->parent)
        {
            result+=ptr->myKey;
        }
        std::reverse(result.begin(),result.end());
        return result;
    }

    //Возвращает строку, показывающую строение дерева (поддерева)
    std::string getFullStructure(int offset = 0, bool goThrough = true) const
    {
        auto whitespaces = std::string(offset, ' ');
        if(goThrough || !myKey)
        {    
            whitespaces = "";
        }
        else
        {
            whitespaces+="└";
        }
        
        auto result = std::string(whitespaces);
        result+=myKey;

        if (isLeaf())
        {
            return result + "$\n";
        }
        else
        {
            if (isBranch())
            {
                //if(children.begin()->first)
                if(getFirstChild()->myKey)
                    result+="\n";
                for (auto &child : children)
                {
                    result += child.second->getFullStructure(offset+1,false);
                }
            }
            else
            {
                return result + getFirstChild()->getFullStructure(offset,true);
            }
        }
        return result;
    }

    //Возвращает префикс строки
    std::string getPrefix(const std::string& s)
    {
        return getLeafOf(s)->getNearestBranch()->getStringToRoot();
    }

    void writeLeafsToVector(std::vector<PrefixNode*>& out) const
    {
        for (auto &child : children)
        {
            if(child.second->isLeaf())
            {
                out.push_back(child.second);
            }
            else
            {
                child.second->writeLeafsToVector(out);
            }
        }
    }

    std::vector<std::pair<std::string,std::string>> getAllStringsWithPrefixes() const
    {
        std::vector<PrefixNode*> leafs;
        writeLeafsToVector(leafs);

        std::vector<std::pair<std::string,std::string>> result;
        result.reserve(leafs.size());

        for(auto leaf: leafs)
        {
            auto str = leaf->getStringToRoot();
            auto prefix = leaf->getNearestBranch()->getStringToRoot();
            result.emplace_back(str,prefix);
        }
        return result;
    }
};

int main()
{
    PrefixNode root;
    auto s = std::string("Alex");
    root.add(s);
    s = "Alexia";
    root.add(s);
    s = "Fedor";
    root.add(s);
    s = "Alexandr";
    root.add(s);
    s = "Alexandria";
    root.add(s);
    s = "Fedora";
    root.add(s);
    s = "Alf";
    root.add(s);
    s = "Amanda";
    root.add(s);
    s = "Fez";
    root.add(s);

    std::cout << root.getFullStructure() << std::endl;

    // s = "Alexandria";
    // std::cout<<root.getPrefix(s)<<std::endl;

    // s = "Ale";
    // auto ale_leaf = root.getLeafOf(s);
    // auto ale_branch = ale_leaf->getNearestBranch();
    // auto ale_prefix = ale_branch->getStringToRoot();
    // std::cout << ale_prefix <<std::endl;

    auto v = root.getAllStringsWithPrefixes();

    for(auto pr: v)
    {
        std::cout<<pr.first<<" "<<pr.second<<std::endl;
    }
    return 0;
}