#include "random_utils.hpp"
#include "test.hpp"

#include <list>
#include <numeric>
#include <sstream>
#include <utility>

template<typename T> class BinaryTree {
public:
    struct Node {
        T data{};
        Node* left = nullptr;
        Node* right = nullptr;
    };

    BinaryTree() = default;
    BinaryTree(const BinaryTree&) = delete;
    BinaryTree& operator=(const BinaryTree&) = delete;
    BinaryTree(BinaryTree&& that) : m_root(std::exchange(that.m_root, nullptr)) {}
    BinaryTree& operator=(BinaryTree&& that) {
        BinaryTree tmp(std::move(that));
        swap(*this, tmp);
        return *this;
    }
    friend void swap(BinaryTree& x, BinaryTree& y) noexcept { std::swap(x.m_root, y.m_root); }

    BinaryTree(std::initializer_list<T> list) {
        for (T e : list) {
            insert(std::move(e));
        }
    }

    ~BinaryTree() {
        // dfs deletion (note: top-down).
        std::list<Node*> stack;
        stack.push_back(m_root);
        while (!stack.empty()) {
            Node* current = stack.back();
            stack.pop_back();
            if (!current)
                continue;
            stack.push_back(current->right);
            stack.push_back(current->left);
            delete current;
        }
    }

    /// API functions

    template<typename U = T> void insert(U&& value) {
        m_root = BinaryTree::insert(m_root, std::forward<U>(value));
    }

    Node* find(const T& value) const { return BinaryTree::find(m_root, value); }

    size_t size() const { return BinaryTree::size(m_root); }

    size_t max_depth() const { return BinaryTree::max_depth(m_root, 0); }

    // Note: instead of print function, better add operator<< + maybe some way to control which
    // version of print to use (e.g. similar to std::boolalpha?)
    template<typename Stream> void print(Stream& stream, const char* space = " ") const {
        BinaryTree::print(m_root, stream, space);
    }

    template<typename Stream> void print_postorder(Stream& stream, const char* space = " ") const {
        BinaryTree::print_postorder(m_root, stream, space);
    }

    template<typename Stream>
    void print_paths(Stream& stream, const char* space = " ", const char* new_line = "\n") const {
        std::vector<std::vector<Node*>> paths{};
        BinaryTree::collect_paths(m_root, paths);
        for (const std::vector<Node*> path : paths) {
            for (Node* n : path) {
                stream << n->data << space;
            }
            stream << new_line;
        }
    }

    bool paths_contain_sum(const T& sum) const {
        std::vector<std::vector<Node*>> paths{};
        BinaryTree::collect_paths(m_root, paths);
        for (const std::vector<Node*> path : paths) {
            T current_sum = std::accumulate(path.cbegin(), path.cend(), T(0),
                                            [](T acc, Node* n) { return acc + n->data; });
            if (current_sum == sum) {
                return true;
            }
        }
        return false;
    }

    void mirror() { BinaryTree::mirror(m_root); }

    void double_tree() { BinaryTree::double_tree(m_root); }

    friend bool operator==(const BinaryTree& x, const BinaryTree& y) {
        return BinaryTree::are_equal(x.m_root, y.m_root);
    }
    friend bool operator!=(const BinaryTree& x, const BinaryTree& y) { return !(x == y); }

private:
    Node* m_root = nullptr;

    template<typename U = T> static Node* insert(Node* root, U&& value) {
        if (!root) {
            return new Node{std::forward<U>(value)};
        }

        if (value < root->data) {
            root->left = insert(root->left, std::forward<U>(value));
        } else {
            root->right = insert(root->right, std::forward<U>(value));
        }
        return root;
    }

    static Node* find(Node* root, const T& value) {
        if (!root) {
            return root;
        }

        if (value < root->data) {
            return find(root->left, value);
        } else if (value == root->data) {
            return root;
        } else {
            return find(root->right, value);
        }
    }

    static size_t size(Node* root) {
        if (!root) {
            return 0;
        }
        return size(root->left) + 1 + size(root->right);
    }

    static size_t max_depth(Node* root, size_t depth = 0) {
        if (!root) {
            return depth;
        }
        ++depth;
        return std::max(max_depth(root->left, depth), max_depth(root->right, depth));
    }

    template<typename Stream> static void print(Node* root, Stream& stream, const char* space) {
        if (!root) {
            return;
        }
        print(root->left, stream, space);
        stream << root->data << space;
        print(root->right, stream, space);
    }

    template<typename Stream>
    static void print_postorder(Node* root, Stream& stream, const char* space) {
        if (!root) {
            return;
        }
        print_postorder(root->left, stream, space);
        print_postorder(root->right, stream, space);
        stream << root->data << space;
    }

    static void clear(Node* root) {
        if (!root) {
            return;
        }

        clear(root->left);
        clear(root->right);
        delete root;
    }

    static void collect_paths(Node* root, std::vector<std::vector<Node*>>& paths,
                              std::vector<Node*> current_path = std::vector<Node*>{}) {
        if (!root) {
            return;
        }
        current_path.emplace_back(root);
        if (!root->left && !root->right) {
            paths.emplace_back(std::move(current_path));
            return;
        }
        collect_paths(root->left, paths, current_path);
        collect_paths(root->right, paths, current_path);
    }

    static void mirror(Node* root) {
        if (!root)
            return;
        using std::swap;
        swap(root->left, root->right);
        mirror(root->left);
        mirror(root->right);
    }

    static void double_tree(Node* root) {
        if (!root)
            return;
        double_tree(root->left);
        double_tree(root->right);
        Node* copy = new Node{root->data};
        copy->left = std::exchange(root->left, copy);
    }

    static bool are_equal(Node* x, Node* y) {
        if (!x || !y) {
            if (!x && !y) {
                return true;
            }
            return false;
        }
        return x->data == y->data && are_equal(x->left, y->left) && are_equal(x->right, y->right);
    }
};

