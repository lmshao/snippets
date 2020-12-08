//
// Copyright (c) 2020 Liming SHAO <lmshao@163.com>. All rights reserved.
//

#ifndef _RBTREE_H
#define _RBTREE_H

#include <algorithm>
#include <iostream>

enum RBColor {RB_RED = true, RB_BLACK = false};

template <typename T>
struct RBNode {
    T data;
    RBColor color;
    RBNode<T> *parent;
    RBNode<T> *left;
    RBNode<T> *right;
    explicit RBNode<T>(const T data) : data(data), color(RB_RED), parent(nullptr), left(nullptr), right(nullptr) {}
};

template <typename T>
class RBTree {
public:
    RBTree<T>() : root(root) {}
    ~RBTree() = default;

    RBNode<T> *root;

    //插入节点
    void Insert(T val);
    //删除节点
    bool Delete(T val);
    //中序遍历: 左根右
    void InorderTraversal(RBNode<T> *t);
    //前序遍历：根左右
    void PreorderTraversal(RBNode<T> *t);

    RBNode<T> *FindMin(RBNode<T> *t) const;
    //最大根节点
    RBNode<T> *FindMax(RBNode<T> *t) const;
    //查找是否包含节点
    bool Contains(T val) const;

private:
    int GetHeight(RBNode<T> *t);
    void RotateL(RBNode<T> *&t);
    void RotateR(RBNode<T> *&t);
    void insertFixup(RBNode<T> *t);
    void deleteFixup(RBNode<T> *node, RBNode<T> *parent);
    RBNode<T> *search(RBNode<T> *x, T val) const;
};

template<typename T>
void RBTree<T>::Insert(T val) {
    RBNode<T> *node = new RBNode<T>(val);

    RBNode<T> *tp = nullptr; // 插入节点的父节点
    RBNode<T> *t = root;    // 插入节点位置
    // 查找插入节点t，tp作为t的父节点
    while (t != nullptr) {
        tp = t;
        if (val < t->data){
            t = t->left;
        } else {
            t = t->right;
        }
    }

    // 插入节点node
    node->parent = tp;
    if (tp == nullptr){
        root = node;    // 空节点直接将node设置为根
    } else {
        if (val < tp->data) {
            tp->left = node;    // node值比tp的小，设为左子
        } else {
            tp->right = node;   // node值比tp的大，设为右子
        }
    }
    //对红黑树的节点进行颜色修改以及旋转，让树T仍然是一颗红黑树
    insertFixup(node);
}

template<typename T>
bool RBTree<T>::Delete(T val) {
    //无此值
    RBNode<T> *node = search(root, val);
    if (node == nullptr)
        return false;

    RBNode<T> *child, *parent;
    RBColor color;

    // Case 3: 左右孩子都非空情形
    if (node->left && node->right) {
        // 被删除的后继节点，取右子最小值
        RBNode<T> *replace = node->right;
        while (replace->left)
            replace = replace->left;

        // 父节点存在，不是根节点
        if (node->parent) {
            if (node->parent->left == node)
                node->parent->left = replace;
            else
                node->parent->right = replace;
        } else {
            // node是根节点，无父节点
            root = replace;
        }

        // 继任节点的右子
        child = replace->right;
        parent = replace->parent;
        color = replace->color;

        // 被删除节点是他的后继节点的父节点
        if (parent == node) {
            parent = replace;
        } else {
            // child 非空，把子节点接在父节点上
            if (child)
                child->parent = parent;
            parent->left = child;

            // 把原节点的右子接在继任节点上
            replace->right = node->right;
            node->right->parent = replace;
        }
        // 给继任节点弄个爹
        replace->parent = node->parent;
        // 继任节点颜色与原节点 相同
        replace->color = node->color;
        // 把原节点的左子接在继任节点上
        replace->left = node->left;
        node->left->parent = replace;

        if (color == RB_BLACK)
            deleteFixup(child, parent);
        delete node;
        return true;
    }

    parent = node->parent;
    if (node->left) {
        child = node->left;
    } else {
        child = node->right;
    }

    // Case 2: node只有一个孩子情形
    // 根据性质可知此节点必为黑，子节点必定是红色，此时将子节点涂黑接在父节点上即可
    if (child) {
        // 父节点存在，把孩子接在父节点上
        if (parent) {
            if (parent->left == node)
                parent->left = child;
            else
                parent->right = child;
        } else {
            // 父节点不存在，把孩子设为根
            root = child;
        }

        child->parent = parent;
        // 将此节点涂黑
        child->color = RB_BLACK;

        delete node;
        return true;
    }

    // Case 1: node无子节点
    // 父节点指向该节点的值设为空
    if (parent) {
        if (parent->left == node)
            parent->left = nullptr;
        else
            parent->right = nullptr;

        // 若为黑色，需要重新平衡；若为红色无需处理
        if (node->color == RB_BLACK)
            deleteFixup(nullptr, parent);
    } else {
        // 父节点不存在，把根设为空，也无需平衡
        root = nullptr;
    }

    delete node;
    return true;
}

