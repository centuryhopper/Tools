

/*
🔸 Use &self when just reading data
🔸 Use &mut self when modifying self
🔸 Use self when taking ownership
🔸 Omit self for static or associated methods
*/


pub trait LinkedListTrait<T>
{
    fn size(&self) -> u32;
    fn tail_insert(&mut self, data: T);
    fn head_insert(&mut self, data: T);
    fn peek_head(&self) -> Option<T>;
    fn peek_tail(&self) -> Option<T>;
    fn show(&self) -> Vec<T>;
    fn head_remove(&mut self);
    fn tail_remove(&mut self);
    fn insert_at_index(&mut self, idx: u32, value: T);
    fn delete_at_index(&mut self, idx: u32);
    fn sort_list(&mut self);
}