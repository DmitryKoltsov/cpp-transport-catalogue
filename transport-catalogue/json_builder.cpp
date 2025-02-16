#include "json_builder.h" 
 
namespace json 
{ 
	json::Builder::Builder() 
	{ 
		Node* root_ptr = &root_; 
		nodes_stack_.emplace_back(root_ptr); 
	} 
 
	DictKeyContext Builder::Key(const std::string& key) 
	{ 
		auto* top_node = nodes_stack_.back(); 
		if (top_node->IsDict() && !key_) 
		{ 
			key_ = std::move(key); 
		} 
		else throw(std::logic_error("Wrong map key: " + key)); 
 
		return *this; 
	} 
 
	Builder& Builder::Value(Node::Value value) 
	{ 
		auto* top_node = nodes_stack_.back(); 
 
		if (top_node->IsDict()) 
		{ 
			if (!key_) 
			{ 
				throw std::logic_error("Could not Value() for dict without key"); 
			} 
 
			auto& dict = std::get<Dict>(top_node->GetValue()); 
			auto [pos, _] = dict.emplace(std::move(key_.value()), Node{}); 
			key_ = std::nullopt; 
			top_node = &pos->second; 
			top_node->GetValue() = std::move(value); 
		} 
		else if (top_node->IsArray()) 
		{ 
			auto& array = std::get<Array>(top_node->GetValue()); 
			array.emplace_back(GetNode(value)); 
			top_node = &array.back(); 
		} 
		else if (root_.IsNull()) 
		{ 
			root_.GetValue() = std::move(value); 
		} 
		else 
		{ 
			throw std::logic_error("Value() called in unknown container"); 
		} 
		return *this; 
	} 
 
	DictItemContext Builder::StartDict() 
	{ 
		auto* top_node = nodes_stack_.back(); 
 
		if (top_node->IsDict()) 
		{ 
			if (!key_) 
			{ 
				throw std::logic_error("Could not StartDict() for dict without key"); 
			} 
 
			auto& dict = std::get<Dict>(top_node->GetValue()); 
			auto [pos, _] = dict.emplace(std::move(key_.value()), Dict()); 
			key_ = std::nullopt; 
			nodes_stack_.emplace_back(&pos->second); 
		} 
		else if (top_node->IsArray()) 
		{ 
			auto& array = std::get<Array>(top_node->GetValue()); 
			array.emplace_back(Dict()); 
			nodes_stack_.emplace_back(&array.back()); 
		} 
		else if (root_.IsNull()) 
		{ 
			top_node->GetValue() = Dict(); 
		} 
		else 
		{ 
			throw std::logic_error("Wrong prev node"); 
		} 
		return *this; 
	} 
 
	DictArrayContext Builder::StartArray() 
	{ 
		auto* top_node = nodes_stack_.back(); 
 
		if (top_node->IsDict()) 
		{ 
			if (!key_) 
			{ 
				throw std::logic_error("Could not StartArray() for dict without key"); 
			} 
 
			auto& dict = std::get<Dict>(top_node->GetValue()); 
			auto [pos, _] = dict.emplace(std::move(key_.value()), Array()); 
			key_ = std::nullopt; 
			nodes_stack_.emplace_back(&pos->second); 
		} 
		else if (top_node->IsArray()) 
		{ 
			auto& array = std::get<Array>(top_node->GetValue()); 
			array.emplace_back(Array()); 
			nodes_stack_.emplace_back(&array.back()); 
		} 
		else if (root_.IsNull()) 
		{ 
			top_node->GetValue() = Array(); 
		} 
		else 
		{ 
			throw std::logic_error("Wrong prev node"); 
		} 
		return *this; 
	} 
 
	Builder& Builder::EndDict() 
	{ 
		auto* top_node = nodes_stack_.back(); 
 
		if (!top_node->IsDict()) 
		{ 
			throw std::logic_error("Prev node is not an Dict"); 
		} 
		nodes_stack_.pop_back(); 
 
		return *this; 
	} 
 
	Builder& Builder::EndArray() 
	{ 
		auto* top_node = nodes_stack_.back(); 
 
		if (!top_node->IsArray()) 
		{ 
			throw std::logic_error("Prev node is not an Array"); 
		} 
		nodes_stack_.pop_back(); 
 
		return *this; 
	} 
 
	Node Builder::Build() 
	{ 
		if (root_.IsNull() || nodes_stack_.size() > 1) 
		{ 
			throw std::logic_error("Wrong Build()"); 
		} 
 
		return root_; 
	} 
 
	Node Builder::GetNode(Node::Value value) 
	{ 
		if (std::holds_alternative<int>(value)) return Node(std::get<int>(value)); 
		if (std::holds_alternative<double>(value)) return Node(std::get<double>(value)); 
		if (std::holds_alternative<std::string>(value)) return Node(std::get<std::string>(value)); 
		if (std::holds_alternative<std::nullptr_t>(value)) return Node(std::get<std::nullptr_t>(value)); 
		if (std::holds_alternative<bool>(value)) return Node(std::get<bool>(value)); 
		if (std::holds_alternative<Dict>(value)) return Node(std::get<Dict>(value)); 
		if (std::holds_alternative<Array>(value)) return Node(std::get<Array>(value)); 
		return {}; 
	} 
     
    DictItemContext::DictItemContext(Builder& builder) : builder_(builder) 
    { 
         
    } 
         
	DictKeyContext DictItemContext::Key(std::string key) 
	{ 
		return builder_.Key(std::move(key)); 
	} 
     
    Builder& DictItemContext::EndDict() 
    { 
        return builder_.EndDict(); 
    } 
	DictArrayContext::DictArrayContext(Builder& builder) : builder_(builder) 
	{ 
 
	} 
	DictArrayContext DictArrayContext::Value(Node::Value value) 
	{ 
		return builder_.Value(move(value)); 
	} 
	DictItemContext DictArrayContext::StartDict() 
	{ 
		return builder_.StartDict(); 
	} 
	DictArrayContext DictArrayContext::StartArray() 
	{ 
		return builder_.StartArray(); 
	} 
	Builder& DictArrayContext::EndArray() 
	{ 
		return builder_.EndArray(); 
	} 
	DictKeyContext::DictKeyContext(Builder& builder) : builder_(builder) 
	{ 
 
	} 
	DictItemContext DictKeyContext::Value(Node::Value value) 
	{ 
		return builder_.Value(move(value)); 
	} 
	DictItemContext DictKeyContext::StartDict() 
	{ 
		return builder_.StartDict(); 
	} 
	DictArrayContext DictKeyContext::StartArray() 
	{ 
		return builder_.StartArray(); 
	} 
	DictValueContext::DictValueContext(Builder& builder) : builder_(builder) 
	{ 
 
	} 
	DictKeyContext DictValueContext::Key(std::string key) 
	{ 
		return builder_.Key(move(key)); 
	} 
	Builder& DictValueContext::EndDict() 
	{ 
		return builder_.EndDict(); 
	} 
	DictArrValContext::DictArrValContext(Builder& builder) : builder_(builder) 
	{ 
 
	} 
	DictArrValContext DictArrValContext::Value(Node::Value value) 
	{ 
		return builder_.Value(move(value)); 
	} 
	DictItemContext DictArrValContext::StartDict() 
	{ 
		return builder_.StartDict(); 
	} 
	DictArrayContext DictArrValContext::StartArray() 
	{ 
		return builder_.StartArray(); 
	} 
	Builder& DictArrValContext::EndArray() 
	{ 
		return builder_.EndArray(); 
	} 
}