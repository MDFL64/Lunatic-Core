virtual at 0
kernel_info_table:
	.mem_low: dd 0
	.mem_high: dd 0
	.kernel_end: dq 0
	.init_file_count: dd 0
	.init_files:
end virtual

; a macro I stole off the FASM forum and edited
macro display_num num 
{
	value = num
	pos=1

	if value<0
		value = -value
		display '-'
	end if

	while pos*10<=value
		pos=pos*10
	end while

	while pos>=1
		digit=value/pos 
		value=value-(digit*pos)
		pos=pos/10
		display ('0'+digit)
	end while
}

macro preserve_eax {
	mov [esp+28],eax
}

macro preserve_ebx {
	mov [esp+16],ebx
}

use16
org 0x7c00
fat_bpb:
	.sectors_per_cluster = $ + 13
	.reserved_sectors = $ + 14
	.fat_count = $ + 16
	.sector_count_tiny = $ + 19
	.sector_count = $ + 32
	.sectors_per_fat = $ + 36
	.root_cluster_n = $ + 44

	jmp boot_stage1
	nop
	rb 87 ; We patch this in later using our build script.

; This is the boot sector.
; We only really load the rest of the boot code.
boot_stage1:

	; store boot disk
	mov [disk_n],dl

	; make sure the video mode is what we want
	mov ax, 3
	int 0x10

	; relocate stack
	mov ax,0x5000
	mov ss,ax
	mov esp,0xFFF0
	mov ebp,esp
	
	mov si, error_a20
	jc boot_halt

	; Check for disk extension support.
	mov dl, [disk_n]
	mov ah, 0x41
	mov bx, 0x55AA
	int 0x13

	mov si,error_disk_unsupported
	jc boot_halt

	; Load stage 2.
	mov ah, 0x42
	mov si, disk_addr
	int 0x13

	mov si,error_disk_failed
	jc boot_halt

	jmp boot_stage2

boot_halt:
	call print_str
	mov si,error_halt
	call print_str
	cli
	hlt

print_str:
	pushad

	mov ah,0x0e
	mov bh,0

	.top:
	mov al,[si]
	cmp al,0
	je .end
	
	int 0x10
	inc si
	jmp .top
	
	.end:

	popad
	ret

disk_n:
	db 0
disk_addr:
	db 0x10 ; packet size
	db 0 ; reserved
	.sector_count:
	dw stage2_sectors ; sector count (MAX 127 ON SOME BIOS)
	dw 0;
	.dest_segment:
	dw 0x07e0 ; read buffer
	.sector_index:
	dq 1

error_disk_unsupported:
	db "Can't read sectors from disk.",0
error_disk_failed:
	db "Failed to read sectors from disk.",0
error_halt:
	db 0x0a,0x0a,0x0d," - NEWMOON BOOTLOADER HALTED -",0

; Fill out boot sector.
bytes_left = 510-($-$$)

display "Bytes left in boot sector: "
display_num (bytes_left-64)
display 0x0a,0x0d

times bytes_left db 0
db 055h, 0AAh

fat_fsi:
	rb 512

; This is stage two. We do a few things here:
; - Check the amount of memory.
; - Set up page tables.
; - Get ready for long mode.
; - Load files that the kernel needs to initialize.
; - Select a video mode.
; - Jump to stage 3.
boot_stage2:

	; check 64 bit support
	mov eax, 0x80000001
	cpuid
	test edx,0x20000000

	mov si, error_longmode
	jz boot_halt

	; enable A20 line
	mov ax, 0x2401
	int 0x15

	call fetch_mmap

	call setup_pagetable

	; enable long mode
	mov ecx, 0xc0000080
	rdmsr
	or eax, 0x100
	wrmsr

	; Calculate FAT cluster base.
	mov eax, [fat_bpb.sectors_per_fat]
	movzx ebx, byte [fat_bpb.fat_count]
	imul eax, ebx
	movzx ebx, word [fat_bpb.reserved_sectors]
	add eax, ebx
	mov [fat_cluster_base], eax

	; Search root directory for lunatic directory
	mov eax, [fat_bpb.root_cluster_n]
	mov si, fname_lunatic
	call fat_find

	; Search lunatic directory for init directory
	mov si, fname_init
	call fat_find

	; Index the init directory so the kernel can see what's there.
	call fat_index_init_dir

	call fat_load_init_files

	jmp boot_stage3

mmap_buffer:
	.base_low: dd 0
	.base_high: dd 0
	.size_low: dd 0
	.size_high: dd 0
	.type: dd 0

