#ifndef B1C2FE29_183E_48A0_9103_13B4313F0E3A
#define B1C2FE29_183E_48A0_9103_13B4313F0E3A

/*
	在数组上实现链表
*/
#include <stdint.h>
/*
 * Singly-linked Tail queue declarations.
 */
#define	STAILQ_HEAD(name, type)					\
struct name {								\
	int32_t first_idx;	/* first element */			\
	int32_t last_idx;	/* addr of last next element */		\
}

#define	STAILQ_HEAD_INITIALIZER(head)					\
	{ -1, (head).first_idx }

#define	STAILQ_ENTRY(type)						\
struct {								\
	int32_t next_idx;	/* next element */			\
	int32_t self_idx;	\
}

/*
 * Singly-linked Tail queue functions.
 */
#define	STAILQ_INIT(head) do {						\
	(head)->first_idx = -1;					\
	(head)->last_idx = (head)->first_idx;				\
} while (/*CONSTCOND*/0)

#define	STAILQ_INSERT_HEAD(head, elm, field) do {			\
	if (((elm)->field.next_idx = (head)->first_idx) == -1)	\
		(head)->last_idx = (elm)->field.next_idx;		\
	(head)->first_idx = (elm)->field.self_idx;					\
} while (/*CONSTCOND*/0)

#define	STAILQ_INSERT_TAIL(head, array, elm, field) do {			\
	(elm)->field.next_idx = -1;					\
	if ((head)->first_idx == -1) { \
		(head)->first_idx = (head)->last_idx = (elm)->field.self_idx; \
	} else { \
		(array)[(head)->last_idx].field.next_idx = (elm)->field.self_idx; \
		(head)->last_idx = (elm)->field.self_idx; \
	} \
} while (/*CONSTCOND*/0)

#define	STAILQ_FOREACH(_idx, head, array, field)				\
	for (int32_t _idx = ((head)->first_idx);				\
		_idx != -1;							\
		_idx = ((array)[_idx].field.next_idx))

#define	STAILQ_REMOVE_HEAD(head, array, field) do {				\
	if (((head)->first_idx = (array)[(head)->first_idx].field.next_idx) == -1) \
		(head)->last_idx = (head)->first_idx;			\
} while (/*CONSTCOND*/0)

#define	STAILQ_REMOVE(head, array, elm, field) do {			\
	if ((head)->first_idx == (elm)->field.self_idx) {				\
		STAILQ_REMOVE_HEAD((head), (array), field);			\
	} else {							\
		STAILQ_FOREACH(_idx, (head), (array), field) { \
			if ((array)[_idx].field.next_idx == (elm)->field.self_idx) { \
				(array)[_idx].field.next_idx = (elm)->field.next_idx; \
				if ((head)->last_idx == (elm)->field.self_idx) { \
					(head)->last_idx = _idx; \
				} \
				break; \
			} \
		} \
	}								\
} while (/*CONSTCOND*/0)

#define	STAILQ_EMPTY(head)	((head)->first_idx == -1)
#define	STAILQ_FIRST(head, array)	((array)[(head)->first_idx])

#endif /* B1C2FE29_183E_48A0_9103_13B4313F0E3A */