int main() {
    {
        BinaryTree<int> tree = {0, -1, 1, -3, -2, 5, 4, 6, 7};
        TEST_RET_ON_ERROR("btree_basic", tree.size() == 9, tree.max_depth() == 5);
    }
    {
        std::stringstream ss;
        BinaryTree<int>({4, 2, 5, 1, 3}).print(ss, " ");
        std::string actual = ss.str();
        string_utils::trim(actual);
        TEST_RET_ON_ERROR("btree_print", actual == std::string("1 2 3 4 5"));
    }
    {
        std::stringstream ss;
        BinaryTree<int>({4, 2, 5, 1, 3}).print_postorder(ss, " ");
        std::string actual = ss.str();
        string_utils::trim(actual);
        TEST_RET_ON_ERROR("btree_print_postorder", actual == std::string("1 3 2 5 4"));
    }
    {
        std::stringstream ss;
        BinaryTree<int>({4, 2, 5, 1, 3}).print_paths(ss, " ", "\n");
        std::string actual = ss.str();

        std::stringstream expected_ss;
        expected_ss << "4 2 1 \n";
        expected_ss << "4 2 3 \n";
        expected_ss << "4 5 \n";
        std::string expected = expected_ss.str();

        TEST_RET_ON_ERROR("btree_print_paths", actual == expected);
    }
    {
        TEST_RET_ON_ERROR("btree_paths_contain_sum",
                          BinaryTree<int>({4, 2, 5, 1, 3}).paths_contain_sum(9) &&
                              !BinaryTree<int>({4, 2, 5, 1, 3}).paths_contain_sum(1));
    }
    {
        BinaryTree<int> actual({4, 2, 5, 1, 3});

        std::stringstream ss;
        actual.print(ss, " ");

        actual.mirror();
        actual.print(ss, " ");

        // NB: because we print twice, it's ascending then descending
        TEST_RET_ON_ERROR("btree_mirror", ss.str() == std::string("1 2 3 4 5 5 4 3 2 1 "));
    }
    {
        BinaryTree<int> actual({2, 1, 3});
        actual.double_tree();

        std::stringstream ss;
        actual.print(ss);
        TEST_RET_ON_ERROR("btree_mirror", ss.str() == std::string("1 1 2 2 3 3 "));
    }
    {
        BinaryTree<int> x({2, 1, 3});
        TEST_RET_ON_ERROR("btree_equality", x == x && x != BinaryTree<int>({3, 2, 1}) &&
                                                BinaryTree<int>() == BinaryTree<int>() &&
                                                x != BinaryTree<int>({8, 9, 1, 4}));
    }
    return 0;
}
