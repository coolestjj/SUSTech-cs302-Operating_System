# OS Mid Report

**李佳骏 11912021**

---

## 1. Terminologies of virtualization

### (1) Define virtualization in 100 words? (2 pts)
Virtualization is making one CPU multiple virtual machines, and each virtual machine can run different OS types and versions. Applications can run in independent spaces without interfering each other.

### (2) Please explain the three usage models of virtualization: workload isolation, workload consolidation, workload migration. (6 pts)

* **Workload isolation**: To make sure the system is secure and reliable, virtualization isolates software stacks so that they don’t interfere with each other.  
* **Workload consolidation**: To decrease the workload cost, virtualization merges workloads to one platform. Also, virtualization allows old and new OS to run simultaneously so that incompatible problems can be fixed.
* **Workload migration**: To migrate the guest to different platforms, virtualization adopts the approach of encapsulating the guest into a virtual machine so that it is easy to disconnect a guest and migrate.

### (3) List 3 well-known VMMs. (2 pts)
KVM, Xen, VMware Workstation

### (4) Explain the following terms: paravirtualization, full virtualization, binary translation, hardware-assisted virtualization, hybrid virtualization. (10 pts)
* **Para-virtualization**: The virtual machine doesn’t simulate hardware but provides the guest with interfaces to the hardware. It doesn't need to change guest applications and it has high performance, which are its benefits. However its supported OSes' number is not big.
* **Full virtualization**: The virtual machine simulates the full hardware environment, so that the OS can operate independently.
* **Hardware-assisted virtualization**: The guest OS can run independently by simulating the running environment with the assistance of hardware.
* **Hybrid virtualization**: It combines para-virtualization with hardware assisted virtualization. Para-virtualization is used for I/O, interrupt controllers and timer, while the same code in the original kernel can be used for CPU virtualization.
* **Binary translation**: Instructions are translated from source to target instruction set. Translation can happen in hardware or software.

## 2. Privilege levels

### (1) How many privilege levels does x86 (IA-32) provide? How are they used by OS and user processes (considering an ordinary OS without virtualization)? Please provide three examples of the importance of privilege separation. (5 pts)
x86 provides with 4 privilege levels. From the most privileged to the least are described as rings. OS code executes in ring 0, ring 1 and 2 are not usually used. OS can be ported to Xen by executing in ring 1. Application code is usually run in ring 3.

Example One: Ring aliasing can be solved by running softwares at a privilege level different from the one it was written.

Example Two: Ring compression can be solved by running guest OS and guest applications at different privilege levels. OS at ring 0 and applications at ring 3.

Example Three: Privilege separation avoids the circumstance that an unprivileged software accesses any part of the CPU state, which would probably cause faults.

### (2) What is ring compression? (3 pts)
Ring compression is a problem when guest OS and guest applications both run at privilege level 3 which is the same level, therefore VMM won’t be protected from guest software.

### (3) Without Intel VT-x, how does Xen address ring compression for X86 (IA-32)? (3 pts)
Xen uses paravirtualization to create an interface that is easier to be virtualized and commits changes to the guest-OS kernel and drivers.

### (4) Without Intel VT-x, how does Xen address ring compression for x86-64? (3 pts)
For x86/64, it runs the the guest kernel on two different page tables. The first one is for application context, and the second one is for kernel context. Transitions from application to kernel or the opposite way is supposed to pass through Xen, and it can switch between page tables.

### (5) What is ring aliasing? (3 pts)
Ring aliasing is a problem when software runs at a privilege level different from the one it was written.

### (6) What are VMX root and VMX non-root in VT-x? (4 pts)
VMX root and VMX non-root are both CPU operations. VMX root operation is in charge of VMM, and VMX non-root operation is in charge of guest software.

### (7) How does Intel VT-x address the challenges of ring aliasing and ring compression? (4 pts)
VT-x solves ring aliasing by allowing guest software to be run at the privilege level it was written to.

VT-x solves ring compression by running guest OS at ring 0 and executing guest applications at ring 3 both at VMX non-root operation.

## 3. System calls, interrupts and exceptions

### (1) In the context of ordinary OS without virtualization, what are the purpose of system calls? What is the main difference between system calls and function calls? On x86-64 Linux, how are system call parameters passed to the kernel? (5 pts)
System calls can deliver arguments to the kernel space so that it can perform tasks according to instructions. It is important because the hardware resource cannot be modified directly by users, so system calls are needed. 

The system calls in different OS are distinct, while function calls are mostly the same as long as they share identical C library. The system calls involve kernel address space while function calls involve user address space. 

Arguments are passed to the kernel in registers, and only parameters of class INTEGER or MEMORY can be passed to the kernel. INTEGER parameters fit into general purpose registers, while MEMORY parameters are passed via stack.

### (2) What is a hypercall in Xen? (2 pts)
Hypercall is a mechanism that allows a domain to make synchronous calls to Xen, therefore controlling the interactions between them.

### (3) How does Xen virtualize exceptions on x86 (IA-32)? What modifications does Xen make to the original x86 exception handlers? (4 pts)
Xen has a table that matches handlers from every exception type, and those handlers are the same with the handlers in real x86.

