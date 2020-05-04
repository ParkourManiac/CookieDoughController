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
    int length = 0; // TODO: Needs to be tested (step 4)

    /**
     * @brief Add the item to the end of the list.
     * 
     * @param item The item to be added.
     */
    void Add(T item) { // TODO: Needs to be tested (step 1)
        Node<T> newNode = new Node<T>();
        newNode.value = item;

        if(head == nullptr) 
        {
            head = &newNode;
            tail = &newNode;
        } 
        else 
        {
            tail->next = &newNode;
            tail = &newNode;
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
    void Insert(int index, T item) { // TODO: Needs to be tested (step 3)
        Node<T> newNode = new Node<T>();
        newNode.value = item;

        if(index == 0) 
        {
            if(head == nullptr) {
                Add(item);
            } 
            else 
            {
                newNode.next = head;
                head = &newNode;
            }
        } 
        else 
        {
            Node<T> precedingNode = this[index - 1];

            if(precedingNode != NULL)
            {
                if(precedingNode.next != nullptr)
                {
                    Node<T>* followingNode = precedingNode.next;

                    precedingNode.next = &newNode;
                    newNode.next = followingNode;
                } 
                else // we are one step past the end.
                {
                    precedingNode.next = &newNode; 
                    tail = &newNode;
                }
            } 
            else 
            {
                // Throw an error..? (Out of range)
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
    T removeAtIndex(int index) { // TODO: Needs to be tested (step 3)
        Node<T>* nodeToBeDeleted;
        if(index == 0) 
        {
            if(head == nullptr) // Empty list
            {
                // Throw error: No elements in list!
                return NULL;
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
            Node<T> precedingNode = this[index - 1];

            if(precedingNode != NULL)
            {
                if(precedingNode.next != nullptr)
                {
                    nodeToBeDeleted = precedingNode.next;
                    Node<T>* followingNode = nodeToBeDeleted->next;

                    precedingNode.next = followingNode;
                } 
                else // we are one step past the end.
                {
                    // Throw an error..? (Out of range)
                    return NULL;
                }
            } 
            else 
            {
                // Throw an error..? (Out of range)
                return NULL;
            }
        }

        T valueOfDeleted = nodeToBeDeleted->value;

        delete(nodeToBeDeleted);
        length--;

        return valueOfDeleted;
    }

    /**
     * @brief Removes all items of the array and clears the memory.
     */
    void Clear() { // TODO: Needs to be tested (step 5)
        while(head != nullptr) {
            removeAtIndex(0);
        }
    }

    /**
     * @brief Retrieves the item at the specified index.
     * 
     * @param idx The index of the item.
     * @return T& The item at the specified index.
     */
    T& operator[](size_t idx) // TODO: Needs to be tested (step 2)
    {
        if(head == nullptr) return NULL; // Throw error: List is empty.

        Node<T>* current = head;
        int currentIndex = 0;

        while(currentIndex < idx)
        {
            current = current->next;
            currentIndex++;

            if(current == nullptr) 
            {
                return NULL; // Throw error instead? (Out of range)
            }
        }

        return *current;
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