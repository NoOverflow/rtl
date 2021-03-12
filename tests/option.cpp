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
    rtl::Option<Unique> opt;

    cr_assert(!opt);
}

Test(option, some_constructor)
{
    rtl::Option<Unique> opt = rtl::some(Unique(4));

    cr_assert(opt);
    cr_assert_eq(opt.unwrap(), 4);
    cr_assert(!opt);
}

Test(option, some_default_constructor)
{
    auto opt = rtl::some(Unique());

    cr_assert(opt);
    cr_assert_eq(opt.unwrap(), 0);
}

Test(option, replace_some)
{
    rtl::Option<std::string> opt;

    cr_assert(opt.replace("hello").is_none());
    cr_assert(opt);
    cr_assert_eq(opt.unwrap(), "hello");
}

Test(option, replace_none)
{
    auto opt = rtl::some<std::string>("hello");

    cr_assert(opt);
    cr_assert_eq(opt.unwrap(), "hello");
    cr_assert(opt.replace("hi").is_none());
    cr_assert(opt);
    cr_assert_eq(opt.unwrap(), "hi");
}

Test(option, map_simple)
{
    auto opt = rtl::some<int>(3);
    auto dbl = opt.map([](int v) { return v * 2; });

    cr_assert(dbl);
    cr_assert_eq(dbl.unwrap(), 6);
}

Test(option, unwrap_none)
{
    cr_assert_any_throw(rtl::none<int>().unwrap());
}

Test(option, unwrap_or)
{
    cr_assert_eq(rtl::none<int>().unwrap_or_default(), 0);
    cr_assert_eq(rtl::some<int>(8).unwrap_or_default(), 8);
    cr_assert_eq(rtl::none<int>().unwrap_or(3), 3);
    cr_assert_eq(rtl::some<int>(8).unwrap_or(3), 8);
    cr_assert_eq(rtl::none<int>().unwrap_or_else([]() { return 3; }), 3);
    cr_assert_eq(rtl::some<int>(8).unwrap_or_else([]() { return 3; }), 8);
}

Test(option, as_ref)
{
    rtl::Option<int> opt = rtl::some<int>(3);
    rtl::Option<const int&> ref = opt.as_ref();

    cr_assert_eq(ref.unwrap(), 3);
}

Test(option, as_mut)
{
    rtl::Option<int> opt = rtl::some<int>(3);
    rtl::Option<int&> mut = opt.as_mut();

    cr_assert(mut);
    mut.unwrap() = 5;
    cr_assert_eq(opt.unwrap(), 5);
}

Test(option, as_ref_none)
{
    rtl::Option<const int&> ref = rtl::none<int>().as_ref();

    cr_assert(!ref);
}

Test(option, as_mut_none)
{
    rtl::Option<int&> mut = rtl::none<int>().as_mut();

    cr_assert(!mut);
}

Test(option, as_ref_from_const)
{
    const rtl::Option<int> opt = rtl::some<int>(3);
    rtl::Option<const int&> ref = opt.as_ref();

    cr_assert_eq(ref.unwrap(), 3);
}

Test(option, map_as_ref)
{
    const rtl::Option<Unique> mbOrig = rtl::some(Unique(3));
    rtl::Option<const Unique&> mbRef = mbOrig.as_ref();
    rtl::Option<Unique> mbDoubled = mbRef.map([](const Unique& u) {
        return Unique(u.get() * 2);
    });

    cr_assert(mbOrig);
    cr_assert(!mbRef);
    cr_assert(mbDoubled);
    cr_assert_eq(mbDoubled, rtl::some(Unique(6)));
}

Test(option, unique_ptr)
{
    rtl::Option<std::unique_ptr<int>> opt(std::make_unique<int>(3));

    cr_assert(opt);
    cr_assert_eq(*opt.unwrap(), 3);
    opt.replace(new int(56));
    opt.replace(new int(139));
    cr_assert(opt);
    cr_assert_eq(*opt.unwrap(), 139);
    opt.replace(std::move(std::make_unique<int>(42)));
}

Test(option, hash_owned)
{
    rtl::Option<int> owned;
    rtl::Option<int&> mut;
    rtl::Option<const int&> ref;

    std::hash<rtl::Option<int>>()(owned);
    std::hash<rtl::Option<int&>>()(mut);
    std::hash<rtl::Option<const int&>>()(ref);
}

Test(option, hash_set)
{
    std::unordered_set<rtl::Option<std::string>> set;

    set.emplace();
    set.emplace();
    set.emplace("hi");
    set.emplace("hello");
    cr_assert_neq(set.find(rtl::none<std::string>()), set.end());
    cr_assert_neq(set.find(rtl::some<std::string>("hi")), set.end());
    cr_assert_neq(set.find(rtl::some<std::string>("hello")), set.end());
    cr_assert_eq(set.find(rtl::some<std::string>("blabla")), set.end());
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
    opt = std::string("im here");

    // as_ref can be used to "borrow" the value (immutably) instead:
    rtl::Option<size_t> mapped = opt
                                     .as_ref()
                                     .map([](const std::string& name) {
                                         return name + ", too!";
                                     })
                                     .unwrap_or_default()
                                     .size();

    cr_assert_eq(mapped.expect("what?!"), std::strlen("im here, too!"));

    // set to none
    opt = rtl::none();
}
