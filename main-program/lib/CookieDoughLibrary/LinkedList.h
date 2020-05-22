#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>
/**
 * @brief An individual node/part of the LinkedList. It contains the value
 * of the stored item and the pointer to the next node.
 * 
 * @tparam T The type of item we want to store in this node.
 */
template <class T>
struct Node {
    /**
     * @brief The value of the node.
     */
    T value;
    /**
     * @brief The pointer to the next node in the chain of nodes.
     */
    Node* next = nullptr;
};

/**
 * @brief A dynamic list where items can be added during runtime.
 * 
 * @tparam T The type of item to be stored in this list.
 */
template <class T>
class LinkedList {
public:
    /**
     * @brief The total length of the list.
     */
    int length = 0;

    /**
     * @brief Add the item to the end of the list.
     * 
     * @param item The item to be added.
     */
    void Add(T item);

    /**
     * @brief Inserts the item into the list at the specified index. 
     * note: The item will be inserted in between the preceding item and the item
     * occupying the index.
     * 
     * @param index The index in which the item should be inserted.
     * @param item The item to be inserted.
     */
    void Insert(size_t index, T item);

    /**
     * @brief Removes the item at the specified index.
     * 
     * @param index The index of the item to be removed.
     * @return T The value of the removed item.
     */
    T* RemoveAtIndex(size_t index);

    /**
     * @brief Removes all items of the array and clears the memory.
     */
    void Clear();

    /**
     * @brief Retrieves the pointer to the Node at the specified index. 
     * 
     * @param idx The index of the node we want to retrieve.
     * @return Node<T>* The node at the specified index.
     */
    Node<T>* GetNodeAtIndex(size_t idx);

    /**
     * @brief Retrieves the pointer to the item at the specified index.
     * 
     * @param idx The index of the item.
     * @return T* The pointer to the item at the specified index.
     */
    T* operator[](size_t idx);

private: 
    /**
     * @brief The first element of the list.
     */
    Node<T>* head = nullptr;
    /**
     * @brief The last element of the list.
     */
    Node<T>* tail = nullptr;
};

#endif