template<typename T>
void RBTree<T>::insertFixup(RBNode<T> *t) {
    RBNode<T> *p = nullptr;     // 父节点
    RBNode<T> *pp = nullptr;    // 祖父节点
    RBNode<T> *pb = nullptr;    // 叔叔节点

    // 插入节点为根节点，直接置黑
    // 插入节点父节点是黑色，不需要处理

    // 插入节点父节点是红色
    while (t->parent && t->parent->color == RB_RED) {    // 父节点存在
        // 根据红黑树性质，父节点为红，则必定存在祖父节点
        p = t->parent;
        pp = p->parent;

        if (p == pp->left){ // 父节点为祖父节点的左子
            pb = pp->right;
            // Case 1: 叔叔节点为红色
            if (pb && pb->color == RB_RED){
                p->color = RB_BLACK;    // 父节点置黑
                pb->color = RB_BLACK;   // 叔叔节点置黑
                pp->color = RB_RED;     // 祖父节点置红
                t = pp; // 将祖父节点置为当前节点
                continue;
            } else {    // 叔叔节点为黑色
                if (t == p->right) {   // 当前节点为右子
                     // 以父节点进行左旋
                     RotateL(p);
                     // 父变子，子变父
                    std::swap(t, p);
                    continue;
                 } else {    // 当前节点为左子
                     p->color = RB_BLACK;   // 祖父节点置黑
                     pp->color = RB_RED;    // 祖父节点置红
                     RotateR(pp);
                 }
             }
        } else { // 父节点为祖父节点的右子
            pb = pp->left;
            if (pb && pb->color == RB_RED){   // 叔叔节点为红色
                p->color = RB_BLACK;    // 父节点置黑
                pb->color = RB_BLACK;   // 叔叔节点置黑
                pp->color = RB_RED;     // 祖父节点置红
                t = pp; // 将祖父节点置为当前节点
                continue;
            } else {    // Case2: 叔叔节点为黑色
                if (t == p->left) {   // 当前节点为右子
                    // 以父节点进行左旋
                    RotateR(p);
                    // 父变子，子变父
                    std::swap(t, p);
                } else {    // 当前节点为左子
                    p->color = RB_BLACK;   // 祖父节点置黑
                    pp->color = RB_RED;    // 祖父节点置红
                    RotateL(pp);
                }
            }
        }
    }

    root->color = RB_BLACK;
}


template<typename T>
void RBTree<T>::RotateL(RBNode<T> *&t) {
    RBNode<T> *q = t->right;
    t->right = q->left;
    if (q->left)
        q->left->parent = t;

    q->parent = t->parent;
    if (t->parent == nullptr)
        root = q;
    else {
        if (t->parent->left == t) {
            t->parent->left = q;
        } else {
            t->parent->right = q;
        }
    }
    q->left = t;
    t->parent = q;
}

template<typename T>
void RBTree<T>::RotateR(RBNode<T> *&t) {
    RBNode<T> *q = t->left;

    t->left = q->right;
    if (q->right)
        q->right->parent = t;

    q->parent = t->parent;
    if (t->parent == nullptr)
        root = q;
    else {
        if (t->parent->left == t){
            t->parent->left = q;
        } else {
            t->parent->right = q;
        }
    }

    q->right = t;
    t->parent = q;
}

