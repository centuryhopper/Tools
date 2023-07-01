from abc import ABC, abstractmethod

class Animal(ABC):
    def __init__(self):
      print('hi')

    @abstractmethod
    def speak(self) -> str:
        print("This is the abstract implementation of speak method in Animal class.")

class Dog(Animal):
    def __init__(self, name=''):
        super().__init__()
        print('dog says hi')

    def speak(self) -> str:
        super().speak()
        return "Woof!"

class Cat(Dog):
    def speak(self) -> str:
        super().speak()
        return "Meow!"

dog = Dog()
print(dog.speak())  # Output: Woof!
# cat = Cat()
# print(cat.speak())