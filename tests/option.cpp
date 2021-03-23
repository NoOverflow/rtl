/*
** EPITECH PROJECT, 2021
** B-OOP-400-TLS-4-1-tekspice-nassim.gharbaoui
** File description:
** option
*/

#include "rtl/Option.hpp"
#include <criterion/criterion.h>
#include <functional>
#include <string>
#include <unordered_set>

using rtl::none;
using rtl::Option;
using rtl::some;

class Unique {
public:
    Unique()
        : m_val(0)
    {
    }

    explicit Unique(int val)
        : m_val(val)
    {
    }

    Unique(const Unique&) = delete;

    Unique(Unique&& other)
        : m_val(other.m_val)
    {
    }

    ~Unique()
    {
    }

    bool operator==(const Unique& rhs) const
    {
        return m_val == rhs.m_val;
    }

    bool operator==(int rhs) const
    {
        return m_val == rhs;
    }

    int get() const
    {
        return m_val;
    }

private:
    int m_val = 0;
};

Test(option, default_constructor_is_none)
{
    Option<Unique> opt;

    cr_assert(!opt);
}

Test(option, some_constructor)
{
    Option<Unique> opt = some(Unique(4));

    cr_assert(opt);
    cr_assert_eq(opt.unwrap(), 4);
    cr_assert(!opt);
}

Test(option, some_default_constructor)
{
    auto opt = some(Unique());

    cr_assert(opt.is_some());
    cr_assert_eq(opt.unwrap(), 0);
}

Test(option, replace_some)
{
    Option<std::string> opt;

    cr_assert(opt.replace("hello").is_none());
    cr_assert(opt);
    cr_assert_eq(opt.unwrap(), "hello");
}

Test(option, replace_none)
{
    auto opt = some<std::string>("hello");

    cr_assert(opt.is_some());
    cr_assert_eq(opt.unwrap(), "hello");
    cr_assert(opt.replace("hi").is_none());
    cr_assert(opt.is_some());
    cr_assert_eq(opt.unwrap(), "hi");
}

Test(option, map_simple)
{
    auto opt = some(3);
    auto dbl = opt.map([](int v) { return v * 2; });

    cr_assert(dbl.is_some());
    cr_assert_eq(dbl.unwrap(), 6);
}

Test(option, unwrap_none)
{
    cr_assert_any_throw(none<int>().unwrap());
}

Test(option, unwrap_or)
{
    cr_assert_eq(none<int>().unwrap_or_default(), 0);
    cr_assert_eq(some(8).unwrap_or_default(), 8);
    cr_assert_eq(none<int>().unwrap_or(3), 3);
    cr_assert_eq(some(8).unwrap_or(3), 8);
    cr_assert_eq(none<int>().unwrap_or_else([]() { return 3; }), 3);
    cr_assert_eq(some(8).unwrap_or_else([]() { return 3; }), 8);
}

Test(option, stream_operator)
{
    std::stringstream ss;

    ss << some(9) << ", " << none<int>();
    cr_assert_eq(ss.str(), "Some(9), None");
}

Test(option, as_ref)
{
    Option<int> opt = some(3);
    Option<const int&> ref = opt.as_ref();

    cr_assert_eq(ref.unwrap(), 3);
}

Test(option, as_mut)
{
    Option<int> opt = some(3);
    Option<int&> mut = opt.as_mut();

    cr_assert(mut);
    mut.unwrap() = 5;
    cr_assert_eq(opt.unwrap(), 5);
}

Test(option, as_ref_none)
{
    Option<const int&> ref = none<int>().as_ref();

    cr_assert(!ref);
}

Test(option, as_mut_none)
{
    Option<int&> mut = none<int>().as_mut();

    cr_assert(!mut);
}

Test(option, as_ref_from_const)
{
    const Option<int> opt = some(3);
    Option<const int&> ref = opt.as_ref();

    cr_assert_eq(ref.unwrap(), 3);
}

Test(option, map_as_ref)
{
    const Option<Unique> mbOrig = some(Unique(3));
    Option<const Unique&> mbRef = mbOrig.as_ref();
    Option<Unique> mbDoubled = mbRef.map([](const Unique& u) {
        return Unique(u.get() * 2);
    });

    cr_assert(mbOrig);
    cr_assert(!mbRef);
    cr_assert(mbDoubled);
    cr_assert_eq(mbDoubled, some(Unique(6)));
}

Test(option, unique_ptr)
{
    Option<std::unique_ptr<int>> opt = some(std::make_unique<int>(3));

    cr_assert(opt);
    cr_assert_eq(*opt.unwrap(), 3);
    opt.replace(new int(56));
    opt.replace(new int(139));
    cr_assert(opt);
    cr_assert_eq(*opt.unwrap(), 139);
    opt.replace(std::make_unique<int>(42));
}

