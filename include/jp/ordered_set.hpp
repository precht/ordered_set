/**
 *  MIT License
 *
 *  Copyright (c) 2020 Jakub Precht <github.com/precht>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma once

#include <queue>
#include <ostream>
#include <cassert>
#include <sstream>
#include <string_view>

namespace jp {

/////////////////////////////////////////////////////////////////////////////// DECLARATION ////////////////////////////

/**
 * A drop-in replacement for the the equivalent specialization of __gnu_pbds::tree template.
 *
 * PBDS docs:
 * https://gcc.gnu.org/onlinedocs/libstdc++/ext/pb_ds/tree_based_containers.html
 *
 * Red-black tree implementation according to 'Introduction to Algorithms, Third Edition' by T. H. Cormen.
 */
template<
        typename Key,
        typename Cmp_Fn = std::less<Key>
        >
class ordered_set
{
    struct node
    {
        size_t size;
        node* left;
        node* right;
        node* parent;
        Key key;
        bool color;

        node() = delete;
        node(const Key& key, size_t size, node* left, node* right, node* parent, bool color);
        std::string str() const;
    };

public:
    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, node>
    {
        friend class ordered_set<Key, Cmp_Fn>;
        const_iterator(const ordered_set* tree, node* nd);
    public:
        const_iterator() = delete;
        const_iterator(const const_iterator& other);
        const_iterator(const_iterator&&) = delete;
        const_iterator operator=(const const_iterator& other);
        const_iterator operator=(const_iterator&&) = delete;
        ~const_iterator() = default;
        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator--();
        const_iterator operator--(int);
        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;
        const Key* operator->();
        const Key& operator*();
    private:
        const ordered_set* m_tree;
        node* m_node;
    };

    ordered_set();
    ordered_set(const ordered_set& other);
    ordered_set(ordered_set&& other);
    ordered_set operator=(const ordered_set& other);
    ordered_set operator=(ordered_set&& other);
    ~ordered_set();
    std::pair<const_iterator, bool> insert(const Key& key);
    const_iterator erase(const Key& key);
    size_t order_of_key(const Key& key) const;
    const_iterator find(const Key& key) const;
    const_iterator find_by_order(size_t order) const;
    const_iterator min() const;
    const_iterator max() const;
    const_iterator begin() const;
    const_iterator end() const;
    size_t size() const;
    bool empty() const;
    void clear();

    template<typename T, typename C>
    friend std::ostream& operator<<(std::ostream& out, const ordered_set<T, C>& tree);

private:
    void delete_all_memory();
    void deep_copy(const ordered_set& src, ordered_set& dst);
    bool equal(const Key& lhs, const Key& rhs) const;
    bool not_equal(const Key& lhs, const Key& rhs) const;
    node* successor(node* x) const;
    node* predecessor(node* x) const;
    node* min(node* x) const;
    node* max(node* x) const;
    node* search(const Key& key) const;
    void erase_tree(node* root);
    void rotate_left(node* x);
    void rotate_right(node* x);
    void transplant(node* u, node* v);
    void updateSize(node* start, node* end, size_t value);
    const_iterator erase(node* z);
    void fixup_erase(node* x);
    void fixup_insert(node* z);
    void print(std::ostream& out, node* x, std::string& prefix) const;

    static constexpr bool RED = 0;
    static constexpr bool BLACK = 1;
    static constexpr Cmp_Fn CMP = Cmp_Fn();
    node* m_nil;
    node* m_root;
};


/////////////////////////////////////////////////////////////////////////////// DEFINITION /////////////////////////////

template<typename Key, typename CmpFn> inline
ordered_set<Key, CmpFn>::node::node(const Key& key, size_t size, node* left, node* right, node* parent, bool color)
    : size{size}
    , left{left}
    , right{right}
    , parent{parent}
    , key{key}
    , color{color}
{ }

template<typename Key, typename CmpFn> inline
ordered_set<Key, CmpFn>::const_iterator::const_iterator(const ordered_set* tree, node* nd)
    : m_tree{tree}
    , m_node{nd}
{ }

