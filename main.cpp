
// mempool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <cassert>
#include <ctime>

using namespace std;


template<int size_obj, int Num_obj = 10>//前面是对象的字节数，后面是每一个块中节点(对象)数量
class MemoryPool {
public:
    MemoryPool () noexcept;

    ~MemoryPool () noexcept;

    void *alloc ();

    void expand_freelist ();

    void free (void *p);

private:
    struct FreeNode//空闲节点 链表
    {
        FreeNode *next;
        char val[size_obj];//节点数据域大小和对象size一样
    };
    struct MemBlock//块结构体 链表
    {
        FreeNode val[Num_obj];
        MemBlock *next;
    };
    FreeNode *freenodehead;//当前第一个能用的空闲节点
    MemBlock *memblockhead;//当前第一个能用的内存块
};

template<int size_obj, int Num_obj>
MemoryPool<size_obj, Num_obj>::MemoryPool () noexcept {
    freenodehead = nullptr;
    memblockhead = nullptr;
}

template<int size_obj, int Num_obj>
MemoryPool<size_obj, Num_obj>::~MemoryPool () noexcept {
    MemBlock *ptr;
    while (memblockhead) {
        ptr = memblockhead->next;
        delete memblockhead;
        memblockhead = ptr;
    }
}

template<int size_obj, int Num_obj>
void *MemoryPool<size_obj, Num_obj>::alloc () {
    if (freenodehead == nullptr) //如果没有空闲队列了
    {
        expand_freelist ();
    }
    void *ptr = freenodehead;
    freenodehead = freenodehead->next;
    return ptr;
}

template<int size_obj, int Num_obj>
void MemoryPool<size_obj, Num_obj>::expand_freelist () //没有空闲链表了 新创建块和链表 新创建的块插到头部
{
    //块之间也是链表
    //不用担心快之间链表没有联系 执行释放的话 会把他们连在一起的
    MemBlock *newblock = new MemBlock ();
    newblock->next = nullptr;
    freenodehead = &newblock->val[0];//新建的块的元素赋值给空闲节点
    //把新建的节点连起来（释放之后 连的顺序就不一定了）
    for (int i = 0; i < Num_obj - 1; i++) {
        newblock->val[i].next = &newblock->val[i + 1];
    }
    newblock->val[Num_obj - 1].next = nullptr;
    if (memblockhead == nullptr) //没有第一个块 他就是第一个
    {
        memblockhead = newblock;
    } else//已经有了 把他插入到第一个
    {
        newblock->next = memblockhead;
        memblockhead = newblock;
    }

};

template<int size_obj, int Num_obj>
void MemoryPool<size_obj, Num_obj>::free (void *p) {
    FreeNode *ptr = (FreeNode *) p;
    ptr->next = freenodehead;
    freenodehead = ptr;
};

class Test//测试类
{
public:
    Test () {};

    explicit Test (int x) : val (x) {};

    ~Test () {};

private:
    int val;
};

int main () {

    clock_t start;
    MemoryPool<sizeof (int)> mempool;

    start = clock ();
    for (int i = 0; i < 10000000; i++) {
        //Test *t1 = (Test*)mempool.alloc();
        int *t1 = (int *) mempool.alloc ();
        //p1->print();
        mempool.free (t1);
    }

    std::cout << "new Allocator Time: ";
    std::cout << (((double) clock () - start) / CLOCKS_PER_SEC) << "\n\n";

    start = clock ();
    for (int i = 0; i < 10000000; i++) {
        //Test *t2 = new Test();
        int *t2 = new int ();
        delete t2;
    }

    std::cout << "old Allocator Time: ";
    std::cout << (((double) clock () - start) / CLOCKS_PER_SEC) << "\n\n";

    return 0;


}



