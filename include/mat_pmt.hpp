
#include <igraph_pmt.h>

template<> class igMat<BASE> {
    using igraph_type = TYPE(igraph_matrix);

    igraph_type mat;
    igraph_type *ptr = &mat;

    bool is_alias() const { return ptr != &mat; }

    friend class igMatList<BASE>;

public:
    using value_type = BASE;
    using reference = BASE &;
    using const_reference = const BASE &;
    using iterator = BASE *;
    using const_iterator = const BASE *;
    using difference_type = igraph_integer_t;
    using size_type = igraph_integer_t;

    explicit igMat(igCaptureType<igraph_type> m) : mat(m.obj) { }
    explicit igMat(igAliasType<igraph_type> m) : ptr(&m.obj) { }

    explicit igMat(size_type n = 0, size_type m = 0) {
        igCheck(FUNCTION(igraph_matrix, init)(ptr, n, m));
    }

    igMat(igMat &&other) noexcept {
        if (other.is_alias()) {
            ptr = other.ptr;
        } else {
            mat = other.mat;
        }
        other.ptr = nullptr;
    }

    igMat(const igMat &other) {
        igCheck(FUNCTION(igraph_matrix, init_copy)(ptr, other.ptr));
    }

    igMat(const igraph_type *v) {
        igCheck(FUNCTION(igraph_matrix, init_copy)(ptr, v));
    }

    igMat & operator = (const igMat &other) {
        igCheck(FUNCTION(igraph_matrix, update)(ptr, other.ptr));
        return *this;
    }

    igMat & operator = (igMat &&other) noexcept {
        if (! is_alias())
            FUNCTION(igraph_matrix, destroy)(ptr);
        if (other.is_alias()) {
            ptr = other.ptr;
        } else {
            mat = other.mat;
            ptr = &mat;
        }
        other.ptr = nullptr;
        return *this;
    }

    igMat(std::initializer_list<std::initializer_list<BASE>> values) {
        size_type n = values.size();
        size_type m = n > 0 ? values.begin()->size() : 0;
        igCheck(FUNCTION(igraph_matrix, init)(ptr, n, m));

        size_type i = 0;
        for (const auto &row : values) {
            assert(row.size() == (size_t) m);
            size_type j = 0;
            for (const auto &el : row) {
                MATRIX(*ptr, i, j) = el;
                j++;
            }
            i++;
        }
    }

    ~igMat() {
        if (!is_alias()) {
            FUNCTION(igraph_matrix, destroy)(ptr);
        }
    }

    operator igraph_type *() { return ptr; }
    operator const igraph_type *() const { return ptr; }

    iterator begin() { return ptr->data.stor_begin; }
    iterator end() { return ptr->data.end; }

    const_iterator begin() const { return ptr->data.stor_begin; }
    const_iterator end() const { return ptr->data.end; }

    const_iterator cbegin() const { return ptr->data.stor_begin; }
    const_iterator cend() const { return ptr->data.end; }

    value_type *data() { return begin(); }

    size_type size() const { return ptr->data.end - ptr->data.stor_begin; }
    constexpr size_type max_size() const { return IGRAPH_INTEGER_MAX; }
    size_type capacity() const { return ptr->data.stor_end - ptr->data.stor_begin; }

    bool empty() const { return ptr->data.end == ptr->data.stor_begin; }

    size_type nrow() const { return ptr->nrow; }
    size_type ncol() const { return ptr->ncol; }

    reference operator [] (size_type i) { return begin()[i]; }
    const_reference operator [] (size_type i) const { return begin()[i]; }

    reference operator () (size_type i, size_type j) { return MATRIX(*ptr, i, j); }
    const_reference operator () (size_type i, size_type j) const { return MATRIX(*ptr, i, j); }

    void resize(size_type n, size_type m) { igCheck(FUNCTION(igraph_matrix, resize)(ptr, n, m)); }
    void shrink_to_fit() { FUNCTION(igraph_matrix, resize_min)(ptr); }


    friend void swap(igMat &m1, igMat &m2) noexcept {
        FUNCTION(igraph_matrix, swap)(m1.ptr, m2.ptr);
    }

    // Necessary to allow some STL algorithms to work on igMatList,
    // whose iterator dereferences to an aliasing igMat.
    friend void swap(igMat &&m1, igMat &&m2) noexcept {
        FUNCTION(igraph_matrix, swap)(m1.ptr, m2.ptr);
    }

    friend bool operator == (const igMat &lhs, const igMat &rhs) {
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

    friend bool operator != (const igMat &lhs, const igMat &rhs) {
        return ! (lhs == rhs);
    }
};

#include <igraph_pmt_off.h>
