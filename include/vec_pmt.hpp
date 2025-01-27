
#include <igraph_pmt.h>

// This wrapper class can operate in two modes, and can switch between them dynamically
// as needed.
//  - It can own an igraph_vector_t, meaning that it is responsible for destroying it.
//    In this case ptr points to the internal vec object, which is initialized.
//  - It can alias an igraph_vector_t, essentially act as a reference to it. In this
//    case ptr is pointing to the external vector, and the destructor does not do anything.
// To create an igVec that aliases v, use igVec(igAlias(v)). To take over the ownership of
// v's data, use igVec(igCapture(v)). In the latter case, v must no longer be used directly.
template<> class igVec<BASE> {
    using igraph_type = TYPE(igraph_vector);

    igraph_type vec;
    igraph_type *ptr = &vec;

    bool is_alias() const { return ptr != &vec; }

    friend class igVecList<BASE>;

public:
    using value_type = BASE;
    using reference = BASE &;
    using const_reference = const BASE &;
    using iterator = BASE *;
    using const_iterator = const BASE *;
    using difference_type = igraph_integer_t;
    using size_type = igraph_integer_t;

    explicit igVec(igCaptureType<igraph_type> v) : vec(v.obj) { }
    explicit igVec(igAliasType<igraph_type> v) : ptr(&v.obj) { }

    explicit igVec(size_type n = 0) {
        igCheck(FUNCTION(igraph_vector, init)(ptr, n));
    }

    igVec(igVec &&other) noexcept {
        if (other.is_alias()) {
            ptr = other.ptr;
        } else {
            vec = other.vec;
        }
        other.ptr = nullptr;
    }

    igVec(const igVec &other) {
        igCheck(FUNCTION(igraph_vector, init_copy)(ptr, other.ptr));
    }

    igVec(const igraph_type *v) {
        igCheck(FUNCTION(igraph_vector, init_copy)(ptr, v));
    }

    igVec(std::initializer_list<BASE> list) {
        igCheck(FUNCTION(igraph_vector, init_array)(ptr, list.begin(), list.size()));
    }

    igVec & operator = (const igVec &other) {
        igCheck(FUNCTION(igraph_vector, update)(ptr, other.ptr));
        return *this;
    }

    igVec & operator = (igVec &&other) noexcept {
        if (! is_alias())
            FUNCTION(igraph_vector, destroy)(ptr);
        if (other.is_alias()) {
            ptr = other.ptr;
        } else {
            vec = other.vec;
            ptr = &vec;
        }
        other.ptr = nullptr;
        return *this;
    }

    ~igVec() {
        if (! is_alias())
            FUNCTION(igraph_vector, destroy)(ptr);
    }

    operator igraph_type *() { return ptr; }
    operator const igraph_type *() const { return ptr; }

    iterator begin() { return ptr->stor_begin; }
    iterator end() { return ptr->end; }

    const_iterator begin() const { return ptr->stor_begin; }
    const_iterator end() const { return ptr->end; }

    const_iterator cbegin() const { return ptr->stor_begin; }
    const_iterator cend() const { return ptr->end; }

    value_type *data() { return begin(); }

    reference &back() { return *(ptr->end-1); }
    const_reference &back() const { return *(ptr->end-1); }

    size_type size() const { return ptr->end - ptr->stor_begin; }
    constexpr size_type max_size() const { return IGRAPH_INTEGER_MAX; }
    size_type capacity() const { return ptr->stor_end - ptr->stor_begin; }

    bool empty() const { return ptr->end == ptr->stor_begin; }

    reference operator [] (size_type i) { return begin()[i]; }
    const_reference operator [] (size_type i) const { return begin()[i]; }

    void clear() { FUNCTION(igraph_vector, clear)(ptr); }
    void resize(size_type size) { igCheck(FUNCTION(igraph_vector, resize)(ptr, size)); }
    void reserve(size_type capacity) { igCheck(FUNCTION(igraph_vector, reserve)(ptr, capacity)); }
    void shrink_to_fit() { FUNCTION(igraph_vector, resize_min)(ptr); }

    void push_back(value_type elem) { igCheck(FUNCTION(igraph_vector, push_back)(ptr, elem)); }
    value_type pop_back() { return FUNCTION(igraph_vector, pop_back)(ptr); }

    iterator erase(const_iterator pos) {
        FUNCTION(igraph_vector, remove)(ptr, pos - ptr->stor_begin);
        return const_cast<iterator>(pos);
    }

    iterator erase(const_iterator first, const_iterator last) {
        FUNCTION(igraph_vector, remove_section)(ptr, first - ptr->stor_begin, last - ptr->stor_begin);
        return const_cast<iterator>(first);
    }

    friend void swap(igVec &v1, igVec &v2) noexcept {
        FUNCTION(igraph_vector, swap)(v1.ptr, v2.ptr);
    }

    // Necessary to allow some STL algorithms to work on igVecList,
    // whose iterator dereferences to an aliasing igVec.
    friend void swap(igVec &&v1, igVec &&v2) noexcept {
        FUNCTION(igraph_vector, swap)(v1.ptr, v2.ptr);
    }

    friend bool operator == (const igVec &lhs, const igVec &rhs) {
        if (lhs.ptr == rhs.ptr)
            return true;
        size_type n = lhs.size();
        if (rhs.size() != n)
            return false;
        for (size_type i = 0; i < n; ++i)
            if (lhs[i] != rhs[i])
                return false;
        return true;
    }

    friend bool operator != (const igVec &lhs, const igVec &rhs) {
        return ! (lhs == rhs);
    }
};

#include <igraph_pmt_off.h>
