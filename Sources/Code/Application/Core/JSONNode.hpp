#ifndef __JSON_NODE_HPP__
#define __JSON_NODE_HPP__

#include <memory>
#include <string>

class Buffer;
class JSONNode;
struct JSONNodeImpl;
struct JSONNodeIteratorImpl;

class JSONNodeIterator {
public:

    JSONNodeIterator();
    explicit JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>&& iterImpl);
    JSONNodeIterator(JSONNodeIterator&& other);
    ~JSONNodeIterator();

    JSONNodeIterator& operator=(JSONNodeIterator&& other);
    JSONNodeIterator& operator++();
    bool operator!=(const JSONNodeIterator& other) const;
    bool operator==(const JSONNodeIterator& other) const;
    JSONNode& operator*();
    JSONNode* operator->();

private:

    std::unique_ptr<JSONNodeIteratorImpl> iterImpl;
};

class JSONNode {

    friend struct JSONNodeIteratorImpl;

public:

    static JSONNode ParseBuffer(const Buffer& buff);
    static JSONNode ParseString(const std::string& str);

public:

    JSONNode();
    JSONNode(JSONNode&& node);
    JSONNode& operator=(JSONNode&& node);
    ~JSONNode();

    JSONNodeIterator begin();
    JSONNodeIterator end();

    explicit operator bool() const;

    void value(const std::string& key, std::string& value) const;
    void value(const std::string& key, float& value) const;
    void value(const std::string& key, int& value) const;

    void value(std::string& value) const;
    void value(float& value) const;
    void value(int& value) const;

    const char* key() const;
    JSONNode object(const std::string& key) const;

    size_t size() const;

private:

    explicit JSONNode(std::unique_ptr<JSONNodeImpl>&& jsonImpl);

private:

    std::string nodeKey;
    std::unique_ptr<JSONNodeImpl> nodeImpl;
};

#endif /* __JSON_NODE_HPP__ */