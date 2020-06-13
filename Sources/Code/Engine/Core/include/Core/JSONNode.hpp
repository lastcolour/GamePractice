#ifndef __JSON_NODE_HPP__
#define __JSON_NODE_HPP__

#include "Core/Buffer.hpp"

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
    static JSONNode ParseString(const char* str);

public:

    JSONNode();
    JSONNode(JSONNode&& node);
    JSONNode& operator=(JSONNode&& node);
    ~JSONNode();

    JSONNodeIterator begin() const;
    JSONNodeIterator end() const;

    explicit operator bool() const;

    void read(const char* key, std::string& value) const;
    void read(const char* key, float& value) const;
    void read(const char* key, int& value) const;
    void read(const char* key, bool& value) const;

    void read(std::string& value) const;
    void read(float& value) const;
    void read(int& value) const;
    void read(bool& value) const;

    void write(const char* key, const JSONNode& node);
    void write(const char* key, const std::string& value);
    void write(const char* key, const char* value);
    void write(const char* key, float value);
    void write(const char* key, int value);
    void write(const char* key, bool value);

    void write(float value);
    void write(int value);
    void write(bool value);
    void write(const char* value);
    void write(const std::string& value);

    const char* key() const;
    JSONNode object(const char* key) const;
    bool hasKey(const char* key) const;

    bool isArray() const;
    size_t size() const;

    Buffer flushToBuffer() const;

private:

    explicit JSONNode(std::unique_ptr<JSONNodeImpl>&& jsonImpl);

private:

    bool mutateToArray();
    void updateDocRoot();

private:

    std::string nodeKey;
    std::unique_ptr<JSONNodeImpl> nodeImpl;
};

#endif /* __JSON_NODE_HPP__ */