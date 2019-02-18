#include "JSONNode.hpp"
#include "Logger.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

typedef rapidjson::GenericValue<rapidjson::UTF8<>> JSONValue;

enum class JSONNodeType {
    None = 0,
    Document,
    Array,
    ArrayElement,
    Object
};

struct JSONNodeImpl {

    JSONValue* val;
    bool isOwnValue;

public:

    JSONNodeImpl() :
        val(nullptr) {}

    explicit JSONNodeImpl(JSONValue* jsonVal) :
        val(jsonVal), isOwnValue(false) {}

    ~JSONNodeImpl() {
        if(isOwnValue) {
            delete val;
        }
        val = nullptr;
    }
};

struct JSONNodeIteratorImpl {

    typedef JSONValue::MemberIterator MemberIterator;
    typedef JSONValue::ValueIterator ValueIterator;

public:

    JSONNode node;
    union {
        MemberIterator memIt;
        ValueIterator valIt;
    };
    bool isMemIter;

public:

    JSONNodeIteratorImpl(MemberIterator& iter) :
        memIt(iter), isMemIter(true) {
        node.nodeImpl.reset(new JSONNodeImpl);
        node.nodeImpl->isOwnValue = false;
    }

    JSONNodeIteratorImpl(ValueIterator& iter) :
        valIt(iter), isMemIter(false) {
        node.nodeImpl.reset(new JSONNodeImpl);
        node.nodeImpl->isOwnValue = false;
    }

    ~JSONNodeIteratorImpl() {
        if(isMemIter) {
            memIt.~MemberIterator();
        } else {
            valIt.~ValueIterator();
        }
    }

    void operator++() {
        if(isMemIter) {
            ++memIt;
        } else {
            ++valIt;
        }
    }

    bool operator==(const JSONNodeIteratorImpl& other) {
        if(isMemIter) {
            if(other.isMemIter) {
                return memIt == other.memIt;
            }
        } else if(!other.isMemIter) {
            return valIt == other.valIt;
        }
        return false;
    }

    JSONNode& getValue() {
        if(isMemIter) {
            node.nodeImpl->val = &(memIt->value);
            if(!node.nodeImpl->val->IsNull()) {
                node.nodeKey = memIt->name.GetString();
            }
        } else {
            node.nodeImpl->val = &(*valIt);
        }
        return node;
    }
};

JSONNodeIterator::JSONNodeIterator() {
}

JSONNodeIterator::JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>&& iter) :
    iterImpl(std::move(iter)) {
}

JSONNodeIterator::JSONNodeIterator(JSONNodeIterator&& other) :
    iterImpl(std::move(other.iterImpl)) {
}

JSONNodeIterator::~JSONNodeIterator() {
}

JSONNodeIterator& JSONNodeIterator::operator=(JSONNodeIterator&& other) {
    if(this != &other) {
        iterImpl = std::move(other.iterImpl);
    }
    return *this;
}

JSONNodeIterator& JSONNodeIterator::operator++() {
    ++(*iterImpl);
    return *this;
}

bool JSONNodeIterator::operator!=(const JSONNodeIterator& other) const {
    return !(*this == other);
}

bool JSONNodeIterator::operator==(const JSONNodeIterator& other) const {
    if(iterImpl == nullptr) {
        if(other.iterImpl == nullptr) {
            return true;
        } else {
            return false;
        }
    } else {
        if (other.iterImpl == nullptr) {
            return false;
        } else {
            return *iterImpl == *(other.iterImpl);
        }
    }
}

JSONNode& JSONNodeIterator::operator*() {
    return iterImpl->getValue();
}

JSONNode* JSONNodeIterator::operator->() {
    return &(iterImpl->getValue());
}

JSONNode::JSONNode() :
    nodeImpl(new JSONNodeImpl) {
}

JSONNode::JSONNode(JSONNode&& node) :
    nodeKey(std::move(node.nodeKey)),
    nodeImpl(std::move(node.nodeImpl)) {
}

JSONNode& JSONNode::operator=(JSONNode&& node) {
    if(this != &node) {
        nodeKey = std::move(node.nodeKey);
        nodeImpl = std::move(node.nodeImpl);
    }
    return *this;
}

JSONNode::JSONNode(std::unique_ptr<JSONNodeImpl>&& jsonImpl) :
    nodeImpl(std::move(jsonImpl)) {
}

JSONNode::~JSONNode() {
}

