#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

//#define debug_flag
spinlock_t lock;

#ifdef debug_flag

static int kthreadTest(void* param)
{
	printk(KERN_INFO "+++++ SpinLock kthreadTest0 00000  +++++! \n");
	printk(KERN_INFO "current->wake_cpu %d\n", current->wake_cpu);
	printk(KERN_INFO "current->on_cpu %d\n", current->on_cpu);
	printk(KERN_INFO "current->cpu %d\n", task_thread_info(current)->cpu);
	//local_irq_disable();
	
	printk("kthreadTest ! \n");
	spin_lock_irqsave(&lock, flags);
	//local_irq_save(flags);
	//spin_lock_irq(&lock);
	//do_raw_spin_lock_flags(spinlock_check(&lock), &flags);
	printk("kthreadTest after spin_lock_irqsave! \n");
	spin_lock_irqsave(&lock, flags);
	return 0;
}

#else

static int kthreadTest0(void* param)
{
	printk(KERN_INFO "+++++ SpinLock kthreadTest0 00000  +++++! \n");
	printk(KERN_INFO "current->wake_cpu %d\n", current->wake_cpu);
	printk(KERN_INFO "current->on_cpu %d\n", current->on_cpu);
	printk(KERN_INFO "current->cpu %d\n", task_thread_info(current)->cpu);
	//local_irq_disable();
	unsigned long flags;
	
	int tem = 999999;
	long long sum = 0;
	//printk("kthread 0 msleep 100 \n");
	spin_lock_irqsave(&lock, flags);
	//local_irq_save(flags);
	//spin_lock_irq(&lock);
	//do_raw_spin_lock_flags(spinlock_check(&lock), &flags);
	msleep(100);
	#if 0 
	while (tem--)
	{
		sum = sum + tem;
		udelay(1);
	}
	#endif
	//printk("kthread 0 after msleep 100 \n");
	printk("kthread 0 after calc %lld \n", sum);
	spin_unlock_irqrestore(&lock, flags);
	return 0;
}

static int kthreadTest1(void* param)
{
	printk(KERN_INFO "-----  SpinLock kthreadTest1! 11111  ------ \n");
	printk(KERN_INFO "current->wake_cpu %d\n", current->wake_cpu);
	printk(KERN_INFO "current->on_cpu %d\n", current->on_cpu);
	printk(KERN_INFO "current->cpu %d\n", task_thread_info(current)->cpu);
	unsigned long flags;
	//local_irq_disable();
	
	printk("kthread 1 \n");
	spin_lock_irqsave(&lock, flags);
	printk("kthread 1 ready to exit \n");
	spin_unlock_irqrestore(&lock, flags);
	printk("kthread 1 exit \n");
	return 0;
}
#endif

static int __init hello_init(void)
{	
	struct task_struct *task[4];
	int err_code;
	int i = 0;
	
	printk(KERN_INFO "SpinLock Test init! \n");	
	spin_lock_init(&lock);
	
#ifdef debug_flag
	for (i = 0; i < 1; i++)
	{
		task[i] = kthread_create(kthreadTest, NULL, "kthreadTest %d", i);
		if (IS_ERR(task[i])) 
		{
			err_code = PTR_ERR(task[i]);
			printk(KERN_ALERT "Create thread failed: %d.\n", err_code);
			task[i] = NULL;
		}
		
		kthread_bind(task[i], i);
		wake_up_process(task[i]);
	}
#else
	task[0] = kthread_create(kthreadTest0, NULL, "kthreadTest0");
	if (IS_ERR(task[0])) 
	{
		err_code = PTR_ERR(task[0]);
		printk(KERN_ALERT "Create thread0 failed: %d.\n", err_code);
		task[0] = NULL;
	}
	
	kthread_bind(task[0], 0);
	wake_up_process(task[0]);
	
	msleep(10);
	
	task[1] = kthread_create(kthreadTest1, NULL, "kthreadTest1");
	if (IS_ERR(task[1])) 
	{
		err_code = PTR_ERR(task[1]);
		printk(KERN_ALERT "Create thread1 failed: %d.\n", err_code);
		task[1] = NULL;
	}
	
	kthread_bind(task[1], 0);
	wake_up_process(task[1]);
#endif	
	return 0;
}

static void __exit hello_exit(void)
{
	//spin_unlock_irq(&lock);
	
	//spin_unlock_irqrestore(&lock, flags);
	//local_irq_restore(flags);
	//do_raw_spin_unlock(spinlock_check(&lock));
	printk("SpinLock Test Bye! \n");
	return;
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
