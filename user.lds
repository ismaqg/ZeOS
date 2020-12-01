/*
 *  ZeOS - jcosta septembre 2006
 *  user.lds - Linker Script to create user memory image
 */

ENTRY(main)
SECTIONS
{
  . = 0x100000; /* User CODE will start at this address */
    
  .text.main : { *(.text.main) }
  .text : { *(.text) }

  . = 0x108000; /* User DATA will start at this address */
  .rodata : { *(.rodata) }      /* Read Only Data */
  .data : { *(.data) }          /* Normal Data */
  .bss : { *(.bss) }            /* Uninitialized Data */


  . = 0x109000; /*Isma: Seccion añadida en la direccion 0x109000 para almacenar errno. Ahora la variable errno tendra siempre esta direccion*/
  .MySection : { *(.MySection) } /*Isma: Nota que ahora la cabecera de errno en el lib.c es así:
				      int errno __attribute__((section(".MySection")));*/
}
