template <class T>
struct Node {
    T value;
    Node* next = nullptr;
};

// TODO: DOCUMENT ALL OF THIS CODE!!!!

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
    void Add(T item) {
        Node<T>* newNode = new Node<T>();
        newNode->value = item;

        if(head == nullptr) 
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

    /**
     * @brief Inserts the item into the list at the specified index. 
     * note: The item will be inserted in between the preceding item and the item
     * occupying the index.
     * 
     * @param index The index in which the item should be inserted.
     * @param item The item to be inserted.
     */
    void Insert(size_t index, T item) {
        Node<T>* newNode = new Node<T>();
        newNode->value = item;

        if(index == 0)
        {
            if(head == nullptr) {
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
            Node<T>* precedingNode = GetNodeAtIndex(index - 1);

            if(precedingNode != nullptr)
            {
                if(precedingNode->next != nullptr)
                {
                    Node<T>* followingNode = precedingNode->next;

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
                return;
            }
        }
        
        length++;
    }

    /**
     * @brief Removes the item at the specified index.
     * 
     * @param index The index of the item to be removed.
     * @return T The value of the removed item.
     */
    T* RemoveAtIndex(size_t index) {
        Node<T>* nodeToBeDeleted;
        if(index == 0) 
        {
            if(head == nullptr) // Empty list
            {
                // Throw error: No elements in list!
                return nullptr;
            } 
            else
            {
                if(head->next == nullptr) { // Head is the last item.
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
            Node<T>* precedingNode = GetNodeAtIndex(index - 1);

            if(precedingNode != nullptr)
            {
                if(precedingNode->next != nullptr)
                {
                    nodeToBeDeleted = precedingNode->next;
                    Node<T>* followingNode = nodeToBeDeleted->next;
                    precedingNode->next = followingNode;

                    if(followingNode == nullptr) { // We just deleted the tail.
                        tail = precedingNode;
                    }

                } 
                else // we are one step past the end.
                {
                    // Throw error: Out of range.
                    return nullptr;
                }
            } 
            else 
            {
                // Throw error: Out of range.
                return nullptr;
            }
        }

        T valueOfDeleted = nodeToBeDeleted->value;

        delete(nodeToBeDeleted);
        length--;

        return &valueOfDeleted; // TODO: Compiler warns this is a stack variable. This could either be fixed by throwing errors and returning a copy of the value. 2. This can be fixed by modifying a parameter reference instead of returning a pointer. 
    }

    /**
     * @brief Removes all items of the array and clears the memory.
     */
    void Clear() { // TODO: Needs to be tested (step 5)
        while(head != nullptr) {
            RemoveAtIndex(0);
        }
    }

    /**
     * @brief Retrieves the pointer to the Node at the specified index. 
     * 
     * @param idx The index of the node we want to retrieve.
     * @return Node<T>* The node at the specified index.
     */
    Node<T>* GetNodeAtIndex(size_t idx)
    {
        if(head == nullptr) return nullptr; // Throw error: List is empty.

        Node<T>* current = head;
        int currentIndex = 0;

        while(currentIndex < idx)
        {
            current = current->next;
            currentIndex++;

            if(current == nullptr) 
            {
                return nullptr; // Throw error: Out of range.
            }
        }

        return current;
    }

    /**
     * @brief Retrieves the pointer to the item at the specified index.
     * 
     * @param idx The index of the item.
     * @return T* The pointer to the item at the specified index.
     */
    T* operator[](size_t idx)
    {
        return &GetNodeAtIndex(idx)->value;
    }

private: 
    /**
     * @brief The first element of the list.
     */
    Node<T>* head = nullptr; // TODO: Needs to be tested.
    /**
     * @brief The last element of the list.
     */
    Node<T>* tail = nullptr; // TODO: Needs to be tested.
};