fetch_mmap:
	pushad

	mov bx, 0x6000
	mov fs, bx
	mov ebx, 0
	mov es, bx
	mov di, mmap_buffer

	mov [fs:kernel_info_table.mem_low], ebx
	mov [fs:kernel_info_table.mem_high], ebx

	.mmap_loop:
	mov eax, 0x0000E820
	mov edx, 0x534D4150
	mov ecx, 20
	int 0x15
	mov si, error_mmap_failed
	jc boot_halt

	mov eax, [mmap_buffer.type]
	cmp eax, 1
	jne .mmap_skip

	mov eax, [mmap_buffer.base_low]
	mov ecx, [mmap_buffer.size_low]

	add ecx,eax

	mov eax, [mmap_buffer.base_high]
	mov edx, [mmap_buffer.size_high]

	adc edx, eax

	; ecx = low
	; edx = high

	cmp edx, [fs:kernel_info_table.mem_high]
	jb .mmap_skip
	ja .mmap_write

	cmp ecx, [fs:kernel_info_table.mem_low]
	jb .mmap_skip

	.mmap_write:
	mov [fs:kernel_info_table.mem_low], ecx
	mov [fs:kernel_info_table.mem_high], edx

	.mmap_skip:
	cmp ebx, 0
	jne .mmap_loop

	popad
	ret

setup_pagetable:
	pushad

	mov bx, 0x7000
	mov es, bx
	
	; zero all three tables
	mov bx, 0
	mov eax, 0
	.zero_pages:
	mov [es:bx], eax
	add bx, 4
	cmp bx, 0x3000
	jb .zero_pages

	; root points to 512*512 gig (256 tb) -> only ever fill first entry, mark rest as non-existant
	mov bx, 0
	mov eax, 0x71001
	mov [es:bx], eax

	; mid table points to 512*1 gig tables (512 gig) -> fill first entry initially, mark rest as non-existant, allow extension later
	mov bx,0x1000
	mov eax, 0x72001
	mov [es:bx], eax

	; lowest table points to 512*2 mb pages (1 gig) -> always fill, PS SET
	mov bx, 0x2000
	mov eax, 0x0081
	.fill_table:
	mov [es:bx], eax
	add bx, 8
	add eax, 0x00200000
	cmp bx, 0x3000
	jb .fill_table
	
	; tell the processor where the page table is
	mov eax, 0x70000
	mov cr3, eax

	; enable pae, sse
	mov eax, cr4
	or eax, 0x20
	mov cr4, eax

	popad
	ret


fat_table_cache_index:
	dd 1 ; set 1st index to an invalid value
	rd 127

; eax = current cluster -> eax = next cluster
fat_next_cluster:
	pushad
	
	and eax,0x0FFFFFFF

	mov ebx,eax
	shr ebx,7
	; ebx = table sector

	mov ecx,ebx
	and ecx,0x7F
	; ecx = table sector IN cache

	mov edx,[fat_table_cache_index+ecx*4]
	; edx = table sector currently loaded in cache

	cmp ebx,edx
	je .noload

	push eax

	; table sector not cached, load it
	movzx eax, word [fat_bpb.reserved_sectors]
	add eax, ebx
	mov [disk_addr.sector_index], eax
	mov [disk_addr.sector_count], word 1

	mov ax, 0x20
	imul ax,cx
	add ax,0x2000
	mov [disk_addr.dest_segment], ax

	mov dl, [disk_n]
	mov ah, 0x42
	mov si, disk_addr
	int 0x13
	
	mov si, error_fat_table
	jc boot_halt

	; update cache index entry
	mov [fat_table_cache_index+ecx*4], ebx

	pop eax

	.noload:

	; Finally, read from the FAT.
	and eax,0x3FFF
	
	mov bx,0x2000
	mov es,bx

	mov eax,[es:eax*4]

	preserve_eax
	popad
	ret

fat_cluster_base:
	dd 0

; eax = cluster
; ebx = offset in io buffer -> ebx = new offset in io buffer
fat_load_cluster:
	pushad

	and eax,0x0FFFFFFF

	movzx ecx, byte [fat_bpb.sectors_per_cluster]
	; ecx = cluster size

	sub eax,2
	imul eax,ecx
	add eax, [fat_cluster_base]
	mov [disk_addr.sector_index], eax

	mov [disk_addr.sector_count], cx
	
	mov eax,ebx
	shr ax,4
	add ax,0x1000
	mov [disk_addr.dest_segment], ax

	mov dl, [disk_n]
	mov ah, 0x42
	mov si, disk_addr
	int 0x13
	
	mov si, error_fat_cluster
	jc boot_halt

	imul ecx, 512
	add ebx, ecx

	preserve_ebx
	popad
	ret

