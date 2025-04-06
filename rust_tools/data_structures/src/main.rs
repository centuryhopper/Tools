use doubly_linked_list::DoublyLinkedList;
use linked_list_trait::LinkedListTrait;

fn main() {
    let mut list = DoublyLinkedList::new(3);
    // println!("{:?}", list.peek_head().unwrap());
    list.tail_insert(4);
    list.tail_insert(5);
    list.tail_insert(6);
    list.tail_insert(7);
    list.head_insert(11);

    // list.show();

    list.insert_at_index(2, 10);

    // list.show();

    list.delete_at_index(3);

    // list.show();

    list.head_remove();
    list.tail_remove();

    // list.show();

    // println!("{:?}", list.peek_tail().unwrap());
    list.sort_list();

    println!("{:?}", list.peek_head().unwrap());
}
