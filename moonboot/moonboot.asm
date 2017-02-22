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

; This is the boot sector.
; All we really want to do here is load the rest of the boot code!
; Assume a 1.44mb floppy. 80 tracks, 18 sectors, 2 heads

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

boot_stage1:

	; store boot disk
	mov [disk_n],dl

	; relocate stack
	mov ax,0x5000
	mov ss,ax
	mov esp,0xFFF0
	mov ebp,esp
	
	; check 64 bit support
	mov eax, 0x80000001
	cpuid
	test edx,0x20000000

	mov si, error_longmode
	jz boot_halt

	; enable A20 line
	mov ax, 0x2401
	int 0x15
	
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

; we should be safe pushing text over the partition table.

error_longmode:
	db "Your processor does not support long mode.",0x0a,0x0d
	db "If you're using a virtual machine, it may be configured incorrectly.",0
error_a20:
	db "Failed to fix A20 line.",0
error_disk_unsupported:
	db "Can't read sectors from disk.",0
error_disk_failed:
	db "Failed to read sectors from disk.",0
error_halt:
	db 0x0a,0x0a,0x0d," - BOOT HALTED -",0

; Fill out boot sector.
bytes_left = 510-($-$$)

display "Bytes left in boot sector: "
display_num (bytes_left-64)
display 0x0a,0x0d

times bytes_left db 0
db 055h, 0AAh

fat_fsi:
	rb 512

boot_stage2:

	; load gdt
	lgdt [_gdt.ptr]

	; set up page tables
	mov bx, 0x7000
	mov es, bx
	
	; zero all three tables
	mov bx, 0
	mov eax, 0
	.zero_pages:
	mov [es:bx], eax
	add bx, 4
	cmp bx, 0x3000
	jl .zero_pages

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
	jl .fill_table
	
	; tell the processor where the page table is
	mov eax, 0x70000
	mov cr3, eax

	; enable pae
	mov eax, cr4
	or eax, 0x20
	mov cr4, eax

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
	call fat_seek

	; Search lunatic directory for init directory
	mov si, fname_init
	call fat_seek

	; Read the directory, so we can see what's there.
	call fat_read_file

	; Say hi
	mov si, hello
	call print_str

	cli
	hlt

print_filename:
	pushad

	mov ah,0x0e
	mov bh,0

	mov cx,si
	add cx,11

	.top:
	mov al,[es:si]
	cmp si,cx
	jge .end
	
	int 0x10
	inc si
	jmp .top
	
	.end:
	mov al,0x0a
	int 0x10

	mov al,0x0d
	int 0x10

	popad
	ret

fat_table_cache_index:
	dd 1 ; set 1st index to an invalid value
	rd 127

; eax = current cluster -> eax = next cluster
fat_next_cluster:
	pushad
	
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

	and eax,0x0FFFFFFF

	preserve_eax
	popad
	ret

fat_cluster_base:
	dd 0

; eax = cluster
; ebx = offset in io buffer -> ebx = new offset in io buffer
fat_load_cluster:
	pushad
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
	jge .eof
	cmp eax, 1
	jle .eof

	cmp ebx, 0x10000
	jl .read_loop
	jg .overflow

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
fat_seek:
	pushad

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
	jmp .top

	.success:
	mov eax, [es:edi+0x14]
	shl eax,16
	or eax, [es:edi+0x1A]
	preserve_eax
	popad
	ret

	.fail:
	mov si,error_fat_file_not_found
	call boot_halt

error_fat_table:
	db "Failed to load part of the FAT table.",0

error_fat_cluster:
	db "Failed to load a FAT cluster.",0

error_fat_overflow:
	db "The FAT read function overflowed its buffer. Whoops.",0

error_fat_file_not_found:
	db "Failed to find a file required to boot.",0

hello:
	db "Moonboot's work is done, for now.",0

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
	dw 0 ; limit, low
	dw 0 ; base, low
	db 0 ; base, middle
	db 10011010b; access
	db 00100000b ; flags, limit high ???
	db 0 ; base, high

	.data = $ - _gdt
	dw 0 ; limit, low
	dw 0 ; base, low
	db 0 ; base, middle
	db 10010010b; access
	db 00100000b ; flags, limit high ???
	db 0 ; base, high

	.ptr:
	dw $ - _gdt - 1 ; single entry
	dd _gdt

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