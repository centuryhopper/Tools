#![allow(warnings)]

use std::cell::{Ref, RefCell};
use std::rc::Rc;

use linked_list_trait::LinkedListTrait;

type NodePointer<T> = Option<Rc<RefCell<ListNode<T>>>>;

#[derive(Debug)]
struct ListNode<T> {
    pub data: T,
    pub next: NodePointer<T>,
    pub prev: NodePointer<T>,
}

impl<T> ListNode<T> {
    pub fn new(data: T) -> Self {
        Self {
            data: data,
            next: None,
            prev: None,
        }
    }
}

/// doubly linked list
#[derive(Debug)]
pub struct DoublyLinkedList<T> {
    head: NodePointer<T>,
    tail: NodePointer<T>,
    size: u32,
}

impl<T: std::fmt::Debug + Clone + std::cmp::Ord> DoublyLinkedList<T>
{
    pub fn new(data: T) -> Self {
        let newNode = Rc::new(RefCell::new(ListNode::new(data)));
        DoublyLinkedList {
            head: Some(newNode.clone()),
            tail: Some(newNode),
            size: 1,
        }
    }
}


impl<T: std::fmt::Debug + Clone + std::cmp::Ord> LinkedListTrait<T> for DoublyLinkedList<T> {
    fn size(&self) -> u32 {
        self.size
    }

    fn tail_insert(&mut self, data: T) {
        let newNode = Rc::new(RefCell::new(ListNode::new(data)));
        match self.tail.take() {
            Some(tailNode) => {
                tailNode.borrow_mut().next = Some(newNode.clone());
                newNode.borrow_mut().prev = Some(tailNode);
                self.tail = Some(newNode);
            }
            None => {
                // panic!("Couldn't tailInsert this value");
                println!("list is empty so initializing its head and tail to this value...");

                self.head = Some(newNode.clone());
                self.tail = Some(newNode);
            }
        }

        self.size += 1;
    }

    fn head_insert(&mut self, data: T) {
        let newNode = Rc::new(RefCell::new(ListNode::new(data)));
        match self.head.take() {
            Some(headNode) => {
                headNode.borrow_mut().prev = Some(newNode.clone());
                newNode.borrow_mut().next = Some(headNode);
                self.head = Some(newNode);
            }
            None => {
                // panic!("Couldn't tailInsert this value");
                println!("list is empty so initializing its head and tail to this value...");

                self.head = Some(newNode.clone());
                self.tail = Some(newNode);
            }
        }

        self.size += 1;
    }

    fn peek_head(&self) -> Option<T>
    where
        T: Clone, // Ensure T can be cloned to return a copy
    {
        self.head.as_ref().map(|node| node.borrow().data.clone())
    }

    fn peek_tail(&self) -> Option<T>
    where
        T: Clone, // Ensure T can be cloned to return a copy
    {
        self.tail.as_ref().map(|node| node.borrow().data.clone())
    }

    fn show(&self) -> Vec<T> {
        if self.size == 0 {
            println!("List is empty.");
            return vec![];
        }

        let mut result: Vec<T> = vec![];

        let mut current = self.head.clone();
        while let Some(node) = current {
            // Borrow the RefCell to access the ListNode
            let node_ref: Ref<'_, ListNode<T>> = node.borrow();

            // Print the data
            // print!("{:?} ", node_ref.data);
            result.push(node_ref.data.clone());

            // Move to the next node
            current = node_ref.next.clone();
        }
        println!();

        return result;
    }

    fn tail_remove(&mut self) {
        if self.size() == 0 {
            return;
        }
        // move tail to tail's prev

        match self.tail.take() {
            Some(old_tail) => {
                if let Some(prev) = old_tail.borrow_mut().prev.take() {
                    prev.borrow_mut().next = None; // Disconnect the old tail from the previous node
                    self.tail = Some(prev); // Update the tail to the previous node
                } else {
                    // If there's no previous node, the list becomes empty
                    self.head = None;
                    self.tail = None;
                }
                self.size -= 1; // Decrement the size of the list
            }
            None => println!("list is empty"),
        }
    }

    fn head_remove(&mut self) {
        if self.size() == 0 {
            return;
        }
        // move head to head next

        match self.head.take() {
            Some(old_head) => {
                if let Some(next) = old_head.borrow_mut().next.take() {
                    next.borrow_mut().prev = None; // Disconnect the old tail from the previous node
                    self.head = Some(next); // Update the tail to the previous node
                } else {
                    // If there's no previous node, the list becomes empty
                    self.head = None;
                    self.tail = None;
                }
                self.size -= 1; // Decrement the size of the list
            }
            None => println!("list is empty"),
        }
    }