; eax = cluster -> cluster to continue at
fat_read_file:
	pushad

	mov ebx, 0
	
	.read_loop:
	call fat_load_cluster
	call fat_next_cluster

	cmp eax, 0xFFFFFF0
	jae .eof
	cmp eax, 1
	jbe .eof

	cmp ebx, 0x10000
	jb .read_loop
	ja .overflow

	preserve_eax
	popad
	ret

	.eof:

	mov eax, 0

	preserve_eax
	popad
	ret

	.overflow:
	mov si, error_fat_overflow
	jmp boot_halt

; eax = cluster of the directory to search -> eax = cluster of file, zero if not found
; si = 11 byte FAT file name
; Note that this will fail for directorys larger than 64k. Should be easy to fix if there's ever a need...
fat_find:
	pushad

	.next_cluster:
	call fat_read_file
	; eax = cluster to continue @

	; set up read segment and pointer
	mov bx,0x1000
	mov es,bx
	xor edi,edi

	.top:
	
	; end of directory
	mov bl, [es:edi]
	cmp bl, 0
	je .fail
	
	; empty entry
	cmp bl, 0x05
	je .skip
	cmp bl, 0xE5
	je .skip

	; dotfile
	cmp bl, 0x2E
	je .skip

	; volume labels and LFNs
	mov bl, [es:edi+0x0B]
	test bl,0x08
	jnz .skip

	mov ecx, 11
	push esi
	push edi
	repe cmpsb
	pop edi
	pop esi

	je .success

	.skip:
	add edi, 32
	cmp edi,0x10000
	jb .top
	
	; end of buffer reached

	; make sure there's another cluster
	test eax,eax
	jz .fail

	jmp .next_cluster

	.success:
	mov eax, [es:edi+0x14]
	shl eax,16
	movzx ebx, word [es:edi+0x1A]
	or eax, ebx
	preserve_eax
	popad
	ret

	.fail:
	mov si,error_fat_file_not_found
	call boot_halt


; eax = cluster of directory to save
fat_index_init_dir:
	pushad

	mov edx,0

	.next_cluster:
	call fat_read_file
	; eax = cluster to continue @

	; set up read segment and pointer
	cli
	mov bx,0x1000
	mov ds,bx
	mov bx,0x6000
	mov es,bx
	xor esi,esi
	mov edi,kernel_info_table.init_files


	.top:
	
	; end of directory
	mov bl, [ds:esi]
	cmp bl, 0
	je .done
	
	; empty entry
	cmp bl, 0x05
	je .skip
	cmp bl, 0xE5
	je .skip

	; dotfile
	cmp bl, 0x2E
	je .skip

	; volume labels and LFNs -AND- subdirectories
	mov bl, [ds:esi+0x0B]
	test bl,0x18
	jnz .skip

	; file size > 0
	mov ebx, [ds:esi+0x1C]
	test ebx,ebx
	jz .skip

	mov ecx, 11
	push esi
	rep movsb
	pop esi
	
	; cluster
	mov ebx, [ds:esi+0x14]
	shl ebx,16
	movzx ecx, word [ds:esi+0x1A]
	or ebx, ecx
	mov [es:edi],ebx ;GOOD
	add edi,4

	
	; file size
	mov ebx, [ds:esi+0x1C]
	mov [es:edi],ebx
	add edi,4

	inc edx

	.skip:
	add esi, 32
	cmp esi,0x10000
	jb .top
	
	; end of buffer reached
	; reset ds since we're either going to load more stuff, or fail
	mov bx,0
	mov ds,bx

	; make sure there's another cluster
	test eax,eax
	jz .fail

	jmp .next_cluster

	.done:
	mov bx,0
	mov ds,bx

	mov [es:kernel_info_table.init_file_count],edx

	popad
	ret

	.fail:
	mov si,error_fat_index_init_dir
	call boot_halt

fat_init_write_head:
	dd 0x100000

