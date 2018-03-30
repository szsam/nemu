#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "monitor/monitor.h"
#include "cpu/reg.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
// add extra dummy node to simplify operations of linked list
static WP dummy_using = {-1, "", 0, NULL};
static WP dummy_free = {-1, "", 0, wp_pool};

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = &dummy_using;
	free_ = &dummy_free;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp() {
	if (free_->next == NULL)
		assert(0);

	WP *new_node = free_->next;
	// remove new_node from free_
	free_->next = new_node->next;
	// add new_node to head (insert at the front)
    new_node->next = head->next;
	head->next = new_node;

	return new_node;
}

void free_wp(int no) {
	WP *prev = head,
	   *cur = head->next;

	while (cur != NULL && cur->NO != no) {
		prev = cur;
		cur = cur->next;
	}
	
	if (cur == NULL) {
		fprintf(stderr, "No watchpoint number %d\n", no);	
		return;
	}

	// remove cur from head
	prev->next = cur->next;
	// add it to free_
	cur->next = free_->next;
	free_->next = cur;

}

void print_watchpoints() {
	WP *p = head->next;
	if (p == NULL) {
		printf("No watchpoints.\n");
		return;
	}

	printf("Num\tExpression\n");

	while (p != NULL) {
		printf("%d\t%s\n", p->NO, p->expr);
		p = p->next;
	}
	
}

void check_watchpoints() {
	WP *p = head->next;

	while (p != NULL) {
		bool success;
		uint32_t cur_val = expr(p->expr, &success);
		// don't need to check success, it must be true
		if (cur_val != p->value) {	// NOT equal to last value
			nemu_state = NEMU_STOP;
			printf("Watchpoint %d: %s\n", p->NO, p->expr);
			printf("Old value = %u\n", p->value);
			printf("New value = %u\n", cur_val);
			printf("at eip = 0x%x\n", cpu.eip);
			p->value = cur_val;
			return;
		}

		p = p->next;

	}
}
