from abc import ABC, abstractmethod


class DatingBot(ABC):
    """Abstract class for the dating bots"""
    def wait_and_get_all(self, xpath, seconds = 5):
        pass
    def wait_and_get(self, xpath, seconds = 2.5):
        pass
    def screenshot(self, path=''):
        pass
    def get_button_parent(self, e):
        pass

    @abstractmethod
    def try_login(self) -> bool:
        """login to the url"""
        pass
    @abstractmethod
    def dispose(self) -> bool:
        """dispose of the webdriver"""
        pass
    @abstractmethod
    def handle_popups(self, milliseconds=0):
        """handle any popups that would obstruct bot from making progress"""
        pass
    @abstractmethod
    def send_message_to_matches(self):
        """send a greet message to each match in your queue"""
        pass
    @abstractmethod
    def like(self):
        """swipe right"""
        pass
    @abstractmethod
    def dislike(self):
        """swipe left"""
        pass
    @abstractmethod
    def no_more_likes(self) -> bool:
        """invoke an event for reaching swipe limit (useful when you're using the free version of the dating app or when your subscription has run out)"""
        pass
    @abstractmethod
    def ran_out_of_matches(self) -> bool:
        """invoke an event when you have no more people in your area to swipe on"""
        pass
    @abstractmethod
    def auto_swipe(self):
        """this is where all the main activity happens"""
        pass
    

# Subclass 1: Dog (overrides both methods)
# class Dog(Animal):
#     def sound(self):
#         return "Woof! Woof!"

#     def habitat(self):
#         return "Dogs usually live in houses or yards."

# # Subclass 2: Cat (overrides only sound)
# class Cat(Animal):
#     def sound(self):
#         return "Meow! Meow!"

#     def habitat(self):
#         """Uses the default implementation from the base class"""
#         return super().habitat()

# Testing the Abstract Class and Subclasses
# def main():
#     dog = Dog()
#     cat = Cat()

#     print("Dog sound:", dog.sound())
#     print("Dog habitat:", dog.habitat())

#     print("\nCat sound:", cat.sound())
#     print("Cat habitat:", cat.habitat())

# if __name__ == "__main__":
#     main()
