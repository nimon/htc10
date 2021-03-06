/* Copyright (c) 2002,2007-2015, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __ADRENO_RINGBUFFER_H
#define __ADRENO_RINGBUFFER_H

#include "kgsl_iommu.h"
#include "adreno_dispatch.h"

#define KGSL_RB_SIZE (32 * 1024)

#define KGSL_RB_DWORDS (KGSL_RB_SIZE >> 2)

struct kgsl_device;
struct kgsl_device_private;

struct adreno_submit_time {
	uint64_t ticks;
	u64 ktime;
	struct timespec utime;
};

struct adreno_ringbuffer_pagetable_info {
	int current_global_ptname;
	int current_rb_ptname;
	int incoming_ptname;
	int switch_pt_enable;
	uint64_t ttbr0;
	unsigned int contextidr;
};

/**
 * struct adreno_ringbuffer - Definition for an adreno ringbuffer object
 * @device: KGSL device that owns the ringbuffer object
 * @flags: Internal control flags for the ringbuffer
 * @buffer_desc: Pointer to the ringbuffer memory descriptor
 * @wptr: Local copy of the wptr offset
 * @rptr: Read pointer offset in dwords from baseaddr
 * @last_wptr: offset of the last H/W committed wptr
 * @rb_ctx: The context that represents a ringbuffer
 * @id: Priority level of the ringbuffer, also used as an ID
 * @fault_detect_ts: The last retired global timestamp read during fault detect
 * @timestamp: The RB's global timestamp
 * @events: A kgsl_event_group for this context - contains the list of GPU
 * events
 * @drawctxt_active: The last pagetable that this ringbuffer is set to
 * @preemption_desc: The memory descriptor containing
 * preemption info written/read by CP
 * @pagetable_desc: Memory to hold information about the pagetables being used
 * and the commands to switch pagetable on the RB
 * @pt_update_desc: The memory descriptor containing commands that update
 * pagetable
 * @dispatch_q: The dispatcher side queue for this ringbuffer
 * @ts_expire_waitq: Wait queue to wait for rb timestamp to expire
 * @ts_expire_waitq: Wait q to wait for rb timestamp to expire
 * @wptr_preempt_end: Used during preemption to check that preemption occurred
 * at the right rptr
 * @gpr11: The gpr11 value of this RB
 * @preempted_midway: Indicates that the RB was preempted before rptr = wptr
 * @sched_timer: Timer that tracks how long RB has been waiting to be scheduled
 * or how long it has been scheduled for after preempting in
 * @starve_timer_state: Indicates the state of the wait.
 */
struct adreno_ringbuffer {
	struct kgsl_device *device;
	uint32_t flags;
	struct kgsl_memdesc buffer_desc;
	unsigned int sizedwords;
	unsigned int wptr;
	unsigned int rptr;
	unsigned int last_wptr;
	int id;
	unsigned int fault_detect_ts;
	unsigned int timestamp;
	struct kgsl_event_group events;
	struct adreno_context *drawctxt_active;
	struct kgsl_memdesc preemption_desc;
	struct kgsl_memdesc pagetable_desc;
	struct kgsl_memdesc pt_update_desc;
	struct adreno_dispatcher_cmdqueue dispatch_q;
	wait_queue_head_t ts_expire_waitq;
	unsigned int wptr_preempt_end;
	unsigned int gpr11;
	int preempted_midway;
	unsigned long sched_timer;
	enum adreno_dispatcher_starve_timer_states starve_timer_state;
};

#define GSL_RB_MEMPTRS_SCRATCH_MASK 0x1

#define GSL_RB_PROTECTED_MODE_CONTROL		0x200001F2

#define ADRENO_CURRENT_RINGBUFFER(a)	((a)->cur_rb)

#define KGSL_MEMSTORE_RB_OFFSET(rb, field)	\
	KGSL_MEMSTORE_OFFSET((rb->id + KGSL_MEMSTORE_MAX), field)

int cp_secure_mode(struct adreno_device *adreno_dev, uint *cmds, int set);

int adreno_ringbuffer_issueibcmds(struct kgsl_device_private *dev_priv,
				struct kgsl_context *context,
				struct kgsl_cmdbatch *cmdbatch,
				uint32_t *timestamp);

int adreno_ringbuffer_submitcmd(struct adreno_device *adreno_dev,
		struct kgsl_cmdbatch *cmdbatch,
		struct adreno_submit_time *time);

int adreno_ringbuffer_init(struct adreno_device *adreno_dev, bool nopreempt);

int adreno_ringbuffer_start(struct adreno_device *adreno_dev,
		unsigned int start_type);

void adreno_ringbuffer_stop(struct adreno_device *adreno_dev);

void adreno_ringbuffer_close(struct adreno_device *adreno_dev);

int adreno_ringbuffer_issuecmds(struct adreno_ringbuffer *rb,
					unsigned int flags,
					unsigned int *cmdaddr,
					int sizedwords);

void adreno_ringbuffer_submit(struct adreno_ringbuffer *rb,
		struct adreno_submit_time *time);

int adreno_ringbuffer_submit_spin(struct adreno_ringbuffer *rb,
		struct adreno_submit_time *time, unsigned int timeout);

int adreno_ringbuffer_submit_spin_retry(struct adreno_ringbuffer *rb,
		struct adreno_submit_time *time, unsigned int timeout,
		unsigned int retry);

void kgsl_cp_intrcallback(struct kgsl_device *device);

unsigned int *adreno_ringbuffer_allocspace(struct adreno_ringbuffer *rb,
						unsigned int numcmds);

void adreno_ringbuffer_read_pfp_ucode(struct kgsl_device *device);

void adreno_ringbuffer_read_pm4_ucode(struct kgsl_device *device);

void adreno_ringbuffer_mmu_disable_clk_on_ts(struct kgsl_device *device,
			struct adreno_ringbuffer *rb, unsigned int ts);

int adreno_ringbuffer_waittimestamp(struct adreno_ringbuffer *rb,
					unsigned int timestamp,
					unsigned int msecs);

int adreno_rb_readtimestamp(struct kgsl_device *device,
	void *priv, enum kgsl_timestamp_type type,
	unsigned int *timestamp);

int adreno_ringbuffer_submit_preempt_token(struct adreno_ringbuffer *rb,
					struct adreno_ringbuffer *incoming_rb);

static inline int adreno_ringbuffer_count(struct adreno_ringbuffer *rb,
	unsigned int rptr)
{
	if (rb->wptr >= rptr)
		return rb->wptr - rptr;
	return rb->wptr + KGSL_RB_DWORDS - rptr;
}

static inline unsigned int adreno_ringbuffer_inc_wrapped(unsigned int val,
							unsigned int size)
{
	return (val + sizeof(unsigned int)) % size;
}

static inline unsigned int adreno_ringbuffer_dec_wrapped(unsigned int val,
							unsigned int size)
{
	return (val + size - sizeof(unsigned int)) % size;
}

static inline int adreno_ringbuffer_check_timestamp(
			struct adreno_ringbuffer *rb,
			unsigned int timestamp, int type)
{
	unsigned int ts;
	adreno_rb_readtimestamp(rb->device, rb, type, &ts);
	return (timestamp_cmp(ts, timestamp) >= 0);
}

#endif  
