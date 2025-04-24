#![allow(warnings)]

use std::cell::{Ref, RefCell};
use std::rc::{Rc, Weak};

use linked_list_trait::LinkedListTrait;

type Next<T> = Option<Rc<RefCell<DoublyListNode<T>>>>;
type Prev<T> = Option<Weak<RefCell<DoublyListNode<T>>>>;

#[derive(Debug)]
struct DoublyListNode<T> {
    pub data: T,
    pub next: Next<T>,
    pub prev: Prev<T>,
}

impl<T> DoublyListNode<T> {
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
pub struct DoublyLinkedList<T: std::fmt::Debug + Clone + std::cmp::Ord + Default> {
    head: Next<T>,
    tail: Next<T>,
    size: u32,
}

impl<T: std::fmt::Debug + Clone + std::cmp::Ord + Default> DoublyLinkedList<T> {
    pub fn new(data: T) -> Self {
        let newNode = Rc::new(RefCell::new(DoublyListNode::new(data)));
        DoublyLinkedList {
            head: Some(newNode.clone()),
            tail: Some(newNode),
            size: 1,
        }
    }

    fn get_linked_list_parts(
        list: &mut DoublyLinkedList<T>,
    ) -> (DoublyLinkedList<T>, DoublyLinkedList<T>) {
        let mut slow = list.head.clone();
        let mut fast = list.head.clone();
        let mut mid = list.head.clone();
        let mut left_half_cnt = 0;
        while fast.is_some() && fast.clone().unwrap().borrow().next.is_some() {
            left_half_cnt += 1;
            mid = slow.clone();
            slow = slow.unwrap().borrow_mut().next.clone();
            fast = fast
                .unwrap()
                .borrow_mut()
                .next
                .clone()
                .unwrap()
                .borrow_mut()
                .next
                .clone();
        }

        {
            if let Some(mid_clone) = mid.clone() {
                let mid_next_node_option = { mid_clone.borrow_mut().next.clone() };

                if let Some(mid_next_node) = mid_next_node_option {
                    let mid_next_prev_weak_ptr_option = mid_next_node.borrow_mut().prev.clone();

                    if let Some(weakptr) = mid_next_prev_weak_ptr_option {
                        if let Some(upgraded_ptr) = weakptr.upgrade() {
                            upgraded_ptr.borrow_mut().prev = None;
                        }
                    }
                }
            }
        }

        // set mid next to None (severs the linked list)
        {
            if let Some(mid_node) = mid.clone() {
                mid_node.borrow_mut().next = None;
            }
        }

        (
            DoublyLinkedList {
                head: list.head.clone(),
                tail: mid.clone(),
                size: left_half_cnt,
            },
            DoublyLinkedList {
                head: slow,
                tail: list.tail.clone(),
                size: list.size() - left_half_cnt,
            },
        )
    }

    fn merge_two_sorted_list(
        head_a: &mut Option<Rc<RefCell<DoublyListNode<T>>>>,
        head_b: &mut Option<Rc<RefCell<DoublyListNode<T>>>>,
    ) -> DoublyLinkedList<T>
    where
        T: Default + PartialOrd + std::fmt::Debug + Clone,
    {
        // println!("vvv head_a");
        // DoublyLinkedList::print_list(&head_a);

        // println!("vvv head_b");
        // DoublyLinkedList::print_list(&head_b);

        let dummy_head = Rc::new(RefCell::new(DoublyListNode {
            data: T::default(),
            next: None,
            prev: None,
        }));

        let mut tail = dummy_head.clone();

        while head_a.is_some() && head_b.is_some() {
            let (next_node, should_advance_a) = {
                let a_ref = head_a.as_ref().unwrap().borrow();
                let b_ref = head_b.as_ref().unwrap().borrow();

                if a_ref.data < b_ref.data {
                    (head_a.clone(), true)
                } else {
                    (head_b.clone(), false)
                }
            };

            // set aux tail's pointer accordingly
            tail.borrow_mut().next = next_node.clone();

            // outer scope not necessary but added for clarity
            {
                // limit the scope of each mutable borrow by introducing separate blocks or by dropping the borrow explicitly before creating a new one.

                // set next_node's prev to tail
                let next_node_prev: Prev<T> = {
                    if let Some(next_node_rc) = next_node.clone() {
                        let next_node_rc_borrow_mut = next_node_rc.borrow_mut();
                        next_node_rc_borrow_mut.prev.clone();
                    }
                    None
                };

                if let Some(weak_ptr) = next_node_prev {
                    if let Some(rc) = weak_ptr.upgrade() {
                        rc.borrow_mut().prev = Some(Rc::downgrade(&tail));
                    }
                }
            }

            // move aux tail to its new next pointer
            tail = tail.clone().borrow_mut().next.clone().unwrap();

            // move a or b pointer depending on what was found before
            if should_advance_a {
                *head_a = tail.borrow().next.clone();
            } else {
                *head_b = tail.borrow().next.clone();
            }
        }

        // scope curly braces are needed to drop mutable borrow
        // since we immutably borrow dummy head afterwards
        // dummy head and tail refer to nodes within the same linked list
        {
            let mut tail_mut = tail.borrow_mut();
            tail_mut.next = if head_a.is_some() {
                head_a.clone()
            } else {
                head_b.clone()
            };
        }

        // exclude dummy head value
        let cnt = DoublyLinkedList::cnt_listnodes(&dummy_head.borrow().next);
        // include dummy head value
        // let cnt = DoublyLinkedList::cnt_listnodes(&Some(dummy_head.clone()));

        DoublyLinkedList {
            head: dummy_head.clone().borrow().next.clone(),
            tail: Self::get_tail(&Some(tail)).clone(),
            size: cnt,
        }
    }

