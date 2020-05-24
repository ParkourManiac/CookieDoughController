#ifndef LINKED_LIST_CPP
#define LINKED_LIST_CPP

#include "LinkedList.h"

template <class T>
void LinkedList<T>::Add(T item)
{
    Node<T> *newNode = new Node<T>();
    newNode->value = item;

    if (head == nullptr)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }

    length++;
}

template <class T>
void LinkedList<T>::Insert(size_t index, T item)
{
    Node<T> *newNode = new Node<T>();
    newNode->value = item;

    if (index == 0)
    {
        if (head == nullptr)
        {
            delete (newNode);
            Add(item);
            return; // Will prevent adding to length twice.
        }
        else
        {
            newNode->next = head;
            head = newNode;
        }
    }
    else
    {
        Node<T> *precedingNode = GetNodeAtIndex(index - 1);

        if (precedingNode != nullptr)
        {
            if (precedingNode->next != nullptr)
            {
                Node<T> *followingNode = precedingNode->next;

                precedingNode->next = newNode;
                newNode->next = followingNode;
            }
            else // we are one step past the end.
            {
                precedingNode->next = newNode;
                tail = newNode;
            }
        }
        else
        {
            // Throw error: Out of range.
            delete (newNode);
            return;
        }
    }

    length++;
}

template <class T>
bool LinkedList<T>::RemoveAtIndex(size_t index, T *valueOfDeleted) // TODO: Needs to be tested for memory leaks
{
    Node<T> *nodeToBeDeleted;
    if (index == 0)
    {
        if (head == nullptr) // Empty list
        {
            // Throw error: No elements in list!
            return false;
        }
        else
        {
            if (head->next == nullptr)
            { // Head is the last item.
                nodeToBeDeleted = head;
                head = nullptr;
                tail = nullptr;
            }
            else
            {
                nodeToBeDeleted = head;
                head = head->next;
            }
        }
    }
    else
    {
        Node<T> *precedingNode = GetNodeAtIndex(index - 1);

        if (precedingNode != nullptr)
        {
            if (precedingNode->next != nullptr)
            {
                nodeToBeDeleted = precedingNode->next;
                Node<T> *followingNode = nodeToBeDeleted->next;
                precedingNode->next = followingNode;

                if (followingNode == nullptr)
                { // We just deleted the tail.
                    tail = precedingNode;
                }
            }
            else // we are one step past the end.
            {
                // Throw error: Out of range.
                return false;
            }
        }
        else
        {
            // Throw error: Out of range.
            return false;
        }
    }

    *valueOfDeleted = nodeToBeDeleted->value;

    delete (nodeToBeDeleted);
    length--;

    return true;
}

template <class T>
bool LinkedList<T>::RemoveAtIndex(size_t index)
{
    T *ptr = new T();
    bool result =  LinkedList<T>::RemoveAtIndex(index, ptr);
    delete(ptr);

    return result;
}

template <class T>
void LinkedList<T>::Clear()
{
    while (head != nullptr)
    {
        RemoveAtIndex(0);
    }
}

template <class T>
Node<T> *LinkedList<T>::GetNodeAtIndex(size_t idx)
{
    if (head == nullptr)
        return nullptr; // Throw error: List is empty.

    Node<T> *current = head;
    unsigned int currentIndex = 0;

    while (currentIndex < idx)
    {
        current = current->next;
        currentIndex++;

        if (current == nullptr)
        {
            return nullptr; // Throw error: Out of range.
        }
    }

    return current;
}

template <class T>
T *LinkedList<T>::operator[](size_t idx)
{
    return &GetNodeAtIndex(idx)->value;
}

#endif