The modification is in the page fault handler, in original x86, it reads the faulting address from CR2, but in Xen, the faulting address is written into an extended stack frame.

### (4) What are the challenges of virtualizing interrupts (especially regarding interrupt masking) on x86 (IA-32)? (3 pts)
VMM’s protection mechanisms to control interrupt masking will cause faulting in ring deprivileging. When mask and unmask interrupts frequently, it could have negative impact on system performance. Even without frequent interception of each attempt, there are still challenges concerning to “virtual interrupt”. To deliver virtual interrupts timely, interceptions of some attempts to change interrupt masking will make VMM structure too complex.

### (5) How does Xen virtualize interrupts on x86 (IA-32)? What is the benefit of such a design? (4pts)

When an interrupt is asserted, a stub routine in Xen would be triggered to avoid entering into the managing domain immediately. Thus Xen can control the system by scheduling the domain's ISR, which stands for interrupt service routine. Also, Xen can respond to the interrupt controller in time and switch address space whenever a distinct domain is in the process of execution. Furthermore, Xen use event channels mechanism to convey asynchronous event messages to domains. Each channel in each domain consists of 2 bit flags that can signal whether an event is pending or to mask the event.

Xen can control the system by scheduling the domain's ISR in a tight way. Unneccesary upcalls and unmask downcalls can be avoided. 

### (6) What is VMCS in Intel VT-x? What are VM exits and VM entry? How are VMCS used during VM exits and VM entry (4 pts)
VMCS is a kind of data structure and it can control VM entries, VM exits as well as the behavior of processor in VMX non-root operations. 

VM entries is a transition from VMM to guest, and VM exit is a transition from guest to VMM. 

There are two sections in VMCS called guest-state area and host-state area, VM entry load from guest-state area while VM exit save to it and load from host-state area.

### (7) How does Xen leverage Intel VT-x to virtualize interrupts? (3 pts)
VT-x leverages external interrupt existing control as well as interrupt window existing control. If the former is set, external interrupts will make VM exit, and at the mean time it can't be masked. If the latter is set, when the guest software is prepared for interrupts, the VM will exit.

### (8) How does Intel VT-x support exception virtualization? (3 pts)
VT-x uses an exception bitmap. This map has 32 entries for IA-32 exceptions, therefore the VMM can see what the exception type is that causes the exits of VM according to the map.


## 4. Address translation

### (1) Explain x86 (IA-32) address translation. (3 pts)
In the linear address, the highest 10 bits are used as the index of page directory table to locate page directory table entry. The middle 10 bits are used as index of page table to locate page table entry. And the PTE points to the physical page. Lastly, the remaining 12 bits are the offset to locate the physical address.

### (2) Explain x86-64 address translation (2 pts)
Among the 64 bits in the linear address, 48 bits are used: the highest 36 bits are indexes to a series of tables, which eventually leads to the physical page's base address. The remaining 12 bits are the offset to locate the physical address.

### (3) Explain the relationship between guest virtual memory, guest physical memory, and machine memory. (3 pts)
Virtual memory’s address space is mapped to physical memory’s physical frames by the OS using the page table. The VMM beneath the OS maps physical frames to machine memory’s machine frames. Therefore OSes can share physical memory.

### (4) How does Xen manage the per-process page table in the VM and the per-OS page table in the VMM? (4 pts)

* **per-process**: When a guest OS needs a new page table, it creates a page independently and it is registered with Xen. OS cannot write to page-table memory directly because Xen controls all updates. Every address space's highest 64MB will not be accessible or mappable because it is for Xen.

* **per-OS**: Xen uses MMU to register per-OS page tables, and then give guest OSes the authority of read-only. Updates of page tables can be received by Xen using a hypercall. Requests are confirmed first, then can be applied in order to guarantee safety. 

### (5) What does “Address-space compression” mean? (3 pts)
Address-space compression means the difficulties of preventing the virtual-address space portions that the VMM is using from being accessed by guest and also refers to the difficulties of supporting guest access to such portions.

### (6) How does Xen address the problem of “Address-space compression”? (4 pts)
The transition between VMM and guest software changes the linear-address space, which can let guest software fully use its address space. The VMX transitions cause changes in the physical-address space which is different from linear-address space, excluding interference. Every address space's highest 64MB will not be accessible or mappable because it is for Xen.

### (7) What is Intel EPT? How to do MMU virtualization with Intel EPT? (5 pts)
EPT is short for Extended Page Tables. It is used to alleviate the overhead of page table shadowing. EPT based pointer points to extended page tables, which are controled by VMM. In order to reach the guest domains' memory, EPT can map guest-physical to host-physical address. Therefore it prevents some VM exits and reduces overheads in paging.

After activating EPT, VMM can usepage directory address pointer, which is also called CR3 register, to locate IA-32 page tables' address. Then according to the table, Guest Linear Address can be converted into Guest Physical Address. EPT base pointer can locate EPT page tables' address. Then Guest Physical Address can be converted into Host Physical Address.

### (8) How does Xen allocate physical memory to each domain? (3 pts)
If a domain requires more memory, Xen may allocate more memory pages to it; If a domain requires less memory, it may release memory pages to Xen. By passing memory pages from one to another frequently, Xen can effectively allocate physical memory to each domain.