fat_load_init_files:
	pushad

	; reset segment
	mov bx,0x6000
	mov es,bx

	xor eax,eax
	mov esi, kernel_info_table.init_files+15
	mov ecx, [es:kernel_info_table.init_file_count]
	.count_size:
	
	add eax, [es:esi]
	add esi, 19
	loop .count_size

	cmp eax,0xE00000
	ja .fail

	; actually load stuff
	mov esi, kernel_info_table.init_files+11
	mov ecx, [es:kernel_info_table.init_file_count]

	.next_file:
	; reset the segment just to be safe
	mov bx,0x6000
	mov es,bx

	; Read cluster id, write back memory address loaded to
	mov eax, [es:esi]
	mov ebx, [fat_init_write_head]
	mov [es:esi], ebx

	mov ebx, [es:esi+4]

	; eax = cluster
	; ebx = size

	.next_cluster:
	call fat_read_file

	call fat_relocate_init_data

	test eax,eax
	jnz .next_cluster

	add esi, 19
	loop .next_file

	popad
	ret

	.fail:
	mov si,error_fat_init_toobig
	call boot_halt

; ebx = file size remaining -> file size remaining after copy

fat_relocate_init_data:
	pushad

	call enter_long
	use64

	; clear registers used by copy instruction
	xor rcx,rcx
	xor rsi,rsi
	xor rdi,rdi

	; ecx = bytes to copy
	mov ecx,ebx
	cmp ecx, 0x10000
	jbe .not_too_big
	mov ecx, 0x10000
	.not_too_big:

	; ebx = bytes left
	sub ebx, ecx
	mov esi, 0x10000
	mov edi, [fat_init_write_head]

	; copy!
	rep movsb

	mov [fat_init_write_head], edi

	call enter_real
	use16

	popad
	ret

enter_temp_var:
	dd 0

; switch to long mode... very carefully
enter_long:
	cli

	mov [enter_temp_var],eax

	; extend the return address
	pop ax
	shl eax, 16
	push ax
	push ax
	push ax
	shr eax, 16
	push ax

	; enable protected mode, paging
	mov eax, cr0
	or eax, 0x80000001
	mov cr0, eax

	; load our gdt
	lgdt [_gdt.ptr]

	; CODE
	jmp _gdt.code:.switch_long
	.switch_long:
	use64
	; DATA
	mov ax,_gdt.data
	mov ds,ax
	mov es,ax
	; STACK
	mov ss,ax
	add esp,0x50000
	
	mov eax, [enter_temp_var]
	ret

; switch to real mode... even more carefully
enter_real:
	mov [enter_temp_var],eax

	; switch to temp mode
	; CODE
	jmp far dword [.temp_addr]
	.switch_temp:
	use16
	; DATA
	mov ax,_gdt.data16
	mov ds,ax
	mov es,ax
	; STACK [NOW BROKEN]
	mov ss,ax

	; disable protected mode, paging
	mov eax, cr0
	and eax,0x7FFFFFFE
	mov cr0, eax

	; fix segments for good
	; CODE
	jmp 0:.switch_real
	.switch_real:
	; DATA
	mov ax,0
	mov ds,ax
	mov es,ax
	; STACK
	mov ax,0x5000
	mov ss,ax
	sub esp, 0x50000

	; shorten return address
	pop ax
	add esp,6
	push ax

	mov eax, [enter_temp_var]
	ret

	.temp_addr:
	dw .switch_temp
	dw _gdt.code16

error_longmode:
	db "Your processor does not support long mode.",0x0a,0x0d
	db "If you're using a VM, it may be configured wrong.",0
error_a20:
	db "Failed to fix A20 line.",0
error_mmap_failed:
	db "Failed to fetch memory information.",0
error_fat_table:
	db "Failed to load part of the FAT table.",0
error_fat_cluster:
	db "Failed to load a FAT cluster.",0
error_fat_overflow:
	db "The FAT read function overflowed its buffer. Whoops.",0
error_fat_file_not_found:
	db "Failed to find a file required to boot.",0
error_fat_index_init_dir:
	db "Failed to index init directory.",0

error_fat_init_toobig:
	db "Failed to load init files, not enough room.",0

fname_lunatic:
	db "LUNATIC    "
fname_init:
	db "INIT       "

_gdt:
	.null = $ - _gdt
	dw 0 ; limit, low
	dw 0 ; base, low
	db 0 ; base, middle
	db 0 ; access
	db 0 ; flags, limit high ???
	db 0 ; base, high

	.code = $ - _gdt
	dw 0x0000 ; limit, low
	dw 0 ; base, low
	db 0 ; base, middle
	db 10011010b; access
	db 00100000b ; flags, limit high ???
	db 0 ; base, high

	.data = $ - _gdt
	dw 0x0000 ; limit, low
	dw 0 ; base, low
	db 0 ; base, middle
	db 10010010b; access
	db 00100000b ; flags, limit high ???
	db 0 ; base, high

	.code16 = $ - _gdt
	dw 0xFFFF ; limit, low
	dw 0 ; base, low
	db 0 ; base, middle
	db 10011010b; access
	db 00000000b ; flags, limit high ???
	db 0 ; base, high

	.data16 = $ - _gdt
	dw 0xFFFF ; limit, low
	dw 0 ; base, low
	db 0 ; base, middle
	db 10010010b; access
	db 00000000b ; flags, limit high ???
	db 0 ; base, high

	.ptr:
	dw $ - _gdt - 1 ; length
	dq _gdt

