/*
** EPITECH PROJECT, 2021
** B-OOP-400-TLS-4-1-tekspice-nassim.gharbaoui
** File description:
** Option
*/

#ifndef RTL_OPTION_HPP_
#define RTL_OPTION_HPP_

#include <functional>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace rtl {
namespace impl {

    template <typename T>
    class Base {
    public:
        constexpr Base() noexcept = default;

        Base(Base&&) = delete;

        ~Base()
        {
            clear();
        }

        // contains a value?
        bool is_some() const
        {
            return m_some;
        }

        // get a pointer to the contained value
        T* get()
        {
            if (m_some) {
                return reinterpret_cast<T*>(&m_value);
            } else {
                return nullptr;
            }
        }

        const T* get() const
        {
            if (m_some) {
                return reinterpret_cast<const T*>(&m_value);
            } else {
                return nullptr;
            }
        }

        // set to none
        void clear()
        {
            if (m_some) {
                get()->~T();
                m_some = false;
            }
        }

        // set to some
        template <
            typename... Args,
            typename = std::enable_if_t<
                std::is_constructible<T, Args...>::value>>
        void emplace(Args&&... args)
        {
            new (&m_value) T(std::forward<Args>(args)...);
            m_some = true;
        }

    private:
        bool m_some = false;
        alignas(T) unsigned char m_value[sizeof(T)];
    };

    template <typename T>
    class Base<T&> {
    public:
        constexpr Base() noexcept = default;

        ~Base()
        {
            clear();
        }

        // contains a value?
        bool is_some() const
        {
            return m_value != nullptr;
        }

        // get a pointer to the contained value
        std::add_pointer_t<T> get()
        {
            return m_value;
        }

        std::add_const_t<std::add_pointer_t<T>> get() const
        {
            return m_value;
        }

        // set to none
        void clear()
        {
            m_value = nullptr;
        }

        // set to some
        void emplace(std::add_lvalue_reference_t<T> ref)
        {
            m_value = std::addressof(ref);
        }

    private:
        std::add_pointer_t<T> m_value = nullptr;
    };

    template <typename M, typename T>
    class MethodProxy {
    public:
        using class_type = std::remove_cv_t<std::remove_reference_t<T>>;
        using method_type = M class_type::*;

    private:
        T instance;
        method_type method;

    public:
        MethodProxy(T obj, method_type method)
            : instance(std::move(obj))
            , method(method)
        {
        }

        template <typename... Args>
        decltype(auto) operator()(Args&&... args)
        {
            return (instance.*method)(std::forward<Args>(args)...);
        }
    };
}

template <typename T>
class Option;

template <typename T>
constexpr Option<T> some(T&&);

template <typename>
struct is_option : std::false_type {
};

template <typename T>
struct is_option<Option<T>> : std::true_type {
};

template <typename T>
class Option : impl::Base<T> {
public:
    constexpr Option() noexcept
        : impl::Base<T>()
    {
    }

    ~Option() = default;

    template <typename U,
        typename = std::enable_if_t<std::is_constructible<T, U>::value>>
    Option(Option<U>&& other)
        : impl::Base<T>()
    {
        if (other.is_some()) {
            impl::Base<T>::emplace(std::forward<U>(other.unwrap()));
        }
    }

    template <typename U,
        typename = std::enable_if_t<std::is_constructible<T, U>::value>>
    Option<T>& operator=(Option<U>&& other)
    {
        if (other) {
            impl::Base<T>::emplace(other.unwrap());
        } else {
            take();
        }

        return *this;
    }

    template <typename U>
    bool operator==(const Option<U>& other) const
    {
        if (is_none() || other.is_none()) {
            return is_none() && other.is_none();
        }

        return as_ref().unwrap() == other.as_ref().unwrap();
    }

    explicit operator bool() const
    {
        return is_some();
    }

    bool is_some() const
    {
        return impl::Base<T>::is_some();
    }

    bool is_none() const
    {
        return !is_some();
    }

    T expect(const std::string& msg)
    {
        if (is_none()) {
            throw std::runtime_error(msg);
        }

        T val(std::forward<T>(*impl::Base<T>::get()));

        impl::Base<T>::clear();
        return std::forward<T>(val);
    }

    T unwrap()
    {
        return std::forward<T>(expect("unwrap() called on a `none` value"));
    }

    template <typename = std::enable_if_t<!std::is_member_pointer<T>::value>>
    T unwrap_or(T val)
    {
        if (is_some()) {
            return std::forward<T>(unwrap());
        } else {
            return std::forward<T>(val);
        }
    }

    template <typename = std::enable_if_t<
                  std::is_default_constructible<T>::value>>
    T unwrap_or_default()
    {
        if (is_some()) {
            return std::forward<T>(unwrap());
        } else {
            return {};
        }
    }

