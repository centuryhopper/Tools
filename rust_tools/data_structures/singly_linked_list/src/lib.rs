use std::{cell::RefCell, rc::Rc};

use linked_list_trait::LinkedListTrait;

#[derive(Debug)]
struct SinglyListNode<T> {
    pub data: T,
    pub next: Option<Rc<RefCell<SinglyListNode<T>>>>,
}

impl<T> SinglyListNode<T> {
    pub fn new(data: T) -> Self {
        Self {
            data: data,
            next: None,
        }
    }
}

/// for the sake of simplicity,
/// we will not use a tail pointer otherwise we need to use reference counting
#[derive(Debug)]
pub struct SinglyLinkedList<T: std::fmt::Debug + Clone + std::cmp::Ord + Default> {
    head: Option<Rc<RefCell<SinglyListNode<T>>>>,
    size: u32,
}

impl<T: std::fmt::Debug + Clone + std::cmp::Ord + Default> SinglyLinkedList<T> {
    pub fn new(data: T) -> Self {
        let new_node = Rc::new(RefCell::new(SinglyListNode::new(data)));
        Self {
            head: Some(new_node),
            size: 1,
        }
    }

    fn get_linked_list_parts(list: &mut SinglyLinkedList<T>) -> (SinglyLinkedList<T>, SinglyLinkedList<T>) {
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

        mid.unwrap().borrow_mut().next = None;

        (
            SinglyLinkedList {
                head: list.head.clone(),
                size: left_half_cnt,
            },
            SinglyLinkedList {
                head: slow,
                size: list.size() - left_half_cnt,
            },
        )
    }