template<typename Key, typename CmpFn> inline
ordered_set<Key, CmpFn>::const_iterator::const_iterator(const ordered_set::const_iterator& other)
    : m_tree{other.m_tree}
    , m_node{other.m_node}
{ }

template<typename Key, typename CmpFn>
typename ordered_set<Key, CmpFn>::const_iterator
ordered_set<Key, CmpFn>::const_iterator::operator=(const ordered_set::const_iterator& other)
{
    m_tree = other.m_tree;
    m_node = other.m_node;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator& ordered_set<Key, CmpFn>::const_iterator::operator++()
{
    m_node = m_tree->successor(m_node);
    return *this;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::const_iterator::operator++(int)
{
    ConstIterator tmp{*this};
    operator++();
    return tmp;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator& ordered_set<Key, CmpFn>::const_iterator::operator--()
{
    m_node = m_tree->predecessor(m_node);
    return *this;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::const_iterator::operator--(int)
{
    ConstIterator tmp{*this};
    operator--();
    return tmp;
}

template<typename Key, typename CmpFn> inline
bool ordered_set<Key, CmpFn>::const_iterator::operator==(const ordered_set::const_iterator& other) const
{
    return m_node == other.m_node;
}

template<typename Key, typename CmpFn> inline
bool ordered_set<Key, CmpFn>::const_iterator::operator!=(const ordered_set::const_iterator& other) const
{
    return m_node != other.m_node;
}

template<typename Key, typename CmpFn> inline
const Key* ordered_set<Key, CmpFn>::const_iterator::operator->()
{
    return &(m_node->key);
}

template<typename Key, typename CmpFn> inline
const Key& ordered_set<Key, CmpFn>::const_iterator::operator*()
{
    return m_node->key;
}

template<typename Key, typename CmpFn> inline
ordered_set<Key, CmpFn>::ordered_set()
    : m_nil(new node{Key{}, 0, nullptr, nullptr, nullptr, BLACK})
    , m_root(m_nil)
{
    m_nil->left = m_nil;
    m_nil->right = m_nil;
    m_nil->parent = m_nil;
}

template<typename Key, typename CmpFn> inline
ordered_set<Key, CmpFn>::ordered_set(const ordered_set& other)
    : ordered_set()
{
    deep_copy(other, *this);
}

template<typename Key, typename CmpFn>
ordered_set<Key, CmpFn>::ordered_set(ordered_set&& other)
    : m_nil{other.m_nil}
    , m_root{other.m_root}
{
    other.m_nil = new node{Key{}, 0, nullptr, nullptr, nullptr, BLACK};
    other.m_root = other.m_nil;
}

template<typename Key, typename CmpFn>
ordered_set<Key, CmpFn> ordered_set<Key, CmpFn>::operator=(const ordered_set& other)
{
    if(&other == this)
        return *this;
    deep_copy(other, *this);
    return *this;
}

template<typename Key, typename CmpFn>
ordered_set<Key, CmpFn> ordered_set<Key, CmpFn>::operator=(ordered_set&& other)
{
    if(&other == this)
        return *this;
    delete_all_memory();
    m_nil = other.m_nil;
    m_root = other.m_root;
    other.m_nil = new node{Key{}, 0, nullptr, nullptr, nullptr, BLACK};
    other.m_root = other.m_nil;
    return *this;
}

template<typename Key, typename CmpFn> inline
ordered_set<Key, CmpFn>::~ordered_set()
{
    delete_all_memory();
}

template<typename Key, typename CmpFn>
void ordered_set<Key, CmpFn>::delete_all_memory()
{
    erase_tree(m_root);
    delete m_nil;
    m_nil = nullptr;
    m_root = nullptr;
}

template<typename Key, typename CmpFn>
void ordered_set<Key, CmpFn>::deep_copy(const ordered_set& src, ordered_set& dst)
{
    if (src.m_root == src.m_nil)
        return;
    std::queue<node*> buffor{};
    buffor.push(src.m_root);
    while (!buffor.empty()) {
        node* x = buffor.front();
        buffor.pop();
        dst.insert(x->key);
        if (x->left != src.m_nil)
            buffor.push(x->left);
        if (x->right != src.m_nil)
            buffor.push(x->right);
    }
}

template<typename Key, typename CmpFn> inline
std::pair<typename ordered_set<Key, CmpFn>::const_iterator, bool> ordered_set<Key, CmpFn>::insert(const Key& key)
{
    node* x = m_root;
    node* y = m_nil;
    while (x != m_nil) {
        y = x;
        if (equal(key, x->key))
            return std::make_pair(const_iterator{this, x}, false);
        if (CMP(key, x->key))
            x = x->left;
        else
            x = x->right;
    }
    node* z = new node(key, 1, m_nil, m_nil, y, RED);
    if (y == m_nil)
        m_root = z;
    else if (CMP(z->key, y->key))
        y->left = z;
    else
        y->right = z;
    updateSize(z->parent, m_nil, 1);
    fixup_insert(z);
    return std::make_pair(const_iterator{this, z}, true);
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::erase(const Key& key)
{
    node* z = search(key);
    if (z == m_nil)
        return const_iterator{this, m_nil};
    return erase(z);
}

template<typename Key, typename CmpFn> inline
size_t ordered_set<Key, CmpFn>::order_of_key(const Key& key) const {
    size_t current = m_root->size;
    node* x = m_root;
    while (x != m_nil && not_equal(key, x->key)) {
        if (CMP(key, x->key)) {
            current--;
            if (x->right != m_nil)
                current -= x->right->size;
            x = x->left;
        } else {
            x = x->right;
        }
    }
    if (x->right != m_nil)
        current -= x->right->size;
    if (x != m_nil)
        --current;
    return current;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::find(const Key& key) const
{
    return const_iterator{this, search(key)};
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::find_by_order(size_t order) const
{
    size_t current = m_root->left->size;
    node* x = m_root;
    while (x != m_nil && current != order) {
        if (current > order) {
            current -= x->left->size;
            x = x->left;
            current += x->left->size;
        } else {
            x = x->right;
            current += 1 + x->left->size;
        }
    }
    return const_iterator{this, x};
}

template<typename Key, typename CmpFn> inline
size_t ordered_set<Key, CmpFn>::size() const
{
    return m_root->size;
}

template<typename Key, typename CmpFn> inline
bool ordered_set<Key, CmpFn>::empty() const
{
    return m_root->size == 0;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::min() const
{
    return const_iterator{this, min(m_root)};
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::max() const
{
    return const_iterator{this, max(m_root)};
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::begin() const
{
    return find_by_order(0);
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::end() const
{
    return const_iterator{this, m_nil};
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::clear()
{
    erase_tree(m_root);
    m_root = m_nil;
}

template<typename Key, typename CmpFn> inline
bool ordered_set<Key, CmpFn>::equal(const Key& lhs, const Key& rhs) const
{
    return (!CMP(lhs, rhs)) & (!CMP(rhs, lhs));
}

template<typename Key, typename CmpFn> inline
bool ordered_set<Key, CmpFn>::not_equal(const Key& lhs, const Key& rhs) const
{
    return CMP(lhs, rhs) | CMP(rhs, lhs);
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::node* ordered_set<Key, CmpFn>::successor(node* x) const
{
    if (x->right != m_nil)
        return min(x->right);
    while (x != m_nil) {
        if (x == x->parent->left)
            return x->parent;
        x = x->parent;
    }
    return m_nil;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::node* ordered_set<Key, CmpFn>::predecessor(node* x) const
{
    if (x->left != m_nil)
        return max(x->left);
    while (x != m_nil) {
        if (x == x->parent->right)
            return x->parent;
        x = x->parent;
    }
    return m_nil;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::node* ordered_set<Key, CmpFn>::min(node* x) const
{
    if (x != m_nil)
        while (x->left != m_nil)
            x = x->left;
    return x;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::node* ordered_set<Key, CmpFn>::max(node* x) const
{
    if (x != m_nil)
        while (x->right != m_nil)
            x = x->right;
    return x;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::node* ordered_set<Key, CmpFn>::search(const Key& key) const
{
    node* x = m_root;
    while (x != m_nil && not_equal(key, x->key)) {
        if (CMP(key, x->key))
            x = x->left;
        else
            x = x->right;
    }
    return x;
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::erase_tree(node* root)
{
    if (root == m_nil)
        return;
    std::queue<node*> buffor;
    buffor.push(root);
    while (!buffor.empty()) {
        node* x = buffor.front();
        buffor.pop();
        if (x->left != m_nil)
            buffor.push(x->left);
        if (x->right != m_nil)
            buffor.push(x->right);
        delete x;
    }
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::rotate_left(node* x)
{
    node* y = x->right;
    x->right = y->left;
    if (y->left != m_nil)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == m_nil)
        m_root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
    if(x->left != m_nil)
        y->size += x->left->size;
    y->size++;
    if (y->right != m_nil)
        x->size -= y->right->size;
    x->size--;
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::rotate_right(node* x)
{
    node* y = x->left;
    x->left = y->right;
    if (y->right != m_nil)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == m_nil)
        m_root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->right = x;
    x->parent = y;
    if(x->right != m_nil)
        y->size += x->right->size;
    y->size++;
    if (y->left!= m_nil)
        x->size -= y->left->size;
    x->size--;
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::transplant(node* u, node* v)
{
    if (u->parent == m_nil)
        m_root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::updateSize(node* start, node* end, size_t value)
{
    while (start != end) {
        start->size += value;
        start = start->parent;
    }
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::fixup_insert(node* z)
{
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            node* y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotate_left(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotate_right(z->parent->parent);
            }
        } else {
            node* y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotate_right(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotate_left(z->parent->parent);
            }
        }
    }
    m_root->color = BLACK;
}

template<typename Key, typename CmpFn> inline
typename ordered_set<Key, CmpFn>::const_iterator ordered_set<Key, CmpFn>::erase(node* z)
{
    updateSize(z->parent, m_nil, -1);
    auto it = const_iterator{this, successor(z)};
    node* y = z;
    node* x = nullptr;
    bool y_original_color = y->color;
    if (z->left == m_nil) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == m_nil) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = min(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            updateSize(y->parent, z, -1);
            y->size -= y->right->size;
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
            y->size += y->right->size;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
        y->size += y->left->size;
    }
    if (y_original_color == BLACK)
        fixup_erase(x);
    m_nil->parent = m_nil;
    assert(m_nil->color == BLACK);
    delete z;
    return it;
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::fixup_erase(node* x)
{
    while (x != m_root && x->color == BLACK) {
        if (x == x->parent->left) {
            node* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotate_left(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rotate_right(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotate_left(x->parent);
                x = m_root;
            }
        } else {
            node* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotate_right(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rotate_left(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rotate_right(x->parent);
                x = m_root;
            }
        }
    }
    x->color = BLACK;
}

template<typename Key, typename CmpFn> inline
std::ostream& operator<<(std::ostream& out, const ordered_set<Key, CmpFn>& tree)
{
    if (tree.m_root == tree.m_nil) {
        out << "(empty_tree)";
        return out;
    }

    std::string prefix = " ";
    tree.print(out, tree.m_root, prefix);
    out << "(key,size,color)";
    return out;
}

template<typename Key, typename CmpFn> inline
void ordered_set<Key, CmpFn>::print(std::ostream& out, node* x, std::string& prefix) const
{
    auto prefixEnd = prefix.back();
    auto prefixSize = prefix.size();
    std::string str = x->str();

    prefix[prefixSize - 1] = (x->parent->right == x) ? ' ' : prefixEnd;
    prefix.resize(prefix.size() + str.size() - 1, ' ');
    prefix.back() = '|';
    if (x->right != m_nil)
        print(out, x->right, prefix);

    std::string_view prefixView = prefix;
    out << prefixView.substr(0, prefix.size() - str.size()) << str << '\n';

    prefix[prefixSize - 1] = (x->parent->right == x) ? prefixEnd : ' ';
    if (x->left != m_nil)
        print(out, x->left, prefix);
    prefix.resize(prefix.size() - str.size() + 1);
}

template<typename Key, typename CmpFn> inline
std::string ordered_set<Key, CmpFn>::node::str() const
{
    std::stringstream ss{};
    ss << '(' << key << ',' << size << ',' << (color ? 'b' : 'r') << ')';
    return ss.str();
}

} //!jp
