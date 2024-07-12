
#include <igraph_pmt.h>

#ifndef GRAPH_LIST
template<>
#endif
class LIST_TYPE_TEMPL {
    using igraph_type = TYPE;

    igraph_type list;
    igraph_type *ptr = &list;

    bool is_alias() const { return ptr != &list; }

public:
    using value_type = ELEM_TYPE;
    using reference = value_type;
    using size_type = igraph_integer_t;
    using difference_type = igraph_integer_t;

    class iterator;

    explicit LIST_TYPE(igCaptureType<igraph_type> tl) : list(tl.obj) { }
    explicit LIST_TYPE(igAliasType<igraph_type> tl) : ptr(&tl.obj) { }

    explicit LIST_TYPE(size_type n = 0) {
        igCheck(FUNCTION(init)(ptr, n));
    }

    LIST_TYPE(LIST_TYPE &&other) noexcept {
        if (other.is_alias()) {
            ptr = other.ptr;
        } else {
            list = other.list;
        }
        other.ptr = nullptr;
    }

    LIST_TYPE(const LIST_TYPE &other) = delete;

    LIST_TYPE & operator = (const LIST_TYPE &other) = delete;

    LIST_TYPE & operator = (LIST_TYPE &&other) = delete;

    ~LIST_TYPE() {
        if (! is_alias())
            FUNCTION(destroy)(ptr);
    }

    operator TYPE *() { return ptr; }
    operator const TYPE *() const { return ptr; }

    size_type size() const { return ptr->end - ptr->stor_begin; }
    size_type capacity() const { return ptr->stor_end - ptr->stor_begin; }

    reference operator [] (size_type i) { return value_type(igAlias(ptr->stor_begin[i])); }
    const reference operator [] (size_type i) const { return value_type(igAlias(ptr->stor_begin[i])); }

    void clear() { FUNCTION(clear)(ptr); }
    void resize(size_type size) { igCheck(FUNCTION(resize)(ptr, size)); }
    void reserve(size_type capacity) { igCheck(FUNCTION(reserve)(ptr, capacity)); }

    iterator begin();
    iterator end();

    reference back() { return value_type(igAlias(*FUNCTION(tail_ptr)(ptr))); }
    const reference back() const { return value_type(igAlias(*FUNCTION(tail_ptr)(ptr))); }

    // List takes ownership of t
    void set(igraph_integer_t pos, value_type &t) {
        FUNCTION(set)(ptr, pos, t);
        t.ptr = &ptr->stor_begin[pos]; // set as alias
    }

    // List takes ownership of t
    void set(igraph_integer_t pos, value_type &&t) {
        FUNCTION(set)(ptr, pos, t);
        t.ptr = nullptr;
    }

    // List takes ownership of t
    void set(igraph_integer_t pos, value_type::igraph_type *t) {
        FUNCTION(set)(ptr, pos, t);
    }

    // List takes ownership of t
    void push_back(value_type &t) {
        igCheck(FUNCTION(push_back)(ptr, t));
        t.ptr = FUNCTION(tail_ptr)(ptr); // set as alias
    }

    // List takes ownership of t
    void push_back(value_type &&t) {
        igCheck(FUNCTION(push_back)(ptr, t));
        t.ptr = nullptr;
    }

    // List takes ownership of t
    void push_back(value_type::igraph_type *t) {
        igCheck(FUNCTION(push_back)(ptr, t));
    }

    void push_back_copy(const value_type::igraph_type *t) {
        igCheck(FUNCTION(push_back_copy)(ptr, t));
    }

    reference push_back_new() {
        value_type::igraph_type *t;
        igCheck(FUNCTION(push_back_new)(ptr, &t));
        return value_type(igAlias(*t));
    }

    value_type pop_back() {
        return value_type(igCapture(FUNCTION(pop_back)(ptr)));
    }

    friend void swap(LIST_TYPE &t1, LIST_TYPE &t2) noexcept {
        FUNCTION(swap)(t1.ptr, t2.ptr);
    }
};

class LIST_TYPE_TEMPL::iterator {
public:
    using value_type = LIST_TYPE::value_type;
    using difference_type = LIST_TYPE::difference_type;
    using pointer = value_type *;
    using reference = value_type;
    using iterator_category = std::random_access_iterator_tag;

private:
    value_type::igraph_type *p;

    friend class LIST_TYPE;
    iterator(value_type::igraph_type *p_) : p(p_) { }

public:

    reference operator * () const { return value_type(igAlias(*p)); }
    reference operator [] (difference_type i) const { return value_type(igAlias(p[i])); }

    iterator & operator ++ () { ++p; return *this; }
    iterator operator ++ (int) { ++p; return *this; }
    iterator & operator -- () { --p; return *this; }
    iterator operator -- (int) { --p; return *this; }

    iterator & operator += (difference_type n) { p += n; return *this; }
    iterator & operator -= (difference_type n) { p -= n; return *this; }

    friend bool operator == (const iterator &lhs, const iterator &rhs) {
        return lhs.p == rhs.p;
    }

    friend bool operator != (const iterator &lhs, const iterator &rhs) {
        return lhs.p != rhs.p;
    }

    friend bool operator < (const iterator &lhs, const iterator &rhs) {
        return lhs.p < rhs.p;
    }

    friend bool operator > (const iterator &lhs, const iterator &rhs) {
        return lhs.p > rhs.p;
    }

    friend bool operator <= (const iterator &lhs, const iterator &rhs) {
        return lhs.p <= rhs.p;
    }

    friend bool operator >= (const iterator &lhs, const iterator &rhs) {
        return lhs.p >= rhs.p;
    }

    friend iterator operator + (const iterator &it, difference_type n) {
        return it.p + n;
    }

    friend iterator operator - (const iterator &it, difference_type n) {
        return it.p - n;
    }

    friend difference_type operator - (const iterator &lhs, const iterator &rhs) {
        return lhs.p - rhs.p;
    }
};

LIST_TYPE_TEMPL::iterator LIST_TYPE_TEMPL::begin() { return ptr->stor_begin; }
LIST_TYPE_TEMPL::iterator LIST_TYPE_TEMPL::end() { return ptr->end; }

#include <igraph_pmt_off.h>