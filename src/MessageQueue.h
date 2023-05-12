#pragma once
#include "includes.h"
#define MESSAGE_QUEUE_SIZE 128

template <typename T>
struct MessageQueue {
	T buffer[MESSAGE_QUEUE_SIZE];
	u8 head;
	u8 tail;

	bool hasMessage() {
		return head != tail;
	}

	void push(T element) {
		buffer[tail] = element;
		tail = (tail + 1) % MESSAGE_QUEUE_SIZE;
		assert(tail != head); //NOTE(oliver): If we run over we need to enlarge
	}

	T pop() {
		assert(head != tail);
		u8 oldHead = head;
		head = (head + 1) % MESSAGE_QUEUE_SIZE;
		return buffer[oldHead];
	}
};