template<typename T>
int RBTree<T>::GetHeight(RBNode<T> *t) {
    return t ? t->height : -1;
}

template<typename T>
void RBTree<T>::InorderTraversal(RBNode<T> *t) {
    if (t) {
        InorderTraversal(t->left);
        std::cout << t->data << (t->color ? "(红)" : "(黑)") << " ";
        InorderTraversal(t->right);
    }
}

template<typename T>
void RBTree<T>::PreorderTraversal(RBNode<T> *t) {
    if (t) {
        std::cout << t->data << (t->color ? "(红)" : "(黑)") << " ";
        PreorderTraversal(t->left);
        PreorderTraversal(t->right);
    }
}

template<typename T>
RBNode<T> *RBTree<T>::FindMin(RBNode<T> *t) const {
    if (t == nullptr)
        return nullptr;

    while (t->left != nullptr)
        t = t->left;

    return t;
}

template<typename T>
RBNode<T> *RBTree<T>::FindMax(RBNode<T> *t) const {
    if (t == nullptr)
        return nullptr;

    while (t->right != nullptr)
        t = t->right;

    return t;
}

template<typename T>
bool RBTree<T>::Contains(T val) const {
    RBNode<T> *node = search(root, val);
    return node != nullptr;
}

template<typename T>
RBNode<T> *RBTree<T>::search(RBNode<T> *x, T val) const {
    if (x == nullptr || x->data == val)
        return x;

    if (val < x->data)
        return search(x->left, val);
    else
        return search(x->right, val);
}

template<typename T>
void RBTree<T>::deleteFixup(RBNode<T> *node, RBNode<T> *parent) {
    RBNode<T> *sibling;

    while ((!node || node->color == RB_BLACK) && node != root) {
        // 兄在右
        if (parent->left == node) {
            sibling = parent->right;

            // Case 1: 兄弟是红色
            if (sibling->color == RB_RED) {
                sibling->color = RB_BLACK;
                parent->color = RB_RED;
                RotateL(parent);
                sibling = parent->right;
            }

            // Case 2.1.2: 兄弟是黑色，两个孩子也是黑色
            if ((!sibling->left || sibling->left->color == RB_BLACK) &&
                    (!sibling->right || sibling->right->color == RB_BLACK)){
                sibling->color = RB_RED;
                node = parent;
                parent = node->parent;
            } else {
                // Case 2.2.2.2: 兄在右，兄左子为红
                if (!sibling->right || sibling->right->color == RB_BLACK) {
                    sibling->left->color = RB_BLACK;
                    sibling->color = RB_RED;
                    RotateR(sibling);
                    sibling = parent->right;
                }

                // Case 2.2.1.2: 兄黑，兄在右，右子红
                sibling->color = parent->color;
                parent->color = RB_BLACK;
                sibling->right->color = RB_BLACK;
                RotateL(parent);
                node = root;
                break;
            }
        } else {
            // 兄在左
            sibling = parent->left;
            // Case 3.1 兄是红色
            if (sibling->color == RB_RED) {
                sibling->color = RB_BLACK;
                parent->color = RB_RED;
                RotateR(parent);
                sibling = parent->left; // 转向 兄弟黑色了
            }

            // 兄在左，两个孩子都是黑色
            if ((!sibling->left || sibling->left->color == RB_BLACK) &&
                    (!sibling->right || sibling->right->color == RB_BLACK)) {
                sibling->color = RB_RED;
                node = parent;
                parent = node->parent;
            } else {
                // Case 2.2.1.1: 兄在左，左子红，右子黑
                if (!sibling->left || sibling->left->color == RB_BLACK) {
                    sibling->right->color = RB_BLACK;
                    sibling->color = RB_RED;
                    RotateL(sibling);
                    sibling = parent->left;
                }

                // Case : 兄弟是黑色，右子是红色
                sibling->color = parent->color;
                parent->color = RB_BLACK;
                sibling->left->color = RB_BLACK;
                RotateR(parent);
                node = root;
                break;
            }
        }
    }

    if (node)
        node->color = RB_BLACK;
}

#endif //_RBTREE_H
