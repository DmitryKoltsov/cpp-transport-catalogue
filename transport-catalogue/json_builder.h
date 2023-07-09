#pragma once

#include "json.h"
#include <optional>

namespace json
{
    class DictArrayContext;
    class DictKeyContext;
    class DictItemContext;

	class Builder
	{
	public:
		Builder();
		DictKeyContext Key(const std::string& key);
		Builder& Value(Node::Value value);
		DictItemContext StartDict();
		DictArrayContext StartArray();
		Builder& EndDict();
		Builder& EndArray();
		Node Build();
		Node GetNode(Node::Value value);
	private:
		Node root_{ nullptr };
		std::vector<Node*> nodes_stack_;
		std::optional < std::string >key_{std::nullopt};
	};
    
    class DictItemContext
    {
        public:
        DictItemContext(Builder& builder);
        
        DictKeyContext Key(std::string key);
        Builder& EndDict();
        
        private:
        Builder& builder_;
    };
    
    class DictArrayContext
    {
        public:
        DictArrayContext(Builder& builder);
        
        DictArrayContext Value(Node::Value value);
        DictItemContext StartDict();
        DictArrayContext StartArray();
        Builder& EndArray();
        
        private:
        Builder& builder_;
    };
    
    class DictKeyContext
    {
        public:
        DictKeyContext(Builder& builder);
        
        DictItemContext Value(Node::Value value);
        DictItemContext StartDict();
        DictArrayContext StartArray();
        

        private:
        Builder& builder_;
    };

    class DictValueContext
    {
    public:
        DictValueContext(Builder& builder);

        DictKeyContext Key(std::string key);
        Builder& EndDict();


    private:
        Builder& builder_;
    };

    class DictArrValContext
    {
    public:
        DictArrValContext(Builder& builder);

        DictArrValContext Value(Node::Value value);
        DictItemContext StartDict();
        DictArrayContext StartArray();
        Builder& EndArray();
    private:
        Builder& builder_;
    };
}