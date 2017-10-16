(define-module macaddr
  (use gauche.uvector)
  (export string->mac-address))

(select-module macaddr)

(define (string->mac-address str)
  (list->u8vector (map (cut string->number <> 16)
		       (string-split str #[-:]))))
