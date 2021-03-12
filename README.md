# Rusty Template Library

One day, I had to write C++. But I refused!

## Option<T>

```cpp
// none
rtl::Option<std::string> opt;

// implicit cast to false for "None" values
assert(!opt);

// assign some value
opt = rtl::some("hello");

// implicit cast to true for "Some" values
assert(opt);

// unwrap takes ownership, leaving "None"
assert(opt.unwrap() == "hello");
assert(opt.unwrap_or(" world") == " world");

// alternative syntax to assign a value
opt = std::string("im here");

// as_ref can be used to "borrow" the value (immutably) instead:
rtl::Option<size_t> mapped = opt
                                 .as_ref()
                                 .map([](const std::string& name) {
                                     return name + ", too!";
                                 })
                                 .unwrap_or_default()
                                 .size();

assert(mapped.expect("what?!") == std::strlen("im here, too!"));

// set to none
opt = rtl::none();
```
