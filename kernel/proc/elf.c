#include "elf.h"
#include <errno/error.h>
#include <ak/mman.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <proc/task.h>
#include <debug/debug.h>
#include <ak/string.h>

#define NO_ERROR 0
#define ERR_NOT_ELF_FILE 1
#define ERR_NOT_SUPPORTED_PLATFORM 2
#define ERR_NOT_SUPPORTED_ENCODING 3
#define ERR_WRONG_VERSION 4
#define ERR_NOT_SUPPORTED_TYPE 5

/**
 * @brief elf verify
 * 
 * @param elf_header 
 * @return int 
 */
static int elf_verify(struct elf32_ehdr *elf_header) {
	if (!(elf_header->e_ident[EI_MAG0] == ELFMAG0 &&
		  elf_header->e_ident[EI_MAG1] == ELFMAG1 &&
		  elf_header->e_ident[EI_MAG2] == ELFMAG2 &&
		  elf_header->e_ident[EI_MAG3] == ELFMAG3))
		return -ERR_NOT_ELF_FILE;

	if (elf_header->e_ident[EI_CLASS] != ELFCLASS32)
		return -ERR_NOT_SUPPORTED_PLATFORM;

	if (elf_header->e_ident[EI_DATA] != ELFDATA2LSB)
		return -ERR_NOT_SUPPORTED_ENCODING;

	if (elf_header->e_ident[EI_VERSION] != EV_CURRENT)
		return -ERR_WRONG_VERSION;

	if (elf_header->e_machine != EM_386)
		return -ERR_NOT_SUPPORTED_PLATFORM;

	if (elf_header->e_type != ET_EXEC)
		return -ERR_NOT_SUPPORTED_TYPE;

	return NO_ERROR;
}

struct elf32_layout *elf_load(const char *path) {
	const char *buf = vfs_read(path);
	struct elf32_ehdr *elf_header = (struct elf32_ehdr *)buf;

	if (elf_verify(elf_header) != NO_ERROR || elf_header->e_phoff == 0) {
		log("ELF: %s is not correct format", path);
		return NULL;
	}

	log("ELF: Load %s", path);
	struct mm_struct *mm = current_process->mm;
	struct elf32_layout *layout = kcalloc(1, sizeof(struct elf32_layout));
	layout->entry = elf_header->e_entry;
	for (struct elf32_phdr *ph = (struct elf32_phdr *)(buf + elf_header->e_phoff);
		 ph && (char *)ph < (buf + elf_header->e_phoff + elf_header->e_phentsize * elf_header->e_phnum);
		 ++ph)
	{
		if (ph->p_type != PT_LOAD)
			continue;


		if ((ph->p_flags & PF_X) != 0 && (ph->p_flags & PF_R) != 0)
		{
			do_mmap(ph->p_vaddr, ph->p_memsz, 0, 0, -1, 0);
			mm->start_code = ph->p_vaddr;
			mm->end_code = ph->p_vaddr + ph->p_memsz;
		}

		else if ((ph->p_flags & PF_W) != 0 && (ph->p_flags & PF_R) != 0) {
			do_mmap(ph->p_vaddr, ph->p_memsz, 0, 0, -1, 0);
			mm->start_data = ph->p_vaddr;
			mm->end_data = ph->p_memsz;
		}

		else
			do_mmap(ph->p_vaddr, ph->p_memsz, 0, 0, -1, 0);

		memset((char *)ph->p_vaddr, 0, ph->p_memsz);
		memcpy((char *)ph->p_vaddr, buf + ph->p_offset, ph->p_filesz);
	}

	uint32_t heap_start = do_mmap(0, UHEAP_SIZE, 0, 0, -1, 0);
	mm->start_brk = heap_start;
	mm->brk = heap_start;
	mm->end_brk = USER_HEAP_TOP;

	uint32_t stack_start = do_mmap(0, STACK_SIZE, 0, 0, -1, 0);
	layout->stack = stack_start + STACK_SIZE;

	return layout;
}

/**
 * @brief elf unload
 * 
 */
void elf_unload() {
	signalemptyset(&current_process->thread->pending);

	struct vm_area_struct *iter, *next;

	list_for_each_entry_safe(iter, next, &current_process->mm->mmap, vm_sibling) {
		if (!iter->vm_file && (iter->vm_flags & MAP_SHARED) == 0) {
			vmm_unmap_range(current_process->pdir, iter->vm_start, iter->vm_end);
			list_del(&iter->vm_sibling);
		}
	}

	memset(current_process->mm, 0, sizeof(struct mm_struct));
	init_list(&current_process->mm->mmap);
}