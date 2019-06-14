	;; I needed this to test the behaviour of V-Blanks when
	;; I didn't touch the PPU for an entire frame
	;; I wrote those files' roms directly in machine code,
	;; I'm only providing the source code for completeness but don't expect
	;; these to be in any standard format
	;; ===========================================================================
	;; Mapper: 0
	;; CHR ROM: 8K
	;; PRG ROM: 16K
	.code
$8000:	jmp $8000
$bffc:	.data $00, $80