    fn insert_at_index(&mut self, idx: u32, value: T) {
        if self.size < idx {
            println!(
                "cannot insert beyond the list's current capacity: {}",
                self.size
            );
            return;
        }
        if idx == self.size() {
            self.tail_insert(value);
            return;
        }

        if idx == 0 {
            self.head_insert(value);
            return;
        }

        let mut cur = self.head.clone();
        let mut idxCnt = 0;
        let newNode = Rc::new(RefCell::new(ListNode::new(value)));
        while cur.is_some() {
            let node = cur.clone().unwrap();
            if idxCnt < idx {
                idxCnt += 1;
            } else {
                // put newNode right before it
                if let Some(prev) = node.borrow_mut().prev.take() {
                    prev.borrow_mut().next = Some(newNode.clone());
                    newNode.borrow_mut().prev = Some(prev);
                }

                newNode.borrow_mut().next = Some(node.clone());
                node.borrow_mut().prev = Some(newNode.clone());
                break;
            }
            cur = node.borrow().next.clone();
        }

        self.size += 1;
    }

    fn delete_at_index(&mut self, idx: u32) {
        if self.size == 0 {
            println!("cannot delete from an empty list");
            return;
        }
        if idx == self.size() {
            self.tail_remove();
            return;
        }

        if idx == 0 {
            self.head_remove();
            return;
        }

        let mut cur = self.head.clone();
        let mut idxCnt = 0;
        while cur.is_some() {
            let mut node = cur.clone().unwrap();
            if idxCnt == idx {
                let mut node = node.borrow_mut(); // Borrow the node mutably

                // First, take the `next` and `prev` pointers so we don't borrow the node twice
                let prev = node.prev.take();
                let next = node.next.take();

                // If there's a previous node, link it to the next node
                if let Some(prev_node) = prev {
                    if let Some(next_node) = next.clone() {
                        prev_node.borrow_mut().next = Some(next_node.clone());
                        next_node.borrow_mut().prev = Some(prev_node);
                    } else {
                        prev_node.borrow_mut().next = None; // If no next node, it's the tail
                    }
                } else if let Some(next_node) = next {
                    next_node.borrow_mut().prev = None; // If no previous node, it's the head
                    self.head = Some(next_node); // Move the head
                }

                break; // Exit the loop after deletion
            }

            cur = node.borrow().next.clone();
            idxCnt += 1;
        }

        self.size -= 1;
    }

    fn sort_list(&mut self) {
        if self.size == 0 {
            println!("{:?}", self.peek_head().unwrap());
            return;
        }

        let mut results = Vec::with_capacity(self.size() as usize);

        let mut tmp = self.head.clone();
        while tmp.is_some() {
            let cur = tmp.take().unwrap();
            results.push(cur.borrow().data.clone());
            tmp = cur.borrow().next.clone();
        }

        results.sort();

        println!("{:?}", results);
    }

    // size()
    // peekHead()
    // tailInsert()
    // headInsert()
    // show()
    // deleteAtIndex()
    // insertAtIndex()
    // tailRemove()
    // head_remove()
}

#[cfg(test)]
mod tests {

    // Note this useful idiom: importing names from outer (for mod tests) scope.
    use super::*;

    #[test]
    fn test_peek_head() {
        let mut list = DoublyLinkedList::new(3);
        assert!(!list.peek_head().is_none());
        if let Some(value) = list.peek_head() {
            assert_eq!(value, 3);
        }
    }

    #[test]
    fn test_peek_tail() {
        let mut list = DoublyLinkedList::new(3);
        list.tail_insert(7);
        assert!(!list.peek_tail().is_none());
        if let Some(value) = list.peek_tail() {
            assert_eq!(value, 7);
        }
    }

    #[test]
    fn test_size() {
        let mut list = DoublyLinkedList::new(3);
        list.tail_insert(4);
        list.tail_insert(5);
        list.tail_insert(6);
        list.tail_insert(7);
        list.headInsert(11);
        assert_eq!(list.size(), 6);
        list.head_remove();
        assert_eq!(list.size(), 5);
        list.tailRemove();
        assert_eq!(list.size(), 4);
    }

    #[test]
    pub fn overall_test() {
        let mut list = DoublyLinkedList::new(3);
        list.tail_insert(4);
        list.tail_insert(5);
        list.tail_insert(6);
        list.tail_insert(7);
        list.headInsert(11);

        assert_eq!(list.show(), vec![11, 3, 4, 5, 6, 7]);

        list.insert_at_index(2, 10);

        assert_eq!(list.show(), vec![11, 3, 10, 4, 5, 6, 7]);

        list.delete_at_index(3);

        assert_eq!(list.show(), vec![11, 3, 10, 5, 6, 7]);

        list.head_remove();
        list.tail_remove();

        assert_eq!(list.show(), vec![3, 10, 5, 6,]);
    }
}