; Stage 3 runs entirely in long mode.
; It has one job: Relocate and launch the kernel!
boot_stage3:
	call enter_long
	use64

	xor rcx, rcx
	mov ecx, [kernel_info_table.init_file_count+0x60000]
	mov rsi, kernel_info_table.init_files+0x60000
	mov rbx, "FULLMOON"
	mov edx, "NKRN"
	
	find_kernel_loop:
	mov rax, [rsi]
	cmp rax, rbx
	jne find_kernel_skip

	mov eax, [rsi+7]
	cmp eax, edx
	je find_kernel_end

	find_kernel_skip:
	add rsi, 19
	loop find_kernel_loop

	mov si, error_kernel_missing
	jmp long_halt

	find_kernel_end:
	xor rax,rax
	mov eax, [rsi+11] ; kernel loc
	;mov ebx, [rsi+15]

	mov si, error_kernel_invalid

	mov bx, [eax]
	cmp bx, 0x5A4D
	jne long_halt

	push rax
	add eax, [eax+0x3C]
	mov ebx, [eax]
	cmp ebx, 0x00004550
	jne long_halt
	; eax = start of PE header

	; check extended header size
	mov bx, [eax+0x14]
	cmp bx, 0xf0
	jb long_halt

	; check 64 bit
	mov cx, [eax+0x18]
	cmp cx, 0x020B
	mov si, error_kernel_32
	jne long_halt

	; check subsystem
	mov cx, [eax+0x5C]
	cmp cx, 1
	mov si, error_kernel_not_native
	jne long_halt

	movzx edx, bx
	add edx, eax
	add edx, 0x18
	
	xor rbx,rbx
	mov ebx, [eax+0x30]
	cmp ebx,0x1000000
	mov si, error_kernel_bad_offset
	jne long_halt

	; grab entry point
	xor r11,r11
	mov r11d, [eax+0x28]
	add r11d, ebx

	xor rcx, rcx
	mov cx, [eax+0x06]

	pop rax

	xor r8,r8
	xor r9,r9
	xor r10,r10
	.section_loop:
	; eax = source base
	; ebx = dest base
	; ecx = sections left
	; edx = current section base
	
	;mov r?, [edx] ; section name [meh]
	mov r8d, [edx+0x14] ; source offset
	mov r9d, [edx+0x0C] ; dest offset
	mov r10d, [edx+0x10] ; size

	mov rsi, rax
	add rsi, r8
	mov rdi, rbx
	add rdi, r9
	push rcx
	mov rcx, r10

	rep movsb

	cmp rdi, [kernel_info_table.kernel_end+0x60000]
	jb .no_update_kernel_end

	mov [kernel_info_table.kernel_end+0x60000], rdi

	.no_update_kernel_end:
	pop rcx
	
	add edx,40
	loop .section_loop

	; enable sse for float ops
	mov rax, cr4
	or rax, 0x200
	mov cr4, rax

	; reset fpu
	fninit

	jmp r11

;kernel_source:
;	dd 0
;kernel_size: ; not sure I even need this...
;	dd 0







; drop back into real mode, maybe reset graphics mode, print message
long_halt:
	call enter_real
	use16
	jmp boot_halt

error_kernel_missing:
	db "Failed to locate kernel.",0

error_kernel_invalid:
	db "The supplied kernel is invalid.",0

error_kernel_32:
	db "A 32 bit kernel was supplied. Only 64 bit kernels are supported.",0

error_kernel_not_native:
	db "The kernel is a non-native PE.",0

error_kernel_bad_offset:
	db "The kernel does not want to be loaded in the correct place.",0

; Fill disk to even number of sectors!
times 512-(($-$$) mod 512) db 0

; Determine sector count
stage2_sectors = (($-$$) / 512)-1

display "Sectors in second stage: "
display_num stage2_sectors
display 0x0a,0x0d

display "Max address: "
display_num $
display " / "
display_num 0xFFFF
display 0x0a,0x0d