JSONNodeIterator JSONNode::begin() {
    if(!nodeImpl->val) {
        return JSONNodeIterator();
    } else if(nodeImpl->val->IsObject()) {
        auto begIt = nodeImpl->val->MemberBegin();
        return JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>(new JSONNodeIteratorImpl(begIt)));
    } else if(nodeImpl->val->IsArray()) {
        auto begIt = nodeImpl->val->Begin();
        return JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>(new JSONNodeIteratorImpl(begIt)));
    }
    return JSONNodeIterator();
}

JSONNodeIterator JSONNode::end() {
    if(!nodeImpl->val) {
        return JSONNodeIterator();
    } else if(nodeImpl->val->IsObject()) {
        auto endIt = nodeImpl->val->MemberEnd();
        return JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>(new JSONNodeIteratorImpl(endIt)));
    } else if(nodeImpl->val->IsArray()) {
        auto endIt = nodeImpl->val->End();
        return JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>(new JSONNodeIteratorImpl(endIt)));
    }
    return JSONNodeIterator();
}

void JSONNode::value(const std::string& key, std::string& value) const {
    if(key.empty()) {
        return;
    }
    if(!nodeImpl->val) {
        return;
    }
    auto memIt = nodeImpl->val->FindMember(key.c_str());
    if(memIt != nodeImpl->val->MemberEnd()) {
        if(memIt->value.IsString()) {
            value = memIt->value.GetString();
        }
    }
}

void JSONNode::value(const std::string& key, float& value) const {
    if(key.empty()) {
        return;
    }
    if(!nodeImpl->val) {
        return;
    }
    auto memIt = nodeImpl->val->FindMember(key.c_str());
    if(memIt != nodeImpl->val->MemberEnd()) {
        if(memIt->value.IsFloat()) {
            value = memIt->value.GetFloat();
        }
    }
}

void JSONNode::value(const std::string& key, int& value) const {
    if(key.empty()) {
        return;
    }
    if(!nodeImpl->val) {
        return;
    }
    auto memIt = nodeImpl->val->FindMember(key.c_str());
    if(memIt != nodeImpl->val->MemberEnd()) {
        if(memIt->value.IsInt()) {
            value = memIt->value.GetInt();
        }
    }
}

void JSONNode::value(std::string& value) const {
    if(!nodeImpl->val) {
        return;
    }
    if(nodeImpl->val->IsString()) {
        value = nodeImpl->val->GetString();
    }
}

void JSONNode::value(float& value) const {
    if(!nodeImpl->val) {
        return;
    }
    if(nodeImpl->val->IsFloat()) {
        value = nodeImpl->val->GetFloat();
    }
}

void JSONNode::value(int& value) const {
    if(!nodeImpl->val) {
        return;
    }
    if(nodeImpl->val->IsInt()) {
        value = nodeImpl->val->GetInt();
    }
}

JSONNode JSONNode::object(const std::string& key) const {
    JSONNode node;
    if(!nodeImpl->val) {
        return node;
    }
    if(nodeImpl->val->IsObject()) {
        auto it = nodeImpl->val->FindMember(key.c_str());
        if(it != nodeImpl->val->MemberEnd()) {
            std::unique_ptr<JSONNodeImpl> newNodeImpl(new JSONNodeImpl());
            newNodeImpl->val = &(it->value);
            newNodeImpl->isOwnValue = false;
            node.nodeKey = key;
            node.nodeImpl = std::move(newNodeImpl);
        }
    }
    return node;
}

const char* JSONNode::key() const {
    return nodeKey.c_str();
}

size_t JSONNode::size() const {
    if(!nodeImpl->val) {
        return 0u;
    } else if(nodeImpl->val->IsObject()) {
        return nodeImpl->val->MemberCount();
    } else if(nodeImpl->val->IsArray()) {
        nodeImpl->val->Size();
    }
    return 0u;
}

JSONNode::operator bool() const {
    return nodeImpl->val != nullptr;
}

JSONNode JSONNode::ParseBuffer(const Buffer& buff) {
    return ParseString(buff.getString());
}

JSONNode JSONNode::ParseString(const std::string& str) {
    if(str.empty()) {
        return JSONNode();
    }
    rapidjson::Document doc;
    doc.Parse(str.c_str());
    if(doc.HasParseError()) {
        LogError("Unexpected JSON parse error: %s (Offset %i)", rapidjson::GetParseError_En(
            doc.GetParseError()), doc.GetErrorOffset());
        return JSONNode();
    }
    std::unique_ptr<JSONNodeImpl> impl(new JSONNodeImpl);
    impl->isOwnValue = true;
    impl->val = new rapidjson::Document(std::move(doc));
    return JSONNode(std::move(impl));
}