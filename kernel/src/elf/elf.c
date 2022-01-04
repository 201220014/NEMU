#include "common.h"
#include "memory.h"
#include "string.h"

#include <elf.h>

#ifdef HAS_DEVICE_IDE
#define ELF_OFFSET_IN_DISK 0
#endif

#define STACK_SIZE (1 << 20)

void ide_read(uint8_t *, uint32_t, uint32_t);
void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph, *eph;

#ifdef HAS_DEVICE_IDE
	uint8_t buf[4096];
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
	elf = (void *)buf;
	Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	Log("ELF loading from ram disk.");
#endif

	/* Load each program segment */
	ph = (void *)elf + elf->e_phoff;
	Log("%x", ph);
	eph = ph + elf->e_phnum;
	Log("%x", eph);
	for (; ph < eph; ph++)
	{
		if (ph->p_type == PT_LOAD)
		{
			// remove this panic!!!
			// panic("Please implement the loader");
#ifdef IA32_PAGE
            uint32_t paddr = mm_malloc(ph->p_vaddr, ph->p_memsz);
            // Log("line43: mm_malloc paddr = 0x%x, ph->p_vaddr = 0x%x, ph->p_memsz = 0x%x", paddr, ph->p_vaddr, ph->p_memsz);
#endif

#ifndef HAS_DEVICE_IDE
/* TODO: copy the segment from the ELF file to its proper memory area */
            memcpy((void *)paddr, (void *)ph->p_offset, ph->p_filesz);
#else
            ide_read((void *)paddr, ph->p_offset, ph->p_filesz);
#endif
/* TODO: zeror the memory area [vaddr + file_sz, vaddr + mem_sz) */
            memset((void *)paddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);

#ifdef IA32_PAGE
			/* Record the program break for future use */
			extern uint32_t brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if (brk < new_brk)
			{
				brk = new_brk;
			}
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	// uint32_t p =
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
	// Log("line67: mm_malloc stack: paddr = 0x%x, vaddr = 0x%x, memsz = 0x%x", p, KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif

	write_cr3(get_ucr3());
#endif
	return entry;
}
