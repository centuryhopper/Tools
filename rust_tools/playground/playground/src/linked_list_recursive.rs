use std::{cell::RefCell, rc::Rc};

// Option<T> implements the Clone trait if and only if the inner type T also implements Clone.
type SharedNode = Option<Rc<RefCell<Node>>>;
type SharedRefNode<'a> = Option<&'a Rc<RefCell<Node>>>;

struct Node {
    data: i32,
    next: SharedNode,
}

pub struct LinkedList {
    head: SharedNode,
    tail: SharedNode,
    size: usize,
}

impl LinkedList {
    pub fn new() -> Self {
        Self {
            head: None,
            tail: None,
            size: 0,
        }
    }
    pub fn front(&self) -> Option<i32> {
        self.head.as_ref().map(|n| n.borrow().data)
    }
    pub fn back(&self) -> Option<i32> {
        self.tail.as_ref().map(|n| n.borrow().data)
    }

    fn clear_runner(&mut self, mut cur: SharedNode) {
        match cur {
            Some(node) => {
                let next = node.as_ref().borrow_mut().next.take();
                self.clear_runner(next);
            }
            None => {
                let free_tail = self.tail.take();
                self.size = 0;
            }
        }
    }

    pub fn clear(&mut self) {
        let head = self.head.take();
        self.clear_runner(head);
    }

    pub fn size(&self) -> usize {
        self.size
    }

    fn to_vec_runner(&self, cur: SharedRefNode) -> Vec<i32> {
        let mut res: Vec<i32> = vec![];
        match cur {
            Some(node) => {
                res.push(node.borrow().data);
                let next = cur.unwrap().borrow().next.clone();
                [res, self.to_vec_runner(next.as_ref())].concat()
            }
            None => vec![],
        }
    }

    pub fn to_vec(&self) -> Vec<i32> {
        self.to_vec_runner(self.head.clone().as_ref())
    }

    pub fn to_vec_iterative(&self) -> Vec<i32> {
        let mut result = vec![];
        let mut cur = self.head.clone();
        while let Some(node) = cur {
            result.push(node.borrow().data);
            cur = node.borrow().next.clone();
        }

        result
    }

    fn find_runner(&self, cur: SharedRefNode, value: i32, idx: usize) -> Option<usize> {
        match cur {
            Some(node) => {
                if value == node.borrow().data {
                    return Some(idx);
                }

                return self.find_runner(
                    cur.unwrap().borrow().next.clone().as_ref(),
                    value,
                    idx + 1,
                );
            }
            None => None,
        }
    }

    fn node_create(value: i32, next: SharedNode) -> SharedNode {
        Some(Rc::new(RefCell::new(Node {
            data: value,
            next: next,
        })))
    }

    fn list_initialize(&mut self, value: i32) {
        self.head = Self::node_create(value, None);
        self.tail = self.head.clone();
        self.size += 1;
    }

    fn push_at_runner(&mut self, idx: usize, value: i32, prev: SharedRefNode, cur: SharedRefNode) {
        match cur {
            Some(node) => {
                if idx == 0 {
                    //let prev_clone = prev.cloned().clone();
                    // link new node's next to prev's next
                    let new_node = Self::node_create(value, cur.cloned());
                    // link prev's next to new node
                    if let Some(prev_node) = prev {
                        prev_node.borrow_mut().next = new_node;
                    }
                    self.size += 1;
                    return;
                }

                // as_ref converts Option<T> to Option<&T>
                let node_next = node.borrow().next.clone();
                self.push_at_runner(idx - 1, value, Some(node), node_next.as_ref());
            }
            None => {
                // head, tail
                //      x     =>  None
                let new_node = Self::node_create(value, None);
                // create new node and set prev next to it
                if let Some(prev_node) = prev {
                    prev_node.borrow_mut().next = new_node.clone();
                }
                // update tail pointer
                self.tail = new_node;
                self.size += 1;
            }
        };
    }

    fn erase_back_runner(&mut self, prev: SharedRefNode, cur: SharedRefNode) {
        let next_node_option = cur.unwrap().borrow().next.clone();
        match next_node_option {
            Some(node) => {
                self.erase_back_runner(cur, Some(&node));
            }
            None => {
                self.tail = prev.cloned();
                if let Some(tail) = &self.tail {
                    tail.borrow_mut().next = None;
                }
                self.size -= 1;

                // handle edge case
                if self.size == 0 {
                    self.head = None
                }
            }
        }
    }

    fn erase_at_runner(&mut self, idx: usize, prev: SharedRefNode, cur: SharedRefNode) {
        match cur {
            Some(node) => {
                if idx == 0 {
                    // set prev's next to cur's next
                    prev.as_ref().unwrap().borrow_mut().next = node.borrow().next.clone();
                    // set cur's next to None
                    node.as_ref().borrow_mut().next = None;
                    self.size -= 1;
                    return;
                }

                // Don't `let` a reference to a temporary; it dies at `;` (E0716).
                // `let` the owned value first, then `.as_ref()` where you use it.
                // With RefCell: borrow on its own line so the guard drops before recursing.
                let next = node.borrow().next.clone();
                self.erase_at_runner(idx - 1, cur, next.as_ref());
            }
            None => {
                panic!("shouldnt be here");
            }
        }
    }

    // return the index for which the item is found
    // or the size of the list if not found
    pub fn find(&self, value: i32) -> usize {
        self.find_runner(self.head.clone().as_ref(), value, 0)
            .unwrap_or(self.size)
    }

    pub fn push_front(&mut self, value: i32) {
        // empty list scenario
        if self.head.is_none() {
            self.list_initialize(value);
            return;
        }
        // new_head    head    tail
        // x    =>      y   =>  z
        let mut new_head = Self::node_create(value, self.head.clone());

        self.head = new_head.clone();

        self.size += 1;
    }
    pub fn push_back(&mut self, value: i32) {
        // empty list scenario
        if self.head.is_none() {
            self.list_initialize(value);
            return;
        }
        // head      tail      new_tail
        // x    =>      y   =>  z
        let mut new_tail = Self::node_create(value, None);
        self.tail.as_ref().unwrap().borrow_mut().next = new_tail.clone();
        self.tail = new_tail.clone();

        self.size += 1;
    }

    pub fn push_at(&mut self, idx: usize, value: i32) {
        if idx > self.size {
            panic!("idx cant exceed size of list")
        }

        // edge case
        if idx == 0 {
            self.push_front(value);
            return;
        }

        //if idx == self.size - 1 {
        //    self.push_back(value);
        //    return;
        //}

        // .as_ref() relieves the need to do this.
        //let head_clone = self.head.clone();
        //let head_ref = match &head_clone {
        //    Some(node) => Some(node),
        //    None => None,
        //};

        self.push_at_runner(idx, value, None, self.head.clone().as_ref());
    }
    pub fn erase_at(&mut self, idx: usize) {
        if idx > self.size {
            panic!("idx cant exceed size of list")
        }

        if idx == 0 {
            self.erase_front();
            return;
        }

        if idx == self.size - 1 {
            self.erase_back();
            return;
        }

        self.erase_at_runner(idx, None, self.head.clone().as_ref());
    }
    pub fn erase_back(&mut self) {
        if self.size == 0 {
            return;
        }

        self.erase_back_runner(None, self.head.clone().as_ref());
    }
    pub fn erase_front(&mut self) {
        if self.size == 0 {
            return;
        }
        self.head = self.head.take().unwrap().borrow().next.clone();
        if self.head.is_none() {
            self.tail = None
        }
        self.size -= 1;
    }
}

#[cfg(test)]
mod tests;
