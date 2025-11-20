#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

public:
    // Итератор
    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) {
            node_ = node;
        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept: node_(other.node_){}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this -> node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(this -> node_ == rhs.node_);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this -> node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return this -> node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept {
			assert(node_ != nullptr); 
			// Не совсем понял, о каком съезде строки речь, у меня всё хорошо :)
            node_ = node_ -> next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
			assert(node_ != nullptr);
            auto old_it(*this);
            ++(*this);
            return old_it;
        }

        [[nodiscard]] reference operator*() const noexcept {
			assert(node_ != nullptr);
            return node_ -> value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
			assert(node_ != nullptr);
            return &node_ -> value;
        }

    private:
        Node* node_ = nullptr;
    };



public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    // Конструкторы
    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        for(const auto& value : values){
            PushBack(value);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList tmp;
        for(const auto& el : other){
            tmp.PushBack(el);
        }
        swap(tmp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (&rhs == this) {
            return *this;
        }
        SingleLinkedList tmp(rhs);
        this -> swap(tmp);
        return *this;
    }

    ~SingleLinkedList(){
        Clear();
    }


    // Методы для работы с элементами списка
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value,head_.next_node);
        ++size_;
    }

    void PushBack(const Type& value) {
        Node* new_node = new Node(value, nullptr);

        if (!head_.next_node) { // Если список пуст
            head_.next_node = new_node;
        } else {
            Node* current = head_.next_node;
            while (current->next_node) {
                current = current->next_node;
            }
            current->next_node = new_node;
        }
        ++size_;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
		assert(pos.node_ != nullptr); 
		// Тут у меня тоже всё в порядке, строка хорошая, может там, где у вас отображается съезжает. На гите тоже ок

        Node* new_node = new Node(value, pos.node_ -> next_node);
        pos.node_ -> next_node = new_node;

        ++size_;

        return Iterator{new_node};
    }

    void PopFront() noexcept {
		assert(head_.next_node != nullptr);

		auto* del_node = head_.next_node;
		head_.next_node = head_.next_node -> next_node;
		delete del_node;

        size_--;
    }


    Iterator EraseAfter(ConstIterator pos) noexcept {
		assert(pos.node_ != nullptr);
		assert(IsEmpty() == false);

        auto* del_node = pos.node_ -> next_node;
        pos.node_ -> next_node = pos.node_ -> next_node -> next_node;
        delete del_node;

        size_--;

        return Iterator{pos.node_ -> next_node};
    }

    void Clear() noexcept {
        while(head_.next_node){
            Node* for_deleted = head_.next_node;
            head_.next_node = head_.next_node -> next_node;
            delete for_deleted;
        }

        size_ = 0;
    }

    void swap(SingleLinkedList& other) noexcept{
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }



    //Методы для работы с итератором списка
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

	// Тут я пытался найти более элегентаное решение, либо то, что упоминалось в задании
	// Но увы. С конструкцией return ConstIterator{const_cast<Node*>(&head_)}; мне помогла нейросеть.
	// Иначе была ошибка приведения типа.
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_{};
    size_t size_ = 0;
};


template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	if(&lhs == &rhs){
		return true;
	} else if(lhs.GetSize() == rhs.GetSize()){
		return true;
	} else{
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(),lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs > rhs);
}