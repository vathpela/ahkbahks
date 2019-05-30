SECTIONS
{
   . = NEXT(CONSTANT(MAXPAGESIZE));
   bss_ahkbahks_canary_start_ = .;
   .bss.ahkbahks.canary ALIGN(CONSTANT(MAXPAGESIZE)) : { *(.bss.ahkbahks.canary) }
   bss_ahkbahks_canary_end_ = .;
   . = NEXT(CONSTANT(MAXPAGESIZE));
   bss_ahkbahks_canary_pages_end_ = .;
}
INSERT AFTER .bss;
