# Coding in C++ Using Rust Principles

Most of Rust's safety comes from ideas you can apply in modern C++ by convention. The difference is that in C++ you enforce them with types, warnings and tooling instead of a borrow checker.

## Quick mapping

| Rust              | C++ equivalent                                          |
|-------------------|---------------------------------------------------------|
| `Box<T>`          | `std::unique_ptr<T>`                                    |
| `Arc<T>`          | `std::shared_ptr<T>` (`Rc` has no non-atomic equivalent) |
| `&T` / `&mut T`   | `const T&` / `T&` (non-owning, never stored)            |
| `&[T]` / `&str`   | `std::span<const T>` / `std::string_view`               |
| `Option<T>`       | `std::optional<T>`                                      |
| `Result<T, E>`    | `std::expected<T, E>` (C++23)                           |
| `enum` with data  | `std::variant` + `std::visit`                           |
| traits            | concepts                                                |
| `Drop`            | destructors (RAII)                                      |
| `Mutex<T>`        | write your own wrapper (see section 7)                  |

## 1. Single ownership by default

Never use `new` or `delete`, and never let a raw pointer own anything. Use values first and `unique_ptr` when you need the heap. Reach for `shared_ptr` only when ownership really is shared, the same way you'd justify an `Arc`. Follow the rule of zero: if your members manage themselves, don't write any special member functions.

## 2. Make types move-only with an explicit `clone()`

Rust types move by default and copy only when you ask. C++ is the opposite, so turn off implicit copies for anything expensive or resource-like:

```cpp
class Buffer {
    std::vector<std::byte> data_;
public:
    Buffer() = default;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept = default;
    Buffer& operator=(Buffer&&) noexcept = default;

    [[nodiscard]] Buffer clone() const { Buffer b; b.data_ = data_; return b; }
};
```

C++ still lets you use a moved-from object. Turn on clang-tidy's `bugprone-use-after-move` check to catch that.

## 3. Borrow, but don't store borrows

Pass `const T&`, `std::span` and `std::string_view` into functions the way you would `&T`. The danger is that C++ won't check lifetimes. A good rule: references and views are fine as parameters and short-lived locals. Never put them in struct members or return them unless the lifetime is obvious. Clang's `[[clang::lifetimebound]]` attribute and `-Wdangling` catch some of the mistakes.

## 4. `const` by default

Treat `const` like Rust's default immutability. Write `const auto x = ...;` everywhere and drop the `const` only when you actually need to mutate. Mark member functions `const` unless they change state.

## 5. Errors as values, not exceptions

```cpp
enum class ParseError { Empty, NotANumber };

[[nodiscard]] std::expected<int, ParseError> parse(std::string_view s);

auto doubled = parse(input).transform([](int x) { return x * 2; });
if (!doubled) handle(doubled.error());
```

`and_then`, `transform` and `or_else` give you the combinator style. The biggest gap is that C++ has no `?` operator, so you'll write `if (!r) return std::unexpected(r.error());` a lot. Put `[[nodiscard]]` on anything that returns a result so callers can't ignore it.

## 6. Sum types and exhaustive matching

```cpp
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

using Shape = std::variant<Circle, Rect, Triangle>;

double area(const Shape& s) {
    return std::visit(overloaded{
        [](const Circle& c)   { return std::numbers::pi * c.r * c.r; },
        [](const Rect& r)     { return r.w * r.h; },
        [](const Triangle& t) { return 0.5 * t.b * t.h; },
    }, s);
}
```

If you add a variant and forget a case, it fails to compile, just like a non-exhaustive `match`. Use `enum class` instead of plain enums, and turn on `-Wswitch-enum`.

## 7. Data behind the lock, like `Mutex<T>`

Rust's best concurrency trick is that you can't touch the data without holding the lock. You can get most of that in C++:

```cpp
template <typename T>
class Mutex {
    std::mutex m_;
    T value_;
public:
    template <typename... Args>
    explicit Mutex(Args&&... args) : value_(std::forward<Args>(args)...) {}

    template <typename F>
    decltype(auto) with_lock(F&& f) {
        std::scoped_lock lock(m_);
        return std::invoke(std::forward<F>(f), value_);
    }
};

Mutex<std::vector<int>> shared;
shared.with_lock([](auto& v) { v.push_back(42); });
```

The lambda could still leak a reference out, but this design makes the right thing the easy thing. There's no `Send`/`Sync` checking, so run TSan on anything multithreaded.

## 8. Concepts instead of inheritance

Use concepts the way you'd use trait bounds, and use composition instead of class hierarchies. Save virtual dispatch for when you actually need `dyn Trait`-style runtime polymorphism.

## 9. No null, no implicit conversions

Use references or `gsl::not_null<T*>` when something can't be null, and `std::optional` when it can be missing. Make single-argument constructors `explicit`, and use strong wrapper types (`struct UserId { int value; };`) instead of passing raw ints around. That's the C++ version of Rust newtypes.

## 10. Keep the "unsafe" code in one place

Raw pointer math, `reinterpret_cast` and C APIs should live in a small, well-reviewed module behind a safe interface. This is the same discipline as keeping `unsafe` blocks small. Use ranges and `std::views::enumerate`/`zip` (C++23) instead of index loops so bounds bugs mostly can't happen.

## 11. Let tooling stand in for the borrow checker

- **Warnings:** `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror`
- **Static analysis:** clang-tidy with `bugprone-*`, `cppcoreguidelines-*` and `modernize-*`
- **Sanitizers:** ASan and UBSan in debug and test builds, TSan for threaded code
- **Library hardening:** `-D_GLIBCXX_ASSERTIONS` (libstdc++) adds bounds checks to `operator[]` and friends

With CMake, put all of this in a preset or a shared warnings target and every project gets it for free.

## Bottom line

These habits remove most memory bugs in practice, but they don't give you guarantees. A dangling `string_view` or a data race will still compile. Rust proves the absence of those bugs; disciplined C++ just makes them rare and easier to find.