    pub fn get_tail(head: &Next<T>) -> Next<T> {
        match head {
            Some(node) => {
                if let Some(node_next) = node.borrow().next.clone() {
                    return Self::get_tail(&Some(node_next));
                }
                return Some(node.clone());
            }

            None => {
                return None;
            }
        }
    }

    pub fn cnt_listnodes(head: &Option<Rc<RefCell<DoublyListNode<T>>>>) -> u32 {
        let mut cur = head.clone();
        let mut cnt = 0;
        while cur.is_some() {
            let x = cur.unwrap();
            cur = x.borrow().next.clone();
            cnt += 1;
        }
        // println!("cnt: {cnt}");
        cnt
    }

    // divide link list into two parts
    // and recursively do the same for them and then merge them when they're both sorted
    fn merge_sort(list: &mut DoublyLinkedList<T>) -> DoublyLinkedList<T> {
        // if there's only one node, it should return that node as a sorted one-element list, not an empty one.
        if list.head.is_none() || list.head.clone().unwrap().borrow().next.is_none() {
            return DoublyLinkedList {
                head: list.head.clone(),
                tail: list.tail.clone(),
                size: list.size,
            };
        }
        let (mut a_list, mut b_list) = DoublyLinkedList::get_linked_list_parts(list);
        let mut sorted_a = DoublyLinkedList::merge_sort(&mut a_list);
        let mut sorted_b = DoublyLinkedList::merge_sort(&mut b_list);
        let result =
            DoublyLinkedList::merge_two_sorted_list(&mut sorted_a.head, &mut sorted_b.head);
        return result;
    }
}

impl<T: std::fmt::Debug + Clone + std::cmp::Ord + Default> LinkedListTrait<T>
    for DoublyLinkedList<T>
{
    fn size(&self) -> u32 {
        self.size
    }

    fn tail_insert(&mut self, data: T) {
        let newNode = Rc::new(RefCell::new(DoublyListNode::new(data)));
        match self.tail.take() {
            Some(tailNode) => {
                tailNode.borrow_mut().next = Some(newNode.clone());
                newNode.borrow_mut().prev = Some(Rc::downgrade(&tailNode));
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
        let newNode = Rc::new(RefCell::new(DoublyListNode::new(data)));
        match self.head.take() {
            Some(headNode) => {
                headNode.borrow_mut().prev = Some(Rc::downgrade(&newNode));
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
            let node_ref: Ref<'_, DoublyListNode<T>> = node.borrow();

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
                    if let Some(prev_rc) = prev.upgrade() {
                        prev_rc.borrow_mut().next = None; // Disconnect the old tail from the previous node
                        self.tail = Some(prev_rc); // Update the tail to the previous node
                    }
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
        let newNode = Rc::new(RefCell::new(DoublyListNode::new(value)));
        while cur.is_some() {
            let node = cur.clone().unwrap();
            if idxCnt < idx {
                idxCnt += 1;
            } else {
                // put newNode right before "node"
                if let Some(prev) = node.borrow_mut().prev.take() {
                    // upgrade weak pointer to rc pointer
                    if let Some(prev_rc) = prev.upgrade() {
                        prev_rc.borrow_mut().next = Some(newNode.clone());
                        newNode.borrow_mut().prev = Some(Rc::downgrade(&prev_rc));
                    }
                }

                newNode.borrow_mut().next = Some(node.clone());
                node.borrow_mut().prev = Some(Rc::downgrade(&newNode));
                self.size += 1;
                break;
            }
            cur = node.borrow().next.clone();
        }
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
                    if let Some(prev_node_rc) = prev_node.upgrade() {
                        if let Some(next_node) = next.clone() {
                            prev_node_rc.borrow_mut().next = Some(next_node.clone());
                            next_node.borrow_mut().prev = Some(prev_node);
                        } else {
                            self.size -= 1;
                            prev_node_rc.borrow_mut().next = None; // If no next node, it's the tail
                        }
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
    }

    fn sort_list(&mut self) {
        // merge sort this doubly linked list
        let result = Self::merge_sort(self);
        self.head = result.head;
        self.tail = result.tail;
        self.size = result.size;
        // if self.size == 0 {
        //     println!("{:?}", self.peek_head().unwrap());
        //     return;
        // }

        // let mut results = Vec::with_capacity(self.size() as usize);

        // let mut tmp = self.head.clone();
        // while tmp.is_some() {
        //     let cur = tmp.take().unwrap();
        //     results.push(cur.borrow().data.clone());
        //     tmp = cur.borrow().next.clone();
        // }

        // results.sort();

        // println!("{:?}", results);
    }

    // size()
    // peekHead()
    // tailInsert()
    // head_insert()
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
    fn test_tail_and_head_inserts_and_peeks() {
        let mut list = DoublyLinkedList::new(10);
        assert_eq!(list.size(), 1);
        assert_eq!(list.peek_head(), Some(10));
        assert_eq!(list.peek_tail(), Some(10));

        list.tail_insert(20);
        list.head_insert(5);
        assert_eq!(list.show(), vec![5, 10, 20]);
        assert_eq!(list.peek_head(), Some(5));
        assert_eq!(list.peek_tail(), Some(20));
        assert_eq!(list.size(), 3);
    }

    #[test]
    fn test_tail_and_head_removals() {
        let mut list = DoublyLinkedList::new(10);
        list.tail_insert(20);
        list.head_insert(5);

        list.tail_remove();
        assert_eq!(list.show(), vec![5, 10]);

        list.head_remove();
        assert_eq!(list.show(), vec![10]);

        list.head_remove();
        assert_eq!(list.size(), 0);
        assert_eq!(list.peek_head(), None);
        assert_eq!(list.peek_tail(), None);
    }

    #[test]
    fn test_insert_and_delete_at_index() {
        let mut list = DoublyLinkedList::new(10);
        list.tail_insert(20);
        list.tail_insert(30);

        list.insert_at_index(1, 15);
        assert_eq!(list.show(), vec![10, 15, 20, 30]);

        list.delete_at_index(1);
        assert_eq!(list.show(), vec![10, 20, 30]);

        list.delete_at_index(0);
        assert_eq!(list.show(), vec![20, 30]);

        list.delete_at_index(1);
        assert_eq!(list.show(), vec![20]);

        list.delete_at_index(0);
        assert_eq!(list.show(), vec![]);
    }

    #[test]
    fn test_merge_sort_on_list() {
        let mut list = DoublyLinkedList::new(5);
        list.tail_insert(3);
        list.tail_insert(8);
        list.tail_insert(1);
        list.tail_insert(7);
        list.tail_insert(2);

        list.sort_list();
        assert_eq!(list.show(), vec![1, 2, 3, 5, 7, 8]);
    }

    #[test]
    fn test_merge_sort_on_empty_list() {
        let mut list: DoublyLinkedList<i32> = DoublyLinkedList {
            head: None,
            tail: None,
            size: 0,
        };

        list.sort_list();
        assert_eq!(list.show(), vec![]);
    }

    #[test]
    fn test_merge_sort_on_single_element() {
        let mut list = DoublyLinkedList::new(42);
        list.sort_list();
        assert_eq!(list.show(), vec![42]);
    }

    #[test]
    fn test_insert_at_invalid_index() {
        let mut list = DoublyLinkedList::new(1);
        list.insert_at_index(5, 10); // Should do nothing
        assert_eq!(list.show(), vec![1]);

        list.delete_at_index(5); // Should do nothing
        assert_eq!(list.show(), vec![1]);
    }

    

    #[test]
    fn test_get_tail() {
        // Case 1: Single element
        let list = DoublyLinkedList::new(42);
        let tail = DoublyLinkedList::get_tail(&list.head);
        assert!(tail.is_some());
        assert_eq!(tail.unwrap().borrow().data, 42);

        // Case 2: Multiple elements
        let mut list = DoublyLinkedList::new(1);
        list.tail_insert(2);
        list.tail_insert(3);
        let tail = DoublyLinkedList::get_tail(&list.head);
        assert!(tail.is_some());
        assert_eq!(tail.unwrap().borrow().data, 3);

        // Case 3: Empty list (only if allowed to create one)
        let list: DoublyLinkedList<i32> = DoublyLinkedList {
            head: None,
            tail: None,
            size: 0,
        };
        assert!(DoublyLinkedList::get_tail(&list.head).is_none());
    }

    #[test]
    fn test_empty_list_behavior() {
        let mut list: DoublyLinkedList<i32> = DoublyLinkedList {
            head: None,
            tail: None,
            size: 0,
        };

        assert_eq!(list.peek_head(), None);
        assert_eq!(list.peek_tail(), None);
        assert_eq!(list.size(), 0);
        assert_eq!(list.show(), vec![]);

        list.sort_list(); // Should not panic
        assert_eq!(list.show(), vec![]);
    }

    #[test]
    fn test_single_element_sort() {
        let mut list = DoublyLinkedList::new(42);
        list.sort_list();
        assert_eq!(list.show(), vec![42]);
    }

    #[test]
    fn test_insert_at_bounds() {
        let mut list = DoublyLinkedList::new(10);
        list.insert_at_index(0, 5); // head insert
        list.insert_at_index(2, 15); // tail insert
        assert_eq!(list.show(), vec![5, 10, 15]);
    }

    #[test]
    fn test_delete_at_bounds() {
        let mut list = DoublyLinkedList::new(1);
        list.tail_insert(2);
        list.tail_insert(3);
        list.tail_insert(4);
        list.delete_at_index(0); // delete head
        list.delete_at_index(2); // delete tail (after head removal, index 2 is tail)
        assert_eq!(list.show(), vec![2, 3]);
    }

    #[test]
    fn test_invalid_index_operations() {
        let mut list: DoublyLinkedList<i32> = DoublyLinkedList {
            head: None,
            tail: None,
            size: 0,
        };

        list.insert_at_index(1, 100); // Should print error
        list.delete_at_index(0); // Should print error
        assert_eq!(list.show(), vec![]);

        list = DoublyLinkedList::new(1);

        list.insert_at_index(5, 10); // Should print error
        list.delete_at_index(10); // Should print error
        println!("list size : {}", list.size());

        // println!("list: {}", list.size());
        assert_eq!(list.show(), vec![1]);
    }

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
    fn test_sort_already_sorted() {
        let mut list = DoublyLinkedList::new(1);
        for i in 2..=5 {
            list.tail_insert(i);
        }
        list.sort_list();
        assert_eq!(list.show(), vec![1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_sort_reverse_list() {
        let mut list = DoublyLinkedList::new(5);
        for i in (1..5).rev() {
            list.tail_insert(i);
        }
        list.sort_list();
        assert_eq!(list.show(), vec![1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_sort_with_duplicates() {
        let mut list = DoublyLinkedList::new(5);
        list.tail_insert(1);
        list.tail_insert(5);
        list.tail_insert(3);
        list.tail_insert(1);
        list.tail_insert(3);
        list.sort_list();
        assert_eq!(list.show(), vec![1, 1, 3, 3, 5, 5]);
    }

    #[test]
    fn test_size() {
        let mut list = DoublyLinkedList::new(3);
        list.tail_insert(4);
        list.tail_insert(5);
        list.tail_insert(6);
        list.tail_insert(7);
        list.head_insert(11);
        assert_eq!(list.size(), 6);
        list.head_remove();
        assert_eq!(list.size(), 5);
        list.tail_remove();
        assert_eq!(list.size(), 4);
    }

    #[test]
    pub fn overall_test() {
        let mut list = DoublyLinkedList::new(3);
        list.tail_insert(4);
        list.tail_insert(5);
        list.tail_insert(6);
        list.tail_insert(7);
        list.head_insert(11);

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
