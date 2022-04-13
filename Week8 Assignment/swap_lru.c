#include <defs.h>
#include <riscv.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_lru.h>
#include <list.h>
#include <pmm.h>


list_entry_t pra_list_head, *curr_ptr;

static int
_lru_init_mm(struct mm_struct *mm)
{     
    //TODO
    list_init(&pra_list_head);
    mm->sm_priv = &pra_list_head;

    //let current pointer point to the head
    list_entry_t *head=(list_entry_t*) mm->sm_priv;
    curr_ptr = head;

    return 0;
}

static int
_lru_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    //TODO
    list_entry_t *head=(list_entry_t*) mm->sm_priv;
    list_entry_t *entry=&(page->pra_page_link);

    assert(entry != NULL && head != NULL);

    //link the most recent arrival page at the back of the pra_list_head queue.
    list_add(head, entry);

    return 0;
}


static int
_lru_swap_out_victim(struct mm_struct *mm, struct Page ** ptr_page, int in_tick)
{
    //TODO
    list_entry_t *head=(list_entry_t*) mm->sm_priv;
    assert(head != NULL);
    assert(in_tick==0);

    //let pointer le point to the next of head
    list_entry_t *le = head;
    le = list_next(le);
    //calculate the first page's constant as the minimal one
    struct Page *start_page = le2page(le, pra_page_link);
    int min = *(unsigned char *)start_page->pra_vaddr;
    //traverse the list
    while(le != head) {
        //calculate the current page's constant as temp
        struct Page *curr_page = le2page(le, pra_page_link);
        int temp = *(unsigned char *)curr_page->pra_vaddr;
        //pick the smaller constant as min and make curr_ptr
        //make curr_ptr point to the corresponding page
        if (temp < min) {
            min = temp;
            curr_ptr = le;
        }
        //move the pointer le to the next
        le = list_next(le);
    }
    //unlink the page pointed by curr_ptr
    //which is the one with the smallest constant
    list_entry_t *result = curr_ptr;
    if (result != head) {
        list_del(result);
        *ptr_page = le2page(result, pra_page_link);
    } else {
        *ptr_page = NULL;
    }
    return 0;
}


static int
_lru_check_swap(void) {

    int i = 0x10;
    
    cprintf("---------LRU check begin----------\n");
    cprintf("write Virt Page 3 in lru_check_swap\n");
    *(unsigned char *)0x3000 = i;i+=1;
    assert(pgfault_num==4);
    cprintf("write Virt Page 1 in lru_check_swap\n");
    *(unsigned char *)0x1000 =  i;i+=1;
    assert(pgfault_num==4);
    cprintf("write Virt Page 4 in lru_check_swap\n");
    *(unsigned char *)0x4000 =  i;i+=1;
    assert(pgfault_num==4);
    cprintf("write Virt Page 2 in lru_check_swap\n");
    *(unsigned char *)0x2000 =  i;i+=1;
    assert(pgfault_num==4);
    cprintf("write Virt Page 5 in lru_check_swap\n");
    *(unsigned char *)0x5000 =  i;i+=1;
    assert(pgfault_num==5);
    cprintf("write Virt Page 3 in lru_check_swap\n");
    *(unsigned char *)0x3000 =  i;i+=1;
    assert(pgfault_num==6);
    cprintf("write Virt Page 1 in lru_check_swap\n");
    *(unsigned char *)0x1000 =  i;i+=1;
    assert(pgfault_num==7);
    cprintf("write Virt Page 4 in lru_check_swap\n");
    *(unsigned char *)0x4000 =  i;i+=1;
    assert(pgfault_num==8);
    cprintf("write Virt Page 4 in lru_check_swap\n");
    *(unsigned char *)0x4000 =  i;i+=1;
    assert(pgfault_num==8);
    cprintf("write Virt Page 5 in lru_check_swap\n");
    *(unsigned char *)0x5000 =  i;i+=1;
    assert(pgfault_num==8);
    cprintf("write Virt Page 2 in lru_check_swap\n");
    *(unsigned char *)0x2000 =  i;i+=1;
    assert(pgfault_num==9);
    cprintf("write Virt Page 3 in lru_check_swap\n");
    *(unsigned char *)0x3000 =  i;i+=1;
    assert(pgfault_num==10);
    cprintf("LRU check succeed!\n");

    return 0;
}


static int
_lru_init(void)
{
    return 0;
}

static int
_lru_set_unswappable(struct mm_struct *mm, uintptr_t addr)
{
    return 0;
}

static int
_lru_tick_event(struct mm_struct *mm)
{ return 0; }


struct swap_manager swap_manager_lru =
{
     .name            = "lru swap manager",
     .init            = &_lru_init,
     .init_mm         = &_lru_init_mm,
     .tick_event      = &_lru_tick_event,
     .map_swappable   = &_lru_map_swappable,
     .set_unswappable = &_lru_set_unswappable,
     .swap_out_victim = &_lru_swap_out_victim,
     .check_swap      = &_lru_check_swap,
};
