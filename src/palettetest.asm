	;; To test palette rendering
	;; I wrote those files' roms directly in machine code,
	;; I'm only providing the source code for completeness but don't expect
	;; these to be in any standard format
	;; ===========================================================================
	;; Mapper: 0
	;; CHR ROM: 8K
	;; PRG ROM: 16K
	.code
$8000:	lda #$00
	sta $2000
	lda #$3f
	sta $2006
	lda #$00
	sta $2006
	lda #$05
	sta $2007
	lda #$08
	sta $2007
$8019:	jmp $8019
$bffc:	.data $00, $80