    template <typename F>
    T unwrap_or_else(F&& f)
    {
        if (is_some()) {
            return std::forward<T>(unwrap());
        } else {
            return std::forward<T>(f());
        }
    }

    Option<T> take()
    {
        if (is_some()) {
            Option<T> oldVal;
            oldVal.impl::Base<T>::emplace(std::forward<T>(unwrap()));
            return oldVal;
        } else {
            return {};
        }
    }

    template <typename... Args,
        typename = std::enable_if_t<!std::is_member_pointer<T>::value
            && std::is_constructible<T, Args...>::value>>
    Option<T> replace(Args&&... args)
    {
        Option<T> oldVal = take();

        impl::Base<T>::emplace(std::forward<Args>(args)...);

        return oldVal;
    }

    template <typename = std::enable_if_t<std::is_member_pointer<T>::value>>
    Option<T> replace(T mem)
    {
        Option<T> oldVal = take();

        impl::Base<T>::emplace(mem);

        return oldVal;
    }

    Option<const T&> as_ref() const
    {
        Option<const T&> opt;

        if (is_some()) {
            opt.replace(*impl::Base<T>::get());
        }

        return opt;
    }

    Option<T&> as_mut()
    {
        Option<T&> opt;

        if (is_some()) {
            opt.replace(*impl::Base<T>::get());
        }

        return opt;
    }

    template <typename F,
        typename = std::enable_if_t<
            !std::is_void<std::result_of_t<F(T)>>::value>>
    decltype(auto) map(F&& f)
    {
        if (is_some()) {
            return some(f(unwrap()));
        } else {
            return none<decltype(f(unwrap()))>();
        }
    }

    template <typename F,
        typename = std::enable_if_t<
            !std::is_void<std::result_of_t<F(T)>>::value>>
    decltype(auto) and_then(F&& f)
    {
        return map(std::forward<F>(f)).flatten();
    }

    template <typename = std::enable_if_t<is_option<T>::value>>
    decltype(auto) flatten()
    {
        return unwrap_or_default();
    }

    template <typename... Args>
    typename Option<typename std::result_of<T(Args...)>::type>
    call(Args&&... args)
    {
        if (is_some()) {
            return some(unwrap()(std::forward<Args>(args)...));
        } else {
            return {};
        }
    }

    template <typename U>
    Option<U> operator&(Option<U>&& optb)
    {
        return is_some() ? std::forward<Option<U>>(optb) : {};
    }

    template <typename F,
        typename = std::enable_if_t<
            !std::is_void<std::result_of_t<F(T)>>::value>>
    decltype(auto) operator|(F&& f)
    {
        return map(f);
    }

    template <typename F,
        typename = std::enable_if_t<
            std::is_void<std::result_of_t<F(T)>>::value>>
    bool operator|(F&& f)
    {
        if (is_some()) {
            f(unwrap());
            return true;
        } else {
            return false;
        }
    }

    template <typename U, typename Q = T>
    decltype(auto) operator|(U Q::*method)
    {
        return *this | std::mem_fn(method);
    }

    template <typename M, typename Q = T>
    typename std::enable_if_t<!std::is_function<M>::value, Option<M>>
    operator[](M Q::*member)
    {
        return map([&](T val) {
            return val.*member;
        });
    }

    template <typename M, typename Q = T,
        typename = std::enable_if_t<std::is_function<M>::value>>
    decltype(auto) operator[](M Q::*member)
    {
        return map([&](T val) {
            return impl::MethodProxy<M, T>(std::forward<T>(val), member);
        });
    }

    template <typename... Args>
    typename Option<typename std::result_of<T(Args...)>::type>
    operator()(Args&&... args)
    {
        return call(std::forward<Args>(args)...);
    }
};

template <typename T>
constexpr Option<T> none()
{
    return {};
}

template <typename T>
constexpr Option<T> some(T&& v)
{
    Option<T> opt;

    opt.replace(std::forward<T>(v));
    return opt;
}

}

namespace std {

template <typename T>
struct hash<rtl::Option<T>> {
    using argument_type = rtl::Option<T>;
    using result_type = std::size_t;

    using base_type = std::remove_cv_t<std::remove_reference_t<T>>;

    result_type operator()(const argument_type& opt) const
    {
        result_type hash = 17;
        hash = hash * 31 + std::hash<bool>()(opt.is_some());
        if (opt) {
            hash = hash * 31 + std::hash<base_type>()(opt.as_ref().unwrap());
        }
        return hash;
    }
};

}

template <typename T>
std::ostream& operator<<(std::ostream& os, const rtl::Option<T>& opt)
{
    if (opt) {
        return os << "Some(" << opt.as_ref().unwrap() << ")";
    } else {
        return os << "None";
    }
}

#endif /* !RTL_OPTION_HPP_ */
