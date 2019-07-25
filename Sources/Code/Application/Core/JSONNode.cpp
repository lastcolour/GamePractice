#include "JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>


using JSONValueT = rapidjson::GenericValue<rapidjson::UTF8<>>;
using JSONDocPtrT = std::shared_ptr<rapidjson::Document>;

struct JSONNodeImpl {

    JSONDocPtrT root;
    JSONValueT* val;

public:

    JSONNodeImpl(JSONNodeImpl&& node) :
        root(std::move(node.root)), val(node.val) {
        node.val = nullptr;
    }

    explicit JSONNodeImpl(const JSONDocPtrT& document) :
        root(document), val(nullptr) {}

    ~JSONNodeImpl() = default;

    JSONNodeImpl& operator=(JSONNodeImpl&& node) {
        if (this != &node) {
            root = std::move(node.root);
            val = node.val;
            node.val = nullptr;
        }
        return *this;
    }
};

struct JSONNodeIteratorImpl {

    typedef JSONValueT::MemberIterator MemberIterator;
    typedef JSONValueT::ValueIterator ValueIterator;

public:

    JSONNode node;
    union {
        MemberIterator memIt;
        ValueIterator valIt;
    };
    bool isMemIter;

public:

    JSONNodeIteratorImpl(MemberIterator& iter, JSONDocPtrT& doc) :
        memIt(iter), isMemIter(true) {
        node.nodeImpl.reset(new JSONNodeImpl(doc));
    }

    JSONNodeIteratorImpl(ValueIterator& iter, JSONDocPtrT& doc) :
        valIt(iter), isMemIter(false) {
        node.nodeImpl.reset(new JSONNodeImpl(doc));
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
    nodeImpl(new JSONNodeImpl(JSONDocPtrT())) {
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
        return JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>(
            new JSONNodeIteratorImpl(begIt, nodeImpl->root)));
    } else if(nodeImpl->val->IsArray()) {
        auto begIt = nodeImpl->val->Begin();
        return JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>(
            new JSONNodeIteratorImpl(begIt, nodeImpl->root)));
    }
    return JSONNodeIterator();
}

JSONNodeIterator JSONNode::end() {
    if(!nodeImpl->val) {
        return JSONNodeIterator();
    } else if(nodeImpl->val->IsObject()) {
        auto endIt = nodeImpl->val->MemberEnd();
        return JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>(
            new JSONNodeIteratorImpl(endIt, nodeImpl->root)));
    } else if(nodeImpl->val->IsArray()) {
        auto endIt = nodeImpl->val->End();
        return JSONNodeIterator(std::unique_ptr<JSONNodeIteratorImpl>(
            new JSONNodeIteratorImpl(endIt, nodeImpl->root)));
    }
    return JSONNodeIterator();
}

void JSONNode::value(const char* key, std::string& value) const {
    if(!key || !key[0]) {
        return;
    }
    if(!nodeImpl->val) {
        return;
    }
    auto memIt = nodeImpl->val->FindMember(key);
    if(memIt != nodeImpl->val->MemberEnd()) {
        if(memIt->value.IsString()) {
            value = memIt->value.GetString();
        }
    }
}

void JSONNode::value(const char* key, float& value) const {
    if(!key || !key[0]) {
        return;
    }
    if(!nodeImpl->val) {
        return;
    }
    auto memIt = nodeImpl->val->FindMember(key);
    if(memIt != nodeImpl->val->MemberEnd()) {
        if(memIt->value.IsFloat()) {
            value = memIt->value.GetFloat();
        } else if(memIt->value.IsInt()) {
            value = static_cast<float>(memIt->value.GetInt());
        }
    }
}

void JSONNode::value(const char* key, int& value) const {
    if(!key || !key[0]) {
        return;
    }
    if(!nodeImpl->val) {
        return;
    }
    auto memIt = nodeImpl->val->FindMember(key);
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
    } else if(nodeImpl->val->IsInt()) {
        value = static_cast<float>(nodeImpl->val->GetInt());
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

JSONNode JSONNode::object(const char* key) const {
    JSONNode node;
    if(!key || !key[0]) {
        return node;
    }
    if(!nodeImpl->val) {
        return node;
    }
    if(nodeImpl->val->IsObject()) {
        auto it = nodeImpl->val->FindMember(key);
        if(it != nodeImpl->val->MemberEnd() && !it->value.IsNull()) {
            std::unique_ptr<JSONNodeImpl> newNodeImpl(new JSONNodeImpl(nodeImpl->root));
            newNodeImpl->val = &(it->value);
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
        return nodeImpl->val->Size();
    }
    return 0u;
}

JSONNode::operator bool() const {
    return nodeImpl->val != nullptr;
}

JSONNode JSONNode::ParseBuffer(const Buffer& buff) {
    auto str = buff.getString();
    return ParseString(str.c_str());
}

JSONNode JSONNode::ParseString(const char* str) {
    if(!str || !str[0]) {
        return JSONNode();
    }
    rapidjson::Document doc;
    doc.Parse(str);
    if(doc.HasParseError()) {
        LogError("Unexpected JSON parse error: %s (Offset %i)", rapidjson::GetParseError_En(
            doc.GetParseError()), doc.GetErrorOffset());
        return JSONNode();
    }
    JSONDocPtrT docPtr(new rapidjson::Document(std::move(doc)));
    std::unique_ptr<JSONNodeImpl> impl(new JSONNodeImpl(docPtr));
    impl->val = impl->root.get();
    return JSONNode(std::move(impl));
}