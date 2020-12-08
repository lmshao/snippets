//
// Copyright (c) 2020 Liming SHAO <lmshao@163.com>. All rights reserved.
//

#ifndef _AVLTREE_H
#define _AVLTREE_H

#include <iostream>

template <typename T>
struct AVLNode {
    T data;
    int height;
    AVLNode<T> *left;
    AVLNode<T> *right;
    explicit AVLNode<T>(const T data) : data(data), height(0), left(nullptr), right(nullptr) {}
};

template <typename T>
class AVLTree
{
  public:
    AVLTree<T>() : root(nullptr){};
    ~AVLTree<T>() = default;

    AVLNode<T> *root;
    //插入节点
    void Insert(AVLNode<T> *&t, T x);
    //删除节点
    bool Delete(AVLNode<T> *&t, T x);
    //中序遍历: 左根右
    void InorderTraversal(AVLNode<T> *t);
    //前序遍历：根左右
    void PreorderTraversal(AVLNode<T> *t);
    AVLNode<T> *FindMin(AVLNode<T> *t) const;
    //最大根节点
    AVLNode<T> *FindMax(AVLNode<T> *t) const;
    //查找是否包含节点
    bool Contains(AVLNode<T> *t, T x) const;

  private:
    int GetHeight(AVLNode<T> *t);
    //右旋
    void RotateR(AVLNode<T> *&t);
    //右旋
    void RotateL(AVLNode<T> *&t);
    //双旋转 右左
    void RotateLR(AVLNode<T> *&t);
    //双旋转 左右
    void RotateRL(AVLNode<T> *&t);
};

template <typename T>
void AVLTree<T>::Insert(AVLNode<T> *&t, T x)
{
    if (t == nullptr)
        t = new AVLNode<T>(x);
    else if (x < t->data) {  // 新值插在左边
        Insert(t->left, x);
        // 判断平衡性
        if (GetHeight(t->left) - GetHeight(t->right) > 1) {
            if (x < t->left->data)  // 新值插入在左左下面
                RotateR(t);
            else  // 新值插入在左右下面
                RotateLR(t);
        }
    } else if (x > t->data) {  // 新值插在右边
        Insert(t->right, x);
        // 判断平衡性
        if (GetHeight(t->right) - GetHeight(t->left) > 1) {
            if (x > t->right->data)
                RotateL(t);  // 新值插在右右下面
            else
                RotateRL(t);  // 新值插在右左下面
        }
    } else {
        //        printf("重复数据");
    }
    t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
}

template <typename T>
bool AVLTree<T>::Delete(AVLNode<T> *&t, T x)
{
    if (t == nullptr) {
        return false;
    }
    else if (t->data == x) {
        //左右子树都非空
        if (t->left && t->right) {
            //在高度最大的那个子树上执行删除操作
            //左子树高度大，删除左子树里值最大的节点，将其赋值给根节点
            if (GetHeight(t->left) > GetHeight(t->right)) {
                t->data = FindMax(t->left)->data;
                Delete(t->left, t->data);
            }
            //右子树高度更大，删除右子树里值最小的节点，将其赋值给根节点
            else {
                t->data = FindMin(t->right)->data;
                Delete(t->right, t->data);
            }
        }
        // 左右子树有一个不为空，直接用需要删除的节点子节点替换即可
        else {
            AVLNode<T> *old = t;
            t = t->left ? t->left : t->right;
            delete old;
        }
    }
    //要删除的点在左子树上
    else if (x < t->data) {
        Delete(t->left, x);
        // 删除点在左边，右边比较深的话需要旋转
        if (GetHeight(t->right) - GetHeight(t->left) > 1) {
            if (GetHeight(t->right->left) > GetHeight(t->right->right)) {
                RotateRL(t);    // 右左比较深，先右旋再左旋
            } else {
                RotateL(t);     // 右右比较深，左旋
            }
        } else {
            t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
        }
    }
    // 要删除的点在右子树上
    else {
        //递归删除右子树节点
        Delete(t->right, x);
        // 删除点在右边，左边比较深的话需要旋转
        if (GetHeight(t->left) - GetHeight(t->right) > 1) {
            if (GetHeight(t->left->right) > GetHeight(t->left->left)) {
                RotateLR(t);
            } else {
                RotateR(t);
            }
        } else {
            t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
        }
    }
    return true;
}

//中序遍历 左中右
template <typename T>
void AVLTree<T>::InorderTraversal(AVLNode<T> *t)
{
    if (t) {
        InorderTraversal(t->left);
        std::cout << t->data << " ";
        InorderTraversal(t->right);
    }
}

//前序遍历
template <typename T>
void AVLTree<T>::PreorderTraversal(AVLNode<T> *t)
{
    if (t) {
        std::cout << t->data << " ";
        PreorderTraversal(t->left);
        PreorderTraversal(t->right);
    }
}

template <typename T>
AVLNode<T> *AVLTree<T>::FindMin(AVLNode<T> *t) const
{
    if (t == nullptr)
        return nullptr;

    while (t->left != nullptr)
        t = t->left;

    return t;
}

template <typename T>
AVLNode<T> *AVLTree<T>::FindMax(AVLNode<T> *t) const
{
    if (t == nullptr)
        return nullptr;

    while (t->right != nullptr)
        t = t->right;

    return t;
}

template <typename T>
int AVLTree<T>::GetHeight(AVLNode<T> *t)
{
    return t ? t->height : -1;
}

//左左插入导致的不平衡
//左儿子升级为根，根降为根的右儿子
template <typename T>
void AVLTree<T>::RotateR(AVLNode<T> *&t)
{
    AVLNode<T> *q = t->left;
    t->left = q->right;
    q->right = t;
    t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
    q->height = std::max(GetHeight(q->left), GetHeight(q->right)) + 1;
    t = q;
}

//右右插入导致的不平衡
//左旋
//右儿子升级为根，根降为左儿子
template <typename T>
void AVLTree<T>::RotateL(AVLNode<T> *&t)
{
    AVLNode<T> *q = t->right;
    t->right = q->left;
    q->left = t;
    t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
    q->height = std::max(GetHeight(q->left), GetHeight(q->right)) + 1;
    t = q;
}

//双旋转
//插入点位于t的左儿子的右子树
template <typename T>
void AVLTree<T>::RotateLR(AVLNode<T> *&t)
{
    //先对左儿子左旋
    RotateL(t->left);
    //再对根节点右旋
    RotateR(t);
}

template <typename T>
void AVLTree<T>::RotateRL(AVLNode<T> *&t)
{
    //先对右儿子右旋
    RotateR(t->right);
    //再对根节点左旋
    RotateL(t);
}

template <typename T>
bool AVLTree<T>::Contains(AVLNode<T> *t, T x) const
{
    if (t == nullptr)
        return false;
    if (x < t->data)
        return Contains(t->left, x);
    else if (x > t->data)
        return Contains(t->right, x);
    else
        return true;
}

#endif  //_AVLTREE_H