    fn merge_two_sorted_list(
        head_a: &mut Option<Rc<RefCell<SinglyListNode<T>>>>,
        head_b: &mut Option<Rc<RefCell<SinglyListNode<T>>>>,
    ) -> SinglyLinkedList<T>
    where
        T: Default + PartialOrd + std::fmt::Debug + Clone,
    {
        // println!("vvv head_a");
        // SinglyLinkedList::print_list(&head_a);

        // println!("vvv head_b");
        // SinglyLinkedList::print_list(&head_b);

        let dummy_head = Rc::new(RefCell::new(SinglyListNode {
            data: T::default(),
            next: None,
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

            // move aux tail to its new next pointer
            tail = tail.clone().borrow_mut().next.clone().unwrap();

            // move a or b pointer depending on what was found before
            if should_advance_a {
                *head_a = tail.borrow().next.clone();
            } else {
                *head_b = tail.borrow().next.clone();
            }

            // original code that didn't work
            // because we were mutably borrowing within a scope that was also immutably borrowing already (look at a_ref as an example)
            {
                // let attacher = tail.clone();
                // let a = head_a.clone();
                // let b = head_b.clone();
                // let a_ref = a.as_ref().unwrap().borrow(); // Borrowed here
                // let b_ref = b.as_ref().unwrap().borrow(); // Also borrowed
                // if a_ref.data < b_ref.data
                // {
                //     attacher.borrow_mut().next = head_a.clone(); // Mutable borrow while a_ref is still alive
                //     head_a = attacher.borrow_mut().next.clone();
                // }
                // else
                // {
                //     attacher.borrow_mut().next = head_b.clone();
                //     head_b = head_b.unwrap().borrow().next.clone();
                // }
                // tail = tail.clone().borrow_mut().next.clone().unwrap();
                // println!("tail: {:?}", tail.borrow().data)
            }
        }

        // scope curly braces are needed to drop mutable borrow
        // since we immutably borrow dummy head afterwards
        // dummy head and tail refer to nodes within the same linked list
        {
            let mut tail_mut = tail.borrow_mut();
            tail_mut.next = if head_a.is_some() { head_a.clone() } else { head_b.clone() };
        }

        // exclude dummy head value
        let cnt = SinglyLinkedList::cnt_listnodes(&dummy_head.borrow().next);
        // include dummy head value
        // let cnt = SinglyLinkedList::cnt_listnodes(&Some(dummy_head.clone()));

        SinglyLinkedList {
            head: dummy_head.borrow().next.clone(),
            size: cnt,
        }
    }

    // divide link list into two parts
    // and recursively do the same for them and then merge them when they're both sorted
    fn merge_sort(list: &mut SinglyLinkedList<T>) -> SinglyLinkedList<T> {
        // if there's only one node, it should return that node as a sorted one-element list, not an empty one.
        if list.head.is_none() || list.head.clone().unwrap().borrow().next.is_none()
        {
            return SinglyLinkedList {
                head: list.head.clone(),
                size: list.size,
            };
        }
        let (mut a_list, mut b_list) = SinglyLinkedList::get_linked_list_parts(list);
        let mut sorted_a = SinglyLinkedList::merge_sort(&mut a_list);
        let mut sorted_b = SinglyLinkedList::merge_sort(&mut b_list);
        let result = SinglyLinkedList::merge_two_sorted_list(&mut sorted_a.head, &mut sorted_b.head);
        return result;
    }

    pub fn cnt_listnodes(head: &Option<Rc<RefCell<SinglyListNode<T>>>>) -> u32 {
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

    pub fn print_list(head: &Option<Rc<RefCell<SinglyListNode<T>>>>) {
        let mut cur = head.clone();
        while cur.is_some() {
            let x = cur.unwrap();
            println!("value: {:?}", x.borrow().data);
            cur = x.borrow().next.clone();
        }
    }
}

impl<T: std::fmt::Debug + Clone + std::cmp::Ord + Default> LinkedListTrait<T>
    for SinglyLinkedList<T>
{
    // uses merge sort to sort the singly linked list
    fn sort_list(&mut self) {
        // merge sort this singly linked list
        let result = SinglyLinkedList::merge_sort(self);
        self.head = result.head;
        self.size = result.size;
    }

    fn size(&self) -> u32 {
        self.size
    }

    fn tail_insert(&mut self, data: T) {
        let new_node = Rc::new(RefCell::new(SinglyListNode::new(data)));

        // make sure head is not empty
        // If list is empty, insert as head and tail
        if self.head.is_none() {
            self.head = Some(new_node);
            self.size += 1;
            return;
        }

        // Walk to the end using a mutable reference
        let mut current = self.head.clone(); // Get mutable reference to head
        while let Some(node) = current {
            let mut node_ref = node.borrow_mut();
            if node_ref.next.is_none() {
                node_ref.next = Some(new_node);
                self.size += 1;
                return;
            } else {
                current = node_ref.next.clone();
            }
        }
    }

    fn head_insert(&mut self, data: T) {
        let new_node = Rc::new(RefCell::new(SinglyListNode::new(data)));
        if self.head.is_none() {
            self.head = Some(new_node);
            return;
        }
        new_node.borrow_mut().next = self.head.take();
        self.head = Some(new_node);
        self.size += 1;
    }

    fn peek_head(&self) -> Option<T>
    where
        T: Clone,
    {
        self.head.as_ref().map(|node| {
            let node_ref = node.borrow();
            node_ref.data.clone()
        })
    }

    fn peek_tail(&self) -> Option<T> {
        let mut cur = self.head.clone();
        while let Some(node) = cur {
            let mut tmp = node.borrow_mut();
            if tmp.next.is_none() {
                return Some(tmp.data.clone());
            } else {
                cur = tmp.next.clone();
            }
        }

        return None;
    }

    fn show(&self) -> Vec<T> {
        if self.size() == 0 {
            println!("List is empty.");
            return vec![];
        }

        let mut res = vec![];

        let mut cur = self.head.clone();
        while let Some(node) = cur {
            let tmp = node.borrow_mut();
            res.push(tmp.data.clone());
            cur = tmp.next.clone();
        }

        res
    }

    fn head_remove(&mut self) {
        if self.size == 0 {
            return;
        }

        match self.head.take() {
            Some(old_head) => {
                if let Some(next) = old_head.borrow_mut().next.take() {
                    self.head = Some(next);
                } else {
                    self.head = None;
                }

                self.size -= 1;
            }
            None => {
                println!("list is empty");
            }
        }
    }

    fn tail_remove(&mut self) {
        if self.size == 0 {
            return;
        }

        self.size -= 1;

        // one node in the list
        if self.head.as_ref().unwrap().borrow().next.is_none() {
            self.head = None;
            return;
        }

        let mut cur = self.head.clone();

        while let Some(whats_inside) = cur {
            let mut node = whats_inside.borrow_mut();
            // arrived at tail
            if let Some(next_node) = &node.next {
                if next_node.borrow().next.is_none() {
                    node.next = None;
                    break;
                }
            }

            cur = node.next.clone();
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

        self.size += 1;

        let mut i = 0;
        let mut cur = self.head.clone();
        while i < idx - 1 {
            // println!("{:?}", cur.clone().unwrap().borrow().data);
            cur = cur.unwrap().borrow().next.clone();
            i += 1;
        }

        let new_node = Rc::new(RefCell::new(SinglyListNode::new(value)));
        // longer living unwrapped value with 'let binding'
        let cur_unwrapped = cur.unwrap();
        let mut cur_ref = cur_unwrapped.borrow_mut();
        // cur.next is now None after the line below
        // we do this for clarity
        new_node.borrow_mut().next = cur_ref.next.take();
        cur_ref.next = Some(new_node);
    }

    fn delete_at_index(&mut self, idx: u32) {
        if self.size < idx {
            println!(
                "cannot delete beyond the list's current capacity: {}",
                self.size
            );
            return;
        }

        if self.size == 0 {
            println!("cannot delete from an empty list");
            return;
        }
        if idx == self.size {
            self.tail_remove();
            return;
        }

        if idx == 0 {
            self.head_remove();
            return;
        }

        self.size -= 1;

        let mut i = 0;
        let mut cur = self.head.clone();
        while i < idx - 1 {
            cur = cur.unwrap().borrow().next.clone();
            i += 1;
        }

        // set cur.next to cur.next.next
        let tmp = cur.unwrap();
        let mut cur_ref = tmp.borrow_mut();
        let to_delete = cur_ref.next.take();
        cur_ref.next = to_delete.unwrap().borrow_mut().next.clone();
        // if let Some(node_to_delete) = to_delete {
        //     cur_ref.next = node_to_delete.borrow_mut().next.take(); // Skip over the node to delete
        // }
    }
}

#[cfg(test)]
mod tests {

    // Note this useful idiom: importing names from outer (for mod tests) scope.
    use super::*;

    // #[test]
    // fn test_peek_head() {
    //     let list = SinglyLinkedList::new(3);
    //     assert!(!list.peek_head().is_none());
    //     if let Some(value) = list.peek_head() {
    //         assert_eq!(value, 3);
    //     }
    // }

    // #[test]
    // fn test_peek_tail() {
    //     let mut list = SinglyLinkedList::new(3);
    //     list.tail_insert(7);
    //     assert!(!list.peek_tail().is_none());
    //     if let Some(value) = list.peek_tail() {
    //         assert_eq!(value, 7);
    //     }
    // }

    // #[test]
    // fn test_size() {
    //     let mut list = SinglyLinkedList::new(3);
    //     list.tail_insert(4);
    //     list.tail_insert(5);
    //     list.tail_insert(6);
    //     list.tail_insert(7);
    //     list.head_insert(11);
    //     assert_eq!(list.size(), 6);
    //     list.head_remove();
    //     assert_eq!(list.size(), 5);
    //     list.tail_remove();
    //     assert_eq!(list.size(), 4);
    // }

    #[test]
    pub fn overall_test() {
        let mut list = SinglyLinkedList::new(3);
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
        // let (mut a, mut b) = SinglyLinkedList::get_linked_list_parts(&mut list);
        // SinglyLinkedList::print_list(&a);
        // println!("a size: {}", a.size());
        // println!("b size: {}", b.size());
        // assert_eq!(a.show(), vec![3, 10]);
        // assert_eq!(b.show(), vec![5, 6]);

        list.tail_insert(5);
        list.tail_insert(4);
        list.tail_insert(3);
        list.tail_insert(2);
        list.tail_insert(45);
        list.tail_insert(33);
        // println!("{:?}", list.show());

        list.sort_list();
        assert_eq!(list.show(), vec![2, 3, 3, 4, 5, 5, 6, 10, 33, 45]);

        // assert_eq!(1, 2);
    }
}
