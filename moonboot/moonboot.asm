; a macro I stole off the FASM forum and edited
macro display_num num 
{
	value = num
	pos=1

	while pos*10<=num
		pos=pos*10
	end while

	while pos>=1
		digit=value/pos 
		value=value-(digit*pos)
		pos=pos/10
		display ('0'+digit)
	end while
}

; This is the boot sector.
; All we really want to do here is load the rest of the boot code!
; Assume a 1.44mb floppy. 80 tracks, 18 sectors, 2 heads

use16
org 0x7c00
	jmp boot_stage1
	nop

bpb:
	rb 87 ; We patch this in later using our build script.

boot_stage1:

	; No interrupts!
	cli
	
	; store boot disk
	mov [disk_n],dl

	; enable A20 line
	mov ax, 0x2401
	int 0x15

	mov si, error_a20
	jc boot_halt

	; check 64 bit support
	mov eax, 0x80000001
	cpuid
	test edx,0x20000000

	mov si, error_longmode
	jz boot_halt

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

	; Load stage 2
	;mov ah,0x02
	;mov al,stage2_sectors
	;mov ch,0
	;mov cl,2 ;fucking sector indexing starts at 1, wtf
	;mov dh,0
	;mov bx,0x07e0
	;mov es,bx
	;mov bx,0
	;int 0x13

	;mov si,error_disk
	;jc fail
	;jmp boot_stage2

boot_halt:
	call putstr
	mov si,error_halt
	call putstr
	hlt

putstr:
	mov ah,0x0e
	mov bh,0
	mov bl,0x0d

	.top:
	mov al,[si]
	cmp al,0
	je .end
	
	int 0x10
	inc si
	jmp .top
	
	.end:
	retn

error_a20:
	db "Failed to fix A20 line.",0
error_longmode:
	db "Your processor does not support long mode.",0x0a,0x0d
	db "If you're using a virtual machine, it may be configured incorrectly.",0
error_disk_unsupported:
	db "Can't load the rest of the boot loader.",0
error_disk_failed:
	db "Failed to load the rest of the boot loader.",0
error_halt:
	db 0x0a,0x0a,0x0d," - BOOT HALTED -",0

disk_n:
	db 0
disk_addr:
	db 0x10 ; packet size
	db 0 ; reserved
	dw stage2_sectors ; sector count (MAX 127 ON SOME BIOS)
	dw 0,0x07e0 ; read buffer
	dq 1


; Fill out boot sector.
bytes_left = 510-($-$$)

display "Bytes left in boot sector: "
display_num bytes_left
display 0x0a,0x0d

times bytes_left db 0
db 055h, 0AAh

boot_stage2:
	; load gdt
	lgdt [_gdt.ptr]

	; set up page tables
	mov bx, page_table/16
	mov es, bx
	
	; zero all three tables
	mov bx,0
	mov eax, 0
	.zero_pages:
	mov [es:bx], eax
	add bx,4
	cmp bx,0x3000
	jl .zero_pages

	; root points to 512*512 gig (256 tb) -> only ever fill first entry, mark rest as non-existant
	mov bx,0
	mov eax,page_table+0x1000+1
	mov [es:bx], eax

	; mid table points to 512*1 gig tables (512 gig) -> fill first entry initially, mark rest as non-existant, allow extension later
	mov bx,0x1000
	mov eax,page_table+0x2000+1
	mov [es:bx], eax

	; lowest table points to 512*2 mb pages (1 gig) -> always fill, PS SET
	mov bx,0x2000
	mov eax,0x0081
	.fill_table:
	mov [es:bx], eax
	add bx,8
	add eax,0x00200000
	cmp bx,0x3000
	jl .fill_table
	
	; tell the processor where the page table is
	mov eax, page_table
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

	; enable paging and protected mode
	;mov eax, cr0
	;or eax, 0x80000001 ; 0x80000001
	;mov cr0, eax


	;mov ax, _gdt.unreal
	;mov es, ax

	;hlt

	; switch back to real mode
	;mov eax, cr0
	;and eax,0x7FFFFFFE
	;mov cr0, eax
	
	; hlt

	; sti
	;hlt

	mov si, hello
	call putstr
	; ???
	;hlt

	;mov ax, 0
	;mov es, ax

	;mov eax,"BALZ"
	;mov bx,0
	;mov [es:bx],eax

	;mov ah,0x02
	;mov al,stage2_sectors
	;mov ch,0
	;mov cl,2 ;fucking sector indexing starts at 1, wtf
	;mov dh,0
	;mov bx,0
	;int 0x13

	hlt
	hlt
	hlt
	hlt

	;jmp _gdt.code:boot_stage3

use64
boot_stage3:
	; fix the rest of the segment selectors
	mov ax, _gdt.data
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov rsp, 0x7FFFF

	hlt



hello:
	db "Everything seems to be working! Yay~",0

_gdt:
	.null = $ - _gdt
	dw 0 ; limit, low
	dw 0 ; base, low
	db 0 ; base, middle
	db 0 ; access
	db 0 ; flags, limit high ???
	db 0 ; base, high

	;.unreal = $ - _gdt
	;dw 0xFFFF ; limit, low
	;dw 0 ; base, low
	;.unreal_base_middle:
	;db 0x10 ; base, middle
	;db 10010010b; access
	;db 00000000b ; flags, limit high ???
	;db 0 ; base, high

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

; Determine sector count, get angry if too many!
stage2_sectors = (($-$$) / 512)-1

display "Sectors in stage 2: "
display_num stage2_sectors
display 0x0a,0x0d

if stage2_sectors>17
	display "TOO MANY SECTORS IN STAGE 2!",0x0a,0x0d
end if

page_table = $
if (page_table mod 0x1000) <> 0
	display "PAGE TABLE NOT ALIGNED",0x0a,0x0d
end if