Test(option, hash_owned)
{
    Option<int> owned;
    Option<int&> mut;
    Option<const int&> ref;

    std::hash<Option<int>>()(owned);
    std::hash<Option<int&>>()(mut);
    std::hash<Option<const int&>>()(ref);
}

Test(option, hash_set)
{
    std::unordered_set<Option<std::string>> set;

    set.emplace();
    set.emplace();
    set.emplace(some("hi"));
    set.emplace(some("hello"));
    cr_assert_neq(set.find(none<std::string>()), set.end());
    cr_assert_neq(set.find(some("hi")), set.end());
    cr_assert_neq(set.find(some("hello")), set.end());
    cr_assert_eq(set.find(some("blabla")), set.end());
}

Test(option, flatten)
{
    cr_assert_eq(none<Option<int>>().flatten(), none<int>());
    cr_assert_eq(some(none<int>()).flatten(), none<int>());
    cr_assert_eq(some(some(3)).flatten(), some(3));
}

Test(option, and_then)
{
    auto sqr = [](int v) { return some(v * v); };
    auto nonify = [](int) { return none<int>(); };

    cr_assert_eq(none<int>().and_then(sqr), none<int>());
    cr_assert_eq(none<int>().and_then(nonify), none<int>());
    cr_assert_eq(some(5).and_then(sqr), some(25));
    cr_assert_eq(some(5).and_then(nonify), none<int>());
}

Test(option, pipe_some)
{
    auto opt = some(5);

    cr_assert_eq(
        opt
            | [](int n) { return n * n; }
            | [](int n) { return n + 1; }
            | [](int n) { return n / 2; }
            | [](int n) { return n - 5; },
        some((5 * 5 + 1) / 2 - 5));
}

Test(option, pipe_none)
{
    auto opt = none<int>();

    cr_assert((
        opt
        | [](int n) { return n * n; }
        | [](int n) { return n + 1; }
        | [](int n) { return n / 2; }
        | [](int n) { return n - 5; })
                  .is_none());
}

Test(option, pipe_some_void)
{
    auto opt = some(5);
    int piped = 0;

    cr_assert(
        opt
        | [](int n) { return n * n; }
        | [](int n) { return n + 1; }
        | [](int n) { return n / 2; }
        | [](int n) { return n - 5; }
        | [&](int n) { piped = n; });

    cr_assert_eq(piped, (5 * 5 + 1) / 2 - 5);
}

Test(option, pipe_none_void)
{
    auto opt = none<int>();
    int piped = 0;

    cr_assert_not(
        opt
        | [](int n) { return n * n; }
        | [](int n) { return n + 1; }
        | [](int n) { return n / 2; }
        | [](int n) { return n - 5; }
        | [&](int n) { piped = n; });

    cr_assert_eq(piped, 0);
}

static int return_five()
{
    return 5;
}

Test(option, function_pointer)
{
    Option<int (*)()> opt;

    cr_assert(opt.is_none());

    opt.replace(&return_five);
    cr_assert_eq(opt, some(&return_five));
}

Test(option, member_pointer)
{
    Option<decltype(&std::string::size)> opt;

    cr_assert(opt.is_none());

    opt.replace(&std::string::size);
    cr_assert(opt == some(&std::string::size));
}

Test(option, function_call_coalescing)
{
    auto opt = some([](int n) { return n * 2; });

    cr_assert_eq(opt(3), some(6));

    opt.take();
    cr_assert_eq(opt(5), none<int>());
}

Test(option, member_access)
{
    struct A {
        int field;

        int method() const
        {
            return field;
        }
    };

    cr_assert_eq(some<A>({ 5 })[&A::field], some(5));
    cr_assert_eq(some<A>({ 5 })[&A::method](), some(5));
    cr_assert_eq(some<A>({ 5 }) | &A::method, some(5));

    auto nothing = none<A>();
    cr_assert(nothing[&A::field].is_none());
    cr_assert(nothing[&A::method]().is_none());
    cr_assert((nothing | &A::method).is_none());
}

Test(option, example)
{
    // none
    rtl::Option<std::string> opt;

    // implicit cast to false for "None" values
    cr_assert(!opt);

    // assign some value
    opt = rtl::some("hello");

    // implicit cast to true for "Some" values
    cr_assert(opt);

    // unwrap takes ownership, leaving "None"
    cr_assert_eq(opt.unwrap(), "hello");
    cr_assert_eq(opt.unwrap_or(" world"), " world");

    // alternative syntax to assign a value
    opt = rtl::some("im here");

    // as_ref can be used to "borrow" the value (immutably) instead:
    rtl::Option<size_t> mapped = opt
                                     .as_ref()
                                     .map([](const std::string& name) {
                                         return name + ", too!";
                                     })
                                     .map([](const std::string& str) {
                                         return str.size();
                                     });

    cr_assert_eq(mapped.expect("what?!"), std::strlen("im here, too!"));

    // set to none
    opt = rtl::none<std::string>();
}
