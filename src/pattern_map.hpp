#include "attribute_set.hpp"

#include <algorithm>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace co
{

template <typename T> class pattern_map
{
    using mapping_type = std::map<pattern, T>;
    using pattern_index_type = std::vector<typename mapping_type::iterator>;

public:
    using key_type = pattern;
    using mapped_type = T;
    using value_type = std::pair<const key_type, mapped_type>;
    using iterator = typename mapping_type::iterator;
    using const_iterator = typename mapping_type::const_iterator;

    /// Constructors
    pattern_map() = default;
    pattern_map(const pattern_map& other) = delete;
    pattern_map(pattern_map&& other);

    template <typename InputIt> pattern_map(InputIt first, InputIt last);

    pattern_map(std::initializer_list<value_type> ilist);

    /// Capacity/size
    bool empty() const { return m_items.empty(); }
    std::size_t size() const { return m_items.size(); }

    /// Lookup by path.
    bool contains(const fs::path& p) const;
    const_iterator find(const fs::path& p) const;
    const T& operator[](const fs::path& p) const { return at(p); }
    const T& at(const fs::path& p) const;
    const T* get(const fs::path& p) const;

    /// Lookup by pattern.
    bool contains(const key_type& key) const { return m_items.find(key) != m_items.end(); }
    iterator find(const key_type& key) { return m_items.find(key); }
    const_iterator find(const key_type& key) const { return m_items.find(key); }

    /// Return a reference to the mapped value if `pat` is already in the
    /// mapping; otherwise, associates a default-constructed value with `pat`
    /// and returns a reference to it.
    T& operator[](const pattern& pat);

    /// Insertion
    std::pair<iterator, bool> insert(const value_type& pair);
    template <typename InputIt> void insert(InputIt first, InputIt last);
    void insert(std::initializer_list<value_type> ilist);

    std::pair<iterator, bool> insert_or_assign(const key_type& key, const mapped_type& m);

    /// General modification
    void clear();
    void swap(pattern_map& other);

    /// Iterators
    iterator begin() { return m_items.begin(); }
    const_iterator begin() const { return m_items.begin(); }
    const_iterator cbegin() const { return m_items.cbegin(); }
    iterator end() { return m_items.end(); }
    const_iterator end() const { return m_items.end(); }
    const_iterator cend() const { return m_items.cend(); }

private:
    std::size_t update_pattern_index(iterator it);

    /* Class invariants are:
     *
     *   - Patterns (keys) in `m_items` are unique.
     *
     *   - There is a one-to-one correspondence between pairs in `m_items`
     *     and valid iterators in `m_pattern_index` pointing to them.
     *     (This implies the containers are the same size.)
     *
     *   - There is exactly one write into `m_attr_set` for each pattern,
     *     even if values associated with the pattern are updated.
     *
     */
    void assert_invariant()
    {
        assert(m_items.size() == m_pattern_index.size());
        assert(std::all_of(m_pattern_index.begin(), m_pattern_index.end(),
                           [&](const auto it) { return m_items.find(it->first) == it; }));
    }

private:
    mapping_type m_items;
    pattern_index_type m_pattern_index;
    attribute_set m_attr_set;
};

template <typename T>
template <typename InputIt>
pattern_map<T>::pattern_map(InputIt first, InputIt last)
    : pattern_map{}
{
    insert(first, last);
}

template <typename T>
pattern_map<T>::pattern_map(std::initializer_list<value_type> ilist)
    : pattern_map{}
{
    insert(ilist);
}

template <typename T> bool pattern_map<T>::contains(const fs::path& p) const
{
    auto it = find(p);
    return it != m_items.end();
}

template <typename T> auto pattern_map<T>::find(const fs::path& p) const -> const_iterator
{
    auto maybe_index_str = m_attr_set.get_optional(p);
    if (maybe_index_str)
    {
        const std::size_t idx = std::stoi(*maybe_index_str);
        assert(idx < m_pattern_index.size());
        return m_pattern_index.at(idx);
    }
    return m_items.end();
}

template <typename T> const T& pattern_map<T>::at(const fs::path& p) const
{
    auto it = find(p);
    if (it == m_items.end())
    {
        throw std::out_of_range{p.c_str()};
    }
    return it->second;
}

template <typename T> const T* pattern_map<T>::get(const fs::path& p) const
{
    auto it = find(p);
    if (it == m_items.end())
    {
        return nullptr;
    }
    return std::addressof(it->second);
}

template <typename T> T& pattern_map<T>::operator[](const pattern& pat)
{
    // TODO: avoid default construction if not needed.
    auto [it, inserted] = insert(std::make_pair(pat, T{}));
    (void)inserted;
    assert_invariant();
    return it->second;
}

template <typename T>
auto pattern_map<T>::insert(const value_type& pair) -> std::pair<iterator, bool>
{
    auto [it, inserted] = m_items.insert(pair);
    if (inserted)
    {
        update_pattern_index(it);
    }
    assert_invariant();
    return std::make_pair(it, inserted);
}

template <typename T>
auto pattern_map<T>::insert_or_assign(const key_type& key, const mapped_type& m)
    -> std::pair<iterator, bool>
{
    auto [it, inserted] = m_items.insert_or_assign(key, m);
    if (inserted)
    {
        update_pattern_index(it);
    }
    assert_invariant();
    return std::make_pair(it, inserted);
}

template <typename T>
template <typename InputIt>
void pattern_map<T>::insert(InputIt first, InputIt last)
{
    while (first != last)
    {
        insert(*first);
        first++;
    }
    assert_invariant();
}

template <typename T> void pattern_map<T>::insert(std::initializer_list<value_type> ilist)
{
    insert(ilist.begin(), ilist.end());
    assert_invariant();
}

template <typename T> void pattern_map<T>::clear()
{
    pattern_map<T> cleared;
    swap(cleared);
}

template <typename T> void pattern_map<T>::swap(pattern_map<T>& other)
{
    using std::swap;
    swap(m_pattern_index, other.m_pattern_index);
    swap(m_attr_set, other.m_attr_set);
    swap(m_items, other.m_items);
}

template <typename T> std::size_t pattern_map<T>::update_pattern_index(iterator it)
{
    assert(m_items.size() == m_pattern_index.size() + 1);
    m_pattern_index.push_back(it);
    auto index = m_pattern_index.size() - 1;
    m_attr_set.add_pattern(it->first, std::to_string(index));
    assert_invariant();
    return index;
}

template <typename T> inline void swap(pattern_map<T>& a, pattern_map<T>& b) { a.swap(b); }

} /* end namespace 'co' */